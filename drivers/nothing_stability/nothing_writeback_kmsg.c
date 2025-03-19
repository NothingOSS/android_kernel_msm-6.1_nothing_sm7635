#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/kmsg_dump.h>
#include <linux/mount.h>
#include <linux/kdev_t.h>
#include <linux/uio.h>
#include <linux/version.h>
#include "include/nothing_writeback_kmsg.h"

static struct task_struct *tsk;
static struct device *nt_sys_dev;
static struct class *nt_sys_dev_class;
static dev_t nt_sys_device_devt;
static unsigned int is_boot_system_server = 0;
NT_reserve_log_header nt_kmsg_header;
#if IS_ENABLED(CONFIG_ARCH_QCOM)
NT_reserve_log_header nt_uefi_header;
#else
lk_log_emmc_header nt_lk_header;
#endif

int NT_blkdev_fsync(struct file *filp, loff_t start, loff_t end,
		int datasync)
{
	struct inode *bd_inode = filp->f_mapping->host;
	struct block_device *bdev = I_BDEV(bd_inode);
	int error;

	error = file_write_and_wait_range(filp, start, end);
	if (error)
		return error;

	/*
	 * There is no need to serialise calls to blkdev_issue_flush with
	 * i_mutex and doing so causes performance issues with concurrent
	 * O_SYNC writers to a block device.
	 */
	error = blkdev_issue_flush(bdev);
	if (error == -EOPNOTSUPP)
		error = 0;

	return error;
}
const struct file_operations f_op = {.fsync = NT_blkdev_fsync};

int __verify_target_partition_layout(void){

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
	 //NT_CHECK_LAYOUT_END_OFFSET need equal nt_kmsg partition size = 64MB
	if(NT_CHECK_LAYOUT_END_OFFSET == NT_64M_MULT)
		return 1;
	NT_rkl_err_print("Default offset is %d ,true offset is %d\n", NT_CHECK_LAYOUT_END_OFFSET, NT_64M_MULT);
#else
	 //NT_CHECK_LAYOUT_END_OFFSET need equal logdump partition size = 512MB
	if(NT_CHECK_LAYOUT_END_OFFSET == NT_512M_MULT)
		return 1;
	NT_rkl_err_print("Default offset is %d ,true offset is %d\n", NT_CHECK_LAYOUT_END_OFFSET, NT_512M_MULT);
#endif
	return 0;
}

void __open_target_partition(struct block_device *bdev, struct file *target_partition_file) {
	target_partition_file->f_mapping = bdev->bd_inode->i_mapping;
	target_partition_file->f_flags = O_DSYNC | __O_SYNC | O_NOATIME;
	target_partition_file->f_inode = bdev->bd_inode;
	target_partition_file->f_lock = bdev->bd_inode->i_lock;
}

int __read_buf_from_target_partition(struct file *target_partition_file, void *head, int len, int pos) {
	struct kiocb kiocb;
	struct iov_iter iter;
	struct kvec iov;
	int read_size = 0;

	init_sync_kiocb(&kiocb, target_partition_file);
	kiocb.ki_pos = pos;
	iov.iov_base = head;
	iov.iov_len = len;
	iov_iter_kvec(&iter, READ, &iov, 1, len);

	read_size = generic_file_read_iter(&kiocb, &iter);

	if (read_size <= 0) {
		NT_rkl_err_print("Read buf failed, call by %pS\n", __builtin_return_address(0));
		return 1;
	}
	return 0;
}

int __write_buf_to_target_partition(struct file *target_partition_file, void *head, int len, int pos) {
	struct kiocb kiocb;
	struct iov_iter iter;
	struct kvec iov;
	int ret = 0;

	if (in_interrupt() || irqs_disabled())
		return -EBUSY;
	init_sync_kiocb(&kiocb, target_partition_file);
	kiocb.ki_pos = pos;
	iov.iov_base = head;
	iov.iov_len = len;
	iov_iter_kvec(&iter, WRITE, &iov, 1, len);

	ret = generic_write_checks(&kiocb, &iter);
	if (ret > 0) {
		//ret = generic_perform_write(target_partition_file, &iter, kiocb.ki_pos);
		ret = generic_perform_write(&kiocb, &iter);
	}
	if (ret > 0) {
		target_partition_file->f_op = &f_op;
		kiocb.ki_pos += ret;
		ret = generic_write_sync(&kiocb, ret);
		if (ret < 0) {
			NT_rkl_err_print("Write buf failed, call by %pS\n", __builtin_return_address(0));
			return 1;
		}
	}
	return 0;
}

