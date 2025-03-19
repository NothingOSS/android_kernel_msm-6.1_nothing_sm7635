// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <sound/soc.h>
#include <sound/tlv.h>
#include <soc/soundwire.h>
#include <linux/regmap.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <asoc/msm-cdc-pinctrl.h>
#include <asoc/msm-cdc-supply.h>
#include <bindings/audio-codec-port-types.h>
#include <linux/qti-regmap-debugfs.h>

#include "wcd9378-reg-masks.h"
#include "wcd9378.h"
#include "internal.h"
#include "asoc/bolero-slave-internal.h"
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define NUM_SWRS_DT_PARAMS 5

#define WCD9378_MOBILE_MODE 0x01

#define WCD9378_VERSION_1_0 1
#define WCD9378_VERSION_ENTRY_SIZE 32

#define SWR_BASECLK_19P2MHZ      (0x01)
#define SWR_BASECLK_24P576MHZ    (0x03)
#define SWR_BASECLK_22P5792MHZ   (0x04)

#define SWR_CLKSCALE_DIV2        (0x02)
#define SWR_CLKSCALE_DIV4        (0x03)

#define ADC_MODE_VAL_HIFI     0x01
#define ADC_MODE_VAL_NORMAL   0x03
#define ADC_MODE_VAL_LP       0x05

#define PWR_LEVEL_LOHIFI_VAL  0x00
#define PWR_LEVEL_LP_VAL      0x01
#define PWR_LEVEL_HIFI_VAL    0x02
#define PWR_LEVEL_ULP_VAL     0x03

#define MICB_USAGE_VAL_DISABLE    0x00
#define MICB_USAGE_VAL_PULL_DOWN    0x01
#define MICB_USAGE_VAL_1P2V    0x02
#define MICB_USAGE_VAL_1P8VORPULLUP    0x03
#define MICB_USAGE_VAL_2P5V    0x04
#define MICB_USAGE_VAL_2P75V    0x05

#define MICB_USAGE_VAL_2P2V    0xF0
#define MICB_USAGE_VAL_2P7V    0xF1
#define MICB_USAGE_VAL_2P8V    0xF2

#define MICB_USAGE_VAL_MICB1_TABLE_VAL    0xF3
#define MICB_USAGE_VAL_MICB2_TABLE_VAL    0xF4
#define MICB_USAGE_VAL_MICB3_TABLE_VAL    0xF5

#define WCD_TX_SYS_USAGE_BIT_MASK    (0xFC)
#define WCD_RX_SYS_USAGE_BIT_MASK    (0x1F00)

#define MICB_NUM_MAX     3

#define NUM_ATTEMPTS 20

#define WCD9378_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |\
			SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_48000 |\
			SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_192000 |\
			SNDRV_PCM_RATE_384000)
/* Fractional Rates */
#define WCD9378_FRAC_RATES (SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_88200 |\
				SNDRV_PCM_RATE_176400 | SNDRV_PCM_RATE_352800)

#define WCD9378_FORMATS (SNDRV_PCM_FMTBIT_S16_LE |\
		SNDRV_PCM_FMTBIT_S24_LE |\
		SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S32_LE)

#define WCD9378_EAR_PA_GAIN_TLV(xname, reg, shift, max, invert, tlv_array) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
		 SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.tlv.p = (tlv_array), \
	.info = snd_soc_info_volsw, .get = snd_soc_get_volsw,\
	.put = wcd9378_ear_pa_put_gain, \
	.private_value = SOC_SINGLE_VALUE(reg, shift, max, invert, 0) }

#define WCD9378_AUX_PA_GAIN_TLV(xname, reg, shift, max, invert, tlv_array) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
		 SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.tlv.p = (tlv_array), \
	.info = snd_soc_info_volsw, .get = snd_soc_get_volsw,\
	.put = wcd9378_aux_pa_put_gain, \
	.private_value = SOC_SINGLE_VALUE(reg, shift, max, invert, 0) }

enum {
	CODEC_TX = 0,
	CODEC_RX,
};

enum {
	RX2_HP_MODE,
	RX2_NORMAL_MODE,
};

enum {
	CLASS_AB_EN = 0,
	TX1_FOR_JACK,
	TX2_AMIC4_EN,
	TX2_AMIC1_EN,
	TX1_AMIC3_EN,
	TX1_AMIC2_EN,
	TX0_AMIC2_EN,
	TX0_AMIC1_EN,
	RX2_EAR_EN,
	RX2_AUX_EN,
	RX1_AUX_EN,
	RX0_EAR_EN,
	RX0_RX1_HPH_EN,
};

enum {
	WCD_ADC1 = 0,
	WCD_ADC2,
	WCD_ADC3,
	WCD_ADC4,
	ALLOW_BUCK_DISABLE,
	HPH_COMP_DELAY,
	HPH_PA_DELAY,
	AMIC2_BCS_ENABLE,
	WCD_SUPPLIES_LPM_MODE,
	WCD_ADC1_MODE,
	WCD_ADC2_MODE,
	WCD_ADC3_MODE,
	WCD_ADC4_MODE,
	WCD_AUX_EN,
	WCD_EAR_EN,
};

enum {
	SYS_USAGE_0,
	SYS_USAGE_1,
	SYS_USAGE_2,
	SYS_USAGE_3,
	SYS_USAGE_4,
	SYS_USAGE_5,
	SYS_USAGE_6,
	SYS_USAGE_7,
	SYS_USAGE_8,
	SYS_USAGE_9,
	SYS_USAGE_10,
	SYS_USAGE_11,
	SYS_USAGE_12,
	SYS_USAGE_NUM,
};

enum {
	NO_MICB_USED,
	MICB1,
	MICB2,
	MICB3,
	MICB_NUM,
};

enum {
	ADC_MODE_INVALID = 0,
	ADC_MODE_HIFI,
	ADC_MODE_NORMAL,
	ADC_MODE_LP,
};

static const SNDRV_CTL_TLVD_DECLARE_DB_MINMAX(analog_gain, 0, 3000);

static int wcd9378_reset(struct device *dev);
static int wcd9378_reset_low(struct device *dev);
static void wcd9378_class_load(struct snd_soc_component *component);

/* sys_usage:
 * rx0_rx1_hph_en,
 * rx0_ear_en, rx1_aux_en, rx2_aux_en, rx2_ear_en,
 * tx0_amic1_en, tx0_amic2_en, tx1_amic2_en, tx1_amic3_en,
 * tx2_amic1_en, tx2_amic4_en, tx1_for_jack, class_ab_en;
 */
static const int sys_usage[SYS_USAGE_NUM] = {
	[SYS_USAGE_0]         = 0x0c95,        /*0b0 1100 1001 0101*/
	[SYS_USAGE_1]         = 0x12a7,        /*0b1 0010 1010 0111*/
	[SYS_USAGE_2]         = 0x0c99,        /*0b0 1100 1001 1001*/
	[SYS_USAGE_3]         = 0x1aab,        /*0b1 1010 1010 1011*/
	[SYS_USAGE_4]         = 0x0894,        /*0b0 1000 1001 0100*/
	[SYS_USAGE_5]         = 0x11a6,        /*0b1 0001 1010 0110*/
	[SYS_USAGE_6]         = 0x0898,        /*0b0 1000 1001 1000*/
	[SYS_USAGE_7]         = 0x11ab,        /*0b1 0001 1010 1011*/
	[SYS_USAGE_8]         = 0x126a,        /*0b1 0010 0110 1010*/
	[SYS_USAGE_9]         = 0x116b,        /*0b1 0001 0110 1011*/
	[SYS_USAGE_10]        = 0x1ca7,        /*0b1 1100 1010 0111*/
	[SYS_USAGE_11]        = 0x1195,        /*0b1 0001 1001 0101*/
	[SYS_USAGE_12]        = 0x1296,        /*0b1 0010 1001 0101*/
};

static const struct regmap_irq wcd9378_regmap_irqs[WCD9378_NUM_IRQS] = {
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_BUTTON_PRESS_DET, 0, 0x01),
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_BUTTON_RELEASE_DET, 0, 0x02),
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_ELECT_INS_REM_DET, 0, 0x04),
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_ELECT_INS_REM_LEG_DET, 0, 0x08),
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_SW_DET, 0, 0x10),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHR_OCP_INT, 0, 0x20),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHR_CNP_INT, 0, 0x40),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHL_OCP_INT, 0, 0x80),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHL_CNP_INT, 1, 0x01),
	REGMAP_IRQ_REG(WCD9378_IRQ_EAR_CNP_INT, 1, 0x02),
	REGMAP_IRQ_REG(WCD9378_IRQ_EAR_SCD_INT, 1, 0x04),
	REGMAP_IRQ_REG(WCD9378_IRQ_AUX_CNP_INT, 1, 0x08),
	REGMAP_IRQ_REG(WCD9378_IRQ_AUX_SCD_INT, 1, 0x10),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHL_PDM_WD_INT, 1, 0x20),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHR_PDM_WD_INT, 1, 0x40),
	REGMAP_IRQ_REG(WCD9378_IRQ_AUX_PDM_WD_INT, 1, 0x80),
	REGMAP_IRQ_REG(WCD9378_IRQ_LDORT_SCD_INT, 2, 0x01),
	REGMAP_IRQ_REG(WCD9378_IRQ_MBHC_MOISTURE_INT, 2, 0x02),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHL_SURGE_DET_INT, 2, 0x04),
	REGMAP_IRQ_REG(WCD9378_IRQ_HPHR_SURGE_DET_INT, 2, 0x08),
	REGMAP_IRQ_REG(WCD9378_IRQ_SAPU_PROT_MODE_CHG, 2, 0x40),
};

static int wcd9378_handle_post_irq(void *data)
{
	struct wcd9378_priv *wcd9378 = data;
	u32 sts1 = 0, sts2 = 0, sts3 = 0;

	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_1, 0xff);
	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_2, 0xff);
	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_3, 0xff);

	regmap_read(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_1, &sts1);
	regmap_read(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_2, &sts2);
	regmap_read(wcd9378->regmap, SWRS_SCP_SDCA_INTSTAT_3, &sts3);

	wcd9378->tx_swr_dev->slave_irq_pending =
			((sts1 || sts2 || sts3) ? true : false);

	return IRQ_HANDLED;
}

static struct regmap_irq_chip wcd9378_regmap_irq_chip = {
	.name = "wcd9378",
	.irqs = wcd9378_regmap_irqs,
	.num_irqs = ARRAY_SIZE(wcd9378_regmap_irqs),
	.num_regs = 3,
	.status_base = SWRS_SCP_SDCA_INTSTAT_1,
	.unmask_base = SWRS_SCP_SDCA_INTMASK_1,
	.type_base = SWRS_SCP_SDCA_INTRTYPE_1,
	.ack_base = SWRS_SCP_SDCA_INTSTAT_1,
	.use_ack = 1,
	.runtime_pm = false,
	.handle_post_irq = wcd9378_handle_post_irq,
	.irq_drv_data = NULL,
};

static int wcd9378_swr_slv_get_current_bank(struct swr_device *dev, u8 devnum)
{
	int ret = 0;
	int bank = 0;

	ret = swr_read(dev, devnum, SWR_SCP_CONTROL, &bank, 1);
	if (ret)
		return -EINVAL;

	return ((bank & 0x40) ? 1 : 0);
}

static int wcd9378_swr_reset_check(struct wcd9378_priv *wcd9378, int path)
{
	if (((path == TX_PATH) &&
		(wcd9378->sys_usage_status & WCD_TX_SYS_USAGE_BIT_MASK)) ||
		((path == RX_PATH) &&
			(wcd9378->sys_usage_status & WCD_RX_SYS_USAGE_BIT_MASK)))
		return false;

	return true;
}

static int wcd9378_swr_slvdev_datapath_control(struct device *dev,
			int path, bool enable)
{
	struct wcd9378_priv *wcd9378 = NULL;
	struct swr_device *swr_dev = NULL;
	int bank = 0, ret = 0;
	u8 clk_rst = 0x00, scale_rst = 0x00;
	u8 swr_clk = 0, clk_scale = 0;
	u16 scale_reg = 0, scale_reg2 = 0;

	wcd9378 = dev_get_drvdata(dev);
	if (!wcd9378)
		return -EINVAL;

	if (path == RX_PATH) {
		swr_dev = wcd9378->rx_swr_dev;
		swr_clk = wcd9378->rx_swrclk;
		clk_scale = wcd9378->rx_clkscale;
	} else {
		swr_dev = wcd9378->tx_swr_dev;
		swr_clk = wcd9378->tx_swrclk;
		clk_scale = wcd9378->tx_clkscale;
	}

	bank = (wcd9378_swr_slv_get_current_bank(swr_dev,
					swr_dev->dev_num) ? 0 : 1);

	scale_reg = (bank ? SWRS_SCP_BUSCLOCK_SCALE_BANK1 :
				SWRS_SCP_BUSCLOCK_SCALE_BANK0);
	scale_reg2 = (!bank ? SWRS_SCP_BUSCLOCK_SCALE_BANK1 :
				SWRS_SCP_BUSCLOCK_SCALE_BANK0);

	if (enable) {
		swr_write(swr_dev, swr_dev->dev_num,
					SWRS_SCP_BASE_CLK_BASE, &swr_clk);
		swr_write(swr_dev, swr_dev->dev_num,
					scale_reg, &clk_scale);
		swr_write(swr_dev, swr_dev->dev_num,
					scale_reg2, &clk_scale);
		ret = swr_slvdev_datapath_control(swr_dev,
					swr_dev->dev_num, true);
	} else {
		if (wcd9378_swr_reset_check(wcd9378, path)) {
			swr_write(swr_dev, swr_dev->dev_num,
					SWRS_SCP_BASE_CLK_BASE, &clk_rst);
			swr_write(swr_dev, swr_dev->dev_num,
					scale_reg, &scale_rst);
			swr_write(swr_dev, swr_dev->dev_num,
					scale_reg2, &scale_rst);
		}
		ret = swr_slvdev_datapath_control(swr_dev,
					swr_dev->dev_num, false);
	}

	return ret;
}

static int wcd9378_init_reg(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	u32 val = 0;

	val = snd_soc_component_read(component, WCD9378_EFUSE_REG_16);
	if (!val)
		snd_soc_component_update_bits(component, WCD9378_MBHC_CTL_SPARE_1,
			WCD9378_MBHC_CTL_SPARE_1_BIASGEN_RES_CTRL_MASK,
			0x03);
	else
		snd_soc_component_update_bits(component, WCD9378_MBHC_CTL_SPARE_1,
			WCD9378_MBHC_CTL_SPARE_1_BIASGEN_RES_CTRL_MASK,
			0x01);

	/*0.9 Volts*/
	snd_soc_component_update_bits(component, WCD9378_SLEEP_CTL,
			WCD9378_SLEEP_CTL_BG_CTL_MASK, 0x0E);
	/*BG_EN ENABLE*/
	snd_soc_component_update_bits(component, WCD9378_SLEEP_CTL,
			WCD9378_SLEEP_CTL_BG_EN_MASK, 0x80);
	usleep_range(1000, 1010);
	/*LDOL_BG_SEL SLEEP_BG*/
	snd_soc_component_update_bits(component, WCD9378_SLEEP_CTL,
			WCD9378_SLEEP_CTL_LDOL_BG_SEL_MASK, 0x40);
	usleep_range(1000, 1010);

	/*Start up analog master bias. Sequence cannot change*/
	/*VBG_FINE_ADJ 0.005 Volts*/
	snd_soc_component_update_bits(component, WCD9378_BIAS_VBG_FINE_ADJ,
			WCD9378_BIAS_VBG_FINE_ADJ_VBG_FINE_ADJ_MASK, 0xB0);

	/*ANALOG_BIAS_EN ENABLE*/
	snd_soc_component_update_bits(component, WCD9378_ANA_BIAS,
			WCD9378_ANA_BIAS_ANALOG_BIAS_EN_MASK, 0x80);
	/*PRECHRG_EN ENABLE*/
	snd_soc_component_update_bits(component, WCD9378_ANA_BIAS,
			WCD9378_ANA_BIAS_PRECHRG_EN_MASK, 0x40);
	usleep_range(10000, 10010);
	/*PRECHRG_EN DISABLE*/
	snd_soc_component_update_bits(component, WCD9378_ANA_BIAS,
			WCD9378_ANA_BIAS_PRECHRG_EN_MASK, 0x00);
	/*End Analog Master Bias enable*/

	/*ANA_TXSCBIAS_CLK_EN ENABLE*/
	snd_soc_component_update_bits(component, WCD9378_CDC_ANA_TX_CLK_CTL,
			WCD9378_CDC_ANA_TX_CLK_CTL_ANA_TXSCBIAS_CLK_EN_MASK, 0x01);
	/*SEQ_BYPASS ENABLE*/
	snd_soc_component_update_bits(component, WCD9378_TX_COM_TXFE_DIV_CTL,
			WCD9378_TX_COM_TXFE_DIV_CTL_SEQ_BYPASS_MASK, 0x80);
	/*TIME_OUT_SEL_PCM 160_CYCLES*/
	snd_soc_component_update_bits(component, WCD9378_PDM_WD_CTL0,
			WCD9378_PDM_WD_CTL0_TIME_OUT_SEL_PCM_MASK, 0x10);
	/*TIME_OUT_SEL_PCM 160_CYCLES*/
	snd_soc_component_update_bits(component, WCD9378_PDM_WD_CTL1,
			WCD9378_PDM_WD_CTL1_TIME_OUT_SEL_PCM_MASK, 0x10);
	/*IBIAS_LDO_DRIVER 5e-06*/
	snd_soc_component_update_bits(component, WCD9378_MICB1_TEST_CTL_2,
			WCD9378_MICB1_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK, 0x01);
	/*IBIAS_LDO_DRIVER 5e-06*/
	snd_soc_component_update_bits(component, WCD9378_MICB2_TEST_CTL_2,
			WCD9378_MICB2_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK, 0x01);
	/*IBIAS_LDO_DRIVER 5e-06*/
	snd_soc_component_update_bits(component, WCD9378_MICB3_TEST_CTL_2,
			WCD9378_MICB3_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK, 0x01);

	/*HD2_RES_DIV_CTL_L 82.77*/
	snd_soc_component_update_bits(component, WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L,
			WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_HD2_RES_DIV_CTL_L_MASK, 0x04);
	/*HD2_RES_DIV_CTL_R 82.77*/
	snd_soc_component_update_bits(component, WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R,
			WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_HD2_RES_DIV_CTL_R_MASK, 0x04);

	/*RDAC_GAINCTL 0.55*/
	snd_soc_component_update_bits(component, WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL,
			WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL_RDAC_GAINCTL_MASK, 0x50);
	/*HPH_UP_T0: 0.002*/
	snd_soc_component_update_bits(component, WCD9378_HPH_UP_T0,
			WCD9378_HPH_UP_T0_HPH_UP_T0_MASK, 0x05);
	/*HPH_UP_T9: 0.002*/
	snd_soc_component_update_bits(component, WCD9378_HPH_UP_T9,
			WCD9378_HPH_UP_T9_HPH_UP_T9_MASK, 0x05);
	/*HPH_DN_T0: 0.007*/
	snd_soc_component_update_bits(component, WCD9378_HPH_DN_T0,
			WCD9378_HPH_DN_T0_HPH_DN_T0_MASK, 0x06);

	/*SM0 MB SEL:MB1*/
	snd_soc_component_update_bits(component, WCD9378_SM0_MB_SEL,
			WCD9378_SM0_MB_SEL_SM0_MB_SEL_MASK, 0x01);
	/*SM1 MB SEL:MB2*/
	snd_soc_component_update_bits(component, WCD9378_SM1_MB_SEL,
			WCD9378_SM1_MB_SEL_SM1_MB_SEL_MASK, 0x02);
	/*SM2 MB SEL:MB3*/
	snd_soc_component_update_bits(component, WCD9378_SM2_MB_SEL,
			WCD9378_SM2_MB_SEL_SM2_MB_SEL_MASK, 0x03);

	/*INIT SYS_USAGE*/
	snd_soc_component_update_bits(component,
		WCD9378_SYS_USAGE_CTRL,
		WCD9378_SYS_USAGE_CTRL_SYS_USAGE_CTRL_MASK,
		0);
	wcd9378->sys_usage = 0;

	wcd9378_class_load(component);
	return 0;
}

