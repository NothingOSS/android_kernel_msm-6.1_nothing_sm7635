#include <linux/io.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/panic_notifier.h>

#define MAX_SZ_DIAG_ERR_MSG 	100

struct nothing_poweroff_notify {
	struct notifier_block panic_nb;
	struct notifier_block reboot_nb;
	struct notifier_block restart_nb;
};

struct nothing_restart_info {
	char msg[MAX_SZ_DIAG_ERR_MSG];
};

static struct nothing_restart_info *restart_info;
static unsigned rst_msg_size;

static inline void set_restart_msg(const char *msg)
{
    size_t len;

    if (msg) {
        pr_info("%s: %s \n", __func__, msg);
        len = strnlen(msg, rst_msg_size - 1);
        memcpy_toio(restart_info->msg, msg, len);
        iowrite8('\0', &restart_info->msg[len]);
    } else {
        iowrite8('\0', restart_info->msg);
    }

    mb();
}

static int nothing_notifier_panic(struct notifier_block *this, unsigned long event,
			      void *ptr)
{
	char kernel_panic_msg[MAX_SZ_DIAG_ERR_MSG] = "Kernel Panic";
	pr_warn("%s: Get panic notify", __func__);
	if (ptr)
		snprintf(kernel_panic_msg, rst_msg_size - 1, "%s", (char *)ptr);
	set_restart_msg(kernel_panic_msg);
	return NOTIFY_OK;
}

static int nothing_notifier_reboot(struct notifier_block *this, unsigned long event,
			      void *ptr)
{
	char reboot_msg[MAX_SZ_DIAG_ERR_MSG];
	pr_warn("%s: Get reboot notify", __func__);
	if (ptr)
		snprintf(reboot_msg, rst_msg_size - 1, "Reboot: %s", (char *)ptr);
	set_restart_msg(reboot_msg);

	return NOTIFY_OK;
}

static int nothing_notifier_restart(struct notifier_block *this, unsigned long event,
			      void *ptr)
{
	char reboot_msg[MAX_SZ_DIAG_ERR_MSG];
	pr_warn("%s: Get restart notify", __func__);
	if (ptr)
		snprintf(reboot_msg, rst_msg_size - 1, "Reboot: %s", (char *)ptr);
	set_restart_msg(reboot_msg);

	return NOTIFY_OK;
}

static int nothing_restart_info_probe(struct platform_device *pdev)
{

	struct device_node *np;
	unsigned nothing_rst_info_size;
	int ret = 0;
	struct nothing_poweroff_notify *nt_poweroff_notify;

	pr_info("%s Start \n", __func__);

	nt_poweroff_notify = devm_kzalloc(&pdev->dev, sizeof(*nt_poweroff_notify), GFP_KERNEL);
	if (!nt_poweroff_notify) {
		pr_err("%s: Unable to allocate memory for nt_poweroff_notify\n", __func__);
		return -ENOMEM;
	}

	np = of_find_compatible_node(NULL, NULL,
				"nothing,msm-imem-restart_info");
	if (!np) {
		pr_err("%s: unable to find nothing,msm-imem-restart_info\n", __func__);
		return -EINVAL;
	} else {
		restart_info = of_iomap(np, 0);
		if(!restart_info) {
			pr_err("%s: nothing restart_info not valid\n", __func__);
			return -ENOMEM;
		} else {
			ret = of_property_read_u32(np, "info_size", &nothing_rst_info_size);
			if (ret != 0) {
				pr_err("%s: Failed to find info_size property in nothing restart info device node %d\n"
					, __func__, ret);
				return ret;
				}
		}
	}

	rst_msg_size = sizeof(restart_info->msg);

	set_restart_msg("Unknown");

	nt_poweroff_notify->panic_nb.notifier_call = nothing_notifier_panic;
	nt_poweroff_notify->panic_nb.priority = INT_MAX;
	atomic_notifier_chain_register(&panic_notifier_list,
				       &nt_poweroff_notify->panic_nb);

	nt_poweroff_notify->reboot_nb.notifier_call = nothing_notifier_reboot;
	nt_poweroff_notify->reboot_nb.priority = 255;
	register_reboot_notifier(&nt_poweroff_notify->reboot_nb);

	nt_poweroff_notify->reboot_nb.notifier_call = nothing_notifier_restart;
	nt_poweroff_notify->reboot_nb.priority = 201;
	register_restart_handler(&nt_poweroff_notify->restart_nb);

	platform_set_drvdata(pdev, nt_poweroff_notify);

	return 0;
}

static int nothing_restart_info_remove(struct platform_device *pdev)
{
	struct nothing_poweroff_notify *nt_poweroff_notify = platform_get_drvdata(pdev);

	atomic_notifier_chain_unregister(&panic_notifier_list,
					 &nt_poweroff_notify->panic_nb);
	unregister_reboot_notifier(&nt_poweroff_notify->reboot_nb);
	unregister_restart_handler(&nt_poweroff_notify->restart_nb);

	if (restart_info)
		iounmap(restart_info);

	return 0;
}

static const struct of_device_id of_nothing_restart_info_match[] = {
	{ .compatible = "qcom,msm-imem", },
	{},
};
MODULE_DEVICE_TABLE(of, of_nothing_restart_info_match);

static struct platform_driver nothing_restart_info_driver = {
	.probe = nothing_restart_info_probe,
	.remove = nothing_restart_info_remove,
	.driver = {
		.name = "nothing-restart-info",
		.of_match_table = of_nothing_restart_info_match,
	},
};

static int __init nothing_restart_info_driver_init(void)
{
	pr_warn("nothing_restart_info_driver_init\n");

	return platform_driver_register(&nothing_restart_info_driver);
}

module_init(nothing_restart_info_driver_init);

static void __exit nothing_restart_info_driver_exit(void)
{
	return platform_driver_unregister(&nothing_restart_info_driver);
}
module_exit(nothing_restart_info_driver_exit);

MODULE_DESCRIPTION("Nothing Restart Info Driver");
MODULE_LICENSE("GPL v2");
