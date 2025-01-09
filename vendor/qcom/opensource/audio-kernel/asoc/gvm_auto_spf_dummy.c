// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2014-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/of_device.h>
#include <linux/pm_qos.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/info.h>
#include <dsp/audio_notifier.h>
#include "msm_dailink.h"
#include <soc/qcom/boot_stats.h>
#include "msm_common.h"
#include <linux/cdev.h>
#include <linux/err.h>

#define DRV_NAME "spf-asoc-snd"

#define __CHIPSET__ "SA8xx5 "
#define MSM_DAILINK_NAME(name) (__CHIPSET__#name)

#define DEV_NAME_STR_LEN            32

#define SAMPLING_RATE_8KHZ      8000
#define SAMPLING_RATE_11P025KHZ 11025
#define SAMPLING_RATE_16KHZ     16000
#define SAMPLING_RATE_22P05KHZ  22050
#define SAMPLING_RATE_32KHZ     32000
#define SAMPLING_RATE_44P1KHZ   44100
#define SAMPLING_RATE_48KHZ     48000
#define SAMPLING_RATE_88P2KHZ   88200
#define SAMPLING_RATE_96KHZ     96000
#define SAMPLING_RATE_176P4KHZ  176400
#define SAMPLING_RATE_192KHZ    192000
#define SAMPLING_RATE_352P8KHZ  352800
#define SAMPLING_RATE_384KHZ    384000

#define WCD9XXX_MBHC_DEF_RLOADS     5

#define WSA8810_NAME_1 "wsa881x.20170211"
#define WSA8810_NAME_2 "wsa881x.20170212"
#define WCN_CDC_SLIM_RX_CH_MAX 2
#define WCN_CDC_SLIM_TX_CH_MAX 3
#define TDM_CHANNEL_MAX 8

#define ADSP_STATE_READY_TIMEOUT_MS 3000
#define MSM_LL_QOS_VALUE 300 /* time in us to ensure LPM doesn't go in C3/C4 */
#define MSM_HIFI_ON 1
#define DIR_SZ 10

#define AUTO_VIRT_SNDCARD_ONLINE 0
#define AUTO_VIRT_SNDCARD_OFFLINE 1

struct snd_card_pdata {
	struct kobject snd_card_kobj;
	int card_status;
	} *snd_card_pdata;

static struct attribute card_state_attr = {
	.name = "card_state",
	.mode = 0666,
};

enum {
	SLIM_RX_0 = 0,
	SLIM_RX_1,
	SLIM_RX_2,
	SLIM_RX_3,
	SLIM_RX_4,
	SLIM_RX_5,
	SLIM_RX_6,
	SLIM_RX_7,
	SLIM_RX_MAX,
};
enum {
	SLIM_TX_0 = 0,
	SLIM_TX_1,
	SLIM_TX_2,
	SLIM_TX_3,
	SLIM_TX_4,
	SLIM_TX_5,
	SLIM_TX_6,
	SLIM_TX_7,
	SLIM_TX_8,
	SLIM_TX_MAX,
};

enum {
	PRIM_MI2S = 0,
	SEC_MI2S,
	TERT_MI2S,
	QUAT_MI2S,
	QUIN_MI2S,
	MI2S_MAX,
};

enum {
	PRIM_AUX_PCM = 0,
	SEC_AUX_PCM,
	TERT_AUX_PCM,
	QUAT_AUX_PCM,
	QUIN_AUX_PCM,
	AUX_PCM_MAX,
};

struct mi2s_conf {
	struct mutex lock;
	u32 ref_cnt;
	u32 msm_is_mi2s_master;
};

struct dev_config {
	u32 sample_rate;
	u32 bit_format;
	u32 channels;
};

enum {
	DP_RX_IDX = 0,
	EXT_DISP_RX_IDX_MAX,
};

struct msm_wsa881x_dev_info {
	struct device_node *of_node;
	u32 index;
};

enum pinctrl_pin_state {
	STATE_DISABLE = 0, /* All pins are in sleep state */
	STATE_MI2S_ACTIVE,  /* IS2 = active, TDM = sleep */
	STATE_TDM_ACTIVE,  /* IS2 = sleep, TDM = active */
};

