#include <linux/atomic.h>
#include <linux/backing-dev-defs.h>
#include <linux/blkdev.h>
#include <linux/device-mapper.h>
#include <linux/dm-io.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/kallsyms.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/namei.h>
#include <linux/proc_fs.h>
#include <linux/sched/debug.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/slab.h>
#include "../../drivers/md/dm-core.h"
#include "../../drivers/md/dm-verity.h"
#include "include/nothing_check_parts.h"

static char *unknown_dev = "unknown";
static char tracing_symbol[KSYM_NAME_LEN] = {0};
static char rec_err_parts[MAX_STRING_LENGTH] = {0};
static char proc_data[2] = {'0', '\0'};
static int cur_len = 0;
static struct kprobe *g_krp = NULL;
struct proc_dir_entry *root = NULL;
struct proc_dir_entry *enable_node = NULL;
struct proc_dir_entry *result_node = NULL;

static int append_to_arr(const char *str) {
	int str_len;
	int add_len;

	if(!str)
		str = unknown_dev;

	str_len = strlen(str);
	add_len = str_len + (cur_len > 0 ? 1 : 0);

	if (cur_len + add_len >= MAX_STRING_LENGTH) {
		NT_partition_err_print("Array is full\n");
		return -1;
	}

	if (cur_len > 0) {
		rec_err_parts[cur_len] = ',';
		cur_len++;
	}

	strncpy(&rec_err_parts[cur_len], str, str_len);
	cur_len += str_len;
	rec_err_parts[cur_len] = '\0';

	return 0;
}

static int entry_handler(struct kprobe *ri, struct pt_regs *regs) {
	struct dm_verity *dm_verity = (struct dm_verity *)regs->regs[0];
	struct block_device *bdev = NULL;
	struct mapped_device *md = NULL;
	struct nt_hash_cell *hc = NULL;
	char *error_dev_name = NULL;
	dev_t dev;

	if(!dm_verity)
		goto OUT;
	if(!dm_verity->data_dev)
		goto OUT;

	dev = name_to_dev_t(dm_verity->data_dev->name);
	bdev = blkdev_get_by_dev(dev, FMODE_READ, NULL);
	if (IS_ERR(bdev))
		goto OUT;

	md = bdev->bd_disk->private_data;
	if(!md)
		goto OUT_REL_DEV;

	hc = (struct nt_hash_cell *)md->interface_ptr;
	if (hc && hc->name) {
		NT_partition_err_print("Bad block found, Partition name: %s\n", hc->name);
		error_dev_name = hc->name;
	}
	append_to_arr(error_dev_name);

OUT_REL_DEV:
	blkdev_put(bdev, FMODE_READ);
OUT:
	return 0;
}

int insert_kprobe(char *symbol_name) {
	int ret;

	if (proc_data[0] == '0') {
		g_krp = kvzalloc(sizeof(struct kprobe), GFP_KERNEL);
		if (!g_krp) {
			NT_partition_info_print("%s: Failed to allocate memory for kprobe\n", __func__);
			return -ENOMEM;
		}
		g_krp->pre_handler = entry_handler;
		g_krp->symbol_name = symbol_name;
		ret = register_kprobe(g_krp);
		if (ret < 0) {
			NT_partition_info_print("Failed to register_kprobe, ret=%d\n", ret);
			kvfree(g_krp);
			return ret;
		} else {
			NT_partition_info_print("Registered addr=%p\n", g_krp->addr);
			proc_data[0] = '1';
		}
	}

	return 0;
}

void remove_kprobe(char *symbol_name) {
	if (proc_data[0] == '1') {
		if (g_krp) {
			unregister_kprobe(g_krp);
			kvfree(g_krp);
			g_krp = NULL;
		}
		proc_data[0] = '0';
		cur_len = 0;
		memset(tracing_symbol, 0, sizeof(tracing_symbol));
		memset(rec_err_parts, 0, sizeof(rec_err_parts));
		NT_partition_info_print("%s done\n", __func__);
	}
}