static int wcd9378_set_port_params(struct snd_soc_component *component,
			u8 slv_prt_type, u8 *port_id, u8 *num_ch,
			u8 *ch_mask, u32 *ch_rate,
			u8 *port_type, u8 path)
{
	int i, j;
	u8 num_ports = 0;
	struct codec_port_info (*map)[MAX_PORT][MAX_CH_PER_PORT];
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	switch (path) {
	case CODEC_RX:
		map = &wcd9378->rx_port_mapping;
		num_ports = wcd9378->num_rx_ports;
		break;
	case CODEC_TX:
		map = &wcd9378->tx_port_mapping;
		num_ports = wcd9378->num_tx_ports;
		break;
	default:
		dev_err(component->dev, "%s Invalid path selected %u\n",
					__func__, path);
		return -EINVAL;
	}

	for (i = 0; i <= num_ports; i++) {
		for (j = 0; j < MAX_CH_PER_PORT; j++) {
			if ((*map)[i][j].slave_port_type == slv_prt_type)
				goto found;
		}
	}
found:
	if (i > num_ports || j == MAX_CH_PER_PORT) {
		dev_err(component->dev, "%s Failed to find slave port for type %u\n",
						__func__, slv_prt_type);
		return -EINVAL;
	}
	*port_id = i;
	*num_ch = (*map)[i][j].num_ch;
	*ch_mask = (*map)[i][j].ch_mask;
	*ch_rate = (*map)[i][j].ch_rate;
	*port_type = (*map)[i][j].master_port_type;

	return 0;
}

static int wcd9378_parse_port_params(struct device *dev,
			char *prop, u8 path)
{
	u32 *dt_array, map_size, max_uc;
	int ret = 0;
	u32 cnt = 0;
	u32 i, j;
	struct swr_port_params (*map)[SWR_UC_MAX][SWR_NUM_PORTS];
	struct swr_dev_frame_config (*map_uc)[SWR_UC_MAX];
	struct wcd9378_priv *wcd9378 = dev_get_drvdata(dev);

	switch (path) {
	case CODEC_TX:
		map = &wcd9378->tx_port_params;
		map_uc = &wcd9378->swr_tx_port_params;
		break;
	default:
		ret = -EINVAL;
		goto err_port_map;
	}

	if (!of_find_property(dev->of_node, prop,
				&map_size)) {
		dev_err(dev, "missing port mapping prop %s\n", prop);
		ret = -EINVAL;
		goto err_port_map;
	}

	max_uc = map_size / (SWR_NUM_PORTS * SWR_PORT_PARAMS * sizeof(u32));

	if (max_uc != SWR_UC_MAX) {
		dev_err(dev, "%s: port params not provided for all usecases\n",
			__func__);
		ret = -EINVAL;
		goto err_port_map;
	}
	dt_array = kzalloc(map_size, GFP_KERNEL);

	if (!dt_array) {
		ret = -ENOMEM;
		goto err_alloc;
	}
	ret = of_property_read_u32_array(dev->of_node, prop, dt_array,
				SWR_NUM_PORTS * SWR_PORT_PARAMS * max_uc);
	if (ret) {
		dev_err(dev, "%s: Failed to read  port mapping from prop %s\n",
					__func__, prop);
		goto err_pdata_fail;
	}

	for (i = 0; i < max_uc; i++) {
		for (j = 0; j < SWR_NUM_PORTS; j++) {
			cnt = (i * SWR_NUM_PORTS + j) * SWR_PORT_PARAMS;
			(*map)[i][j].offset1 = dt_array[cnt];
			(*map)[i][j].lane_ctrl = dt_array[cnt + 1];
		}
		(*map_uc)[i].pp = &(*map)[i][0];
	}
	kfree(dt_array);
	return 0;

err_pdata_fail:
	kfree(dt_array);
err_alloc:
err_port_map:
	return ret;
}

static int wcd9378_parse_port_mapping(struct device *dev,
			char *prop, u8 path)
{
	u32 *dt_array, map_size, map_length;
	u32 port_num = 0, ch_mask, ch_rate, old_port_num = 0;
	u32 slave_port_type, master_port_type;
	u32 i, ch_iter = 0;
	int ret = 0;
	u8 *num_ports = NULL;
	struct codec_port_info (*map)[MAX_PORT][MAX_CH_PER_PORT];
	struct wcd9378_priv *wcd9378 = dev_get_drvdata(dev);

	switch (path) {
	case CODEC_RX:
		map = &wcd9378->rx_port_mapping;
		num_ports = &wcd9378->num_rx_ports;
		break;
	case CODEC_TX:
		map = &wcd9378->tx_port_mapping;
		num_ports = &wcd9378->num_tx_ports;
		break;
	default:
		dev_err(dev, "%s Invalid path selected %u\n",
			      __func__, path);
		return -EINVAL;
	}

	if (!of_find_property(dev->of_node, prop,
				&map_size)) {
		dev_err(dev, "missing port mapping prop %s\n", prop);
		ret = -EINVAL;
		goto err_port_map;
	}

	map_length = map_size / (NUM_SWRS_DT_PARAMS * sizeof(u32));

	dt_array = kzalloc(map_size, GFP_KERNEL);

	if (!dt_array) {
		ret = -ENOMEM;
		goto err_alloc;
	}
	ret = of_property_read_u32_array(dev->of_node, prop, dt_array,
				NUM_SWRS_DT_PARAMS * map_length);
	if (ret) {
		dev_err(dev, "%s: Failed to read  port mapping from prop %s\n",
					__func__, prop);
		goto err_pdata_fail;
	}

	for (i = 0; i < map_length; i++) {
		port_num = dt_array[NUM_SWRS_DT_PARAMS * i];

		if (port_num >= MAX_PORT || ch_iter >= MAX_CH_PER_PORT) {
			dev_err(dev, "%s: Invalid port or channel number\n", __func__);
			goto err_pdata_fail;
		}

		slave_port_type = dt_array[NUM_SWRS_DT_PARAMS * i + 1];
		ch_mask = dt_array[NUM_SWRS_DT_PARAMS * i + 2];
		ch_rate = dt_array[NUM_SWRS_DT_PARAMS * i + 3];
		master_port_type = dt_array[NUM_SWRS_DT_PARAMS * i + 4];

		if (port_num != old_port_num)
			ch_iter = 0;

		(*map)[port_num][ch_iter].slave_port_type = slave_port_type;
		(*map)[port_num][ch_iter].ch_mask = ch_mask;
		(*map)[port_num][ch_iter].master_port_type = master_port_type;
		(*map)[port_num][ch_iter].num_ch = __sw_hweight8(ch_mask);
		(*map)[port_num][ch_iter++].ch_rate = ch_rate;
		old_port_num = port_num;
	}
	*num_ports = port_num;
	kfree(dt_array);
	return 0;

err_pdata_fail:
	kfree(dt_array);
err_alloc:
err_port_map:
	return ret;
}

static int wcd9378_tx_connect_port(struct snd_soc_component *component,
					u8 slv_port_type, int clk_rate,
					u8 enable)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	u8 port_id, num_ch, ch_mask;
	u8 ch_type = 0;
	u32 ch_rate;
	int slave_ch_idx;
	u8 num_port = 1;
	int ret = 0;

	ret = wcd9378_set_port_params(component, slv_port_type, &port_id,
				&num_ch, &ch_mask, &ch_rate,
				&ch_type, CODEC_TX);
	if (ret)
		return ret;

	if (clk_rate)
		ch_rate = clk_rate;

	slave_ch_idx = wcd9378_slave_get_slave_ch_val(slv_port_type);
	if (slave_ch_idx != -EINVAL)
		ch_type = wcd9378->tx_master_ch_map[slave_ch_idx];

	dev_dbg(component->dev, "%s slv_ch_idx: %d, mstr_ch_type: %d\n",
		__func__, slave_ch_idx, ch_type);

	if (enable)
		ret = swr_connect_port(wcd9378->tx_swr_dev, &port_id,
					num_port, &ch_mask, &ch_rate,
					 &num_ch, &ch_type);
	else
		ret = swr_disconnect_port(wcd9378->tx_swr_dev, &port_id,
					num_port, &ch_mask, &ch_type);
	return ret;

}

static int wcd9378_rx_connect_port(struct snd_soc_component *component,
					u8 slv_port_type, u8 enable)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	u8 port_id, num_ch, ch_mask, port_type;
	u32 ch_rate;
	u8 num_port = 1;
	int ret = 0;

	ret = wcd9378_set_port_params(component, slv_port_type, &port_id,
				&num_ch, &ch_mask, &ch_rate,
				&port_type, CODEC_RX);

	if (ret)
		return ret;

	if (enable)
		ret = swr_connect_port(wcd9378->rx_swr_dev, &port_id,
					num_port, &ch_mask, &ch_rate,
					&num_ch, &port_type);
	else
		ret = swr_disconnect_port(wcd9378->rx_swr_dev, &port_id,
					num_port, &ch_mask, &port_type);
	return ret;
}


static int wcd9378_enable_clsh(struct snd_soc_dapm_widget *w,
			       struct snd_kcontrol *kcontrol,
			       int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int mode = wcd9378->hph_mode;
	int ret = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	if (mode == CLS_H_LOHIFI || mode == CLS_H_ULP ||
		mode == CLS_H_HIFI || mode == CLS_H_LP) {
		wcd9378_rx_connect_port(component, CLSH,
				SND_SOC_DAPM_EVENT_ON(event));
	}
	if (SND_SOC_DAPM_EVENT_OFF(event))
		ret = wcd9378_swr_slvdev_datapath_control(wcd9378->dev,
					RX_PATH, false);

	return ret;
}

static int wcd9378_codec_enable_dmic(struct snd_soc_dapm_widget *w,
				     struct snd_kcontrol *kcontrol,
				     int event)
{
	struct snd_soc_component *component =
				snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	u32 dmic_clk_reg, dmic_clk_en_reg;
	s32 *dmic_clk_cnt;
	u8 dmic_ctl_shift = 0;
	u8 dmic_clk_shift = 0;
	u8 dmic_clk_mask = 0;
	u32 dmic2_left_en = 0;
	int ret = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (w->shift) {
	case 0:
	case 1:
		dmic_clk_cnt = &(wcd9378->dmic_0_1_clk_cnt);
		dmic_clk_reg = WCD9378_CDC_DMIC_RATE_1_2;
		dmic_clk_en_reg = WCD9378_CDC_DMIC1_CTL;
		dmic_clk_mask = 0x0F;
		dmic_clk_shift = 0x00;
		dmic_ctl_shift = 0x00;
		break;
	case 2:
		dmic2_left_en = WCD9378_CDC_DMIC2_CTL;
		fallthrough;
	case 3:
		dmic_clk_cnt = &(wcd9378->dmic_2_3_clk_cnt);
		dmic_clk_reg = WCD9378_CDC_DMIC_RATE_1_2;
		dmic_clk_en_reg = WCD9378_CDC_DMIC2_CTL;
		dmic_clk_mask = 0xF0;
		dmic_clk_shift = 0x04;
		dmic_ctl_shift = 0x01;
		break;
	case 4:
	case 5:
		dmic_clk_cnt = &(wcd9378->dmic_4_5_clk_cnt);
		dmic_clk_reg = WCD9378_CDC_DMIC_RATE_3_4;
		dmic_clk_en_reg = WCD9378_CDC_DMIC3_CTL;
		dmic_clk_mask = 0x0F;
		dmic_clk_shift = 0x00;
		dmic_ctl_shift = 0x02;
		break;
	default:
		dev_err_ratelimited(component->dev, "%s: Invalid DMIC Selection\n",
			__func__);
		return -EINVAL;
	};
	dev_dbg(component->dev, "%s: event %d DMIC%d dmic_clk_cnt %d\n",
			__func__, event,  (w->shift + 1), *dmic_clk_cnt);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		snd_soc_component_update_bits(component,
				WCD9378_CDC_AMIC_CTL,
				(0x01 << dmic_ctl_shift), 0x00);
		/* 250us sleep as per HW requirement */
		usleep_range(250, 260);
		if (dmic2_left_en)
			snd_soc_component_update_bits(component,
				dmic2_left_en, 0x80, 0x80);
		/* Setting DMIC clock rate to 2.4MHz */
		snd_soc_component_update_bits(component,
					dmic_clk_reg, dmic_clk_mask,
					(0x03 << dmic_clk_shift));
		snd_soc_component_update_bits(component,
					dmic_clk_en_reg, 0x08, 0x08);
		/* enable clock scaling */
		snd_soc_component_update_bits(component,
				WCD9378_CDC_DMIC_CTL, 0x06, 0x06);
		ret = swr_slvdev_datapath_control(wcd9378->tx_swr_dev,
				wcd9378->tx_swr_dev->dev_num,
				true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd9378_tx_connect_port(component, DMIC0 + (w->shift), 0,
				false);
		snd_soc_component_update_bits(component,
				WCD9378_CDC_AMIC_CTL,
				(0x01 << dmic_ctl_shift),
				(0x01 << dmic_ctl_shift));
		if (dmic2_left_en)
			snd_soc_component_update_bits(component,
				dmic2_left_en, 0x80, 0x00);
		snd_soc_component_update_bits(component,
					dmic_clk_en_reg, 0x08, 0x00);
		break;
	};
	return ret;
}

/*
 * wcd9378_get_micb_vout_ctl_val: converts micbias from volts to register value
 * @micb_mv: micbias in mv
 *
 * return register value converted
 */
int wcd9378_get_micb_vout_ctl_val(u32 micb_mv)
{
	/* min micbias voltage is 1V and maximum is 2.85V */
	if (micb_mv < 1000 || micb_mv > 2850) {
		pr_err("%s: unsupported micbias voltage\n", __func__);
		return -EINVAL;
	}

	return (micb_mv - 1000) / 50;
}
EXPORT_SYMBOL_GPL(wcd9378_get_micb_vout_ctl_val);

/*
 * wcd9378_mbhc_micb_adjust_voltage: adjust specific micbias voltage
 * @component: handle to snd_soc_component *
 * @req_volt: micbias voltage to be set
 * @micb_num: micbias to be set, e.g. micbias1 or micbias2
 *
 * return 0 if adjustment is success or error code in case of failure
 */
static int wcd9378_micb_table_value_set(struct snd_soc_component *component,
				u32 micb_mv, int micb_num)
{
	int vcout_ctl;

	switch (micb_mv) {
	case 2200:
		return MICB_USAGE_VAL_2P2V;
	case 2700:
		return MICB_USAGE_VAL_2P7V;
	case 2800:
		return MICB_USAGE_VAL_2P8V;
	default:
		vcout_ctl = wcd9378_get_micb_vout_ctl_val(micb_mv);
		if (micb_num == MIC_BIAS_1) {
			snd_soc_component_update_bits(component,
				WCD9378_MICB_REMAP_TABLE_VAL_3,
				WCD9378_MICB_REMAP_TABLE_VAL_3_MICB_REMAP_TABLE_VAL_3_MASK,
				vcout_ctl);
			return MICB_USAGE_VAL_MICB1_TABLE_VAL;
		} else if (micb_num == MIC_BIAS_2) {
			snd_soc_component_update_bits(component,
				WCD9378_MICB_REMAP_TABLE_VAL_4,
				WCD9378_MICB_REMAP_TABLE_VAL_4_MICB_REMAP_TABLE_VAL_4_MASK,
				vcout_ctl);
			return MICB_USAGE_VAL_MICB2_TABLE_VAL;
		} else if (micb_num == MIC_BIAS_3) {
			snd_soc_component_update_bits(component,
				WCD9378_MICB_REMAP_TABLE_VAL_5,
				WCD9378_MICB_REMAP_TABLE_VAL_5_MICB_REMAP_TABLE_VAL_5_MASK,
				vcout_ctl);
			return MICB_USAGE_VAL_MICB3_TABLE_VAL;
		}
	}

	return 0;
}

static int wcd9378_micb_usage_value_convert(struct snd_soc_component *component,
				u32 micb_mv, int micb_num)
{
	switch (micb_mv) {
	case 0:
		return MICB_USAGE_VAL_PULL_DOWN;
	case 1200:
		return MICB_USAGE_VAL_1P2V;
	case 1800:
		return MICB_USAGE_VAL_1P8VORPULLUP;
	case 2500:
		return MICB_USAGE_VAL_2P5V;
	case 2750:
		return MICB_USAGE_VAL_2P75V;
	default:
		return wcd9378_micb_table_value_set(component, micb_mv, micb_num);
	}

	return MICB_USAGE_VAL_DISABLE;
}

int wcd9378_mbhc_micb_adjust_voltage(struct snd_soc_component *component,
				   int req_volt, int micb_num)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	int micb_usage = 0, micb_mask = 0, req_vout_ctl = 0;

	if (wcd9378 == NULL) {
		dev_err(component->dev,
			"%s: wcd9378 private data is NULL\n", __func__);
		return -EINVAL;
	}

	switch (micb_num) {
	case MIC_BIAS_1:
		micb_usage = WCD9378_IT11_USAGE;
		micb_mask = WCD9378_IT11_MICB_IT11_MICB_MASK;
		break;
	case MIC_BIAS_2:
		micb_usage = WCD9378_SMP_MIC_CTRL1_IT11_MICB;
		micb_mask = WCD9378_SMP_MIC_CTRL1_IT11_MICB_IT11_MICB_MASK;
		break;
	case MIC_BIAS_3:
		micb_usage = WCD9378_SMP_MIC_CTRL2_IT11_MICB;
		micb_mask = WCD9378_SMP_MIC_CTRL2_IT11_MICB_IT11_MICB_MASK;
		break;
	default:
		dev_err(component->dev,
			"%s: wcd9378 private data is NULL\n", __func__);
		break;
	}

	mutex_lock(&wcd9378->micb_lock);

	req_vout_ctl =
		wcd9378_micb_usage_value_convert(component, req_volt, micb_num);

	snd_soc_component_update_bits(component,
			micb_usage, micb_mask, req_vout_ctl);

	if (micb_num == MIC_BIAS_2) {
		dev_err(component->dev,
			"%s: sj micbias set\n", __func__);
		snd_soc_component_update_bits(component,
				WCD9378_IT31_MICB,
				WCD9378_IT31_MICB_IT31_MICB_MASK,
				req_vout_ctl);
		wcd9378->curr_micbias2 = req_volt;
	}
	mutex_unlock(&wcd9378->micb_lock);
	return 0;


}
EXPORT_SYMBOL_GPL(wcd9378_mbhc_micb_adjust_voltage);

void wcd9378_disable_bcs_before_slow_insert(struct snd_soc_component *component,
					    bool bcs_disable)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	if (wcd9378->update_wcd_event) {
		if (bcs_disable)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_BCS_CLK_OFF, 0);
		else
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_BCS_CLK_OFF, 1);
	}
}

static void wcd9378_get_swr_clk_val(
			struct snd_soc_component *component,
			int rate)
{
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);

	switch (rate) {
	case SWR_CLK_RATE_4P8MHZ:
		wcd9378->tx_swrclk = SWR_BASECLK_19P2MHZ;
		wcd9378->tx_clkscale = SWR_CLKSCALE_DIV4;
		break;
	case SWR_CLK_RATE_9P6MHZ:
		wcd9378->tx_swrclk = SWR_BASECLK_19P2MHZ;
		wcd9378->tx_clkscale = SWR_CLKSCALE_DIV2;
		break;
	default:
		dev_dbg(component->dev, "%s: unsupport rate: %d\n",
				__func__, rate);
		break;
	}

	dev_dbg(component->dev, "%s: rate: %d, tx_swrclk: 0x%x, tx_clkscale: 0x%x\n",
		__func__, rate, wcd9378->tx_swrclk, wcd9378->tx_clkscale);
}

static int wcd9378_get_clk_rate(int mode)
{
	int rate;

	switch (mode) {
	case ADC_MODE_LP:
		rate = SWR_CLK_RATE_4P8MHZ;
		break;
	case ADC_MODE_INVALID:
	case ADC_MODE_NORMAL:
	case ADC_MODE_HIFI:
	default:
		rate = SWR_CLK_RATE_9P6MHZ;
		break;
	}

	pr_debug("%s: mode: %d, rate: %d\n", __func__, mode, rate);
	return rate;
}

static int wcd9378_get_adc_mode_val(int mode)
{
	int ret = 0;

	switch (mode) {
	case ADC_MODE_INVALID:
	case ADC_MODE_NORMAL:
		ret = ADC_MODE_VAL_NORMAL;
		break;
	case ADC_MODE_HIFI:
		ret = ADC_MODE_VAL_HIFI;
		break;
	case ADC_MODE_LP:
		ret = ADC_MODE_VAL_LP;
		break;
	default:
		ret = -EINVAL;
		pr_err("%s: invalid ADC mode value %d\n", __func__, mode);
		break;
	}
	return ret;
}


static int wcd9378_sys_usage_auto_udpate(struct snd_soc_component *component,
					int sys_usage_bit, bool set_enable)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	int i = 0;

	dev_dbg(component->dev,
		"%s: enter, current sys_usage: %d, sys_usage_status: 0x%x, sys_usage_bit: %d, set_enable: %d\n",
			__func__, wcd9378->sys_usage,
			wcd9378->sys_usage_status,
			sys_usage_bit, set_enable);

	mutex_lock(&wcd9378->sys_usage_lock);
	if (set_enable) {
		set_bit(sys_usage_bit, &wcd9378->sys_usage_status);

		if ((sys_usage[wcd9378->sys_usage] &
			wcd9378->sys_usage_status) == wcd9378->sys_usage_status)
			goto exit;

		for (i = 0; i < SYS_USAGE_NUM; i++) {
			if ((sys_usage[i] & wcd9378->sys_usage_status)
					== wcd9378->sys_usage_status) {
				snd_soc_component_update_bits(component,
					WCD9378_SYS_USAGE_CTRL,
					WCD9378_SYS_USAGE_CTRL_SYS_USAGE_CTRL_MASK,
					i);
				wcd9378->sys_usage = i;
				dev_dbg(component->dev, "%s: update sys_usage: %d\n",
						__func__, wcd9378->sys_usage);
				goto exit;
			}
		}

		dev_dbg(component->dev, "%s: cannot find sys_usage\n",
				__func__);
	} else {
		clear_bit(sys_usage_bit, &wcd9378->sys_usage_status);
	}
