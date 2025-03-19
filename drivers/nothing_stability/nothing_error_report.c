#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/atomic.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/sizes.h>
#include <linux/wait.h>
#include <linux/nt_error_report.h>

#define PROC_NAME "nt_er_test"

#define DEVICE_NAME "nt_er_dev"	 // Device name in /dev
#define CLASS_NAME "nt_er_class" // Device class name

#define NT_ER_BUF_SZ SZ_128

static int major_number;
static struct class *nt_er_class = NULL;
static struct device *nt_er_device = NULL;
//static struct cdev nt_er_cdev;

static DECLARE_WAIT_QUEUE_HEAD(nt_er_wq);
static int data_ready = 0; // Flag indicating if data is ready

static atomic_t nt_er_type = ATOMIC_INIT(-1); // Atomic variable to store error type
static char message[SZ_128]; // Buffer to store the message
static size_t message_len = 0; // Length of the message


// Read operation for the device
static ssize_t nt_er_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
	// If data is not ready, wait unless the file is opened with O_NONBLOCK
	if (!data_ready) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;
		wait_event_interruptible(nt_er_wq, data_ready);
	}

	if (message_len == 0)
		return 0;

	// Adjust the length if necessary
	if (len > message_len)
		len = message_len;

	// Copy the message to user space
	if (copy_to_user(buf, message, len))
		return -EFAULT;

	// Reset the data_ready flag and message length
	data_ready = 0;
	message_len = 0;

	return len;
}

// Poll operation for the device
static unsigned int nt_er_poll(struct file *file, poll_table *wait) {
	poll_wait(file, &nt_er_wq, wait); // Add the wait queue to the poll table
	if (data_ready)
		return POLLIN | POLLRDNORM; // Data is ready to be read
	return 0;
}

static int nt_er_open(struct inode *inode, struct file *file) {
	return 0;
}

static int nt_er_release(struct inode *inode, struct file *file) {
	return 0;
}

// File operations structure
static struct file_operations nt_er_fops = {
	.owner = THIS_MODULE,
	.read = nt_er_read,
	.poll = nt_er_poll,
	.open = nt_er_open,
	.release = nt_er_release,
};

// Schedule error report to user space
void nt_er_in_schedule(int err_type) {
	int cur_er_type = err_type;

	atomic_set(&nt_er_type, cur_er_type); // Set the error type
	snprintf(message, sizeof(message), "%d", cur_er_type); // Format the message
	message_len = strlen(message); // Set the message length
	data_ready = 1; // Set the data ready flag
	wake_up_interruptible(&nt_er_wq); // Wake up any waiting processes

	NT_ER_INFO_PRINT("Reported error to user: %s\n", message);
}
EXPORT_SYMBOL_GPL(nt_er_in_schedule);

static ssize_t nt_er_test_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[NT_ER_BUF_SZ];
	int val = 0;

	if (count > NT_ER_BUF_SZ - 1)
		return -EFAULT;

	if (copy_from_user(buf, ubuf, count))
		return -EFAULT;

	buf[count] = '\0';

	if (kstrtoint(buf, 10, &val) != 0)
		return -EINVAL;

	switch (val) {
		case 1:
			nt_er_in_schedule(NT_BATT_ERR);
			break;
		case 2:
			nt_er_in_schedule(NT_UFS_ERR);
			break;
		case 3:
			nt_er_in_schedule(NT_GLINK_ERR);
			break;
		default:
			break;
	}

	return count;
}

static const struct proc_ops nt_er_test_ops = {
	.proc_write = nt_er_test_write,
};

// Module initialization function
static int __init nt_err_er_init(void) {


	struct proc_dir_entry *entry;

	NT_ER_INFO_PRINT("The module initialized\n");

	entry = proc_create(PROC_NAME, 0660, NULL, &nt_er_test_ops);
	if (entry == NULL) {
		NT_ER_ERR_PRINT("Failed to create /proc/%s\n", PROC_NAME);
		return 0;
	}
	// Register the character device
	major_number = register_chrdev(0, DEVICE_NAME, &nt_er_fops);
	if (major_number < 0) {
		NT_ER_ERR_PRINT("Failed to register a major number\n");
		return major_number;
	}

	// Create the device class
	nt_er_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(nt_er_class)) {
		unregister_chrdev(major_number, DEVICE_NAME);
		NT_ER_ERR_PRINT("Failed to register device class\n");
		return PTR_ERR(nt_er_class);
	}

	// Create the device
	nt_er_device = device_create(nt_er_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(nt_er_device)) {
		class_destroy(nt_er_class);
		unregister_chrdev(major_number, DEVICE_NAME);
		NT_ER_ERR_PRINT("Failed to create the device\n");
		return PTR_ERR(nt_er_device);
	}

	NT_ER_INFO_PRINT("The module initialized\n");
	return 0;
}

// Module exit function
static void __exit nt_err_er_exit(void) {
	// Destroy the device and class, and unregister the character device
	remove_proc_entry(PROC_NAME, NULL);
	device_destroy(nt_er_class, MKDEV(major_number, 0));
	class_unregister(nt_er_class);
	class_destroy(nt_er_class);
	unregister_chrdev(major_number, DEVICE_NAME);

	NT_ER_INFO_PRINT("The module exited\n");
}

module_init(nt_err_er_init);
module_exit(nt_err_er_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("<BSP_CORE@nothing.tech>");
MODULE_DESCRIPTION("NOTHING hardware error detect");