static ssize_t enable_proc_write(struct file *file,
	const char __user *user_buffer, size_t count, loff_t *ppos) {
	char buffer[SZ_128] = {0};
	char input_symbol_str[KSYM_NAME_LEN] = {0};
	int input_target_value = 0, insert_ret = 0;
	ssize_t ret;

	ret = simple_write_to_buffer(buffer, sizeof(buffer), ppos, user_buffer, count);
	if (ret < 0) {
		NT_partition_info_print("%s: simple_write_to_buffer fail, ret:%zd\n",
										__func__, ret);
		return ret;
	}

	buffer[ret] = '\0';
	proc_data[1] = '\0';

	if (sscanf(buffer, "%s %d", input_symbol_str, &input_target_value) == 2) {
		NT_partition_info_print("%s: input_symbol_str:%s, input_target_value:%d\n",
										__func__, input_symbol_str, input_target_value);
		if (input_target_value == 1) {
			insert_ret = insert_kprobe(input_symbol_str);
			if (insert_ret < 0) {
				NT_partition_info_print("%s: insert_kprobe fail\n", __func__);
				return (ssize_t)insert_ret;
			}
			strcpy(tracing_symbol, input_symbol_str);
			NT_partition_info_print("%s : insert_kprobe, tracing_symbol:%s\n",
												__func__, tracing_symbol);
		} else if (input_target_value == 0) {
			remove_kprobe(input_symbol_str);
		} else {
			NT_partition_info_print("%s: input_target_value :%d invalid\n",
												__func__, input_target_value);
		}
	} else{
		NT_partition_info_print("%s: parsing error\n", __func__);
	}
	return ret;
}

static ssize_t enable_proc_read(struct file *file,
	char __user *user_buffer, size_t count, loff_t *ppos) {
	return simple_read_from_buffer(user_buffer, count, ppos, proc_data, strlen(proc_data));
}

static ssize_t result_proc_read(struct file *file,
	char __user *user_buffer, size_t count, loff_t *ppos) {
	return simple_read_from_buffer(user_buffer, count, ppos,
									rec_err_parts, strlen(rec_err_parts));
}

static const struct proc_ops enable_proc_ops = {
	.proc_read = enable_proc_read,
	.proc_write = enable_proc_write,
};

static const struct proc_ops result_proc_ops = {
	.proc_read = result_proc_read,
};

static int create_file_node(void)
{
	root = proc_mkdir("nt_check_partitions", NULL);
	if (!root){
		NT_partition_info_print("mkdir nt_check_partitions node failed!\n");
		return 1;
	}

	enable_node = proc_create("enable", S_IRUGO, root, &enable_proc_ops);
	if (!enable_node) {
		NT_partition_info_print("create enable_node node failed!\n");
		proc_remove(root);
		return 1;
	}

	result_node = proc_create("result", S_IRUGO, root, &result_proc_ops);
	if (!result_node) {
		NT_partition_info_print("create result_node node failed!\n");
		proc_remove(root);
		proc_remove(enable_node);
		return 1;
	}

	return 0;
}

static int __init nt_check_parts_init(void)
{
	if (create_file_node())
		NT_partition_info_print("%s fail\n", __func__);
	else
		NT_partition_info_print("%s done\n", __func__);

	return 0;
}

static void __exit nt_check_parts_exit(void)
{
	if (proc_data[0] == '1')
		remove_kprobe(tracing_symbol);

	if (result_node)
		proc_remove(result_node);

	if (enable_node)
		proc_remove(enable_node);

	if (root)
		proc_remove(root);

	return;
}

module_init(nt_check_parts_init);
module_exit(nt_check_parts_exit);

MODULE_LICENSE("GPL v2");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("<BSP_CORE@nothing.tech>");
MODULE_DESCRIPTION("NOTHING check partitions");