struct msm_pinctrl_info {
	struct pinctrl *pinctrl;
	struct pinctrl_state *mi2s_disable;
	struct pinctrl_state *tdm_disable;
	struct pinctrl_state *mi2s_active;
	struct pinctrl_state *tdm_active;
	enum pinctrl_pin_state curr_state;
};

enum {
	TDM_0 = 0,
	TDM_1,
	TDM_2,
	TDM_3,
	TDM_4,
	TDM_5,
	TDM_6,
	TDM_7,
	TDM_PORT_MAX,
};

enum {
	TDM_PRI = 0,
	TDM_SEC,
	TDM_TERT,
	TDM_QUAT,
	TDM_QUIN,
	TDM_INTERFACE_MAX,
};

struct tdm_port {
	u32 mode;
	u32 channel;
};

struct tdm_conf {
	struct mutex lock;
	u32 ref_cnt;
};

struct msm_asoc_mach_data {
	struct snd_info_entry *codec_root;
	struct msm_common_pdata *common_pdata;
	struct msm_pinctrl_info pinctrl_info;
	struct device_node *us_euro_gpio_p; /* used by pinctrl API */
	struct device_node *hph_en1_gpio_p; /* used by pinctrl API */
	struct device_node *hph_en0_gpio_p; /* used by pinctrl API */
	struct device_node *fsa_handle;
	struct snd_soc_codec *codec;
	struct work_struct adsp_power_up_work;
	struct tdm_conf tdm_intf_conf[TDM_INTERFACE_MAX];
	struct snd_pcm_hardware hw_params;
};

static const char *const pin_states[] = {"sleep", "i2s-active",
					 "tdm-active"};

static struct platform_device *spdev;

static bool codec_reg_done;

/*
 * Need to report LINEIN
 * if R/L channel impedance is larger than 5K ohm
 */

static const struct snd_soc_dapm_widget msm_dapm_widgets[] = {

	SND_SOC_DAPM_MIC("Handset Mic", NULL),
	SND_SOC_DAPM_MIC("Analog Mic3", NULL),
	SND_SOC_DAPM_MIC("Analog Mic4", NULL),

	SND_SOC_DAPM_MIC("Digital Mic0", NULL),
	SND_SOC_DAPM_MIC("Digital Mic1", NULL),
	SND_SOC_DAPM_MIC("Digital Mic2", NULL),
	SND_SOC_DAPM_MIC("Digital Mic3", NULL),
	SND_SOC_DAPM_MIC("Digital Mic4", NULL),
	SND_SOC_DAPM_MIC("Digital Mic5", NULL),
	SND_SOC_DAPM_MIC("Digital Mic6", NULL),
	SND_SOC_DAPM_MIC("Digital Mic7", NULL),
};

static int tdm_snd_startup(struct snd_pcm_substream *substream)
{
	int ret = 0;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_card *card = rtd->card;
	struct snd_soc_dai_link *dai_link = rtd->dai_link;
	struct msm_asoc_mach_data *pdata = snd_soc_card_get_drvdata(card);

	if (!dai_link->no_pcm)
		ret = snd_soc_set_runtime_hwparams(substream, &pdata->hw_params);

	return ret;
}

static const struct snd_soc_ops tdm_be_ops = {
	.startup = tdm_snd_startup,
};

