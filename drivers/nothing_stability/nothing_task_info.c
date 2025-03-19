#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int show_all_process_thread_id(struct seq_file *m, void *p)
{
	struct task_struct *t;

	seq_printf(m, "pid uid tgid name\n");

	rcu_read_lock();
	for_each_process(t) {
		if (!t)
			continue;
		seq_printf(m, "%5d %5d %5d %s\n",
			t->pid,
			from_kuid(&init_user_ns, task_uid(t)),
			t->tgid,
			t->comm
			);
	}

	rcu_read_unlock();
	return 0;
}

static int show_all_tasks_id_open(struct inode *inode, struct file *file)
{
	return single_open(file, show_all_process_thread_id, NULL);
}

static const struct proc_ops show_all_tasks_id_fops = {
	.proc_open       = show_all_tasks_id_open,
	.proc_read       = seq_read,
	.proc_lseek     = seq_lseek,
	.proc_release    = single_release,
};

static int __init nt_taskinfo_init(void)
{
	struct proc_dir_entry *root, *pentry;

	root = proc_mkdir("nt_taskinfo", NULL);
	if(!root){
		pr_err("mkdir nt_taskinfo failed!\n");
		return -1;
	}
	pentry = proc_create("show_all_tasks_id", S_IRUGO, root, &show_all_tasks_id_fops);
	if(!pentry) {
		pr_err("create node show_all_tasks node failed!\n");
		return -1;
	}
	return 0;
}
device_initcall(nt_taskinfo_init);

MODULE_LICENSE("GPL v2");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("<BSP_CORE@nothing.tech>");
MODULE_DESCRIPTION("NOTHING task information");
MODULE_IMPORT_NS(MINIDUMP);