struct block_device *get_target_partition_block_device(char* partition_label)
{
	struct block_device *bdev = NULL;
	int retry_wait_for_device = RETRY_COUNT_FOR_GET_DEV_T;
	dev_t devt;


	while (retry_wait_for_device > 0){
		devt = name_to_dev_t(partition_label);
		bdev = blkdev_get_by_dev(devt, FMODE_READ | FMODE_WRITE, NULL);
			if (!IS_ERR(bdev)) {
				NT_rkl_err_print("Get partition: %s\n", (char *)bdev->bd_meta_info->volname);
				return bdev;
			} else {
				NT_rkl_err_print("Get partition failed, retry count %d\n", retry_wait_for_device);
				msleep_interruptible(PER_LOOP_MS);
			}
		--retry_wait_for_device;
		}
	return NULL;
}

static int bootloader_count_init(void) {
	struct block_device *bdev = NULL;
	struct file target_partition_file;

	bdev = get_target_partition_block_device(NT_UEFI_PARTITION_LABEL);
	if(!bdev) {
		NT_rkl_err_print("%s: Get target partition block device failed!\n", __func__);
		return 1;
	}

	memset(&target_partition_file, 0, sizeof(struct file));

	__open_target_partition(bdev, &target_partition_file);

#if IS_ENABLED(CONFIG_ARCH_QCOM)
	if(__read_buf_from_target_partition(&target_partition_file, &nt_uefi_header,
		sizeof(NT_reserve_log_header), NT_RESERVE_N_HEADER_OFFSET)) {
		nt_uefi_header.bootloader_count = 0;
#else
	if(__read_buf_from_target_partition(&target_partition_file, &nt_lk_header,
		sizeof(lk_log_emmc_header), NT_RESERVE_N_HEADER_OFFSET)) {
		nt_lk_header.reserve_flag[LOG_INDEX] = 0;
#endif
		NT_rkl_err_print("%s: Error getting header from nt_uefi\n", __func__);
		return 1;
	}

	return 0;
}

int action_reserve_kernel_log_header(struct file *target_partition_file, int action) {

	if(__read_buf_from_target_partition(target_partition_file, &nt_kmsg_header
		, sizeof(NT_reserve_log_header), NT_RESERVE_N_HEADER_OFFSET))
		goto out;

	switch(action) {
		case NT_COMPARE_HEADER_MAGIC:
			return memcmp(nt_kmsg_header.magic, NT_LOG_MAGIC, strlen(NT_LOG_MAGIC));
		break;
		case NT_RESET_HEADER:
			memset(&nt_kmsg_header, 0, sizeof(NT_reserve_log_header));
			memcpy(nt_kmsg_header.magic, NT_LOG_MAGIC, strlen(NT_LOG_MAGIC));
			NT_rkl_err_print("Reset header, boot_count is %d bootloader_count is %d\n",
#if IS_ENABLED(CONFIG_ARCH_QCOM)
				nt_kmsg_header.boot_count, nt_uefi_header.bootloader_count);
#else
				nt_kmsg_header.boot_count, nt_lk_header.reserve_flag[LOG_INDEX]);
#endif
		break;
		case NT_ADD_BOOT_COUNT:
			++nt_kmsg_header.boot_count;
			NT_rkl_err_print("boot_count is %d bootloader_count is %d\n",
#if IS_ENABLED(CONFIG_ARCH_QCOM)
				nt_kmsg_header.boot_count, nt_uefi_header.bootloader_count);
#else
				nt_kmsg_header.boot_count, nt_lk_header.reserve_flag[LOG_INDEX]);
#endif
		break;
		case NT_ADD_PANIC_COUNT:
			++nt_kmsg_header.panic_count;
			NT_rkl_err_print("panic_count is %d\n", nt_kmsg_header.panic_count);
		break;
		case NT_SET_LAST_BOOT_IS_FAILED:
			nt_kmsg_header.last_boot_is_fail = 1;
			NT_rkl_err_print("last_boot_is_fail %d, boot_count is %d\n"
				, nt_kmsg_header.last_boot_is_fail, nt_kmsg_header.boot_count);
		break;
		case NT_SET_LAST_REBOOT_IS_PANIC:
			nt_kmsg_header.last_reboot_is_panic = 1;
			NT_rkl_err_print("last_reboot_is_panic %d, panic_count is %d\n"
				, nt_kmsg_header.last_reboot_is_panic, nt_kmsg_header.panic_count);
		break;
		default:
			NT_rkl_err_print("The action is nothing~ nothing~\n");
			goto out;
		break;
	}

	if(__write_buf_to_target_partition(target_partition_file, &nt_kmsg_header
		, sizeof(NT_reserve_log_header), NT_RESERVE_N_HEADER_OFFSET))
		goto out;

	return 0;
	out:
		return 1;
}