static struct snd_soc_dai_link msm_gvm_auto_dai_links[] = {
	/* BackEnd DAI Links */
	{
	.name = "PRI_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(primary_tdm_rx_0_dummy),
	},
	{
	.name = "PRI_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(primary_tdm_tx_0_dummy),
	},
	{
	.name = "SEC_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF-RX-SECONDARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(secondary_tdm_rx_0_dummy),
	},
	{
	.name = "SEC_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF-TX-SECONDARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(secondary_tdm_tx_0_dummy),
	},
	{
	.name = "TERT_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF-RX-TERTIARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(tert_tdm_rx_0_dummy),
	},
	{
	.name = "TERT_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF-TX-TERTIARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(tert_tdm_tx_0_dummy),
	},
	{
	.name = "QUAT_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_RXTX-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(quat_tdm_rx_0_dummy),
	},
	{
	.name = "QUAT_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_RXTX-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(quat_tdm_tx_0_dummy),
	},
	{
	.name = "QUIN_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_VA-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(quin_tdm_rx_0_dummy),
	},
	{
	.name = "QUIN_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_VA-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(quin_tdm_tx_0_dummy),
	},
	{
	.name = "SEN_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_WSA-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(sen_tdm_rx_0_dummy),
	},
	{
	.name = "SEN_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_WSA-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(sen_tdm_tx_0_dummy),
	},
	{
	.name = "SEP_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_AUD-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(sep_tdm_rx_0_dummy),
	},
	{
	.name = "SEP_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_AUD-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(sep_tdm_tx_0_dummy),
	},
	{
	.name = "OCT_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_WSA2-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(oct_tdm_rx_0_dummy),
	},
	{
	.name = "OCT_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_WSA2-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(oct_tdm_tx_0_dummy),
	},
	{
	.name = "HS_IF0_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-RX-PRIMARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if0_tdm_rx_0_dummy),
	},
	{
	.name = "HS_IF0_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-TX-PRIMARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if0_tdm_tx_0_dummy),
	},
	{
	.name = "HS_IF1_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-RX-SECONDARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if1_tdm_rx_0_dummy),
	},
	{
	.name = "HS_IF1_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-TX-SECONDARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if1_tdm_tx_0_dummy),
	},
	{
	.name = "HS_IF2_TDM_RX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-RX-TERTIARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if2_tdm_rx_0_dummy),
	},
	{
	.name = "HS_IF2_TDM_TX_0_DUMMY",
	.stream_name = "TDM-LPAIF_SDR-TX-TERTIARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if2_tdm_tx_0_dummy),
	},
	{
	.name = "HS_IF3_TDM_RX_0",
	.stream_name = "TDM-LPAIF_SDR-RX-QUATERNARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if3_tdm_rx_0_dummy),
	},
	{
	.name = "HS_IF3_TDM_TX_0",
	.stream_name = "TDM-LPAIF_SDR-TX-QUATERNARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if3_tdm_tx_0_dummy),
	},
	{
	.name = "HS_IF4_TDM_RX_0",
	.stream_name = "TDM-LPAIF_SDR-RX-QUINARY",
	.dpcm_playback = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if4_tdm_rx_0_dummy),
	},
	{
	.name = "HS_IF4_TDM_TX_0",
	.stream_name = "TDM-LPAIF_SDR-TX-QUINARY",
	.dpcm_capture = 1,
	.ops = &tdm_be_ops,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(hs_if4_tdm_tx_0_dummy),
	},
	{
	.name = "PCM_DUMMY-TX-0",
	.stream_name = "PCM_DUMMY-TX-0",
	.dpcm_capture = 1,
	.trigger = {SND_SOC_DPCM_TRIGGER_POST,
				SND_SOC_DPCM_TRIGGER_POST},
	.ignore_suspend = 1,
	.ignore_pmdown_time = 1,
	SND_SOC_DAILINK_REG(pcm_dummy_tx0),
	},
};

int snd_card_notify_user(snd_card_status_t card_status)
{
	snd_card_pdata->card_status = card_status;
	sysfs_notify(&snd_card_pdata->snd_card_kobj, NULL, "card_state");
	return 0;
}

int snd_card_set_card_status(snd_card_status_t card_status)
{
	snd_card_pdata->card_status = card_status;
	return 0;
}

static ssize_t snd_card_sysfs_show(struct kobject *kobj,
		struct attribute *attr, char *buf)
{
	/* Max length of buf is PAGE_SIZE */
	return sysfs_emit(buf, "%d\n", snd_card_pdata->card_status);
}

static ssize_t snd_card_sysfs_store(struct kobject *kobj,
		struct attribute *attr, const char *buf, size_t count)
{
	int result;

	result = kstrtoint (buf, 0, &snd_card_pdata->card_status);
	if (result)
		pr_err("%s: Failed to read buf result=%d\n", __func__, result);

	sysfs_notify(kobj, NULL, "card_state");
	return 0;
}

static const struct sysfs_ops snd_card_sysfs_ops = {
	.show = snd_card_sysfs_show,
	.store = snd_card_sysfs_store,
};

static void snd_card_sysfs_release(struct kobject *kobj)
{
	kfree(snd_card_pdata); /* Free the memory */
}

static struct kobj_type snd_card_ktype = {
	.release = snd_card_sysfs_release,
	.sysfs_ops = &snd_card_sysfs_ops,
};

