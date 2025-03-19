#include <linux/atomic.h>
#include <linux/fcntl.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/sched/debug.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/version.h>

#define NT_value_info_print(fmt,arg...) \
pr_info("[nt_value_check] "fmt, ##arg)

#define NT_value_err_print(fmt,arg...) \
pr_err("[nt_value_check] "fmt, ##arg)


static int target_value = 10000;

static int debug_log = 0;

static char tracing_symbol[KSYM_NAME_LEN] = {0};

static char proc_data[2] = {'0', '\0'};

static int ret_handler(struct kretprobe_instance *kri, struct pt_regs *regs)
{
	int value;

	value = regs_return_value(regs);
	if (!current->mm)
		return 0;

	if (debug_log && current->group_leader)
		NT_value_err_print("parent task %s:%d, task %s:%d, value:%d\n", current->group_leader->comm, current->group_leader->pid, current->comm, current->pid, value);

	if (value < target_value)
		return 0;

	if (current->group_leader) {
		NT_value_err_print("parent task %s:%d\n", current->group_leader->comm, current->group_leader->pid);
		sched_show_task(current->group_leader);
	}
	NT_value_err_print("task %s:%d, value:%d\n", current->comm, current->pid, value);
	sched_show_task(current);
	panic("nt value check!");
	BUG_ON(1);

	return 0;
}

static struct kretprobe g_krp = {
	.handler = ret_handler,
	.maxactive = 30,
};

void insert_kprobe(char *symbol_name) {
	int ret;

	memset(tracing_symbol, 0, KSYM_NAME_LEN);
	strncpy(tracing_symbol, symbol_name, strlen(symbol_name));
	g_krp.kp.symbol_name = tracing_symbol;

	ret = register_kretprobe(&g_krp);
	if (ret < 0) {
		NT_value_info_print("Failed to register_kretprobe, ret=%d\n", ret);
	} else {
		NT_value_info_print("Registered addr=%p\n", g_krp.kp.addr);
	}
}

static ssize_t nt_value_check_proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos) {
	return simple_read_from_buffer(user_buffer, count, ppos, proc_data, strlen(proc_data));
}

static ssize_t nt_value_check_proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
	char buffer[SZ_128] = {0};
	char input_symbol_str[KSYM_NAME_LEN] = {0};
	int input_target_value;
	int input_debug_log;
	ssize_t ret;


	ret = simple_write_to_buffer(buffer, sizeof(buffer), ppos, user_buffer, count);
	if (ret < 0)
		return ret;

	buffer[ret] = '\0';
	proc_data[1] = '\0';

	if (proc_data[0] == '0' && sscanf(buffer, "%s %d %d", input_symbol_str, &input_target_value, &input_debug_log) == 3) {
		NT_value_info_print("Parsed string: %s, Parsed int: %d, Parsed int: %d %c\n", input_symbol_str, input_target_value, input_debug_log, proc_data[0]);
		target_value = input_target_value;
		debug_log = input_debug_log;
		insert_kprobe(input_symbol_str);
		proc_data[0] = '1';
	} else if (buffer[0] == '0') {
		proc_data[0] = '0';
		if (g_krp.kp.addr) {
			unregister_kretprobe(&g_krp);
			NT_value_info_print("unregistered addr=%p\n", g_krp.kp.addr);
			g_krp.kp.addr = NULL;
		}
	} else {
		NT_value_err_print("Invalid format %s:%c\n", buffer, proc_data[0]);
	}

	return ret;
}

static const struct proc_ops nt_value_check_proc_ops = {
	.proc_read = nt_value_check_proc_read,
	.proc_write = nt_value_check_proc_write,
};

static int __init nt_value_check_init(void)
{
	struct proc_dir_entry *root = NULL;
	struct proc_dir_entry *value_check_node = NULL;

	root = proc_mkdir("nt_value_check", NULL);
	if (!root){
		NT_value_info_print("mkdir nt_value_check failed!\n");
		return 1;
	}

	value_check_node = proc_create("value_check_node", S_IRUGO, root, &nt_value_check_proc_ops);
	if (!value_check_node) {
		NT_value_info_print("create node value_check_node node failed!\n");
		return 1;
	}

	return 0;
}

static void __exit nt_value_check_exit(void)
{
	return;
}

module_init(nt_value_check_init);
module_exit(nt_value_check_exit);

MODULE_LICENSE("GPL v2");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("<BSP_CORE@nothing.tech>");
MODULE_DESCRIPTION("NOTHING value check");
