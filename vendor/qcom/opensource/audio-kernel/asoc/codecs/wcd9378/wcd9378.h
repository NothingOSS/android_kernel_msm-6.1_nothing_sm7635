/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _WCD9378_H
#define _WCD9378_H

#include <bindings/audio-codec-port-types.h>
#include <sound/info.h>
#include <linux/component.h>

#define WCD9378_MAX_SLAVE_CH_TYPES 13
#define ZERO 0
#define WCD9378_DRV_NAME "wcd9378_codec"

/* from WCD to SWR DMIC events */
enum {
	WCD9378_EVT_SSR_DOWN,
	WCD9378_EVT_SSR_UP,
};

struct wcd9378_swr_slave_ch_map {
	u8 ch_type;
	u8 index;
};

static const struct wcd9378_swr_slave_ch_map wcd9378_swr_slv_tx_ch_idx[] = {
	{ADC1, 0},
	{ADC2, 1},
	{ADC3, 2},
	{ADC4, 3},
	{DMIC0, 4},
	{DMIC1, 5},
	{MBHC, 6},
	{DMIC2, 6},
	{DMIC3, 7},
	{DMIC4, 8},
	{DMIC5, 9},
	{DMIC6, 10},
	{DMIC7, 11},
};

static int wcd9378_swr_master_ch_map[] = {
	ZERO,
	SWRM_TX1_CH1,
	SWRM_TX1_CH2,
	SWRM_TX1_CH3,
	SWRM_TX1_CH4,
	SWRM_TX2_CH1,
	SWRM_TX2_CH2,
	SWRM_TX2_CH3,
	SWRM_TX2_CH4,
	SWRM_TX3_CH1,
	SWRM_TX3_CH2,
	SWRM_TX3_CH3,
	SWRM_TX3_CH4,
	SWRM_TX_PCM_IN,
};

#if IS_ENABLED(CONFIG_SND_SOC_WCD9378)
int wcd9378_info_create_codec_entry(struct snd_info_entry *codec_root,
				struct snd_soc_component *component);

int wcd9378_swr_dmic_register_notifier(struct snd_soc_component *wcd9378,
					struct notifier_block *nblock,
					bool enable);

int wcd9378_codec_get_dev_num(struct snd_soc_component *component);

static inline int wcd9378_slave_get_master_ch_val(int ch)
{
	int i;

	for (i = 0; i < WCD9378_MAX_SLAVE_CH_TYPES; i++)
		if (ch == wcd9378_swr_master_ch_map[i])
			return i;
	return 0;
}

static inline int wcd9378_slave_get_master_ch(int idx)
{
	return wcd9378_swr_master_ch_map[idx];
}

static inline int wcd9378_slave_get_slave_ch_val(int ch)
{
	int i;

	for (i = 0; i < WCD9378_MAX_SLAVE_CH_TYPES; i++)
		if (ch == wcd9378_swr_slv_tx_ch_idx[i].ch_type)
			return wcd9378_swr_slv_tx_ch_idx[i].index;

	return -EINVAL;
}
#else
static inline int wcd9378_info_create_codec_entry(
					struct snd_info_entry *codec_root,
					struct snd_soc_component *component)
{
	return 0;
}

static inline int wcd9378_slave_get_master_ch_val(int ch)
{
	return 0;
}
static inline int wcd9378_slave_get_master_ch(int idx)
{
	return 0;
}
static inline int wcd9378_slave_get_slave_ch_val(int ch)
{
	return 0;
}
static int wcd9378_codec_get_dev_num(struct snd_soc_component *component)
{
	return 0;
}
#endif /* CONFIG_SND_SOC_WCD9378 */
#endif /* _WCD9378_H */