int snd_card_sysfs_init(void)
{
	int ret = 0;
	char dir[DIR_SZ] = "snd_card";

	snd_card_pdata = kcalloc(1, sizeof(struct snd_card_pdata), GFP_KERNEL);
	if (!snd_card_pdata)
		return -ENOMEM;

	/* kernel_kobj is the kobject of /sys/kernel/ */
	ret = kobject_init_and_add(&snd_card_pdata->snd_card_kobj, &snd_card_ktype,
		kernel_kobj, dir);

	if (ret < 0) {
		pr_err("%s: Failed to init and add kobject %s, err = %d\n",
			__func__, dir, ret);
		goto fail;
	}

	ret = sysfs_create_file(&snd_card_pdata->snd_card_kobj, &card_state_attr);
	if (ret < 0) {
		pr_err("%s: Failed to add snd_card sysfs entry to %s\n",
			__func__, dir);
		goto fail;
	}

	return ret;

fail:
	kobject_put(&snd_card_pdata->snd_card_kobj);
	return ret;
}

struct snd_soc_card snd_soc_card_gvm_auto_dummy_msm;

static int msm_populate_dai_link_component_of_node(
					struct snd_soc_card *card)
{
	int i, j, index, ret = 0;
	struct device *cdev = card->dev;
	struct snd_soc_dai_link *dai_link = card->dai_link;
	struct device_node *np;

	if (!cdev) {
		pr_err("%s: Sound card device memory NULL\n", __func__);
		return -ENODEV;
	}

	for (i = 0; i < card->num_links; i++) {
		if (dai_link[i].platforms->of_node && dai_link[i].cpus->of_node)
			continue;

		/* populate cpu_of_node for snd card dai links */
		if (dai_link[i].cpus->dai_name && !dai_link[i].cpus->of_node) {
			index = of_property_match_string(cdev->of_node,
						 "asoc-cpu-names",
						 dai_link[i].cpus->dai_name);
			pr_err("%s: retrieving cpu_of_node for %s\n",
						__func__,
						dai_link[i].cpus->dai_name);
			if (index >= 0) {
				np = of_parse_phandle(cdev->of_node, "asoc-cpu",
						index);
				if (!np) {
					pr_err("%s: retrieving phandle for cpu dai %s failed\n",
						__func__,
						dai_link[i].cpus->dai_name);
					ret = -ENODEV;
					goto err;
				}
				dai_link[i].cpus->of_node = np;
				dai_link[i].cpus->dai_name = NULL;
			}
		}

		/* populate codec_of_node for snd card dai links */
		if (dai_link[i].num_codecs > 0) {
			for (j = 0; j < dai_link[i].num_codecs; j++) {
				if (dai_link[i].codecs[j].of_node ||
						!dai_link[i].codecs[j].name)
					continue;

				index = of_property_match_string(cdev->of_node,
						"asoc-codec-names",
						dai_link[i].codecs[j].name);
				if (index < 0)
					continue;
				np = of_parse_phandle(cdev->of_node,
						      "asoc-codec",
						      index);
				if (!np) {
					pr_err("%s: retrieving phandle for codec %s failed\n",
						__func__, dai_link[i].codecs[j].name);
					ret = -ENODEV;
					goto err;
				}
				dai_link[i].codecs[j].of_node = np;
				dai_link[i].codecs[j].name = NULL;
			}
		}
	}

err:
	return ret;
}

static const struct of_device_id gvm_asoc_machine_of_match[]  = {
	{ .compatible = "qcom,8155-spf-asoc-snd-adp-star",
		.data = "adp_star_codec"},
	{ .compatible = "qcom,6155-spf-asoc-snd-adp-star",
		.data = "adp_star_codec"},
	{ .compatible = "qcom,gvm-auto-spf-asoc-snd-adp-star",
		.data = "adp_star_codec"},
	{},
};

static struct snd_soc_card *populate_snd_card_dailinks(struct device *dev)
{
	struct snd_soc_card *card = NULL;
	const struct of_device_id *match = NULL;

	match = of_match_node(gvm_asoc_machine_of_match, dev->of_node);
	if (!match) {
		dev_err(dev, "%s: No DT match found for sound card\n",
			__func__);
		return NULL;
	}