int clear_kernel_boot_log(struct file *target_partition_file, int offset) {
	int i = 0;
	char* buf = NULL;

	buf = (char*)vzalloc(NT_PAGE_SIZE * sizeof(char));
	if(!buf) {
		NT_rkl_err_print("Failed to allocate boot log buffer!\n");
		goto out;
	}
	for(i = 0; i < BOOT_LOG_PAGES; i++) {
		if( __write_buf_to_target_partition(target_partition_file, buf, NT_PAGE_SIZE, offset+(i * BOOT_LOG_PAGES))){
			goto out;
		}
	}

out:
	if(buf) {
		vfree(buf);
		buf = NULL;
	}
	return 0;
}

void write_back_kernel_boot_log(struct file *target_partition_file) {
	char* buf = NULL;
	char *line_buf = NULL;
	char prefix_buf[PREFIX_BUFFER_SIZE] = {0};
	//struct kmsg_dumper kmsg_dumper;
	struct kmsg_dump_iter iter;
	unsigned int offset = 0;
	unsigned int ori_offset = 0;
	int delta_4K_offset = 0;
	int final_write = 0;
	size_t len = 0;
	size_t now_len = 0;

	NT_rkl_err_print("%s start!\n", __func__);

	if(!target_partition_file->f_inode) { //check target partition is init.
		NT_rkl_err_print("The block device isn't init!\n");
		goto out;
	}
	ori_offset = offset = COMPUTE_BOOT_LOG_OFFSET(nt_kmsg_header.boot_count);

	if(clear_kernel_boot_log(target_partition_file, offset)) {
		goto out;
	}
	buf = (char*)vzalloc(NT_PAGE_SIZE * sizeof(char));
	if(!buf) {
		NT_rkl_err_print("Failed to allocate boot log buffer!\n");
		goto out;
	}
	line_buf = (char*)vzalloc(BOOT_LOG_SIZE_OF_LINE * sizeof(char));
	if(!line_buf) {
		NT_rkl_err_print("Failed to allocate boot log line buffer!\n");
		goto out;
	}

	if(snprintf(prefix_buf, PREFIX_BUFFER_SIZE, "\nNT_boot_count:%d\n", nt_kmsg_header.boot_count) <= 0) {
		NT_rkl_err_print("Store prefix failed\n");
	} else {
		strncpy(buf, prefix_buf, PREFIX_BUFFER_SIZE);
	}
	now_len = strlen(buf);
	//kmsg_dumper.active = true;
	kmsg_dump_rewind(&iter);
	RE_LOOP:
	while (kmsg_dump_get_line(&iter, true, line_buf, BOOT_LOG_SIZE_OF_LINE, &len)){
		delta_4K_offset = now_len + len - (NT_PAGE_SIZE );
		if (delta_4K_offset > 0) {
			memcpy(buf + now_len, line_buf, len - delta_4K_offset);
			__write_buf_to_target_partition(target_partition_file, buf, NT_PAGE_SIZE, offset);
			offset += NT_PAGE_SIZE;
			if ((offset - ori_offset) >= BOOT_LOG_SIZE) {
				NT_rkl_err_print("boot log full %d %d\n", offset, COMPUTE_BOOT_LOG_OFFSET(nt_kmsg_header.boot_count));
				goto out;
			}
			now_len = 0;
			memset(buf, 0, NT_PAGE_SIZE);
			memcpy(buf + now_len, line_buf + (len - delta_4K_offset), delta_4K_offset);
			now_len += delta_4K_offset;
		} else {
			memcpy(buf + now_len, line_buf, len);
			now_len += len;
		}
	}
	__write_buf_to_target_partition(target_partition_file, buf, NT_PAGE_SIZE, offset);
	msleep_interruptible(PER_LOOP_MS);
	if(!is_boot_system_server) {
		goto RE_LOOP;
	} else if(!final_write) {
		NT_rkl_err_print("Boot into system_server,Stop record log!\n");
		final_write = 1;
		goto RE_LOOP;
	}

out:
	if(buf) {
		vfree(buf);
		buf = NULL;
	}
	if(line_buf) {
		vfree(line_buf);
		line_buf = NULL;
	}
	NT_rkl_err_print("%s end!\n", __func__);
}