exit:
	mutex_unlock(&wcd9378->sys_usage_lock);
	return 0;
}

static int wcd9378_sys_usage_bit_get(
		struct snd_soc_component *component, u32 w_shift,
		int *sys_usage_bit, int event)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);

	dev_dbg(component->dev, "%s: wshift: %d event: %d\n", __func__,
			w_shift, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		switch (w_shift) {
		case ADC1:
			if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH12_MUX) &
					WCD9378_TX_NEW_TX_CH12_MUX_CH1_SEL_MASK) == 0x01) {
				*sys_usage_bit = TX0_AMIC1_EN;
			} else if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH12_MUX) &
					WCD9378_TX_NEW_TX_CH12_MUX_CH1_SEL_MASK) == 0x02) {
				*sys_usage_bit = TX0_AMIC2_EN;
			} else {
				dev_err(component->dev, "%s: unsupport usecase, pls check\n",
						__func__);
				return -EINVAL;
			}
			break;
		case ADC2:
			if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH12_MUX) &
					WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_MASK) == 0x10) {
				*sys_usage_bit = TX1_AMIC2_EN;
			} else if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH12_MUX) &
					WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_MASK) == 0x18) {
				*sys_usage_bit = TX1_AMIC3_EN;
			} else {
				dev_err(component->dev, "%s: unsupport usecase, pls check\n",
						__func__);
				return -EINVAL;
			}
			break;
		case ADC3:
			if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH34_MUX) &
					WCD9378_TX_NEW_TX_CH34_MUX_CH3_SEL_MASK) == 0x01) {
				*sys_usage_bit = TX2_AMIC1_EN;
			} else if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH34_MUX) &
					WCD9378_TX_NEW_TX_CH34_MUX_CH3_SEL_MASK) == 0x03) {
				*sys_usage_bit = TX2_AMIC4_EN;
			} else {
				dev_err(component->dev, "%s: unsupport usecase, pls check\n",
						__func__);
				return -EINVAL;
			}
			break;
		default:
			break;
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		switch (w_shift) {
		case ADC1:
			if (test_bit(TX0_AMIC1_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX0_AMIC1_EN;

			if (test_bit(TX0_AMIC2_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX0_AMIC2_EN;
			break;
		case ADC2:
			if (test_bit(TX1_AMIC2_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX1_AMIC2_EN;

			if (test_bit(TX1_AMIC3_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX1_AMIC3_EN;
			break;
		case ADC3:
			if (test_bit(TX2_AMIC1_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX2_AMIC1_EN;

			if (test_bit(TX2_AMIC4_EN, &wcd9378->sys_usage_status))
				*sys_usage_bit = TX2_AMIC4_EN;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	dev_dbg(component->dev, "%s: done, event: %d, sys_usage_bit: %d\n",
			__func__, event, *sys_usage_bit);
	return 0;
}

static int wcd9378_tx_sequencer_enable(struct snd_soc_dapm_widget *w,
			       struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);
	int mode_val = 0, bank = 0, ret = 0, rate = 0;
	int act_ps = 0, sys_usage_bit = 0;

	bank = (wcd9378_swr_slv_get_current_bank(wcd9378->tx_swr_dev,
		wcd9378->tx_swr_dev->dev_num) ? 0 : 1);

	dev_dbg(component->dev, "%s wname: %s wshift: %d event: %d\n", __func__,
		w->name, w->shift, event);

	ret = wcd9378_sys_usage_bit_get(component, w->shift, &sys_usage_bit, event);
	if (ret < 0)
		return ret;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*Update sys_usage*/
		wcd9378_sys_usage_auto_udpate(component, sys_usage_bit, true);

		mode_val = wcd9378_get_adc_mode_val(wcd9378->tx_mode[w->shift - ADC1]);
		if (mode_val < 0) {
			dev_dbg(component->dev,
				"%s: invalid mode, setting to normal mode\n",
				__func__);
			mode_val = ADC_MODE_VAL_NORMAL;
		}

		rate = wcd9378_get_clk_rate(wcd9378->tx_mode[w->shift - ADC1]);
		if (w->shift == ADC2 && ((snd_soc_component_read(component,
				WCD9378_TX_NEW_TX_CH12_MUX) &
				WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_MASK) == 0x10)) {
			if (!wcd9378->bcs_dis) {
				wcd9378_tx_connect_port(component, MBHC,
					SWR_CLK_RATE_4P8MHZ, true);
				set_bit(AMIC2_BCS_ENABLE, &wcd9378->status_mask);
			}
		}

		set_bit(w->shift - ADC1, &wcd9378->status_mask);
		wcd9378_tx_connect_port(component, w->shift, rate,
				true);

		wcd9378_get_swr_clk_val(component, rate);

		switch (w->shift) {
		case ADC1:
			/*SMP MIC0 IT11 USAGE SET*/
			snd_soc_component_update_bits(component, WCD9378_IT11_USAGE,
						WCD9378_IT11_USAGE_IT11_USAGE_MASK, mode_val);

			/*Hold TXFE in Initialization During Startup*/
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
						WCD9378_ANA_TX_CH2_HPF1_INIT_MASK, 0x40);

			/*Power up TX0 sequencer*/
			snd_soc_component_update_bits(component, WCD9378_PDE11_REQ_PS,
					WCD9378_PDE11_REQ_PS_PDE11_REQ_PS_MASK, 0x00);
			break;
		case ADC2:
			/*Check if amic2 is connected to ADC2 MUX*/
			if ((snd_soc_component_read(component,
					WCD9378_TX_NEW_TX_CH12_MUX) &
					WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_MASK) == 0x10) {
				/*SMP JACK IT31 USAGE SET*/
				snd_soc_component_update_bits(component,
						WCD9378_IT31_USAGE,
						WCD9378_IT31_USAGE_IT31_USAGE_MASK, mode_val);
				/*Power up TX1 sequencer*/
				snd_soc_component_update_bits(component,
						WCD9378_PDE34_REQ_PS,
						WCD9378_PDE34_REQ_PS_PDE34_REQ_PS_MASK, 0x00);
			} else {
				snd_soc_component_update_bits(component,
						WCD9378_SMP_MIC_CTRL1_IT11_USAGE,
						WCD9378_SMP_MIC_CTRL1_IT11_USAGE_IT11_USAGE_MASK,
						mode_val);

				/*Hold TXFE in Initialization During Startup*/
				snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
						WCD9378_ANA_TX_CH2_HPF2_INIT_MASK, 0x20);

				/*Power up TX1 sequencer*/
				snd_soc_component_update_bits(component,
					WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS,
					WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS_PDE11_REQ_PS_MASK,
					0x00);
			}
			break;
		case ADC3:
			/*SMP MIC2 IT11 USAGE SET*/
			snd_soc_component_update_bits(component,
						WCD9378_SMP_MIC_CTRL2_IT11_USAGE,
						WCD9378_SMP_MIC_CTRL2_IT11_USAGE_IT11_USAGE_MASK,
						mode_val);

			/*Hold TXFE in Initialization During Startup*/
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH3_HPF,
						WCD9378_ANA_TX_CH3_HPF_HPF3_INIT_MASK, 0x40);

			/*Power up TX2 sequencer*/
			snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS,
					WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS_PDE11_REQ_PS_MASK, 0x00);
			break;
		default:
			break;
		}
		/*default delay 800us*/
		usleep_range(800, 810);

		wcd9378_swr_slvdev_datapath_control(wcd9378->dev, TX_PATH, true);

		switch (w->shift) {
		case ADC1:
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
						WCD9378_ANA_TX_CH2_HPF1_INIT_MASK, 0x00);

			act_ps = snd_soc_component_read(component, WCD9378_PDE11_ACT_PS);
			if (act_ps)
				dev_dbg(component->dev,
					"%s: TX0 sequencer power on failed\n", __func__);
			else
				dev_dbg(component->dev,
					"%s: TX0 sequencer power on success\n", __func__);
			break;
		case ADC2:
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
						WCD9378_ANA_TX_CH2_HPF2_INIT_MASK, 0x00);

			if (test_bit(TX1_AMIC2_EN, &wcd9378->sys_usage_status))
				act_ps = snd_soc_component_read(component,
							WCD9378_PDE34_ACT_PS);
			else
				act_ps = snd_soc_component_read(component,
							WCD9378_SMP_MIC_CTRL1_PDE11_ACT_PS);

			if (act_ps)
				dev_dbg(component->dev,
					"%s: TX1 sequencer power on failed\n", __func__);
			else
				dev_dbg(component->dev,
					"%s: TX1 sequencer power on success\n", __func__);
			break;
		case ADC3:
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH3_HPF,
						WCD9378_ANA_TX_CH3_HPF_HPF3_INIT_MASK, 0x00);

			act_ps = snd_soc_component_read(component,
						WCD9378_SMP_MIC_CTRL2_PDE11_ACT_PS);
			if (act_ps)
				dev_dbg(component->dev,
					"%s: TX2 sequencer power on failed\n", __func__);
			else
				dev_dbg(component->dev,
					"%s: TX2 sequencer power on success\n", __func__);
			break;
		};
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd9378_tx_connect_port(component, w->shift, 0, false);
		if (w->shift == ADC2 &&
			test_bit(AMIC2_BCS_ENABLE, &wcd9378->status_mask)) {
			wcd9378_tx_connect_port(component, MBHC, 0,
					false);
			clear_bit(AMIC2_BCS_ENABLE, &wcd9378->status_mask);
		}

		switch (w->shift) {
		case ADC1:
			snd_soc_component_update_bits(component, WCD9378_IT11_USAGE,
						WCD9378_IT11_USAGE_IT11_USAGE_MASK, 0x00);
			/*Normal TXFE Startup*/
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
					WCD9378_ANA_TX_CH2_HPF1_INIT_MASK, 0x00);

			/*tear down TX0 sequencer*/
			snd_soc_component_update_bits(component, WCD9378_PDE11_REQ_PS,
					WCD9378_PDE11_REQ_PS_PDE11_REQ_PS_MASK, 0x03);

			break;
		case ADC2:
			if (test_bit(TX1_AMIC2_EN, &wcd9378->sys_usage_status)) {
				snd_soc_component_update_bits(component,
						WCD9378_IT31_USAGE,
						WCD9378_IT31_USAGE_IT31_USAGE_MASK, 0x00);

				/*tear down TX1 sequencer*/
				snd_soc_component_update_bits(component, WCD9378_PDE34_REQ_PS,
						WCD9378_PDE34_REQ_PS_PDE34_REQ_PS_MASK, 0x03);
			}

			if (test_bit(TX1_AMIC3_EN, &wcd9378->sys_usage_status)) {
				snd_soc_component_update_bits(component,
						WCD9378_SMP_MIC_CTRL1_IT11_USAGE,
						WCD9378_SMP_MIC_CTRL1_IT11_USAGE_IT11_USAGE_MASK,
						0x00);

				/*Normal TXFE Startup*/
				snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH2,
						WCD9378_ANA_TX_CH2_HPF1_INIT_MASK, 0x00);

				/*tear down TX1 sequencer*/
				snd_soc_component_update_bits(component,
					WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS,
					WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS_PDE11_REQ_PS_MASK,
					0x03);
			}
			break;
		case ADC3:
			snd_soc_component_update_bits(component,
						WCD9378_SMP_MIC_CTRL2_IT11_USAGE,
						WCD9378_SMP_MIC_CTRL2_IT11_USAGE_IT11_USAGE_MASK,
						0x00);

			/*Normal TXFE Startup*/
			snd_soc_component_update_bits(component, WCD9378_ANA_TX_CH3_HPF,
					WCD9378_ANA_TX_CH3_HPF_HPF3_INIT_MASK, 0x00);

			/*tear down TX2 sequencer*/
			snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS,
					WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS_PDE11_REQ_PS_MASK, 0x03);
			break;
		default:
			break;
		}
		/*default delay 800us*/
		usleep_range(800, 810);

		/*Disable sys_usage_status*/
		wcd9378_sys_usage_auto_udpate(component, sys_usage_bit, false);

		wcd9378_swr_slvdev_datapath_control(wcd9378->dev, TX_PATH, false);
		break;
	default:
		break;
	}

	return ret;
}

static int wcd9378_tx_swr_ctrl(struct snd_soc_dapm_widget *w,
				    struct snd_kcontrol *kcontrol,
				    int event)
{
	struct snd_soc_component *component =
					snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int ret = 0;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_tx_connect_port(component, w->shift,
					SWR_CLK_RATE_2P4MHZ, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		ret = swr_slvdev_datapath_control(wcd9378->tx_swr_dev,
				wcd9378->tx_swr_dev->dev_num,
				false);
		break;
	};

	return ret;
}

static int wcd9378_codec_enable_micbias(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	int micb_num = 0;

	dev_dbg(component->dev, "%s: wname: %s, event: %d\n",
		__func__, w->name, event);

	if (strnstr(w->name, "MIC BIAS1", sizeof("MIC BIAS1")))
		micb_num = MIC_BIAS_1;
	else if (strnstr(w->name, "MIC BIAS2", sizeof("MIC BIAS2")))
		micb_num = MIC_BIAS_2;
	else if (strnstr(w->name, "MIC BIAS3", sizeof("MIC BIAS3")))
		micb_num = MIC_BIAS_3;
	else
		return -EINVAL;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_micbias_control(component, micb_num,
				MICB_ENABLE, true);
		break;
	case SND_SOC_DAPM_POST_PMU:
		usleep_range(1000, 1100);
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd9378_micbias_control(component, micb_num,
				MICB_DISABLE, true);
		break;
	};

	return 0;
}

static int wcd9378_codec_enable_micbias_pullup(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	int micb_num = 0;

	dev_dbg(component->dev, "%s: wname: %s, event: %d\n",
		__func__, w->name, event);

	if (strnstr(w->name, "VA MIC BIAS1", sizeof("VA MIC BIAS1")))
		micb_num = MIC_BIAS_1;
	else if (strnstr(w->name, "VA MIC BIAS2", sizeof("VA MIC BIAS2")))
		micb_num = MIC_BIAS_2;
	else if (strnstr(w->name, "VA MIC BIAS3", sizeof("VA MIC BIAS3")))
		micb_num = MIC_BIAS_3;
	else
		return -EINVAL;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_micbias_control(component, micb_num,
				MICB_PULLUP_ENABLE, true);
		break;
	case SND_SOC_DAPM_POST_PMU:
		usleep_range(1000, 1100);
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd9378_micbias_control(component, micb_num,
				MICB_PULLUP_DISABLE, true);
		break;
	};

	return 0;
}


/*
 * wcd9378_soc_get_mbhc: get wcd9378_mbhc handle of corresponding component
 * @component: handle to snd_soc_component *
 *
 * return wcd9378_mbhc handle or error code in case of failure
 */
struct wcd9378_mbhc *wcd9378_soc_get_mbhc(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378;

	if (!component) {
		pr_err_ratelimited("%s: Invalid params, NULL component\n", __func__);
		return NULL;
	}
	wcd9378 = snd_soc_component_get_drvdata(component);

	if (!wcd9378) {
		pr_err_ratelimited("%s: wcd9378 is NULL\n", __func__);
		return NULL;
	}

	return wcd9378->mbhc;
}
EXPORT_SYMBOL_GPL(wcd9378_soc_get_mbhc);