	card = &snd_soc_card_gvm_auto_dummy_msm;

	if (!strcmp(match->compatible, "qcom,gvm-auto-spf-asoc-snd-adp-star")) {
		card->dai_link = msm_gvm_auto_dai_links;
		card->num_links = ARRAY_SIZE(msm_gvm_auto_dai_links);
	}

	return card;
}

struct msm_common_pdata *msm_common_get_pdata(struct snd_soc_card *card)
{
	struct msm_asoc_mach_data *pdata = snd_soc_card_get_drvdata(card);

	if (!pdata)
		return NULL;

	return pdata->common_pdata;
}

void msm_common_set_pdata(struct snd_soc_card *card,
			  struct msm_common_pdata *common_pdata)
{
	struct msm_asoc_mach_data *pdata = snd_soc_card_get_drvdata(card);

	if (!pdata)
		return;

	pdata->common_pdata = common_pdata;
}

static long virt_sndcard_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
	case AUTO_VIRT_SNDCARD_OFFLINE:
		snd_card_notify_user(SND_CARD_STATUS_OFFLINE);
		pr_debug("%s: mark sndcard offline\n", __func__);
	break;
	case AUTO_VIRT_SNDCARD_ONLINE:
		snd_card_notify_user(SND_CARD_STATUS_ONLINE);
		pr_debug("%s: mark sndcard online\n", __func__);
	break;
	default:
		pr_err("%s: Invalid command = %d\n", __func__, cmd);
		ret = -EFAULT;
	break;
	}

	return ret;
}

static const struct file_operations virt_sndcard_ctl_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = virt_sndcard_ioctl,
};

static struct cdev virt_sndcard_ctl = {
	.ops  = &virt_sndcard_ctl_fops,
};

int msm_audio_ssr_register(struct cdev *virt_sndcard_ctl)
{
	static struct class *dev_class;
	dev_t dev;

	if ((alloc_chrdev_region(&dev, 0, 1, "virt_sndcard_ctl")) < 0) {
		pr_err("%s: Cannot allocate major number\n", __func__);
		return -EINVAL;
	}
	pr_debug("Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(virt_sndcard_ctl, &virt_sndcard_ctl_fops);
	if ((cdev_add(virt_sndcard_ctl, dev, 1)) < 0) {
		pr_err("%s: Cannot add the device to the system\n", __func__);
		goto err;
	}
	dev_class = class_create(THIS_MODULE, "SSR");
	if (IS_ERR(dev_class)) {
		pr_err("%s: Cannot create the struct class\n", __func__);
		goto err;
	}
	if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "virt_sndcard_ctl"))) {
		pr_err("%s: Cannot create the Device\n", __func__);
		goto fail;
	}
	return 0;

fail:
	class_destroy(dev_class);
err:
	unregister_chrdev_region(dev, 1);
	return -EINVAL;
}

static int msm_get_hwparams(struct platform_device *pdev)
{
	struct snd_soc_card *card = NULL;
	struct msm_asoc_mach_data *pdata = NULL;
	u32 pcm_info = 0;
	u32 buffer_bytes_max = 0;
	u32 periods_bytes[2] = {0};
	u32 periods_count[2] = {0};
	int ret = 0;

	card = platform_get_drvdata(pdev);
	if (!card) {
		pr_err("%s: card is NULL\n",
			__func__);
		return -EINVAL;
	}
	pdata = snd_soc_card_get_drvdata(card);
	if (!pdata) {
		pr_err("%s: pdata is NULL\n",
			__func__);
		return -EINVAL;
	}

	ret = of_property_read_u32(pdev->dev.of_node,
			"qcom,hw_pcm_info",
			&pcm_info);
	if (ret) {
		pr_err("%s: read pcm info failed\n",
			__func__);
		return ret;
	}

	ret = of_property_read_u32(pdev->dev.of_node,
			"qcom,hw_buffer_size_max",
			&buffer_bytes_max);
	if (ret) {
		pr_err("%s: read buffer size max failed\n",
			__func__);
		return ret;
	}

	ret = of_property_read_u32_array(pdev->dev.of_node,
			"qcom,hw_period_byte_size",
			periods_bytes, 2);
	if (ret) {
		pr_err("%s: read period byte size failed\n",
			__func__);
		return ret;
	}

	ret = of_property_read_u32_array(pdev->dev.of_node,
			"qcom,hw_period_count_size",
			periods_count, 2);
	if (ret) {
		pr_err("%s: read period count size failed\n",
			__func__);
		return ret;
	}

	pdata->hw_params.info = pcm_info;
	pdata->hw_params.buffer_bytes_max = buffer_bytes_max;
	pdata->hw_params.period_bytes_min = periods_bytes[0];
	pdata->hw_params.period_bytes_max = periods_bytes[1];
	pdata->hw_params.periods_min = periods_count[0];
	pdata->hw_params.periods_max = periods_count[1];

	return ret;
}

