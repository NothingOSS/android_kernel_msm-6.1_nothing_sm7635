/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _WCD9378_INTERNAL_H
#define _WCD9378_INTERNAL_H

#include <asoc/wcd-mbhc-v2.h>
#include <asoc/wcd-irq.h>
#include <asoc/wcd-clsh.h>
#include <soc/soundwire.h>
#include "wcd9378-mbhc.h"
#include "wcd9378.h"

#define SWR_SCP_CONTROL    0x44
#define SWR_SCP_HOST_CLK_DIV2_CTL_BANK 0xE0
#define WCD9378_MAX_MICBIAS 3
#define SIM_MIC_NUM	3


/* Convert from vout ctl to micbias voltage in mV */
#define  WCD_VOUT_CTL_TO_MICB(v)  (1000 + v * 50)
#define MAX_PORT 8
#define MAX_CH_PER_PORT 8
#define TX_ADC_MAX 3
#define SWR_NUM_PORTS	4

enum {
	TX_HDR12 = 0,
	TX_HDR34,
	TX_HDR_MAX,
};

enum {
	SIM_MIC0,
	SIM_MIC1,
	SIM_MIC2,
	SIM_JACK,
	MICB_VAL_NUM,
};

extern struct regmap_config wcd9378_regmap_config;

struct codec_port_info {
	u32 slave_port_type;
	u32 master_port_type;
	u32 ch_mask;
	u32 num_ch;
	u32 ch_rate;
};

enum {
	RX_CLK_9P6MHZ,
	RX_CLK_12P288MHZ,
	RX_CLK_11P2896MHZ,
};

enum {
	RX_PATH,
	TX_PATH,
};

struct wcd9378_priv {
	struct device *dev;
	u32 sys_usage;
	/* to track the sys_usage status */
	unsigned long sys_usage_status;
	u32 wcd_mode;

	int variant;
	struct snd_soc_component *component;
	struct device_node *rst_np;
	struct regmap *regmap;

	struct swr_device *rx_swr_dev;
	struct swr_device *tx_swr_dev;

	s32 micb_ref[WCD9378_MAX_MICBIAS];
	s32 pullup_ref[WCD9378_MAX_MICBIAS];

	u32 micb_sel[SIM_MIC_NUM];
	u32 micb_val[MICB_VAL_NUM];

	struct device_node *wcd_rst_np;

	struct mutex micb_lock;
	struct mutex wakeup_lock;
	struct mutex sys_usage_lock;
	s32 dmic_0_1_clk_cnt;
	s32 dmic_2_3_clk_cnt;
	s32 dmic_4_5_clk_cnt;
	int hdr_en[TX_HDR_MAX];
	/* class h specific info */
	struct wcd_clsh_cdc_info clsh_info;
	/* mbhc module */
	struct wcd9378_mbhc *mbhc;

	u32 hph_mode;
	u16 hph_gain;
	u32 curr_micbias2;
	u32 rx2_clk_mode;
	u32 tx_mode[TX_ADC_MAX];
	s32 adc_count;
	bool comp1_enable;
	bool comp2_enable;
	bool ldoh;
	bool bcs_dis;
	bool dapm_bias_off;
	struct irq_domain *virq;
	struct wcd_irq_info irq_info;
	u32 rx_clk_cnt;
	int num_irq_regs;
	/* to track the status */
	unsigned long status_mask;