static int wcd9378_codec_hphl_dac_event(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
					snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*OCP FSM EN*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_OCP_FSM_EN_MASK, 0x10);
		/*SCD OP EN*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_SCD_OP_EN_MASK, 0x02);
		/*HPHL ENABLE*/
		snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
			WCD9378_CDC_HPH_GAIN_CTL_HPHL_RX_EN_MASK, 0x04);
		/*OPAMP_CHOP_CLK DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_RDAC_CLK_CTL1,
			WCD9378_HPH_RDAC_CLK_CTL1_OPAMP_CHOP_CLK_EN_MASK, 0x00);
		wcd9378_rx_connect_port(component, HPH_L, true);

		if (wcd9378->comp1_enable) {
			snd_soc_component_update_bits(component, WCD9378_CDC_COMP_CTL_0,
				WCD9378_CDC_COMP_CTL_0_HPHL_COMP_EN_MASK, 0x02);
			wcd9378_rx_connect_port(component, COMP_L, true);
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		/*OCP FSM DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_OCP_FSM_EN_MASK, 0x00);
		/*SCD OP DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_SCD_OP_EN_MASK, 0x00);
		/*HPHL DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
			WCD9378_CDC_HPH_GAIN_CTL_HPHL_RX_EN_MASK, 0x00);
		wcd9378_rx_connect_port(component, HPH_L, false);

		if (wcd9378->comp1_enable)
			wcd9378_rx_connect_port(component, COMP_L, false);
		break;
	default:
		break;
	};

	return 0;

}

static int wcd9378_codec_hphr_dac_event(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*OCP FSM EN*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_OCP_FSM_EN_MASK, 0x10);
		/*SCD OP EN*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_SCD_OP_EN_MASK, 0x02);
		/*HPHR ENABLE*/
		snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
				WCD9378_CDC_HPH_GAIN_CTL_HPHR_RX_EN_MASK, 0x08);
		/*OPAMP_CHOP_CLK DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_RDAC_CLK_CTL1,
			WCD9378_HPH_RDAC_CLK_CTL1_OPAMP_CHOP_CLK_EN_MASK, 0x00);

		wcd9378_rx_connect_port(component, HPH_R, true);

		if (wcd9378->comp2_enable) {
			snd_soc_component_update_bits(component, WCD9378_CDC_COMP_CTL_0,
				WCD9378_CDC_COMP_CTL_0_HPHR_COMP_EN_MASK, 0x01);
			wcd9378_rx_connect_port(component, COMP_R, true);
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		/*OCP FSM DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_OCP_FSM_EN_MASK, 0x00);
		/*SCD OP DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_HPH_OCP_CTL,
				WCD9378_HPH_OCP_CTL_SCD_OP_EN_MASK, 0x00);
		/*HPHR DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
			WCD9378_CDC_HPH_GAIN_CTL_HPHR_RX_EN_MASK, 0x00);
		wcd9378_rx_connect_port(component, HPH_R, false);

		if (wcd9378->comp2_enable)
			wcd9378_rx_connect_port(component, COMP_R, false);
		break;
	default:
		break;
	};

	return 0;

}

static int wcd9378_codec_enable_hphl_pa(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int bank = 0;
	int act_ps = 0;

	bank = (wcd9378_swr_slv_get_current_bank(wcd9378->rx_swr_dev,
		wcd9378->rx_swr_dev->dev_num) ? 0 : 1);

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX1 << 0x10 | 0x01));

		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX1 << 0x10));
		wcd_enable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHL_PDM_WD_INT);

		act_ps = snd_soc_component_read(component, WCD9378_PDE47_ACT_PS);
		if (act_ps)
			dev_dbg(component->dev,
				"%s: HPH sequencer power on failed\n", __func__);
		else
			dev_dbg(component->dev,
				"%s: HPH sequencer power on success\n", __func__);
		break;
	case SND_SOC_DAPM_POST_PMD:
		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX1 << 0x10 | 0x1));
		wcd_disable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHL_PDM_WD_INT);

		if (wcd9378->update_wcd_event && wcd9378->comp1_enable)
			wcd9378->update_wcd_event(wcd9378->handle,
					SLV_BOLERO_EVT_RX_COMPANDER_SOFT_RST,
					(WCD_RX1 << 0x10));

		blocking_notifier_call_chain(&wcd9378->mbhc->notifier,
					WCD_EVENT_POST_HPHL_PA_OFF,
					&wcd9378->mbhc->wcd_mbhc);
		break;
	default:
		break;
	};

	return 0;
}

static int wcd9378_codec_enable_hphr_pa(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kcontrol,
					int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int act_ps = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX2 << 0x10 | 0x1));

		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX2 << 0x10));
		wcd_enable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHR_PDM_WD_INT);

		act_ps = snd_soc_component_read(component, WCD9378_PDE47_ACT_PS);
		if (act_ps)
			dev_dbg(component->dev,
				"%s: HPH sequencer power on failed\n", __func__);
		else
			dev_dbg(component->dev,
				"%s: HPH sequencer power on success\n", __func__);
		break;
	case SND_SOC_DAPM_POST_PMD:
		if (wcd9378->update_wcd_event)
			wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX2 << 0x10 | 0x1));
		wcd_disable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHR_PDM_WD_INT);

		if (wcd9378->update_wcd_event && wcd9378->comp2_enable)
			wcd9378->update_wcd_event(wcd9378->handle,
					SLV_BOLERO_EVT_RX_COMPANDER_SOFT_RST,
					(WCD_RX2 << 0x10));

		blocking_notifier_call_chain(&wcd9378->mbhc->notifier,
					WCD_EVENT_POST_HPHR_PA_OFF,
					&wcd9378->mbhc->wcd_mbhc);
		break;
	default:
		break;
	};

	return 0;

}

static int wcd9378_codec_enable_aux_pa(struct snd_soc_dapm_widget *w,
				       struct snd_kcontrol *kcontrol,
				       int event)
{
	struct snd_soc_component *component =
				snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);
	int ret = 0, act_ps = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);
	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_swr_slvdev_datapath_control(wcd9378->dev, RX_PATH, true);

		if (test_bit(RX1_AUX_EN, &wcd9378->sys_usage_status)) {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
							SLV_BOLERO_EVT_RX_MUTE,
							(WCD_RX2 << 0x10));
			wcd_enable_irq(&wcd9378->irq_info,
						WCD9378_IRQ_HPHR_PDM_WD_INT);
		} else {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
							SLV_BOLERO_EVT_RX_MUTE,
							(WCD_RX3 << 0x10));
			wcd_enable_irq(&wcd9378->irq_info,
						WCD9378_IRQ_AUX_PDM_WD_INT);
		}

		act_ps = snd_soc_component_read(component, WCD9378_PDE23_ACT_PS);
		if (act_ps)
			dev_dbg(component->dev,
				"%s: SA sequencer power on failed\n", __func__);
		else
			dev_dbg(component->dev,
				"%s: SA sequencer power on success\n", __func__);
		break;
	case SND_SOC_DAPM_POST_PMD:
		if (test_bit(RX1_AUX_EN, &wcd9378->sys_usage_status)) {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
							SLV_BOLERO_EVT_RX_MUTE,
							(WCD_RX2 << 0x10 | 0x1));
			wcd_disable_irq(&wcd9378->irq_info,
						WCD9378_IRQ_HPHR_PDM_WD_INT);
		} else {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
							SLV_BOLERO_EVT_RX_MUTE,
							(WCD_RX3 << 0x10 | 0x1));
			wcd_disable_irq(&wcd9378->irq_info,
						WCD9378_IRQ_AUX_PDM_WD_INT);
		}
		break;
	};
	return ret;
}

static int wcd9378_codec_enable_ear_pa(struct snd_soc_dapm_widget *w,
				       struct snd_kcontrol *kcontrol,
				       int event)
{
	struct snd_soc_component *component =
				snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);
	int ret = 0, act_ps = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_swr_slvdev_datapath_control(wcd9378->dev, RX_PATH, true);

		if (test_bit(RX0_EAR_EN, &wcd9378->sys_usage_status)) {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX1 << 0x10));
			wcd_enable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHL_PDM_WD_INT);

		} else {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX3 << 0x10));
			wcd_enable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_AUX_PDM_WD_INT);
		}

		act_ps = snd_soc_component_read(component, WCD9378_PDE23_ACT_PS);
		if (act_ps)
			dev_dbg(component->dev,
				"%s: SA sequencer power on failed\n", __func__);
		else
			dev_dbg(component->dev,
				"%s: SA sequencer power on successful\n", __func__);

		break;
	case SND_SOC_DAPM_POST_PMD:
		if (test_bit(RX0_EAR_EN, &wcd9378->sys_usage_status)) {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX1 << 0x10 | 0x1));
			wcd_disable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_HPHL_PDM_WD_INT);
		} else {
			if (wcd9378->update_wcd_event)
				wcd9378->update_wcd_event(wcd9378->handle,
						SLV_BOLERO_EVT_RX_MUTE,
						(WCD_RX3 << 0x10 | 0x1));
			wcd_disable_irq(&wcd9378->irq_info,
					WCD9378_IRQ_AUX_PDM_WD_INT);
		}
		break;
	};
	return ret;
}

static int wcd9378_get_hph_pwr_level(int hph_mode)
{
	switch (hph_mode) {
	case CLS_H_LOHIFI:
	case CLS_AB_LOHIFI:
		return PWR_LEVEL_LOHIFI_VAL;
	case CLS_H_LP:
	case CLS_AB_LP:
		return PWR_LEVEL_LP_VAL;
	case CLS_H_HIFI:
	case CLS_AB_HIFI:
		return PWR_LEVEL_HIFI_VAL;
	case CLS_H_ULP:
	case CLS_AB:
	case CLS_H_NORMAL:
	default:
		return PWR_LEVEL_ULP_VAL;
	}

	return PWR_LEVEL_ULP_VAL;
}

static void wcd9378_hph_set_channel_volume(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);
	u8 msb_val = 0, lsb_val = 0;

	if ((!wcd9378->comp1_enable) &&
			(!wcd9378->comp2_enable)) {
		msb_val = (wcd9378->hph_gain >> 8);
		lsb_val = (wcd9378->hph_gain & 0x00ff);

		regmap_write(wcd9378->regmap, WCD9378_FU42_CH_VOL_CH1_MSB, msb_val);
		regmap_write(wcd9378->regmap, WCD9378_FU42_CH_VOL_CH1_LSB, lsb_val);

		regmap_write(wcd9378->regmap, WCD9378_FU42_CH_VOL_CH2_MSB, msb_val);
		regmap_write(wcd9378->regmap, WCD9378_FU42_CH_VOL_CH2_LSB, lsb_val);
	}
}

static int wcd9378_hph_sequencer_enable(struct snd_soc_dapm_widget *w,
				struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component =
				snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 =
				snd_soc_component_get_drvdata(component);
	int power_level, ret = 0;
	struct swr_device *swr_dev = wcd9378->tx_swr_dev;
	u8 commit_val = 0x02;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9378_sys_usage_auto_udpate(component, RX0_RX1_HPH_EN, true);

		regmap_write(wcd9378->regmap, WCD9378_CMT_GRP_MASK, 0x02);

		if ((!wcd9378->comp1_enable) || (!wcd9378->comp2_enable)) {
			snd_soc_component_update_bits(component, WCD9378_HPH_UP_T7,
				WCD9378_HPH_UP_T7_HPH_UP_T7_MASK, 0x07);

			snd_soc_component_update_bits(component, WCD9378_HPH_DN_T1,
				WCD9378_HPH_DN_T1_HPH_DN_T1_MASK, 0x07);
		}

		if ((wcd9378->hph_mode == CLS_AB) ||
			(wcd9378->hph_mode == CLS_AB_HIFI) ||
				(wcd9378->hph_mode == CLS_AB_LP) ||
					(wcd9378->hph_mode == CLS_AB_LOHIFI))
			snd_soc_component_update_bits(component, WCD9378_CP_CP_DTOP_CTRL_14,
				WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_VREF_MASK, 0x80);

		/*GET HPH_MODE*/
		power_level = wcd9378_get_hph_pwr_level(wcd9378->hph_mode);

		/*SET HPH_MODE*/
		snd_soc_component_update_bits(component, WCD9378_IT41_USAGE,
				WCD9378_IT41_USAGE_IT41_USAGE_MASK, power_level);

		/*TURN ON HPH SEQUENCER*/
		snd_soc_component_update_bits(component, WCD9378_PDE47_REQ_PS,
				WCD9378_PDE47_REQ_PS_PDE47_REQ_PS_MASK, 0x00);

		wcd9378_hph_set_channel_volume(component);

		if ((!wcd9378->comp1_enable) || (!wcd9378->comp2_enable))
			/*PA delay is 22400us*/
			usleep_range(22500, 22510);
		else
			/*COMP delay is 9400us*/
			usleep_range(9500, 9510);

		regmap_write(wcd9378->regmap, WCD9378_FU42_MUTE_CH1_CN, 0x00);
		regmap_write(wcd9378->regmap, WCD9378_FU42_MUTE_CH2_CN, 0x00);

		if (wcd9378->sys_usage == SYS_USAGE_10)
			/*FU23 UNMUTE*/
			snd_soc_component_update_bits(component, WCD9378_FU23_MUTE,
					WCD9378_FU23_MUTE_FU23_MUTE_MASK, 0x00);

		swr_write(swr_dev, swr_dev->dev_num, 0x004c, &commit_val);

		wcd9378_swr_slvdev_datapath_control(wcd9378->dev, RX_PATH, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		regmap_write(wcd9378->regmap, WCD9378_FU42_MUTE_CH1_CN, 0x01);
		regmap_write(wcd9378->regmap, WCD9378_FU42_MUTE_CH2_CN, 0x01);

		swr_write(swr_dev, swr_dev->dev_num, 0x004c, &commit_val);

		/*TEAR DOWN HPH SEQUENCER*/
		snd_soc_component_update_bits(component, WCD9378_PDE47_REQ_PS,
				WCD9378_PDE47_REQ_PS_PDE47_REQ_PS_MASK, 0x03);

		if (!wcd9378->comp1_enable || !wcd9378->comp2_enable)
			/*PA delay is 24250us*/
			usleep_range(24300, 24310);
		else
			/*COMP delay is 11250us*/
			usleep_range(11300, 11310);

		wcd9378_sys_usage_auto_udpate(component, RX0_RX1_HPH_EN, false);
		break;
	default:
		break;
	};

	return ret;
}

static int wcd9378_codec_ear_dac_event(struct snd_soc_dapm_widget *w,
				       struct snd_kcontrol *kcontrol,
				       int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int ear_rx2 = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	ear_rx2 = snd_soc_component_read(component, WCD9378_CDC_AUX_GAIN_CTL) &
				WCD9378_CDC_AUX_GAIN_CTL_AUX_EN_MASK;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*SHORT_PROT_EN ENABLE*/
		snd_soc_component_update_bits(component, WCD9378_ANA_EAR,
				WCD9378_ANA_EAR_SHORT_PROT_EN_MASK, 0x40);
		if (!ear_rx2) {
			/*RX0 ENABLE*/
			snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
				WCD9378_CDC_HPH_GAIN_CTL_HPHL_RX_EN_MASK, 0x04);

			wcd9378_sys_usage_auto_udpate(component, RX0_EAR_EN, true);
			if (wcd9378->comp1_enable) {
				snd_soc_component_update_bits(component, WCD9378_CDC_COMP_CTL_0,
					WCD9378_CDC_COMP_CTL_0_EAR_COMP_EN_MASK, 0x04);
				wcd9378_rx_connect_port(component, COMP_L, true);
			}
			wcd9378_rx_connect_port(component, HPH_L, true);
		} else {
			wcd9378_sys_usage_auto_udpate(component, RX2_EAR_EN, true);
			/*FORCE CLASS_AB EN*/
			snd_soc_component_update_bits(component, WCD9378_SEQ_OVRRIDE_CTL0,
				WCD9378_SEQ_OVRRIDE_CTL0_CLASSAB_EN_OVR_MASK, 0x20);

			snd_soc_component_update_bits(component, WCD9378_CP_CP_DTOP_CTRL_14,
				WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_VREF_MASK, 0x80);

			if (wcd9378->rx2_clk_mode)
				snd_soc_component_update_bits(component, WCD9378_CDC_PATH_MODE,
					WCD9378_CDC_PATH_MODE_RX2_CLK_RATE_MASK, 0x40);

			wcd9378_rx_connect_port(component, LO, true);
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		/*SHORT_PROT_EN DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_ANA_EAR,
				WCD9378_ANA_EAR_SHORT_PROT_EN_MASK, 0x00);
		if (test_bit(RX0_EAR_EN, &wcd9378->sys_usage_status)) {
			/*RX0 DISABLE*/
			snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
				WCD9378_CDC_HPH_GAIN_CTL_HPHL_RX_EN_MASK, 0x00);
			wcd9378_rx_connect_port(component, HPH_L, false);

			if (wcd9378->comp1_enable) {
				snd_soc_component_update_bits(component, WCD9378_CDC_COMP_CTL_0,
					WCD9378_CDC_COMP_CTL_0_EAR_COMP_EN_MASK, 0x00);
				wcd9378_rx_connect_port(component, COMP_L, false);
			}

			wcd9378_sys_usage_auto_udpate(component, RX0_EAR_EN, false);
		} else {
			wcd9378_rx_connect_port(component, LO, false);
			wcd9378_sys_usage_auto_udpate(component, RX2_EAR_EN, false);
			wcd9378_swr_slvdev_datapath_control(wcd9378->dev, RX_PATH, false);
		}
		break;
	};

	return 0;
}

static int wcd9378_codec_aux_dac_event(struct snd_soc_dapm_widget *w,
				       struct snd_kcontrol *kcontrol,
				       int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	int aux_rx2 = 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	aux_rx2 = snd_soc_component_read(component, WCD9378_CDC_AUX_GAIN_CTL) &
					WCD9378_CDC_AUX_GAIN_CTL_AUX_EN_MASK;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*AUXPA SHORT PROT ENABLE*/
		snd_soc_component_update_bits(component, WCD9378_AUX_AUXPA,
				WCD9378_AUX_AUXPA_AUX_PA_SHORT_PROT_EN_MASK, 0x40);
		if (!aux_rx2) {
			/*RX1 ENABLE*/
			snd_soc_component_update_bits(component, WCD9378_CDC_HPH_GAIN_CTL,
					WCD9378_CDC_HPH_GAIN_CTL_HPHR_RX_EN_MASK, 0x08);

			wcd9378_sys_usage_auto_udpate(component, RX1_AUX_EN, true);
			wcd9378_rx_connect_port(component, HPH_R, true);
		} else {
			wcd9378_sys_usage_auto_udpate(component, RX2_AUX_EN, true);

			if (wcd9378->rx2_clk_mode)
				snd_soc_component_update_bits(component, WCD9378_CDC_PATH_MODE,
					WCD9378_CDC_PATH_MODE_RX2_CLK_RATE_MASK, 0x40);

			wcd9378_rx_connect_port(component, LO, true);
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		/*AUXPA SHORT PROT DISABLE*/
		snd_soc_component_update_bits(component, WCD9378_AUX_AUXPA,
				WCD9378_AUX_AUXPA_AUX_PA_SHORT_PROT_EN_MASK, 0x00);

		if (test_bit(RX1_AUX_EN, &wcd9378->sys_usage_status)) {
			wcd9378_rx_connect_port(component, HPH_R, false);
			wcd9378_sys_usage_auto_udpate(component, RX1_AUX_EN, false);
		} else {
			wcd9378_rx_connect_port(component, LO, false);
			wcd9378_sys_usage_auto_udpate(component, RX2_AUX_EN, false);
			wcd9378_swr_slvdev_datapath_control(wcd9378->dev, RX_PATH, false);
		}
		break;
	};

	return 0;
}

static int wcd9378_sa_sequencer_enable(struct snd_soc_dapm_widget *w,
				struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component =
					snd_soc_dapm_to_component(w->dapm);

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/*TURN ON AMP SEQUENCER*/
		snd_soc_component_update_bits(component, WCD9378_PDE23_REQ_PS,
				WCD9378_PDE23_REQ_PS_PDE23_REQ_PS_MASK, 0x00);
		/*default delay 8550us*/
		usleep_range(8600, 8610);

		/*FU23 UNMUTE*/
		snd_soc_component_update_bits(component, WCD9378_FU23_MUTE,
				WCD9378_FU23_MUTE_FU23_MUTE_MASK, 0x00);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/*FU23 MUTE*/
		snd_soc_component_update_bits(component, WCD9378_FU23_MUTE,
				WCD9378_FU23_MUTE_FU23_MUTE_MASK, 0x01);

		/*TEAR DOWN AMP SEQUENCER*/
		snd_soc_component_update_bits(component, WCD9378_PDE23_REQ_PS,
				WCD9378_PDE23_REQ_PS_PDE23_REQ_PS_MASK, 0x03);
		/*default delay 1530us*/
		usleep_range(15400, 15410);
		break;
	default:
		break;
	};

	return 0;
}