static int msm_asoc_machine_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct msm_asoc_mach_data *pdata;
	int ret;

	dev_info(&pdev->dev, "M - DRIVER Audio Init\n");

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "No platform supplied from device tree\n");
		return -EINVAL;
	}

	pdata = devm_kzalloc(&pdev->dev,
			sizeof(struct msm_asoc_mach_data), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	card = populate_snd_card_dailinks(&pdev->dev);
	if (!card) {
		dev_err(&pdev->dev, "%s: Card uninitialized\n", __func__);
		ret = -EINVAL;
		return ret;
	}
	card->dev = &pdev->dev;
	platform_set_drvdata(pdev, card);
	snd_soc_card_set_drvdata(card, pdata);

	ret = snd_soc_of_parse_card_name(card, "qcom,model");
	if (ret) {
		dev_err(&pdev->dev, "parse card name failed, err:%d\n",
			ret);
		return ret;
	}

	ret = msm_populate_dai_link_component_of_node(card);
	if (ret) {
		ret = -EPROBE_DEFER;
		return ret;
	}

	ret = devm_snd_soc_register_card(&pdev->dev, card);

	if (ret == -EPROBE_DEFER) {
		if (codec_reg_done)
			ret = -EINVAL;
		return ret;
	} else if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n",
			ret);
		pr_err("snd_soc_register_card failed (%d)\n",
			ret);
		return ret;
	}
	dev_info(&pdev->dev, "Sound card %s registered\n", card->name);
	pr_err("Sound card %s registered\n", card->name);

	dev_info(&pdev->dev, "M - DRIVER Audio Ready\n");

	ret = msm_get_hwparams(pdev);
	if (ret) {
		dev_err(&pdev->dev,
			"%s: hwparams get failed with %d\n",
			__func__, ret);
		return ret;
	}

	spdev = pdev;
	ret = msm_audio_ssr_register(&virt_sndcard_ctl);
	if (ret)
		pr_err("%s: Audio virtual sndcard ctrl register fail, ret=%d\n", __func__, ret);
	dev_info(&pdev->dev, "Audio virtual sndcard ctrl register complete\n");

	ret = snd_card_sysfs_init();
	if (ret)
		pr_err("snd_card_sysfs_init fail, ret=%d\n", ret);

	ret = snd_card_set_card_status(SND_CARD_STATUS_ONLINE);
	if (ret)
		pr_err("snd_card_set_card_status fail, ret=%d\n", ret);

	return 0;
}

static int msm_asoc_machine_remove(struct platform_device *pdev)
{
	/* kobject_put decrease the kref count, once the count reaches 0.
	 * Kobject core will automatically clean up the memory allocated by kobject.
	 * The snd_card_sysfs_release release will help clean up memory allocated by us
	 */
	kobject_put(&snd_card_pdata->snd_card_kobj);
	return 0;
}

static struct platform_driver gvm_asoc_machine_driver = {
	.driver = {
		.name = DRV_NAME,
		.pm = &snd_soc_pm_ops,
		.of_match_table = gvm_asoc_machine_of_match,
	},
	.probe = msm_asoc_machine_probe,
	.remove = msm_asoc_machine_remove,
};

int __init gvm_auto_spf_init(void)
{
	return platform_driver_register(&gvm_asoc_machine_driver);
}

void gvm_auto_spf_exit(void)
{
	platform_driver_unregister(&gvm_asoc_machine_driver);
}

module_init(gvm_auto_spf_init);
module_exit(gvm_auto_spf_exit);

MODULE_DESCRIPTION("ALSA SoC Machine Driver for SPF");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DEVICE_TABLE(of, gvm_asoc_machine_of_match);