	u8 num_tx_ports;
	u8 num_rx_ports;
	struct codec_port_info
			tx_port_mapping[MAX_PORT][MAX_CH_PER_PORT];
	struct codec_port_info
			rx_port_mapping[MAX_PORT][MAX_CH_PER_PORT];
	struct swr_port_params tx_port_params[SWR_UC_MAX][SWR_NUM_PORTS];
	struct swr_dev_frame_config swr_tx_port_params[SWR_UC_MAX];
	struct regulator_bulk_data *supplies;
	struct notifier_block nblock;
	/* wcd callback to bolero */
	void *handle;
	int (*update_wcd_event)(void *handle, u16 event, u32 data);
	int (*register_notifier)(void *handle,
				struct notifier_block *nblock,
				bool enable);
	int (*wakeup)(void *handle, bool enable);
	u32 version;
	/* Entry for version info */
	struct snd_info_entry *entry;
	struct snd_info_entry *version_entry;
	struct snd_info_entry *variant_entry;
	int flyback_cur_det_disable;
	u8 tx_master_ch_map[WCD9378_MAX_SLAVE_CH_TYPES];
	bool usbc_hs_status;
	/* wcd to swr dmic notification */
	bool notify_swr_dmic;
	u8 rx_swrclk;
	u8 rx_clkscale;
	u8 tx_swrclk;
	u8 tx_clkscale;
	struct blocking_notifier_head notifier;
};

struct wcd9378_micbias_setting {
	u8 ldoh_v;
	u32 cfilt1_mv;
	u32 micb1_mv;
	u32 micb2_mv;
	u32 micb3_mv;
	u32 micb1_usage_val;
	u32 micb2_usage_val;
	u32 micb3_usage_val;

	u8 bias1_cfilt_sel;
};

struct wcd9378_pdata {
	struct device_node *rst_np;
	struct device_node *rx_slave;
	struct device_node *tx_slave;
	struct wcd9378_micbias_setting micbias;

	struct cdc_regulator *regulator;
	int num_supplies;
};

struct wcd_ctrl_platform_data {
	void *handle;
	int (*update_wcd_event)(void *handle, u16 event, u32 data);
	int (*register_notifier)(void *handle,
				 struct notifier_block *nblock,
				 bool enable);
};

enum {
	WCD_RX1,
	WCD_RX2,
	WCD_RX3
};

enum {
	/* INTR_CTRL_INT_MASK_0 */
	WCD9378_IRQ_MBHC_BUTTON_PRESS_DET = 0,
	WCD9378_IRQ_MBHC_BUTTON_RELEASE_DET,
	WCD9378_IRQ_MBHC_ELECT_INS_REM_DET,
	WCD9378_IRQ_MBHC_ELECT_INS_REM_LEG_DET,
	WCD9378_IRQ_MBHC_SW_DET,
	WCD9378_IRQ_HPHR_OCP_INT,
	WCD9378_IRQ_HPHR_CNP_INT,
	WCD9378_IRQ_HPHL_OCP_INT,

	/* INTR_CTRL_INT_MASK_1 */
	WCD9378_IRQ_HPHL_CNP_INT,
	WCD9378_IRQ_EAR_CNP_INT,
	WCD9378_IRQ_EAR_SCD_INT,
	WCD9378_IRQ_AUX_CNP_INT,
	WCD9378_IRQ_AUX_SCD_INT,
	WCD9378_IRQ_HPHL_PDM_WD_INT,
	WCD9378_IRQ_HPHR_PDM_WD_INT,
	WCD9378_IRQ_AUX_PDM_WD_INT,

	/* INTR_CTRL_INT_MASK_2 */
	WCD9378_IRQ_LDORT_SCD_INT,
	WCD9378_IRQ_MBHC_MOISTURE_INT,
	WCD9378_IRQ_HPHL_SURGE_DET_INT,
	WCD9378_IRQ_HPHR_SURGE_DET_INT,
	WCD9378_IRQ_SAPU_PROT_MODE_CHG,
	WCD9378_NUM_IRQS,
};

extern struct wcd9378_mbhc *wcd9378_soc_get_mbhc(
				struct snd_soc_component *component);
extern void wcd9378_disable_bcs_before_slow_insert(
				struct snd_soc_component *component,
				bool bcs_disable);
extern int wcd9378_mbhc_micb_adjust_voltage(struct snd_soc_component *component,
					int volt, int micb_num);
extern int wcd9378_get_micb_vout_ctl_val(u32 micb_mv);
extern int wcd9378_micbias_control(struct snd_soc_component *component,
				int micb_num, int req, bool is_dapm);
#endif /* _WCD9378_INTERNAL_H */