int wcd9378_micbias_control(struct snd_soc_component *component,
				int micb_num, int req, bool is_dapm)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	struct wcd9378_pdata *pdata =
			dev_get_platdata(wcd9378->dev);
	struct wcd9378_micbias_setting *mb = &pdata->micbias;
	int micb_usage = 0, micb_mask = 0, micb_usage_val = 0;
	int pre_off_event = 0, post_off_event = 0;
	int post_on_event = 0, post_dapm_off = 0;
	int post_dapm_on = 0;
	int pull_up_mask = 0, pull_up_en = 0;
	int micb_index = 0, ret = 0;

	switch (micb_num) {
	case MIC_BIAS_1:
		pull_up_mask = WCD9378_MB_PULLUP_EN_MB1_1P8V_OR_PULLUP_SEL_MASK;
		pull_up_en = 0x01;
		micb_usage = WCD9378_IT11_MICB;
		micb_mask = WCD9378_IT11_MICB_IT11_MICB_MASK;
		micb_usage_val = mb->micb1_usage_val;
		break;
	case MIC_BIAS_2:
		pull_up_mask = WCD9378_MB_PULLUP_EN_MB2_1P8V_OR_PULLUP_SEL_MASK;
		pull_up_en = 0x02;
		micb_usage = WCD9378_SMP_MIC_CTRL1_IT11_MICB;
		micb_mask = WCD9378_SMP_MIC_CTRL1_IT11_MICB_IT11_MICB_MASK;
		micb_usage_val = mb->micb2_usage_val;
		pre_off_event = WCD_EVENT_PRE_MICBIAS_2_OFF;
		post_off_event = WCD_EVENT_POST_MICBIAS_2_OFF;
		post_on_event = WCD_EVENT_POST_MICBIAS_2_ON;
		post_dapm_on = WCD_EVENT_POST_DAPM_MICBIAS_2_ON;
		post_dapm_off = WCD_EVENT_POST_DAPM_MICBIAS_2_OFF;
		break;
	case MIC_BIAS_3:
		micb_usage = WCD9378_SMP_MIC_CTRL2_IT11_MICB;
		micb_mask = WCD9378_SMP_MIC_CTRL2_IT11_MICB_IT11_MICB_MASK;
		pull_up_mask = WCD9378_MB_PULLUP_EN_MB3_1P8V_OR_PULLUP_SEL_MASK;
		pull_up_en = 0x04;
		micb_usage_val = mb->micb3_usage_val;
		break;
	default:
		dev_err(component->dev, "%s: Invalid micbias number: %d\n",
			__func__, micb_num);
		return -EINVAL;
	}

	mutex_lock(&wcd9378->micb_lock);

	micb_index = micb_num - 1;
	switch (req) {
	case MICB_PULLUP_ENABLE:
		wcd9378->pullup_ref[micb_index]++;
		if ((wcd9378->pullup_ref[micb_index] == 1) &&
			(wcd9378->micb_ref[micb_index] == 0)) {
			snd_soc_component_update_bits(component, WCD9378_MB_PULLUP_EN,
						pull_up_mask, pull_up_en);
			snd_soc_component_update_bits(component,
						micb_usage, micb_mask, micb_usage_val);

			if (micb_num == MIC_BIAS_2) {
				snd_soc_component_update_bits(component,
						WCD9378_IT31_MICB,
						WCD9378_IT31_MICB_IT31_MICB_MASK,
						micb_usage_val);
				wcd9378->curr_micbias2 = mb->micb2_mv;
			}
		}
		break;
	case MICB_PULLUP_DISABLE:
		if (wcd9378->pullup_ref[micb_index] > 0)
			wcd9378->pullup_ref[micb_index]--;

		if ((wcd9378->pullup_ref[micb_index] == 0) &&
			    (wcd9378->micb_ref[micb_index] == 0)) {
			snd_soc_component_update_bits(component, micb_usage, micb_mask, 0x01);

			if (micb_num == MIC_BIAS_2) {
				snd_soc_component_update_bits(component,
						WCD9378_IT31_MICB,
						WCD9378_IT31_MICB_IT31_MICB_MASK,
						0x01);
				wcd9378->curr_micbias2 = 0;
			}
		}
		break;
	case MICB_ENABLE:
		wcd9378->micb_ref[micb_index]++;
		if (wcd9378->micb_ref[micb_index] == 1) {
			dev_dbg(component->dev, "%s: enable micbias, micb_usage:0x%0x, val:0x%0x\n",
			__func__, micb_usage, micb_usage_val);
			snd_soc_component_update_bits(component,
					micb_usage, micb_mask, micb_usage_val);

			if (micb_num == MIC_BIAS_2) {
				snd_soc_component_update_bits(component,
						WCD9378_ANA_MICB2_RAMP,
						WCD9378_ANA_MICB2_RAMP_SHIFT_CTL_MASK,
						0x0C);
				snd_soc_component_update_bits(component,
						WCD9378_ANA_MICB2_RAMP,
						WCD9378_ANA_MICB2_RAMP_RAMP_ENABLE_MASK,
						0x00);
				snd_soc_component_update_bits(component,
						WCD9378_IT31_MICB,
						WCD9378_IT31_MICB_IT31_MICB_MASK,
						micb_usage_val);
				wcd9378->curr_micbias2 = mb->micb2_mv;
			}
			if (post_on_event)
				blocking_notifier_call_chain(
						&wcd9378->mbhc->notifier,
						post_on_event,
						&wcd9378->mbhc->wcd_mbhc);
		}
		if (is_dapm && post_dapm_on && wcd9378->mbhc)
			blocking_notifier_call_chain(&wcd9378->mbhc->notifier,
						     post_dapm_on,
						     &wcd9378->mbhc->wcd_mbhc);
		break;
	case MICB_DISABLE:
		if (wcd9378->micb_ref[micb_index] > 0)
			wcd9378->micb_ref[micb_index]--;
		if ((wcd9378->micb_ref[micb_index] == 0) &&
			(wcd9378->pullup_ref[micb_index] > 0)) {
			snd_soc_component_update_bits(component, WCD9378_MB_PULLUP_EN,
						pull_up_mask, pull_up_en);

			if (micb_num == MIC_BIAS_2)
				wcd9378->curr_micbias2 = mb->micb2_mv;
		} else if ((wcd9378->micb_ref[micb_index] == 0) &&
			 (wcd9378->pullup_ref[micb_index] == 0)) {
			if (pre_off_event && wcd9378->mbhc)
				blocking_notifier_call_chain(
						&wcd9378->mbhc->notifier,
						pre_off_event,
						&wcd9378->mbhc->wcd_mbhc);
			snd_soc_component_update_bits(component, micb_usage,
							micb_mask, 0x00);
			if (micb_num == MIC_BIAS_2) {
				snd_soc_component_update_bits(component,
						WCD9378_IT31_MICB,
						WCD9378_IT31_MICB_IT31_MICB_MASK,
						0x00);
				snd_soc_component_update_bits(component,
						WCD9378_ANA_MICB2_RAMP,
						WCD9378_ANA_MICB2_RAMP_SHIFT_CTL_MASK,
						0x0C);
				snd_soc_component_update_bits(component,
						WCD9378_ANA_MICB2_RAMP,
						WCD9378_ANA_MICB2_RAMP_RAMP_ENABLE_MASK,
						0x80);
				wcd9378->curr_micbias2 = 0;
			}
			if (post_off_event && wcd9378->mbhc)
				blocking_notifier_call_chain(
						&wcd9378->mbhc->notifier,
						post_off_event,
						&wcd9378->mbhc->wcd_mbhc);
		}
		if (is_dapm && post_dapm_off && wcd9378->mbhc)
			blocking_notifier_call_chain(&wcd9378->mbhc->notifier,
						post_dapm_off,
						&wcd9378->mbhc->wcd_mbhc);
		break;
	default:
		dev_err(component->dev, "%s: Invalid req event: %d\n",
			__func__, req);
		return -EINVAL;
	}

	dev_dbg(component->dev,
		"%s: micb_num:%d, micb_ref: %d, pullup_ref: %d\n",
		__func__, micb_num, wcd9378->micb_ref[micb_index],
		wcd9378->pullup_ref[micb_index]);

	mutex_unlock(&wcd9378->micb_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(wcd9378_micbias_control);


static int wcd9378_get_logical_addr(struct swr_device *swr_dev)
{
	int ret = 0;
	uint8_t devnum = 0;
	int num_retry = NUM_ATTEMPTS;

	do {
		/* retry after 4ms */
		usleep_range(4000, 4010);
		ret = swr_get_logical_dev_num(swr_dev, swr_dev->addr, &devnum);
	} while (ret && --num_retry);

	if (ret)
		dev_err(&swr_dev->dev,
			"%s get devnum %d for dev addr %llx failed\n",
			__func__, devnum, swr_dev->addr);

	swr_dev->dev_num = devnum;
	return 0;
}

static bool get_usbc_hs_status(struct snd_soc_component *component,
			struct wcd_mbhc_config *mbhc_cfg)
{
	if (mbhc_cfg->enable_usbc_analog) {
		if (!(snd_soc_component_read(component, WCD9378_ANA_MBHC_MECH)
			& 0x20))
			return true;
	}
	return false;
}

int wcd9378_swr_dmic_register_notifier(struct snd_soc_component *component,
					struct notifier_block *nblock,
					bool enable)
{
	struct wcd9378_priv *wcd9378_priv = NULL;

	if (component == NULL) {
		pr_err_ratelimited("%s: wcd9378 component is NULL\n", __func__);
		return -EINVAL;
	}

	wcd9378_priv = snd_soc_component_get_drvdata(component);
	wcd9378_priv->notify_swr_dmic = enable;
	if (enable)
		return blocking_notifier_chain_register(&wcd9378_priv->notifier,
							nblock);
	else
		return blocking_notifier_chain_unregister(
				&wcd9378_priv->notifier, nblock);
}
EXPORT_SYMBOL_GPL(wcd9378_swr_dmic_register_notifier);

static int wcd9378_event_notify(struct notifier_block *block,
				unsigned long val,
				void *data)
{
	u16 event = (val & 0xffff);
	int ret = 0;
	struct wcd9378_priv *wcd9378 = dev_get_drvdata((struct device *)data);
	struct snd_soc_component *component = wcd9378->component;
	struct wcd_mbhc *mbhc;
	int rx_clk_type;

	switch (event) {
	case BOLERO_SLV_EVT_TX_CH_HOLD_CLEAR:
		if (test_bit(WCD_ADC1, &wcd9378->status_mask)) {
			snd_soc_component_update_bits(component,
					WCD9378_ANA_TX_CH2, 0x40, 0x00);
			set_bit(WCD_ADC1_MODE, &wcd9378->status_mask);
			clear_bit(WCD_ADC1, &wcd9378->status_mask);
		}
		if (test_bit(WCD_ADC2, &wcd9378->status_mask)) {
			snd_soc_component_update_bits(component,
					WCD9378_ANA_TX_CH2, 0x20, 0x00);
			set_bit(WCD_ADC2_MODE, &wcd9378->status_mask);
			clear_bit(WCD_ADC2, &wcd9378->status_mask);
		}
		if (test_bit(WCD_ADC3, &wcd9378->status_mask)) {
			snd_soc_component_update_bits(component,
					WCD9378_ANA_TX_CH3_HPF, 0x40, 0x00);
			set_bit(WCD_ADC3_MODE, &wcd9378->status_mask);
			clear_bit(WCD_ADC3, &wcd9378->status_mask);
		}
		break;
	case BOLERO_SLV_EVT_PA_OFF_PRE_SSR:
		snd_soc_component_update_bits(component, WCD9378_ANA_HPH,
					0xC0, 0x00);
		snd_soc_component_update_bits(component, WCD9378_ANA_EAR,
					0x80, 0x00);
		snd_soc_component_update_bits(component, WCD9378_AUX_AUXPA,
					0x80, 0x00);
		break;
	case BOLERO_SLV_EVT_SSR_DOWN:
		if (wcd9378->notify_swr_dmic)
			blocking_notifier_call_chain(&wcd9378->notifier,
						WCD9378_EVT_SSR_DOWN,
						NULL);
		wcd9378->mbhc->wcd_mbhc.deinit_in_progress = true;
		mbhc = &wcd9378->mbhc->wcd_mbhc;
		wcd9378->usbc_hs_status = get_usbc_hs_status(component,
						mbhc->mbhc_cfg);
		wcd9378_mbhc_ssr_down(wcd9378->mbhc, component);
		wcd9378_reset_low(wcd9378->dev);
		break;
	case BOLERO_SLV_EVT_SSR_UP:
		wcd9378_reset(wcd9378->dev);
		/* allow reset to take effect */
		usleep_range(10000, 10010);

		wcd9378_get_logical_addr(wcd9378->tx_swr_dev);
		wcd9378_get_logical_addr(wcd9378->rx_swr_dev);

		wcd9378->tx_swr_dev->scp1_val = 0;
		wcd9378->tx_swr_dev->scp2_val = 0;
		wcd9378->rx_swr_dev->scp1_val = 0;
		wcd9378->rx_swr_dev->scp2_val = 0;

		wcd9378_init_reg(component);
		regcache_mark_dirty(wcd9378->regmap);
		regcache_sync(wcd9378->regmap);
		/* Initialize MBHC module */
		mbhc = &wcd9378->mbhc->wcd_mbhc;
		ret = wcd9378_mbhc_post_ssr_init(wcd9378->mbhc, component);
		if (ret) {
			dev_err(component->dev, "%s: mbhc initialization failed\n",
				__func__);
		} else {
			wcd9378_mbhc_hs_detect(component, mbhc->mbhc_cfg);
		}
		wcd9378->mbhc->wcd_mbhc.deinit_in_progress = false;
		if (wcd9378->notify_swr_dmic)
			blocking_notifier_call_chain(&wcd9378->notifier,
						WCD9378_EVT_SSR_UP,
						NULL);
		if (wcd9378->usbc_hs_status)
			mdelay(500);
		break;
	case BOLERO_SLV_EVT_CLK_NOTIFY:
		snd_soc_component_update_bits(component,
				WCD9378_TOP_CLK_CFG, 0x06,
				((val >> 0x10) << 0x01));

		rx_clk_type = (val >> 0x10);

		switch (rx_clk_type) {
		case RX_CLK_12P288MHZ:
			wcd9378->rx_swrclk = SWR_BASECLK_24P576MHZ;
			wcd9378->rx_clkscale = SWR_CLKSCALE_DIV2;
			break;
		case RX_CLK_11P2896MHZ:
			wcd9378->rx_swrclk = SWR_BASECLK_22P5792MHZ;
			wcd9378->rx_clkscale = SWR_CLKSCALE_DIV2;
			break;
		default:
			wcd9378->rx_swrclk = SWR_BASECLK_19P2MHZ;
			wcd9378->rx_clkscale = SWR_CLKSCALE_DIV2;
			break;
		}
		dev_dbg(component->dev, "%s: base_clk:0x%0x, clk_scale:0x%x\n",
				__func__, wcd9378->rx_swrclk, wcd9378->rx_clkscale);

		break;
	default:
		dev_dbg(component->dev, "%s: invalid event %d\n", __func__, event);
		break;
	}
	return 0;
}

static int wcd9378_wakeup(void *handle, bool enable)
{
	struct wcd9378_priv *priv;
	int ret = 0;

	if (!handle) {
		pr_err("%s: NULL handle\n", __func__);
		return -EINVAL;
	}
	priv = (struct wcd9378_priv *)handle;
	if (!priv->tx_swr_dev) {
		pr_err("%s: tx swr dev is NULL\n", __func__);
		return -EINVAL;
	}
	mutex_lock(&priv->wakeup_lock);
	if (enable)
		ret = swr_device_wakeup_vote(priv->tx_swr_dev);
	else
		ret = swr_device_wakeup_unvote(priv->tx_swr_dev);
	mutex_unlock(&priv->wakeup_lock);

	return ret;
}

static inline int wcd9378_tx_path_get(const char *wname,
				      unsigned int *path_num)
{
	int ret = 0;
	char *widget_name = NULL;
	char *w_name = NULL;
	char *path_num_char = NULL;
	char *path_name = NULL;

	widget_name = kstrndup(wname, 9, GFP_KERNEL);
	if (!widget_name)
		return -EINVAL;

	w_name = widget_name;

	path_name = strsep(&widget_name, " ");
	if (!path_name) {
		pr_err("%s: Invalid widget name = %s\n",
			__func__, widget_name);
		ret = -EINVAL;
		goto err;
	}
	path_num_char = strpbrk(path_name, "0123");
	if (!path_num_char) {
		pr_err("%s: tx path index not found\n",
			__func__);
		ret = -EINVAL;
		goto err;
	}
	ret = kstrtouint(path_num_char, 10, path_num);
	if (ret < 0)
		pr_err("%s: Invalid tx path = %s\n",
			__func__, w_name);

err:
	kfree(w_name);
	return ret;
}

static int wcd9378_tx_mode_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = NULL;
	int ret = 0;
	unsigned int path = 0;

	if (!component)
		return -EINVAL;

	wcd9378 = snd_soc_component_get_drvdata(component);

	if (!wcd9378)
		return -EINVAL;

	ret = wcd9378_tx_path_get(kcontrol->id.name, &path);
	if (ret < 0)
		return ret;

	ucontrol->value.integer.value[0] = wcd9378->tx_mode[path];

	return 0;
}

static int wcd9378_tx_mode_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = NULL;
	u32 mode_val;
	unsigned int path = 0;
	int ret = 0;

	if (!component)
		return -EINVAL;

	wcd9378  = snd_soc_component_get_drvdata(component);

	if (!wcd9378)
		return -EINVAL;

	ret = wcd9378_tx_path_get(kcontrol->id.name, &path);
	if (ret)
		return ret;

	mode_val = ucontrol->value.enumerated.item[0];

	dev_dbg(component->dev, "%s: mode: %d\n", __func__, mode_val);

	wcd9378->tx_mode[path] = mode_val;

	return 0;
}

static int wcd9378_loopback_mode_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 loopback_mode = 0;

	if (!component)
		return -EINVAL;

	loopback_mode = (snd_soc_component_read(component, WCD9378_LOOP_BACK_MODE) &
				WCD9378_LOOP_BACK_MODE_LOOPBACK_MODE_MASK);

	ucontrol->value.integer.value[0] = loopback_mode;
	return 0;
}

static int wcd9378_loopback_mode_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 loopback_mode = 0;

	if (!component)
		return -EINVAL;

	loopback_mode = ucontrol->value.enumerated.item[0];

	snd_soc_component_update_bits(component,
			WCD9378_LOOP_BACK_MODE,
			WCD9378_LOOP_BACK_MODE_LOOPBACK_MODE_MASK,
			loopback_mode);

	dev_dbg(component->dev, "%s: loopback_mode: %d\n",
			__func__, loopback_mode);
	return 0;
}

static int wcd9378_aux_dsm_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 aux_dsm_in = 0;

	if (!component)
		return -EINVAL;

	aux_dsm_in = (snd_soc_component_read(component, WCD9378_LB_IN_SEL_CTL) &
				WCD9378_LB_IN_SEL_CTL_AUX_LB_IN_SEL_MASK);

	ucontrol->value.integer.value[0] = aux_dsm_in;
	return 0;
}

static int wcd9378_aux_dsm_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 aux_dsm_in = 0;

	if (!component)
		return -EINVAL;

	aux_dsm_in = ucontrol->value.enumerated.item[0];

	snd_soc_component_update_bits(component,
			WCD9378_LB_IN_SEL_CTL,
			WCD9378_LB_IN_SEL_CTL_AUX_LB_IN_SEL_MASK,
			aux_dsm_in);

	dev_dbg(component->dev, "%s: aux_dsm input: %d\n",
			__func__, aux_dsm_in);
	return 0;
}

static int wcd9378_hph_dsm_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 hph_dsm_in = 0;

	if (!component)
		return -EINVAL;

	hph_dsm_in = (snd_soc_component_read(component, WCD9378_LB_IN_SEL_CTL) &
				WCD9378_LB_IN_SEL_CTL_HPH_LB_IN_SEL_MASK);

	ucontrol->value.integer.value[0] = hph_dsm_in;
	return 0;
}

static int wcd9378_hph_dsm_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	u32 hph_dsm_in = 0;

	if (!component)
		return -EINVAL;

	hph_dsm_in = ucontrol->value.enumerated.item[0];

	snd_soc_component_update_bits(component,
			WCD9378_LB_IN_SEL_CTL,
			WCD9378_LB_IN_SEL_CTL_HPH_LB_IN_SEL_MASK,
			hph_dsm_in);

	dev_dbg(component->dev, "%s: hph_dsm input: %d\n",
			__func__, hph_dsm_in);
	return 0;
}

static int wcd9378_hph_put_gain(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	u16 offset = ucontrol->value.enumerated.item[0];
	u32 temp = 0;

	temp = 0x00 - offset * 0x180;

	wcd9378->hph_gain = (u16)(temp & 0xffff);

	dev_dbg(component->dev, "%s: hph gain is 0x%0x\n", __func__, wcd9378->hph_gain);
	return 0;
}

static int wcd9378_hph_get_gain(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	u32 temp = 0;
	u16 offset =  0;

	temp = 0 - wcd9378->hph_gain;

	offset = (u16)(temp & 0xffff);

	offset /= 0x180;
	ucontrol->value.enumerated.item[0] = offset;

	dev_dbg(component->dev, "%s: offset is 0x%0x\n", __func__, offset);
	return 0;
}

static int wcd9378_ear_pa_gain_get(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	int ear_gain = 0;

	if (component == NULL)
		return -EINVAL;

	ear_gain =
		snd_soc_component_read(component, WCD9378_ANA_EAR_COMPANDER_CTL) &
				WCD9378_ANA_EAR_COMPANDER_CTL_EAR_GAIN_MASK;

	ucontrol->value.enumerated.item[0] = ear_gain >> 2;
	dev_dbg(component->dev, "%s: get ear_gain val: 0x%x\n",
			__func__, ear_gain);
	return 0;
}

static int wcd9378_ear_pa_gain_put(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	int ear_gain = 0;

	if (component == NULL)
		return -EINVAL;

	if (ucontrol->value.integer.value[0] < 0 ||
		ucontrol->value.integer.value[0] > 0x10) {
		dev_err(component->dev, "%s: Unsupported gain val %ld\n",
			 __func__, ucontrol->value.integer.value[0]);
		return -EINVAL;
	}

	ear_gain = ucontrol->value.integer.value[0];
	ear_gain = ear_gain << 2;
	snd_soc_component_update_bits(component, WCD9378_ANA_EAR_COMPANDER_CTL,
				WCD9378_ANA_EAR_COMPANDER_CTL_EAR_GAIN_MASK,
				ear_gain);
	dev_dbg(component->dev, "%s: set ear_gain val: 0x%x\n",
			__func__, ear_gain);
	return 0;
}

static int wcd9378_aux_pa_gain_get(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	int aux_gain = 0;

	if (component == NULL)
		return -EINVAL;

	aux_gain = snd_soc_component_read(component, WCD9378_AUX_INT_MISC) &
			WCD9378_AUX_INT_MISC_PA_GAIN_MASK;

	ucontrol->value.enumerated.item[0] = aux_gain;
	dev_dbg(component->dev, "%s: get aux_gain val: 0x%x\n",
			__func__, aux_gain);
	return 0;
}

static int wcd9378_aux_pa_gain_put(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	int aux_gain = 0;

	if (component == NULL)
		return -EINVAL;

	if (ucontrol->value.integer.value[0] < 0 ||
		ucontrol->value.integer.value[0] > 0x8) {
		dev_err(component->dev, "%s: Unsupported gain val %ld\n",
			__func__, ucontrol->value.integer.value[0]);
		return -EINVAL;
	}

	aux_gain = ucontrol->value.integer.value[0];
	snd_soc_component_update_bits(component, WCD9378_AUX_INT_MISC,
				WCD9378_AUX_INT_MISC_PA_GAIN_MASK,
				aux_gain);
	dev_dbg(component->dev, "%s: set aux_gain val: 0x%x\n",
			 __func__, aux_gain);
	return 0;
}

static int wcd9378_rx2_mode_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);

	if (ucontrol->value.enumerated.item[0])
		wcd9378->rx2_clk_mode = RX2_NORMAL_MODE;
	else
		wcd9378->rx2_clk_mode = RX2_HP_MODE;
	return 1;
}

static int wcd9378_rx_hph_mode_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	ucontrol->value.enumerated.item[0] = wcd9378->hph_mode;

	return 0;
}

static int wcd9378_rx_hph_mode_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	if (wcd9378->hph_mode == ucontrol->value.enumerated.item[0])
		return 0;

	wcd9378->hph_mode = ucontrol->value.enumerated.item[0];

	return 1;
}

/* wcd9378_codec_get_dev_num - returns swr device number
 * @component: Codec instance
 *
 * Return: swr device number on success or negative error
 * code on failure.
 */
int wcd9378_codec_get_dev_num(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378;

	if (!component)
		return -EINVAL;

	wcd9378 = snd_soc_component_get_drvdata(component);
	if (!wcd9378 || !wcd9378->rx_swr_dev) {
		pr_err("%s: wcd9378 component is NULL\n", __func__);
		return -EINVAL;
	}

	return wcd9378->rx_swr_dev->dev_num;
}
EXPORT_SYMBOL_GPL(wcd9378_codec_get_dev_num);

static int wcd9378_get_compander(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	bool hphr;
	struct soc_multi_mixer_control *mc;

	mc = (struct soc_multi_mixer_control *)(kcontrol->private_value);
	hphr = mc->shift;

	ucontrol->value.integer.value[0] = hphr ? wcd9378->comp2_enable :
						wcd9378->comp1_enable;
	return 0;
}

static int wcd9378_set_compander(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	int value = ucontrol->value.integer.value[0];
	bool hphr;
	struct soc_multi_mixer_control *mc;

	mc = (struct soc_multi_mixer_control *)(kcontrol->private_value);
	hphr = mc->shift;
	if (hphr)
		wcd9378->comp2_enable = value;
	else
		wcd9378->comp1_enable = value;

	dev_dbg(component->dev, "%s: set compander: %d\n", __func__, value);

	return 0;
}