int init_reserve_kernel_log_header(struct file *target_partition_file) {

	if (!action_reserve_kernel_log_header(target_partition_file, NT_COMPARE_HEADER_MAGIC)) {
		NT_rkl_err_print("Match magic %s\n", nt_kmsg_header.magic);
		return action_reserve_kernel_log_header(target_partition_file, NT_ADD_BOOT_COUNT);
	} else {
		NT_rkl_err_print("No match magic need init\n");
		 return action_reserve_kernel_log_header(target_partition_file, NT_RESET_HEADER);
	}

	return 1;
}

static ssize_t boot_stage_systemserver_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, 10, "%d\n", is_boot_system_server);
}

static ssize_t boot_stage_systemserver_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	unsigned int value;
	int rc;

	if (size > 10)
		return -EINVAL;

	rc = kstrtou32(buf, 10, &value);
	if (rc) {
		NT_rkl_err_print("%s input wrang value!\n", __func__);
		return rc;
	}
	is_boot_system_server = value;

	return size;
}

static DEVICE_ATTR(boot_stage_systemserver, 0664, boot_stage_systemserver_show, boot_stage_systemserver_store);

int create_NT_device(void) {

	nt_sys_device_devt = MKDEV(NT_MAJOR, NT_MINOR);

	nt_sys_dev_class = class_create(THIS_MODULE,"NT");
	if (IS_ERR_OR_NULL(nt_sys_dev_class)) {
		NT_rkl_err_print("sysfs device class creation failed\n");
		goto out;
	}

	nt_sys_dev = device_create(nt_sys_dev_class, NULL,nt_sys_device_devt, "nt_sys_drvdata", "NT_reserve_kernel_log");
	if (IS_ERR_OR_NULL(nt_sys_dev)) {
		NT_rkl_err_print("sysfs device creation failed\n");
		goto out_class_destroy;
	}

	if (device_create_file(nt_sys_dev, &dev_attr_boot_stage_systemserver)) {
		NT_rkl_err_print("sysfs boot_stage_systemserver file creation failed\n");
		goto out_device_destroy;
	}
	return 0;

out_device_destroy:
	device_destroy(nt_sys_dev_class, nt_sys_device_devt);

out_class_destroy:
	class_destroy(nt_sys_dev_class);
out:
	return 1;
}

void remove_NT_device(void) {

	NT_rkl_err_print("%s start!\n", __func__);
	device_remove_file(nt_sys_dev, &dev_attr_boot_stage_systemserver);

	device_destroy(nt_sys_dev_class, nt_sys_device_devt);

	class_destroy(nt_sys_dev_class);
	NT_rkl_err_print("%s end!\n", __func__);
}


static int NT_reserve_kernel_log_main(void *arg)
{
	struct block_device *bdev = NULL;
	struct file target_partition_file;

	bootloader_count_init();

	if(!__verify_target_partition_layout()){
		NT_rkl_err_print("Error setting, plz check your layout setting!\n");
		return 1;
	}

	if(create_NT_device()){
		NT_rkl_err_print("Failed to create NT device!\n");
		return 1;
	}


	bdev = get_target_partition_block_device(NT_KMSG_PARTITION_LABEL);
	if(!bdev) {
		NT_rkl_err_print("%s: Get target partition block device failed!\n", __func__);
		return 1;
	}

	memset(&target_partition_file, 0, sizeof(struct file));

	__open_target_partition(bdev, &target_partition_file);

	if(init_reserve_kernel_log_header(&target_partition_file)){
		NT_rkl_err_print("Init header failed!\n");
		return 1;
	}
	write_back_kernel_boot_log(&target_partition_file);

	remove_NT_device();

	return 0;
}

static int __init NT_reserve_kernel_log_init(void)
{
	tsk = kthread_run(NT_reserve_kernel_log_main, NULL, "NT_reserve_kernel_log");
	if (!tsk)
		NT_rkl_err_print("kthread init failed\n");

	NT_rkl_info_print("kthread init done\n");
	return 0;
}

module_init(NT_reserve_kernel_log_init);

static void __exit NT_reserve_kernel_log_exit(void)
{
	NT_rkl_info_print("Hello bye~bye!\n");
}
module_exit(NT_reserve_kernel_log_exit);

MODULE_LICENSE("GPL v2");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("<BSP_CORE@nothing.tech>");
MODULE_DESCRIPTION("NOTHING record boot KMSG");