static int wcd9378_codec_enable_vdd_buck(struct snd_soc_dapm_widget *w,
					 struct snd_kcontrol *kcontrol,
					 int event)
{
	struct snd_soc_component *component =
			snd_soc_dapm_to_component(w->dapm);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	struct wcd9378_pdata *pdata = NULL;
	int ret = 0;

	pdata = dev_get_platdata(wcd9378->dev);

	if (!pdata) {
		dev_err(component->dev, "%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (!msm_cdc_is_ondemand_supply(wcd9378->dev,
					wcd9378->supplies,
					pdata->regulator,
					pdata->num_supplies,
					"cdc-vdd-buck"))
		return 0;

	dev_dbg(component->dev, "%s wname: %s event: %d\n", __func__,
		w->name, event);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		if (test_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask)) {
			dev_dbg(component->dev,
				"%s: buck already in enabled state\n",
				__func__);
			clear_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask);
			return 0;
		}
		ret = msm_cdc_enable_ondemand_supply(wcd9378->dev,
						wcd9378->supplies,
						pdata->regulator,
						pdata->num_supplies,
						"cdc-vdd-buck");
		if (ret == -EINVAL) {
			dev_err(component->dev, "%s: vdd buck is not enabled\n",
				__func__);
			return ret;
		}
		clear_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask);
		/*
		 * 200us sleep is required after LDO is enabled as per
		 * HW requirement
		 */
		usleep_range(200, 250);
		break;
	case SND_SOC_DAPM_POST_PMD:
		set_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask);
		break;
	}
	return 0;
}

static void wcd9378_tx_get_slave_ch_type_idx(const char *wname, int *ch_idx)
{
	u8 ch_type = 0;

	if (strnstr(wname, "ADC1", sizeof("ADC1")))
		ch_type = ADC1;
	else if (strnstr(wname, "ADC2", sizeof("ADC2")))
		ch_type = ADC2;
	else if (strnstr(wname, "ADC3", sizeof("ADC3")))
		ch_type = ADC3;
	else if (strnstr(wname, "ADC4", sizeof("ADC4")))
		ch_type = ADC4;
	else if (strnstr(wname, "DMIC0", sizeof("DMIC0")))
		ch_type = DMIC0;
	else if (strnstr(wname, "DMIC1", sizeof("DMIC1")))
		ch_type = DMIC1;
	else if (strnstr(wname, "MBHC", sizeof("MBHC")))
		ch_type = MBHC;
	else if (strnstr(wname, "DMIC2", sizeof("DMIC2")))
		ch_type = DMIC2;
	else if (strnstr(wname, "DMIC3", sizeof("DMIC3")))
		ch_type = DMIC3;
	else if (strnstr(wname, "DMIC4", sizeof("DMIC4")))
		ch_type = DMIC4;
	else if (strnstr(wname, "DMIC5", sizeof("DMIC5")))
		ch_type = DMIC5;
	else
		pr_err("%s: port name: %s is not listed\n", __func__, wname);

	if (ch_type)
		*ch_idx = wcd9378_slave_get_slave_ch_val(ch_type);
	else
		*ch_idx = -EINVAL;
}

static int wcd9378_tx_master_ch_get(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = NULL;
	int slave_ch_idx = -EINVAL;

	if (component == NULL)
		return -EINVAL;

	wcd9378 = snd_soc_component_get_drvdata(component);
	if (wcd9378 == NULL)
		return -EINVAL;

	wcd9378_tx_get_slave_ch_type_idx(kcontrol->id.name, &slave_ch_idx);
	if (slave_ch_idx < 0 || slave_ch_idx >= WCD9378_MAX_SLAVE_CH_TYPES)
		return -EINVAL;

	ucontrol->value.integer.value[0] = wcd9378_slave_get_master_ch_val(
			wcd9378->tx_master_ch_map[slave_ch_idx]);

	return 0;
}

static int wcd9378_tx_master_ch_put(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = NULL;
	int slave_ch_idx = -EINVAL, idx = 0;

	if (component == NULL)
		return -EINVAL;

	wcd9378 = snd_soc_component_get_drvdata(component);
	if (wcd9378 == NULL)
		return -EINVAL;

	wcd9378_tx_get_slave_ch_type_idx(kcontrol->id.name, &slave_ch_idx);

	if (slave_ch_idx < 0 || slave_ch_idx >= WCD9378_MAX_SLAVE_CH_TYPES)
		return -EINVAL;

	dev_dbg(component->dev, "%s: slave_ch_idx: %d", __func__, slave_ch_idx);
	dev_dbg(component->dev, "%s: ucontrol->value.enumerated.item[0] = %ld\n",
			__func__, ucontrol->value.enumerated.item[0]);

	idx = ucontrol->value.enumerated.item[0];
	if (idx < 0 || idx >= ARRAY_SIZE(wcd9378_swr_master_ch_map))
		return -EINVAL;

	wcd9378->tx_master_ch_map[slave_ch_idx] = wcd9378_slave_get_master_ch(idx);
	return 0;
}

static int wcd9378_bcs_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
				snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	ucontrol->value.integer.value[0] = wcd9378->bcs_dis;

	return 0;
}

static int wcd9378_bcs_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
			snd_soc_kcontrol_component(kcontrol);
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	wcd9378->bcs_dis = ucontrol->value.integer.value[0];

	return 0;
}

static const char * const loopback_mode_text[] = {
	"NO_LP", "SWR_LP1", "SWR_LP2", "SWR_LP3",
};

static const struct soc_enum loopback_mode_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(loopback_mode_text),
			    loopback_mode_text);

static const char * const aux_dsm_text[] = {
	"TX2->AUX", "TX3->AUX", "TX0->AUX", "TX1->AUX",
};

static const struct soc_enum aux_dsm_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(aux_dsm_text),
			    aux_dsm_text);

static const char * const hph_dsm_text[] = {
	"HPH_DSM_IN0", "HPH_DSM_IN1", "HPH_DSM_IN2", "HPH_DSM_IN3",
};

static const struct soc_enum hph_dsm_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(hph_dsm_text),
			    hph_dsm_text);

static const char * const tx_mode_mux_text[] = {
	"ADC_INVALID", "ADC_HIFI", "ADC_NORMAL", "ADC_LP",
};

static const struct soc_enum tx_mode_mux_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(tx_mode_mux_text),
			    tx_mode_mux_text);

static const char * const rx2_mode_text[] = {
	"HP", "NORMAL",
};

static const struct soc_enum rx2_mode_enum =
		SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(rx2_mode_text),
				    rx2_mode_text);

static const char * const rx_hph_mode_mux_text[] = {
	"CLS_H_INVALID", "CLS_H_HIFI", "CLS_H_LP", "CLS_AB", "CLS_H_LOHIFI",
	"CLS_H_ULP", "CLS_AB_HIFI", "CLS_AB_LP", "CLS_AB_LOHIFI",
};

static const struct soc_enum rx_hph_mode_mux_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(rx_hph_mode_mux_text),
			    rx_hph_mode_mux_text);


static const char * const ear_pa_gain_text[] = {
	"GAIN_6DB", "GAIN_4P5DB", "GAIN_3DB", "GAIN_1P5DB", "GAIN_0DB",
	"GAIN_M1P5DB", "GAIN_M3DB", "GAIN_M4P5DB", "GAIN_M6DB",
	"GAIN_M7P5DB", "GAIN_M9DB", "GAIN_M10P5DB", "GAIN_M12DB",
	"GAIN_M13P5DB", "GAIN_M15DB", "GAIN_M16P5DB", "GAIN_M18DB",
};

static const struct soc_enum ear_pa_gain_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(ear_pa_gain_text),
			    ear_pa_gain_text);

static const char * const aux_pa_gain_text[] = {
	"GAIN_6DB", "GAIN_4P5DB", "GAIN_3DB", "GAIN_1P5DB", "GAIN_0DB",
	"GAIN_M1P5DB", "GAIN_M3DB", "GAIN_M4P5DB", "GAIN_M6DB",
};

static const struct soc_enum aux_pa_gain_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(aux_pa_gain_text),
			    aux_pa_gain_text);

const char * const tx_master_ch_text[] = {
	"ZERO", "SWRM_TX1_CH1", "SWRM_TX1_CH2", "SWRM_TX1_CH3", "SWRM_TX1_CH4",
	"SWRM_TX2_CH1", "SWRM_TX2_CH2", "SWRM_TX2_CH3", "SWRM_TX2_CH4",
	"SWRM_TX3_CH1", "SWRM_TX3_CH2", "SWRM_TX3_CH3", "SWRM_TX3_CH4",
	"SWRM_PCM_IN",
};

const struct soc_enum tx_master_ch_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(tx_master_ch_text),
					tx_master_ch_text);

static const struct snd_kcontrol_new wcd9378_snd_controls[] = {
	SOC_SINGLE_EXT("HPHL_COMP Switch", SND_SOC_NOPM, 0, 1, 0,
		wcd9378_get_compander, wcd9378_set_compander),
	SOC_SINGLE_EXT("HPHR_COMP Switch", SND_SOC_NOPM, 1, 1, 0,
		wcd9378_get_compander, wcd9378_set_compander),
	SOC_SINGLE_EXT("ADC2_BCS Disable", SND_SOC_NOPM, 0, 1, 0,
		wcd9378_bcs_get, wcd9378_bcs_put),

	SOC_ENUM_EXT("LOOPBACK Mode", loopback_mode_enum,
		     wcd9378_loopback_mode_get, wcd9378_loopback_mode_put),
	SOC_ENUM_EXT("AUX_LB_IN SEL", aux_dsm_enum,
		     wcd9378_aux_dsm_get, wcd9378_aux_dsm_put),
	SOC_ENUM_EXT("HPH_LB_IN SEL", hph_dsm_enum,
		     wcd9378_hph_dsm_get, wcd9378_hph_dsm_put),

	SOC_ENUM_EXT("TX0 MODE", tx_mode_mux_enum,
		     wcd9378_tx_mode_get, wcd9378_tx_mode_put),
	SOC_ENUM_EXT("TX1 MODE", tx_mode_mux_enum,
		     wcd9378_tx_mode_get, wcd9378_tx_mode_put),
	SOC_ENUM_EXT("TX2 MODE", tx_mode_mux_enum,
		     wcd9378_tx_mode_get, wcd9378_tx_mode_put),

	SOC_ENUM_EXT("RX2 Mode", rx2_mode_enum,
			NULL, wcd9378_rx2_mode_put),
	SOC_ENUM_EXT("RX HPH Mode", rx_hph_mode_mux_enum,
			wcd9378_rx_hph_mode_get, wcd9378_rx_hph_mode_put),
	SOC_SINGLE_EXT("HPH Volume", SND_SOC_NOPM, 0, 0x14, 0,
			wcd9378_hph_get_gain, wcd9378_hph_put_gain),
	SOC_ENUM_EXT("EAR_PA Gain", ear_pa_gain_enum,
			wcd9378_ear_pa_gain_get, wcd9378_ear_pa_gain_put),
	SOC_ENUM_EXT("AUX_PA Gain", aux_pa_gain_enum,
			wcd9378_aux_pa_gain_get, wcd9378_aux_pa_gain_put),

	SOC_SINGLE_TLV("ADC1 Volume", WCD9378_ANA_TX_CH1, 0, 20, 0,
			analog_gain),
	SOC_SINGLE_TLV("ADC2 Volume", WCD9378_ANA_TX_CH2, 0, 20, 0,
			analog_gain),
	SOC_SINGLE_TLV("ADC3 Volume", WCD9378_ANA_TX_CH3, 0, 20, 0,
			analog_gain),

	SOC_ENUM_EXT("ADC1 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("ADC2 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("ADC3 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC0 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC1 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("MBHC ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC2 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC3 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC4 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
	SOC_ENUM_EXT("DMIC5 ChMap", tx_master_ch_enum,
			wcd9378_tx_master_ch_get, wcd9378_tx_master_ch_put),
};

static const struct snd_kcontrol_new amic1_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new amic2_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new amic3_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new amic4_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new va_amic1_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new va_amic2_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new va_amic3_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new va_amic4_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic1_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic2_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic3_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic4_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic5_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dmic6_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const char * const adc1_mux_text[] = {
	"CH1_AMIC_DISABLE", "CH1_AMIC1", "CH1_AMIC2", "CH1_AMIC3", "CH1_AMIC4"
};

static const char * const adc2_mux_text[] = {
	"CH2_AMIC_DISABLE", "CH2_AMIC1", "CH2_AMIC2", "CH2_AMIC3", "CH2_AMIC4"
};

static const char * const adc3_mux_text[] = {
	"CH3_AMIC_DISABLE", "CH3_AMIC1", "CH3_AMIC3", "CH3_AMIC4"
};

static const char * const ear_mux_text[] = {
	"RX0", "RX2"
};

static const char * const aux_mux_text[] = {
	"RX1", "RX2"
};

static const struct soc_enum adc1_enum =
		SOC_ENUM_SINGLE(WCD9378_TX_NEW_TX_CH12_MUX,
				WCD9378_TX_NEW_TX_CH12_MUX_CH1_SEL_SHIFT,
				ARRAY_SIZE(adc1_mux_text), adc1_mux_text);

static const struct soc_enum adc2_enum =
		SOC_ENUM_SINGLE(WCD9378_TX_NEW_TX_CH12_MUX,
				WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_SHIFT,
				ARRAY_SIZE(adc2_mux_text), adc2_mux_text);

static const struct soc_enum adc3_enum =
		SOC_ENUM_SINGLE(WCD9378_TX_NEW_TX_CH34_MUX,
				WCD9378_TX_NEW_TX_CH34_MUX_CH3_SEL_SHIFT,
				ARRAY_SIZE(adc3_mux_text), adc3_mux_text);

static const struct soc_enum ear_enum =
		SOC_ENUM_SINGLE(WCD9378_CDC_AUX_GAIN_CTL,
				WCD9378_CDC_AUX_GAIN_CTL_AUX_EN_SHIFT,
				ARRAY_SIZE(ear_mux_text), ear_mux_text);

static const struct soc_enum aux_enum =
		SOC_ENUM_SINGLE(WCD9378_CDC_AUX_GAIN_CTL,
				WCD9378_CDC_AUX_GAIN_CTL_AUX_EN_SHIFT,
				ARRAY_SIZE(aux_mux_text), aux_mux_text);

static const struct snd_kcontrol_new tx_adc1_mux =
	SOC_DAPM_ENUM("ADC1 MUX Mux", adc1_enum);

static const struct snd_kcontrol_new tx_adc2_mux =
	SOC_DAPM_ENUM("ADC2 MUX Mux", adc2_enum);

static const struct snd_kcontrol_new tx_adc3_mux =
	SOC_DAPM_ENUM("ADC3 MUX Mux", adc3_enum);

static const struct snd_kcontrol_new ear_mux =
	SOC_DAPM_ENUM("EAR Mux", ear_enum);

static const struct snd_kcontrol_new aux_mux =
	SOC_DAPM_ENUM("AUX Mux", aux_enum);

static const struct snd_kcontrol_new dac1_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new dac2_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new ear_mixer_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new aux_mixer_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new hphl_rdac_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new hphr_rdac_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new rx0_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_kcontrol_new rx1_switch[] = {
	SOC_DAPM_SINGLE("Switch", SND_SOC_NOPM, 0, 1, 0)
};

static const struct snd_soc_dapm_widget wcd9378_dapm_widgets[] = {

	/*input widgets*/
	SND_SOC_DAPM_INPUT("AMIC1"),
	SND_SOC_DAPM_INPUT("AMIC2"),
	SND_SOC_DAPM_INPUT("AMIC3"),
	SND_SOC_DAPM_INPUT("AMIC4"),

	SND_SOC_DAPM_INPUT("VA AMIC1"),
	SND_SOC_DAPM_INPUT("VA AMIC2"),
	SND_SOC_DAPM_INPUT("VA AMIC3"),
	SND_SOC_DAPM_INPUT("VA AMIC4"),

	SND_SOC_DAPM_INPUT("IN1_HPHL"),
	SND_SOC_DAPM_INPUT("IN2_HPHR"),
	SND_SOC_DAPM_INPUT("IN3_AUX"),

	/*tx widgets*/
	SND_SOC_DAPM_MIXER_E("TX0 SEQUENCER", SND_SOC_NOPM, ADC1, 0,
				NULL, 0, wcd9378_tx_sequencer_enable,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("TX1 SEQUENCER", SND_SOC_NOPM, ADC2, 0,
				NULL, 0, wcd9378_tx_sequencer_enable,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("TX2 SEQUENCER", SND_SOC_NOPM, ADC3, 0,
				NULL, 0, wcd9378_tx_sequencer_enable,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_MUX("ADC1 MUX", SND_SOC_NOPM, 0, 0,
				&tx_adc1_mux),
	SND_SOC_DAPM_MUX("ADC2 MUX", SND_SOC_NOPM, 0, 0,
				&tx_adc2_mux),
	SND_SOC_DAPM_MUX("ADC3 MUX", SND_SOC_NOPM, 0, 0,
				&tx_adc3_mux),

	SND_SOC_DAPM_ADC_E("DMIC1", NULL, SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_ADC_E("DMIC2", NULL, SND_SOC_NOPM, 1, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_ADC_E("DMIC3", NULL, SND_SOC_NOPM, 2, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_ADC_E("DMIC4", NULL, SND_SOC_NOPM, 3, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_ADC_E("DMIC5", NULL, SND_SOC_NOPM, 4, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_ADC_E("DMIC6", NULL, SND_SOC_NOPM, 5, 0,
				wcd9378_codec_enable_dmic,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	/*rx widgets*/
	SND_SOC_DAPM_DAC_E("RDAC1", NULL, SND_SOC_NOPM, 0, 0,
			   wcd9378_codec_hphl_dac_event,
			   SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_DAC_E("RDAC2", NULL, SND_SOC_NOPM, 0, 0,
			   wcd9378_codec_hphr_dac_event,
			   SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("HPH SEQUENCER", SND_SOC_NOPM, 0, 0, NULL, 0,
				wcd9378_hph_sequencer_enable,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("HPHL PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
				wcd9378_codec_enable_hphl_pa,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("HPHR PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
				wcd9378_codec_enable_hphr_pa,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_MIXER_E("SA SEQUENCER", SND_SOC_NOPM, 0, 0,
				NULL, 0, wcd9378_sa_sequencer_enable,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_DAC_E("EAR_RDAC", NULL, SND_SOC_NOPM, 0, 0,
				wcd9378_codec_ear_dac_event,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_DAC_E("AUX_RDAC", NULL, SND_SOC_NOPM, 0, 0,
				wcd9378_codec_aux_dac_event,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_PGA_E("EAR PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
				wcd9378_codec_enable_ear_pa,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("AUX PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
				wcd9378_codec_enable_aux_pa,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_SUPPLY("VDD_BUCK", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_vdd_buck,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_SUPPLY_S("CLS_H_PORT", 1, SND_SOC_NOPM, 0, 0,
				wcd9378_enable_clsh,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_MIXER_E("AMIC1_MIXER", SND_SOC_NOPM, 0, 0,
				amic1_switch, ARRAY_SIZE(amic1_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("AMIC2_MIXER", SND_SOC_NOPM, 0, 0,
				amic2_switch, ARRAY_SIZE(amic2_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("AMIC3_MIXER", SND_SOC_NOPM, 0, 0,
				amic3_switch, ARRAY_SIZE(amic3_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("AMIC4_MIXER", SND_SOC_NOPM, 0, 0,
				amic4_switch, ARRAY_SIZE(amic4_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("VA_AMIC1_MIXER", SND_SOC_NOPM, 0, 0,
				va_amic1_switch, ARRAY_SIZE(va_amic1_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("VA_AMIC2_MIXER", SND_SOC_NOPM, 0, 0,
				va_amic2_switch, ARRAY_SIZE(va_amic2_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("VA_AMIC3_MIXER", SND_SOC_NOPM, 0, 0,
				va_amic3_switch, ARRAY_SIZE(va_amic3_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("VA_AMIC4_MIXER", SND_SOC_NOPM, 0, 0,
				va_amic4_switch, ARRAY_SIZE(va_amic4_switch), NULL,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_MIXER_E("DMIC1_MIXER", SND_SOC_NOPM, DMIC1,
				0, dmic1_switch, ARRAY_SIZE(dmic1_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("DMIC2_MIXER", SND_SOC_NOPM, DMIC2,
				0, dmic2_switch, ARRAY_SIZE(dmic2_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("DMIC3_MIXER", SND_SOC_NOPM, DMIC3,
				0, dmic3_switch, ARRAY_SIZE(dmic3_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("DMIC4_MIXER", SND_SOC_NOPM, DMIC4,
				0, dmic4_switch, ARRAY_SIZE(dmic4_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("DMIC5_MIXER", SND_SOC_NOPM, DMIC5,
				0, dmic5_switch, ARRAY_SIZE(dmic5_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_MIXER_E("DMIC6_MIXER", SND_SOC_NOPM, DMIC6,
				0, dmic6_switch, ARRAY_SIZE(dmic6_switch),
				wcd9378_tx_swr_ctrl, SND_SOC_DAPM_PRE_PMU |
				SND_SOC_DAPM_POST_PMD),

	/* micbias widgets*/
	SND_SOC_DAPM_SUPPLY("MIC BIAS1", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("MIC BIAS2", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("MIC BIAS3", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),

	/* micbias pull up widgets*/
	SND_SOC_DAPM_SUPPLY("VA MIC BIAS1", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias_pullup,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("VA MIC BIAS2", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias_pullup,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("VA MIC BIAS3", SND_SOC_NOPM, 0, 0,
				wcd9378_codec_enable_micbias_pullup,
				SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU |
				SND_SOC_DAPM_POST_PMD),

	/* rx mixer widgets*/
	SND_SOC_DAPM_MUX("EAR_MUX", SND_SOC_NOPM, 0, 0, &ear_mux),
	SND_SOC_DAPM_MUX("AUX_MUX", SND_SOC_NOPM, 0, 0, &aux_mux),
	SND_SOC_DAPM_MIXER("EAR_MIXER", SND_SOC_NOPM, 0, 0,
			ear_mixer_switch, ARRAY_SIZE(ear_mixer_switch)),
	SND_SOC_DAPM_MIXER("AUX_MIXER", SND_SOC_NOPM, 0, 0,
			aux_mixer_switch, ARRAY_SIZE(aux_mixer_switch)),
	SND_SOC_DAPM_MIXER("DAC1", SND_SOC_NOPM, 0, 0,
			dac1_switch, ARRAY_SIZE(dac1_switch)),
	SND_SOC_DAPM_MIXER("DAC2", SND_SOC_NOPM, 0, 0,
			dac2_switch, ARRAY_SIZE(dac2_switch)),
	SND_SOC_DAPM_MIXER("HPHL_RDAC", SND_SOC_NOPM, 0, 0,
			hphl_rdac_switch, ARRAY_SIZE(hphl_rdac_switch)),
	SND_SOC_DAPM_MIXER("HPHR_RDAC", SND_SOC_NOPM, 0, 0,
			hphr_rdac_switch, ARRAY_SIZE(hphr_rdac_switch)),

	/*output widgets tx*/
	SND_SOC_DAPM_OUTPUT("ADC1_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("ADC2_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("ADC3_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC1_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC2_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC3_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC4_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC5_OUTPUT"),
	SND_SOC_DAPM_OUTPUT("DMIC6_OUTPUT"),

	/*output widgets rx*/
	SND_SOC_DAPM_OUTPUT("EAR"),
	SND_SOC_DAPM_OUTPUT("AUX"),
	SND_SOC_DAPM_OUTPUT("HPHL"),
	SND_SOC_DAPM_OUTPUT("HPHR"),
};

static const struct snd_soc_dapm_route wcd9378_audio_map[] = {
/*ADC-1 (channel-1)*/
	{"ADC1_OUTPUT", NULL, "TX0 SEQUENCER"},
	{"TX0 SEQUENCER", NULL, "ADC1 MUX"},
	{"ADC1 MUX", "CH1_AMIC1", "AMIC1_MIXER"},
	{"ADC1 MUX", "CH1_AMIC2", "AMIC2_MIXER"},
	{"ADC1 MUX", "CH1_AMIC3", "AMIC3_MIXER"},
	{"ADC1 MUX", "CH1_AMIC4", "AMIC4_MIXER"},

/*ADC-2 (channel-2)*/
	{"ADC2_OUTPUT", NULL, "TX1 SEQUENCER"},
	{"TX1 SEQUENCER", NULL, "ADC2 MUX"},
	{"ADC2 MUX", "CH2_AMIC1", "AMIC1_MIXER"},
	{"ADC2 MUX", "CH2_AMIC2", "AMIC2_MIXER"},
	{"ADC2 MUX", "CH2_AMIC3", "AMIC3_MIXER"},
	{"ADC2 MUX", "CH2_AMIC4", "AMIC4_MIXER"},

/*ADC-3 (channel-3)*/
	{"ADC3_OUTPUT", NULL, "TX2 SEQUENCER"},
	{"TX2 SEQUENCER", NULL, "ADC3 MUX"},
	{"ADC3 MUX", "CH3_AMIC1", "AMIC1_MIXER"},
	{"ADC3 MUX", "CH3_AMIC3", "AMIC3_MIXER"},
	{"ADC3 MUX", "CH3_AMIC4", "AMIC4_MIXER"},

	{"AMIC1_MIXER", "Switch", "AMIC1"},
	{"AMIC1_MIXER", NULL, "VA_AMIC1_MIXER"},
	{"VA_AMIC1_MIXER", "Switch", "VA AMIC1"},

	{"AMIC2_MIXER", "Switch", "AMIC2"},
	{"AMIC2_MIXER", NULL, "VA_AMIC2_MIXER"},
	{"VA_AMIC2_MIXER", "Switch", "VA AMIC2"},

	{"AMIC3_MIXER", "Switch", "AMIC3"},
	{"AMIC3_MIXER", NULL, "VA_AMIC3_MIXER"},
	{"VA_AMIC3_MIXER", "Switch", "VA AMIC3"},

	{"AMIC4_MIXER", "Switch", "AMIC4"},
	{"AMIC4_MIXER", NULL, "VA_AMIC4_MIXER"},
	{"VA_AMIC4_MIXER", "Switch", "VA AMIC4"},

	{"DMIC1_OUTPUT", NULL, "DMIC1_MIXER"},
	{"DMIC1_MIXER", "Switch", "DMIC1"},

	{"DMIC2_OUTPUT", NULL, "DMIC2_MIXER"},
	{"DMIC2_MIXER", "Switch", "DMIC2"},

	{"DMIC3_OUTPUT", NULL, "DMIC3_MIXER"},
	{"DMIC3_MIXER", "Switch", "DMIC3"},

	{"DMIC4_OUTPUT", NULL, "DMIC4_MIXER"},
	{"DMIC4_MIXER", "Switch", "DMIC4"},

	{"DMIC5_OUTPUT", NULL, "DMIC5_MIXER"},
	{"DMIC5_MIXER", "Switch", "DMIC5"},

	{"DMIC6_OUTPUT", NULL, "DMIC6_MIXER"},
	{"DMIC6_MIXER", "Switch", "DMIC6"},

/*Headphone playback*/
	{"IN1_HPHL", NULL, "VDD_BUCK"},
	{"IN1_HPHL", NULL, "CLS_H_PORT"},
	{"HPH SEQUENCER", NULL, "IN1_HPHL"},
	{"RDAC1", NULL, "HPH SEQUENCER"},
	{"HPHL_RDAC", "Switch", "RDAC1"},
	{"HPHL PGA", NULL, "HPHL_RDAC"},
	{"HPHL", NULL, "HPHL PGA"},

	{"IN2_HPHR", NULL, "VDD_BUCK"},
	{"IN2_HPHR", NULL, "CLS_H_PORT"},
	{"HPH SEQUENCER", NULL, "IN2_HPHR"},
	{"RDAC2", NULL, "HPH SEQUENCER"},
	{"HPHR_RDAC", "Switch", "RDAC2"},
	{"HPHR PGA", NULL, "HPHR_RDAC"},
	{"HPHR", NULL, "HPHR PGA"},

/*Amplier playback*/
	{"IN3_AUX", NULL, "VDD_BUCK"},
	{"EAR_MUX", "RX0", "IN1_HPHL"},
	{"EAR_MUX", "RX2", "IN3_AUX"},
	{"DAC1", "Switch", "EAR_MUX"},
	{"EAR_RDAC", NULL, "DAC1"},
	{"SA SEQUENCER", NULL, "EAR_RDAC"},
	{"EAR_MIXER", "Switch", "SA SEQUENCER"},
	{"EAR PGA", NULL, "EAR_MIXER"},
	{"EAR", NULL, "EAR PGA"},

	{"AUX_MUX", "RX1", "IN2_HPHR"},
	{"AUX_MUX", "RX2", "IN3_AUX"},
	{"DAC2", "Switch", "AUX_MUX"},
	{"AUX_RDAC", NULL, "DAC2"},
	{"SA SEQUENCER", NULL, "AUX_RDAC"},
	{"AUX_MIXER", "Switch", "SA SEQUENCER",},
	{"AUX PGA", NULL, "AUX_MIXER"},
	{"AUX", NULL, "AUX PGA"},
};

static ssize_t wcd9378_version_read(struct snd_info_entry *entry,
				   void *file_private_data,
				   struct file *file,
				   char __user *buf, size_t count,
				   loff_t pos)
{
	struct wcd9378_priv *priv;
	char buffer[WCD9378_VERSION_ENTRY_SIZE];
	int len = 0;

	priv = (struct wcd9378_priv *) entry->private_data;
	if (!priv) {
		pr_err("%s: wcd9378 priv is null\n", __func__);
		return -EINVAL;
	}

	switch (priv->version) {
	case WCD9378_VERSION_1_0:
		len = scnprintf(buffer, sizeof(buffer), "WCD9378_1_0\n");
		break;
	default:
		len = scnprintf(buffer, sizeof(buffer), "VER_UNDEFINED\n");
	}

	return simple_read_from_buffer(buf, count, &pos, buffer, len);
}

static struct snd_info_entry_ops wcd9378_info_ops = {
	.read = wcd9378_version_read,
};

/*
 * wcd9378_info_create_codec_entry - creates wcd9378 module
 * @codec_root: The parent directory
 * @component: component instance
 *
 * Creates wcd9378 module, version entry under the given
 * parent directory.
 *
 * Return: 0 on success or negative error code on failure.
 */
int wcd9378_info_create_codec_entry(struct snd_info_entry *codec_root,
				   struct snd_soc_component *component)
{
	struct snd_info_entry *version_entry;
	struct wcd9378_priv *priv;
	struct snd_soc_card *card;

	if (!codec_root || !component)
		return -EINVAL;

	priv = snd_soc_component_get_drvdata(component);
	if (priv->entry) {
		dev_dbg(priv->dev,
			"%s:wcd9378 module already created\n", __func__);
		return 0;
	}
	card = component->card;

	priv->entry = snd_info_create_module_entry(codec_root->module,
					     "wcd9378", codec_root);
	if (!priv->entry) {
		dev_dbg(component->dev, "%s: failed to create wcd9378 entry\n",
			__func__);
		return -ENOMEM;
	}
	priv->entry->mode = S_IFDIR | 0555;
	if (snd_info_register(priv->entry) < 0) {
		snd_info_free_entry(priv->entry);
		return -ENOMEM;
	}

	version_entry = snd_info_create_card_entry(card->snd_card,
						   "version",
						   priv->entry);
	if (!version_entry) {
		dev_dbg(component->dev, "%s: failed to create wcd9378 version entry\n",
			__func__);
		snd_info_free_entry(priv->entry);
		return -ENOMEM;
	}

	version_entry->private_data = priv;
	version_entry->size = WCD9378_VERSION_ENTRY_SIZE;
	version_entry->content = SNDRV_INFO_CONTENT_DATA;
	version_entry->c.ops = &wcd9378_info_ops;

	if (snd_info_register(version_entry) < 0) {
		snd_info_free_entry(version_entry);
		snd_info_free_entry(priv->entry);
		return -ENOMEM;
	}
	priv->version_entry = version_entry;

	return 0;
}
EXPORT_SYMBOL_GPL(wcd9378_info_create_codec_entry);

static void wcd9378_class_load(struct snd_soc_component *component)
{
	/*SMP AMP CLASS LOADING*/
	snd_soc_component_update_bits(component, WCD9378_FUNC_ACT,
			WCD9378_FUNC_ACT_FUNC_ACT_MASK, 0x01);
	usleep_range(20000, 20010);

	snd_soc_component_update_bits(component, WCD9378_SMP_AMP_FUNC_STAT,
			WCD9378_SMP_AMP_FUNC_STAT_FUNC_STAT_MASK, 0xFF);

	/*SMP JACK CLASS LOADING*/
	snd_soc_component_update_bits(component, WCD9378_SMP_JACK_FUNC_ACT,
			WCD9378_SMP_JACK_FUNC_ACT_FUNC_ACT_MASK, 0x01);
	usleep_range(30000, 30010);

	snd_soc_component_update_bits(component, WCD9378_CMT_GRP_MASK,
			WCD9378_CMT_GRP_MASK_CMT_GRP_MASK_MASK, 0x02);

	snd_soc_component_update_bits(component, WCD9378_SMP_JACK_FUNC_STAT,
			WCD9378_SMP_JACK_FUNC_STAT_FUNC_STAT_MASK, 0xFF);

	/*SMP MIC0 CLASS LOADING*/
	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL0_FUNC_ACT,
			WCD9378_SMP_MIC_CTRL0_FUNC_ACT_FUNC_ACT_MASK, 0x01);
	usleep_range(5000, 5010);

	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL0_FUNC_STAT,
			WCD9378_SMP_MIC_CTRL0_FUNC_STAT_FUNC_STAT_MASK, 0xFF);

	/*SMP MIC1 CLASS LOADING*/
	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL1_FUNC_ACT,
			WCD9378_SMP_MIC_CTRL1_FUNC_ACT_FUNC_ACT_MASK, 0x01);
	usleep_range(5000, 5010);

	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL1_FUNC_STAT,
			WCD9378_SMP_MIC_CTRL1_FUNC_STAT_FUNC_STAT_MASK, 0xFF);

	/*SMP MIC2 CLASS LOADING*/
	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL2_FUNC_ACT,
			WCD9378_SMP_MIC_CTRL2_FUNC_ACT_FUNC_ACT_MASK, 0x01);
	usleep_range(5000, 5010);

	snd_soc_component_update_bits(component, WCD9378_SMP_MIC_CTRL2_FUNC_STAT,
			WCD9378_SMP_MIC_CTRL2_FUNC_STAT_FUNC_STAT_MASK, 0xFF);
}

static void wcd9378_micb_value_convert(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);
	struct wcd9378_pdata *pdata =
			dev_get_platdata(wcd9378->dev);
	struct wcd9378_micbias_setting *mb = &pdata->micbias;

	mb->micb1_usage_val = wcd9378_micb_usage_value_convert(component,
						mb->micb1_mv, MIC_BIAS_1);

	mb->micb2_usage_val = wcd9378_micb_usage_value_convert(component,
						mb->micb2_mv, MIC_BIAS_2);

	mb->micb3_usage_val = wcd9378_micb_usage_value_convert(component,
						mb->micb3_mv, MIC_BIAS_3);

	pr_debug("%s: micb1_usage: 0x%x, micb2_usage: 0x%x, micb3_usage: 0x%x\n", __func__,
			mb->micb1_usage_val, mb->micb2_usage_val, mb->micb3_usage_val);
}

static int wcd9378_wcd_mode_check(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 =
			snd_soc_component_get_drvdata(component);

	if (snd_soc_component_read(component,
			WCD9378_EFUSE_REG_29)
			& WCD9378_EFUSE_REG_29_PLATFORM_BLOWN_MASK) {
		if (((snd_soc_component_read(component,
				WCD9378_EFUSE_REG_29) &
				WCD9378_EFUSE_REG_29_PLATFORM_MASK) >> 1) == wcd9378->wcd_mode)
			return true;
		else
			return false;
	} else {
		if ((snd_soc_component_read(component, WCD9378_PLATFORM_CTL)
				& WCD9378_PLATFORM_CTL_MODE_MASK) == wcd9378->wcd_mode)
			return true;
		else
			return false;
	}

	return 0;
}

#ifdef CONFIG_DEBUG_FS

#define WCD_REG_MAX_BUF_LEN 20
#define BYTES_PER_LINE 17

static ssize_t wcd9378_swrslave_reg_show(struct device *pdev, char __user *ubuf,
					 size_t count, loff_t *ppos)
{
	int i, reg_val, len;
	ssize_t total = 0;
	char tmp_buf[WCD_REG_MAX_BUF_LEN];
	int reg_cnt = sizeof(dump_reg) / sizeof(u32);

	if (!ubuf || !ppos)
		return 0;

	for (i = ((int)*ppos / BYTES_PER_LINE); i < reg_cnt; i++) {
		reg_val = snd_soc_component_read(
			(struct snd_soc_component *)pdev, dump_reg[i]);
		len = scnprintf(tmp_buf, sizeof(tmp_buf), "0x%.8x: 0x%.2x\n",
				dump_reg[i], (reg_val & 0xFF));
		if (((total + len) >= count - 1) || (len < 0))
			break;
		if (copy_to_user((ubuf + total), tmp_buf, len)) {
			pr_err("%s: fail to copy reg dump\n", __func__);
			total = -EFAULT;
			goto copy_err;
		}
		total += len;
		*ppos += len;
	}

copy_err:

	return total;
}

static ssize_t codec_debug_dump(struct file *file, char __user *ubuf,
				size_t count, loff_t *ppos)
{
	struct device *pdev;

	if (!count || !file || !ppos || !ubuf)
		return -EINVAL;

	pdev = file->private_data;
	if (!pdev)
		return -EINVAL;

	if (*ppos < 0)
		return -EINVAL;

	return wcd9378_swrslave_reg_show(pdev, ubuf, count, ppos);
}

static const struct file_operations codec_debug_dump_ops = {
	.open = simple_open,
	.read = codec_debug_dump,
};
#endif

static int wcd9378_soc_codec_probe(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);
	struct snd_soc_dapm_context *dapm =
			snd_soc_component_get_dapm(component);
	int ret = -EINVAL;

	wcd9378 = snd_soc_component_get_drvdata(component);
	if (!wcd9378)
		return -EINVAL;

	wcd9378->component = component;
	snd_soc_component_init_regmap(component, wcd9378->regmap);

	devm_regmap_qti_debugfs_register(&wcd9378->tx_swr_dev->dev, wcd9378->regmap);

	ret = wcd9378_wcd_mode_check(component);
	if (!ret) {
		dev_err(component->dev, "wcd mode check failed\n");
		ret = -EINVAL;
		goto exit;
	}

	ret = wcd9378_mbhc_init(&wcd9378->mbhc, component);
	if (ret) {
		pr_err("%s: mbhc initialization failed\n", __func__);
		ret = -EINVAL;
		goto exit;
	}

	snd_soc_dapm_ignore_suspend(dapm, "AMIC1");
	snd_soc_dapm_ignore_suspend(dapm, "AMIC2");
	snd_soc_dapm_ignore_suspend(dapm, "AMIC3");
	snd_soc_dapm_ignore_suspend(dapm, "AMIC4");
	snd_soc_dapm_ignore_suspend(dapm, "VA AMIC1");
	snd_soc_dapm_ignore_suspend(dapm, "VA AMIC2");
	snd_soc_dapm_ignore_suspend(dapm, "VA AMIC3");
	snd_soc_dapm_ignore_suspend(dapm, "VA AMIC4");
	snd_soc_dapm_ignore_suspend(dapm, "IN1_HPHL");
	snd_soc_dapm_ignore_suspend(dapm, "IN2_HPHR");
	snd_soc_dapm_ignore_suspend(dapm, "IN3_AUX");
	snd_soc_dapm_ignore_suspend(dapm, "ADC1_OUTPUT");
	snd_soc_dapm_ignore_suspend(dapm, "ADC2_OUTPUT");
	snd_soc_dapm_ignore_suspend(dapm, "ADC3_OUTPUT");
	snd_soc_dapm_ignore_suspend(dapm, "EAR");
	snd_soc_dapm_ignore_suspend(dapm, "AUX");
	snd_soc_dapm_ignore_suspend(dapm, "HPHL");
	snd_soc_dapm_ignore_suspend(dapm, "HPHR");
	snd_soc_dapm_sync(dapm);

	wcd_cls_h_init(&wcd9378->clsh_info);

	wcd9378_init_reg(component);

	wcd9378_micb_value_convert(component);

	wcd9378->version = WCD9378_VERSION_1_0;
       /* Register event notifier */
	wcd9378->nblock.notifier_call = wcd9378_event_notify;
	if (wcd9378->register_notifier) {
		ret = wcd9378->register_notifier(wcd9378->handle,
						&wcd9378->nblock,
						true);
		if (ret) {
			dev_err(component->dev,
				"%s: Failed to register notifier %d\n",
				__func__, ret);
			return ret;
		}
	}

#ifdef CONFIG_DEBUG_FS
	if (!wcd9378->debugfs_wcd9378_dent) {
		wcd9378->debugfs_wcd9378_dent =
			debugfs_create_dir("wcd9378-register-dump", 0);
		if (!IS_ERR(wcd9378->debugfs_wcd9378_dent)) {
			wcd9378->debugfs_reg_dump = debugfs_create_file(
				"registers", S_IFREG | 0444,
				wcd9378->debugfs_wcd9378_dent,
				(void *)component, &codec_debug_dump_ops);

			if (IS_ERR(wcd9378->debugfs_reg_dump)) {
				pr_err("debugfs_create_file failed\n");
			}
		}
	}
#endif

exit:
	return ret;
}

static void wcd9378_soc_codec_remove(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	if (!wcd9378) {
		dev_err(component->dev, "%s: wcd9378 is already NULL\n",
			__func__);
		return;
	}
	if (wcd9378->register_notifier)
		wcd9378->register_notifier(wcd9378->handle,
						&wcd9378->nblock,
						false);
}

static int wcd9378_soc_codec_suspend(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	if (!wcd9378)
		return 0;
	wcd9378->dapm_bias_off = true;
	return 0;
}

static int wcd9378_soc_codec_resume(struct snd_soc_component *component)
{
	struct wcd9378_priv *wcd9378 = snd_soc_component_get_drvdata(component);

	if (!wcd9378)
		return 0;
	wcd9378->dapm_bias_off = false;
	return 0;
}

static const struct snd_soc_component_driver soc_codec_dev_wcd9378 = {
	.name = WCD9378_DRV_NAME,
	.probe = wcd9378_soc_codec_probe,
	.remove = wcd9378_soc_codec_remove,
	.controls = wcd9378_snd_controls,
	.num_controls = ARRAY_SIZE(wcd9378_snd_controls),
	.dapm_widgets = wcd9378_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(wcd9378_dapm_widgets),
	.dapm_routes = wcd9378_audio_map,
	.num_dapm_routes = ARRAY_SIZE(wcd9378_audio_map),
	.suspend =  wcd9378_soc_codec_suspend,
	.resume = wcd9378_soc_codec_resume,
};

static int wcd9378_reset(struct device *dev)
{
	struct wcd9378_priv *wcd9378 = NULL;
	int rc = 0;
	int value = 0;

	if (!dev)
		return -ENODEV;

	wcd9378 = dev_get_drvdata(dev);
	if (!wcd9378)
		return -EINVAL;

	if (!wcd9378->rst_np) {
		dev_err(dev, "%s: reset gpio device node not specified\n",
				__func__);
		return -EINVAL;
	}

	value = msm_cdc_pinctrl_get_state(wcd9378->rst_np);
	if (value > 0)
		return 0;

	rc = msm_cdc_pinctrl_select_sleep_state(wcd9378->rst_np);
	if (rc) {
		dev_err(dev, "%s: wcd sleep state request fail!\n",
				__func__);
		return -EPROBE_DEFER;
	}
	/* 20us sleep required after pulling the reset gpio to LOW */
	usleep_range(80, 85);

	rc = msm_cdc_pinctrl_select_active_state(wcd9378->rst_np);
	if (rc) {
		dev_err(dev, "%s: wcd active state request fail!\n",
				__func__);
		return -EPROBE_DEFER;
	}
	/* 20us sleep required after pulling the reset gpio to HIGH */
	usleep_range(80, 85);

	return rc;
}

static int wcd9378_read_of_property_u32(struct device *dev, const char *name,
					u32 *val)
{
	int rc = 0;

	rc = of_property_read_u32(dev->of_node, name, val);
	if (rc)
		dev_err(dev, "%s: Looking up %s property in node %s failed\n",
			__func__, name, dev->of_node->full_name);

	return rc;
}

static void wcd9378_dt_parse_micbias_info(struct device *dev,
					struct wcd9378_micbias_setting *mb)
{
	u32 prop_val = 0;
	int rc = 0;

	/* MB1 */
	if (of_find_property(dev->of_node, "qcom,cdc-micbias1-mv",
				    NULL)) {
		rc = wcd9378_read_of_property_u32(dev,
						  "qcom,cdc-micbias1-mv",
						  &prop_val);
		if (!rc)
			mb->micb1_mv = prop_val;
	} else {
		dev_info(dev, "%s: Micbias1 DT property not found\n",
			__func__);
	}

	/* MB2 */
	if (of_find_property(dev->of_node, "qcom,cdc-micbias2-mv",
				    NULL)) {
		rc = wcd9378_read_of_property_u32(dev,
						  "qcom,cdc-micbias2-mv",
						  &prop_val);
		if (!rc)
			mb->micb2_mv = prop_val;
	} else {
		dev_info(dev, "%s: Micbias2 DT property not found\n",
			__func__);
	}

	/* MB3 */
	if (of_find_property(dev->of_node, "qcom,cdc-micbias3-mv",
				    NULL)) {
		rc = wcd9378_read_of_property_u32(dev,
						  "qcom,cdc-micbias3-mv",
						  &prop_val);
		if (!rc)
			mb->micb3_mv = prop_val;
	} else {
		dev_info(dev, "%s: Micbias3 DT property not found\n",
			__func__);
	}
}

static int wcd9378_reset_low(struct device *dev)
{
	struct wcd9378_priv *wcd9378 = NULL;
	int rc = 0;

	if (!dev)
		return -ENODEV;

	wcd9378 = dev_get_drvdata(dev);
	if (!wcd9378)
		return -EINVAL;

	if (!wcd9378->rst_np) {
		dev_err(dev, "%s: reset gpio device node not specified\n",
				__func__);
		return -EINVAL;
	}

	rc = msm_cdc_pinctrl_select_sleep_state(wcd9378->rst_np);
	if (rc) {
		dev_err(dev, "%s: wcd sleep state request fail!\n",
				__func__);
		return rc;
	}
	/* 20us sleep required after pulling the reset gpio to LOW */
	usleep_range(20, 30);

	return rc;
}

struct wcd9378_pdata *wcd9378_populate_dt_data(struct device *dev)
{
	struct wcd9378_pdata *pdata = NULL;

	pdata = devm_kzalloc(dev, sizeof(struct wcd9378_pdata),
				GFP_KERNEL);
	if (!pdata)
		return NULL;

	pdata->rst_np = of_parse_phandle(dev->of_node,
			"qcom,wcd-rst-gpio-node", 0);

	if (!pdata->rst_np) {
		dev_err(dev, "%s: Looking up %s property in node %s failed\n",
				__func__, "qcom,wcd-rst-gpio-node",
				dev->of_node->full_name);
		return NULL;
	}

	/* Parse power supplies */
	msm_cdc_get_power_supplies(dev, &pdata->regulator,
				   &pdata->num_supplies);
	if (!pdata->regulator || (pdata->num_supplies <= 0)) {
		dev_err(dev, "%s: no power supplies defined for codec\n",
			__func__);
		return NULL;
	}

	pdata->rx_slave = of_parse_phandle(dev->of_node, "qcom,rx-slave", 0);
	pdata->tx_slave = of_parse_phandle(dev->of_node, "qcom,tx-slave", 0);

	wcd9378_dt_parse_micbias_info(dev, &pdata->micbias);

	return pdata;
}

static struct snd_soc_dai_driver wcd9378_dai[] = {
	{
		.name = "wcd9378_cdc",
		.playback = {
			.stream_name = "WCD9378_AIF Playback",
			.rates = WCD9378_RATES | WCD9378_FRAC_RATES,
			.formats = WCD9378_FORMATS,
			.rate_max = 384000,
			.rate_min = 8000,
			.channels_min = 1,
			.channels_max = 4,
		},
		.capture = {
			.stream_name = "WCD9378_AIF Capture",
			.rates = WCD9378_RATES | WCD9378_FRAC_RATES,
			.formats = WCD9378_FORMATS,
			.rate_max = 384000,
			.rate_min = 8000,
			.channels_min = 1,
			.channels_max = 4,
		},
	},
};

static irqreturn_t wcd9378_wd_handle_irq(int irq, void *data)
{
	pr_err_ratelimited("%s: Watchdog interrupt for irq =%d triggered\n",
			   __func__, irq);
	return IRQ_HANDLED;
}

static int wcd9378_bind(struct device *dev)
{
	int ret = 0;
	struct wcd9378_pdata *pdata = dev_get_platdata(dev);
	struct wcd9378_priv *wcd9378 = dev_get_drvdata(dev);

	/*
	 * Add 5msec delay to provide sufficient time for
	 * soundwire auto enumeration of slave devices as
	 * per HW requirement.
	 */
	usleep_range(5000, 5010);

	ret = component_bind_all(dev, wcd9378);
	if (ret) {
		dev_err(dev, "%s: Slave bind failed, ret = %d\n",
			__func__, ret);
		return ret;
	}

	wcd9378->rx_swr_dev = get_matching_swr_slave_device(pdata->rx_slave);
	if (!wcd9378->rx_swr_dev) {
		dev_err(dev, "%s: Could not find RX swr slave device\n",
			 __func__);
		ret = -ENODEV;
		goto err;
	}
	wcd9378->rx_swr_dev->paging_support = true;

	wcd9378->tx_swr_dev = get_matching_swr_slave_device(pdata->tx_slave);
	if (!wcd9378->tx_swr_dev) {
		dev_err(dev, "%s: Could not find TX swr slave device\n",
			__func__);
		ret = -ENODEV;
		goto err;
	}
	wcd9378->tx_swr_dev->paging_support = true;

	swr_init_port_params(wcd9378->tx_swr_dev, SWR_NUM_PORTS,
			     wcd9378->swr_tx_port_params);

	wcd9378->regmap = devm_regmap_init_swr(wcd9378->tx_swr_dev,
					       &wcd9378_regmap_config);
	if (!wcd9378->regmap) {
		dev_err(dev, "%s: Regmap init failed\n",
				__func__);
		goto err;
	}

	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTRTYPE_1, 0xff);
	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTRTYPE_2, 0x0b);
	regmap_write(wcd9378->regmap, SWRS_SCP_SDCA_INTRTYPE_3, 0xff);

	wcd9378_regmap_irq_chip.irq_drv_data = wcd9378;
	wcd9378->irq_info.wcd_regmap_irq_chip = &wcd9378_regmap_irq_chip;
	wcd9378->irq_info.codec_name = "WCD9378";
	wcd9378->irq_info.regmap = wcd9378->regmap;
	wcd9378->irq_info.dev = dev;
	ret = wcd_irq_init(&wcd9378->irq_info, &wcd9378->virq);
	if (ret) {
		dev_err(wcd9378->dev, "%s: IRQ init failed: %d\n",
			__func__, ret);
		goto err;
	}

	dev_err(wcd9378->dev, "%s: wcd irq init done\n",
			__func__);
	wcd9378->tx_swr_dev->slave_irq = wcd9378->virq;

	/* Request for watchdog interrupt */
	wcd_request_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHR_PDM_WD_INT,
			"HPHR PDM WD INT", wcd9378_wd_handle_irq, NULL);
	wcd_request_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHL_PDM_WD_INT,
			"HPHL PDM WD INT", wcd9378_wd_handle_irq, NULL);
	wcd_request_irq(&wcd9378->irq_info, WCD9378_IRQ_AUX_PDM_WD_INT,
			"AUX PDM WD INT", wcd9378_wd_handle_irq, NULL);
	/* Disable watchdog interrupt for HPH and AUX */
	wcd_disable_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHR_PDM_WD_INT);
	wcd_disable_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHL_PDM_WD_INT);
	wcd_disable_irq(&wcd9378->irq_info, WCD9378_IRQ_AUX_PDM_WD_INT);

	ret = snd_soc_register_component(dev, &soc_codec_dev_wcd9378,
					wcd9378_dai, ARRAY_SIZE(wcd9378_dai));
	if (ret) {
		dev_err(dev, "%s: Codec registration failed\n",
				__func__);
		goto err_irq;
	}

	return ret;
err_irq:
	wcd_irq_exit(&wcd9378->irq_info, wcd9378->virq);
err:
	component_unbind_all(dev, wcd9378);
	return ret;
}

static void wcd9378_unbind(struct device *dev)
{
	struct wcd9378_priv *wcd9378 = dev_get_drvdata(dev);

	wcd_free_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHR_PDM_WD_INT, NULL);
	wcd_free_irq(&wcd9378->irq_info, WCD9378_IRQ_HPHL_PDM_WD_INT, NULL);
	wcd_free_irq(&wcd9378->irq_info, WCD9378_IRQ_AUX_PDM_WD_INT, NULL);
	wcd_irq_exit(&wcd9378->irq_info, wcd9378->virq);
	snd_soc_unregister_component(dev);
	component_unbind_all(dev, wcd9378);
}

static const struct of_device_id wcd9378_dt_match[] = {
	{ .compatible = "qcom,wcd9378-codec", .data = "wcd9378"},
	{}
};

static const struct component_master_ops wcd9378_comp_ops = {
	.bind   = wcd9378_bind,
	.unbind = wcd9378_unbind,
};

static int wcd9378_compare_of(struct device *dev, void *data)
{
	return dev->of_node == data;
}

static void wcd9378_release_of(struct device *dev, void *data)
{
	of_node_put(data);
}

static int wcd9378_add_slave_components(struct device *dev,
				struct component_match **matchptr)
{
	struct device_node *np, *rx_node, *tx_node;

	np = dev->of_node;

	rx_node = of_parse_phandle(np, "qcom,rx-slave", 0);
	if (!rx_node) {
		dev_err(dev, "%s: Rx-slave node not defined\n", __func__);
		return -ENODEV;
	}
	of_node_get(rx_node);
	component_match_add_release(dev, matchptr,
			wcd9378_release_of,
			wcd9378_compare_of,
			rx_node);

	tx_node = of_parse_phandle(np, "qcom,tx-slave", 0);
	if (!tx_node) {
		dev_err(dev, "%s: Tx-slave node not defined\n", __func__);
			return -ENODEV;
	}
	of_node_get(tx_node);
	component_match_add_release(dev, matchptr,
			wcd9378_release_of,
			wcd9378_compare_of,
			tx_node);
	return 0;
}

static int wcd9378_probe(struct platform_device *pdev)
{
	struct component_match *match = NULL;
	struct wcd9378_priv *wcd9378 = NULL;
	struct wcd9378_pdata *pdata = NULL;
	struct wcd_ctrl_platform_data *plat_data = NULL;
	struct device *dev = &pdev->dev;
	int ret;

	wcd9378 = devm_kzalloc(dev, sizeof(struct wcd9378_priv),
				GFP_KERNEL);
	if (!wcd9378)
		return -ENOMEM;

	dev_set_drvdata(dev, wcd9378);
	wcd9378->dev = dev;

	pdata = wcd9378_populate_dt_data(dev);
	if (!pdata) {
		dev_err(dev, "%s: Fail to obtain platform data\n", __func__);
		return -EINVAL;
	}
	dev->platform_data = pdata;

	wcd9378->rst_np = pdata->rst_np;
	ret = msm_cdc_init_supplies(dev, &wcd9378->supplies,
				    pdata->regulator, pdata->num_supplies);
	if (!wcd9378->supplies) {
		dev_err(dev, "%s: Cannot init wcd supplies\n",
			__func__);
		return ret;
	}

	plat_data = dev_get_platdata(dev->parent);
	if (!plat_data) {
		dev_err(dev, "%s: platform data from parent is NULL\n",
			__func__);
		return -EINVAL;
	}
	wcd9378->handle = (void *)plat_data->handle;
	if (!wcd9378->handle) {
		dev_err(dev, "%s: handle is NULL\n", __func__);
		return -EINVAL;
	}

	wcd9378->update_wcd_event = plat_data->update_wcd_event;
	if (!wcd9378->update_wcd_event) {
		dev_err(dev, "%s: update_wcd_event api is null!\n",
			__func__);
		return -EINVAL;
	}
	wcd9378->register_notifier = plat_data->register_notifier;
	if (!wcd9378->register_notifier) {
		dev_err(dev, "%s: register_notifier api is null!\n",
			__func__);
		return -EINVAL;
	}

	ret = of_property_read_u32(dev->of_node, "qcom,wcd-mode",
			&wcd9378->wcd_mode);
	if (ret) {
		dev_dbg(dev, "%s: wcd-mode read failed, use mobile mode\n",
			__func__);
		wcd9378->wcd_mode = WCD9378_MOBILE_MODE;
	}

	ret = msm_cdc_enable_static_supplies(&pdev->dev, wcd9378->supplies,
					     pdata->regulator,
					     pdata->num_supplies);
	if (ret) {
		dev_err(dev, "%s: wcd static supply enable failed!\n",
			__func__);
		return ret;
	}

	ret = wcd9378_parse_port_mapping(dev, "qcom,rx_swr_ch_map",
					CODEC_RX);
	ret |= wcd9378_parse_port_mapping(dev, "qcom,tx_swr_ch_map",
					CODEC_TX);

	if (ret) {
		dev_err(dev, "Failed to read port mapping\n");
		goto err;
	}
	ret = wcd9378_parse_port_params(dev, "qcom,swr-tx-port-params",
					CODEC_TX);
	if (ret) {
		dev_err(dev, "Failed to read port params\n");
		goto err;
	}

	mutex_init(&wcd9378->wakeup_lock);
	mutex_init(&wcd9378->micb_lock);
	mutex_init(&wcd9378->sys_usage_lock);
	ret = wcd9378_add_slave_components(dev, &match);
	if (ret)
		goto err_lock_init;

	ret = wcd9378_reset(dev);
	if (ret == -EPROBE_DEFER) {
		dev_err(dev, "%s: wcd reset failed!\n", __func__);
		goto err_lock_init;
	}

	wcd9378->wakeup = wcd9378_wakeup;

	return component_master_add_with_match(dev,
					&wcd9378_comp_ops, match);

err_lock_init:
	mutex_destroy(&wcd9378->micb_lock);
	mutex_destroy(&wcd9378->wakeup_lock);
	mutex_destroy(&wcd9378->sys_usage_lock);
err:
	return ret;
}

static int wcd9378_remove(struct platform_device *pdev)
{
	struct wcd9378_priv *wcd9378 = NULL;

	wcd9378 = platform_get_drvdata(pdev);
	component_master_del(&pdev->dev, &wcd9378_comp_ops);
	mutex_destroy(&wcd9378->micb_lock);
	mutex_destroy(&wcd9378->wakeup_lock);
	mutex_destroy(&wcd9378->sys_usage_lock);
	dev_set_drvdata(&pdev->dev, NULL);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int wcd9378_suspend(struct device *dev)
{
	struct wcd9378_priv *wcd9378 = NULL;
	int ret = 0;
	struct wcd9378_pdata *pdata = NULL;

	if (!dev)
		return -ENODEV;

	wcd9378 = dev_get_drvdata(dev);
	if (!wcd9378)
		return -EINVAL;

	pdata = dev_get_platdata(wcd9378->dev);

	if (!pdata) {
		dev_err(dev, "%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (test_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask)) {
		ret = msm_cdc_disable_ondemand_supply(wcd9378->dev,
						wcd9378->supplies,
						pdata->regulator,
						pdata->num_supplies,
						"cdc-vdd-buck");
		if (ret == -EINVAL) {
			dev_err(dev, "%s: vdd buck is not disabled\n",
				__func__);
			return 0;
		}
		clear_bit(ALLOW_BUCK_DISABLE, &wcd9378->status_mask);
	}
	if (wcd9378->dapm_bias_off ||
		(wcd9378->component &&
		(snd_soc_component_get_bias_level(wcd9378->component) ==
			SND_SOC_BIAS_OFF))) {
		msm_cdc_set_supplies_lpm_mode(wcd9378->dev,
					      wcd9378->supplies,
					      pdata->regulator,
					      pdata->num_supplies,
					      true);
		set_bit(WCD_SUPPLIES_LPM_MODE, &wcd9378->status_mask);
	}
	return 0;
}

static int wcd9378_resume(struct device *dev)
{
	struct wcd9378_priv *wcd9378 = NULL;
	struct wcd9378_pdata *pdata = NULL;

	if (!dev)
		return -ENODEV;

	wcd9378 = dev_get_drvdata(dev);
	if (!wcd9378)
		return -EINVAL;

	pdata = dev_get_platdata(wcd9378->dev);

	if (!pdata) {
		dev_err(dev, "%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (test_bit(WCD_SUPPLIES_LPM_MODE, &wcd9378->status_mask)) {
		msm_cdc_set_supplies_lpm_mode(wcd9378->dev,
					      wcd9378->supplies,
					      pdata->regulator,
					      pdata->num_supplies,
					      false);
		clear_bit(WCD_SUPPLIES_LPM_MODE, &wcd9378->status_mask);
	}

	return 0;
}

static const struct dev_pm_ops wcd9378_dev_pm_ops = {
	.suspend_late = wcd9378_suspend,
	.resume_early = wcd9378_resume,
};
#endif

static struct platform_driver wcd9378_codec_driver = {
	.probe = wcd9378_probe,
	.remove = wcd9378_remove,
	.driver = {
		.name = "wcd9378_codec",
		.of_match_table = of_match_ptr(wcd9378_dt_match),
#ifdef CONFIG_PM_SLEEP
		.pm = &wcd9378_dev_pm_ops,
#endif
		.suppress_bind_attrs = true,
	},
};

module_platform_driver(wcd9378_codec_driver);
MODULE_DESCRIPTION("WCD9378 Codec driver");
MODULE_LICENSE("GPL");
