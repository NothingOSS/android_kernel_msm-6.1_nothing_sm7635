/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef WCD9378_REG_MASKS_H
#define WCD9378_REG_MASKS_H
#include <linux/regmap.h>
#include <linux/device.h>
#include "wcd9378-registers.h"

/* Use in conjunction with wcd9378-reg-shifts.c for field values. */
/* field_value = (register_value & field_mask) >> field_shift */

#define FIELD_MASK(register_name, field_name) \
WCD9378_##register_name##_##field_name##_MASK

/* WCD9378_FUNC_EXT_ID_0 Fields: */
#define WCD9378_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK                         0xff

/* WCD9378_FUNC_EXT_ID_1 Fields: */
#define WCD9378_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK                         0xff

/* WCD9378_FUNC_EXT_VER Fields: */
#define WCD9378_FUNC_EXT_VER_FUNC_EXT_VER_MASK                           0xff

/* WCD9378_FUNC_STAT Fields: */
#define WCD9378_FUNC_STAT_FUNC_STAT_MASK                                 0xff

/* WCD9378_DEV_MANU_ID_0 Fields: */
#define WCD9378_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK                         0xff

/* WCD9378_DEV_MANU_ID_1 Fields: */
#define WCD9378_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK                         0xff

/* WCD9378_DEV_PART_ID_0 Fields: */
#define WCD9378_DEV_PART_ID_0_DEV_PART_ID_0_MASK                         0xff

/* WCD9378_DEV_PART_ID_1 Fields: */
#define WCD9378_DEV_PART_ID_1_DEV_PART_ID_1_MASK                         0xff

/* WCD9378_DEV_VER Fields: */
#define WCD9378_DEV_VER_DEV_VER_MASK                                     0xff


/* WCD9378_A_PAGE Fields: */
#define WCD9378_A_PAGE_VALUE_MASK                                        0xff

/* WCD9378_ANA_BIAS Fields: */
#define WCD9378_ANA_BIAS_ANALOG_BIAS_EN_MASK                             0x80
#define WCD9378_ANA_BIAS_PRECHRG_EN_MASK                                 0x40
#define WCD9378_ANA_BIAS_PRECHRG_CTL_MODE_MASK                           0x20

/* WCD9378_ANA_RX_SUPPLIES Fields: */
#define WCD9378_ANA_RX_SUPPLIES_CLASSG_CP_EN_MASK                        0x80
#define WCD9378_ANA_RX_SUPPLIES_NCP_EN_MASK                              0x40
#define WCD9378_ANA_RX_SUPPLIES_SEQ_BYPASS_MASK                          0x20
#define WCD9378_ANA_RX_SUPPLIES_SDCA_BYPASS_MASK                         0x10
#define WCD9378_ANA_RX_SUPPLIES_SYS_USAGE_BYP_MASK                       0x08
#define WCD9378_ANA_RX_SUPPLIES_ANA_SEQ_BYPASS_MASK                      0x02
#define WCD9378_ANA_RX_SUPPLIES_RX_BIAS_ENABLE_MASK                      0x01

/* WCD9378_ANA_HPH Fields: */
#define WCD9378_ANA_HPH_HPHL_ENABLE_MASK                                 0x80
#define WCD9378_ANA_HPH_HPHR_ENABLE_MASK                                 0x40
#define WCD9378_ANA_HPH_HPHL_REF_ENABLE_MASK                             0x20
#define WCD9378_ANA_HPH_HPHR_REF_ENABLE_MASK                             0x10
#define WCD9378_ANA_HPH_PWR_LEVEL_MASK                                   0x0c
#define WCD9378_ANA_HPH_LOW_HIFI_CTL_MASK                                0x02

/* WCD9378_ANA_EAR Fields: */
#define WCD9378_ANA_EAR_ENABLE_MASK                                      0x80
#define WCD9378_ANA_EAR_SHORT_PROT_EN_MASK                               0x40
#define WCD9378_ANA_EAR_OUT_IMPEDANCE_MASK                               0x20
#define WCD9378_ANA_EAR_DAC_CLK_SEL_MASK                                 0x01

/* WCD9378_ANA_EAR_COMPANDER_CTL Fields: */
#define WCD9378_ANA_EAR_COMPANDER_CTL_GAIN_OVRD_REG_MASK                 0x80
#define WCD9378_ANA_EAR_COMPANDER_CTL_EAR_GAIN_MASK                      0x7c
#define WCD9378_ANA_EAR_COMPANDER_CTL_COMP_DFF_BYP_MASK                  0x02
#define WCD9378_ANA_EAR_COMPANDER_CTL_COMP_DFF_CLK_EDGE_MASK             0x01

/* WCD9378_ANA_TX_CH1 Fields: */
#define WCD9378_ANA_TX_CH1_ENABLE_MASK                                   0x80
#define WCD9378_ANA_TX_CH1_PWR_LEVEL_MASK                                0x60
#define WCD9378_ANA_TX_CH1_GAIN_MASK                                     0x1f

/* WCD9378_ANA_TX_CH2 Fields: */
#define WCD9378_ANA_TX_CH2_ENABLE_MASK                                   0x80
#define WCD9378_ANA_TX_CH2_HPF1_INIT_MASK                                0x40
#define WCD9378_ANA_TX_CH2_HPF2_INIT_MASK                                0x20
#define WCD9378_ANA_TX_CH2_GAIN_MASK                                     0x1f

/* WCD9378_ANA_TX_CH3 Fields: */
#define WCD9378_ANA_TX_CH3_ENABLE_MASK                                   0x80
#define WCD9378_ANA_TX_CH3_PWR_LEVEL_MASK                                0x60
#define WCD9378_ANA_TX_CH3_GAIN_MASK                                     0x1f

/* WCD9378_ANA_TX_CH3_HPF Fields: */
#define WCD9378_ANA_TX_CH3_HPF_HPF3_INIT_MASK                            0x40

/* WCD9378_ANA_MICB1_MICB2_DSP_EN_LOGIC Fields: */
#define WCD9378_ANA_MICB1_MICB2_DSP_EN_LOGIC_MICB1_DSP_OVERRIDE_MASK     0x80
#define WCD9378_ANA_MICB1_MICB2_DSP_EN_LOGIC_MICB1_DSP_CTRL_MASK         0x60
#define WCD9378_ANA_MICB1_MICB2_DSP_EN_LOGIC_MICB2_DSP_OVERRIDE_MASK     0x10
#define WCD9378_ANA_MICB1_MICB2_DSP_EN_LOGIC_MICB2_DSP_CTRL_MASK         0x0c

/* WCD9378_ANA_MICB3_DSP_EN_LOGIC Fields: */
#define WCD9378_ANA_MICB3_DSP_EN_LOGIC_MICB3_DSP_OVERRIDE_MASK           0x80
#define WCD9378_ANA_MICB3_DSP_EN_LOGIC_MICB3_DSP_CTRL_MASK               0x60

/* WCD9378_ANA_MBHC_MECH Fields: */
#define WCD9378_ANA_MBHC_MECH_L_DET_EN_MASK                              0x80
#define WCD9378_ANA_MBHC_MECH_GND_DET_EN_MASK                            0x40
#define WCD9378_ANA_MBHC_MECH_MECH_DETECT_TYPE_MASK                      0x20
#define WCD9378_ANA_MBHC_MECH_HPHL_PLUG_TYPE_MASK                        0x10
#define WCD9378_ANA_MBHC_MECH_GND_PLUG_TYPE_MASK                         0x08
#define WCD9378_ANA_MBHC_MECH_MECH_HS_L_PULLUP_COMP_EN_MASK              0x04
#define WCD9378_ANA_MBHC_MECH_MECH_HS_G_PULLUP_COMP_EN_MASK              0x02
#define WCD9378_ANA_MBHC_MECH_SW_HPH_L_P_100K_TO_GND_MASK                0x01

/* WCD9378_ANA_MBHC_ELECT Fields: */
#define WCD9378_ANA_MBHC_ELECT_FSM_EN_MASK                               0x80
#define WCD9378_ANA_MBHC_ELECT_BTNDET_ISRC_CTL_MASK                      0x70
#define WCD9378_ANA_MBHC_ELECT_ELECT_DET_TYPE_MASK                       0x08
#define WCD9378_ANA_MBHC_ELECT_ELECT_SCHMT_ISRC_CTL_MASK                 0x06
#define WCD9378_ANA_MBHC_ELECT_BIAS_EN_MASK                              0x01

/* WCD9378_ANA_MBHC_ZDET Fields: */
#define WCD9378_ANA_MBHC_ZDET_ZDET_L_MEAS_EN_MASK                        0x80
#define WCD9378_ANA_MBHC_ZDET_ZDET_R_MEAS_EN_MASK                        0x40
#define WCD9378_ANA_MBHC_ZDET_ZDET_CHG_EN_MASK                           0x20
#define WCD9378_ANA_MBHC_ZDET_ELECT_ISRC_EN_MASK                         0x02

/* WCD9378_ANA_MBHC_RESULT_1 Fields: */
#define WCD9378_ANA_MBHC_RESULT_1_Z_RESULT_MSB_MASK                      0xff

/* WCD9378_ANA_MBHC_RESULT_2 Fields: */
#define WCD9378_ANA_MBHC_RESULT_2_Z_RESULT_LSB_MASK                      0xff

/* WCD9378_ANA_MBHC_RESULT_3 Fields: */
#define WCD9378_ANA_MBHC_RESULT_3_MIC_SCHMT_RESULT_MASK                  0x20
#define WCD9378_ANA_MBHC_RESULT_3_IN2P_CLAMP_STATE_MASK                  0x10
#define WCD9378_ANA_MBHC_RESULT_3_BTN_RESULT_MASK                        0x07

/* WCD9378_ANA_MBHC_BTN0 Fields: */
#define WCD9378_ANA_MBHC_BTN0_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN1 Fields: */
#define WCD9378_ANA_MBHC_BTN1_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN2 Fields: */
#define WCD9378_ANA_MBHC_BTN2_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN3 Fields: */
#define WCD9378_ANA_MBHC_BTN3_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN4 Fields: */
#define WCD9378_ANA_MBHC_BTN4_VTH_MASK                                   0xfc
#define WCD9378_ANA_MBHC_BTN4_VDD_SW_IO_SEL_MASK                         0x02
#define WCD9378_ANA_MBHC_BTN4_LKGCOMP_EN_MASK                            0x01

/* WCD9378_ANA_MBHC_BTN5 Fields: */
#define WCD9378_ANA_MBHC_BTN5_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN6 Fields: */
#define WCD9378_ANA_MBHC_BTN6_VTH_MASK                                   0xfc

/* WCD9378_ANA_MBHC_BTN7 Fields: */
#define WCD9378_ANA_MBHC_BTN7_VTH_MASK                                   0xfc

/* WCD9378_ANA_MICB1 Fields: */
#define WCD9378_ANA_MICB1_ENABLE_MASK                                    0xc0
#define WCD9378_ANA_MICB1_VOUT_CTL_MASK                                  0x3f

/* WCD9378_ANA_MICB2 Fields: */
#define WCD9378_ANA_MICB2_ENABLE_MASK                                    0xc0
#define WCD9378_ANA_MICB2_VOUT_CTL_MASK                                  0x3f

/* WCD9378_ANA_MICB2_RAMP Fields: */
#define WCD9378_ANA_MICB2_RAMP_RAMP_ENABLE_MASK                          0x80
#define WCD9378_ANA_MICB2_RAMP_MB2_IN2P_SHORT_ENABLE_MASK                0x40
#define WCD9378_ANA_MICB2_RAMP_ALLSW_OVRD_ENABLE_MASK                    0x20
#define WCD9378_ANA_MICB2_RAMP_SHIFT_CTL_MASK                            0x1c

/* WCD9378_ANA_MICB3 Fields: */
#define WCD9378_ANA_MICB3_ENABLE_MASK                                    0xc0
#define WCD9378_ANA_MICB3_PRECHARGE_OVERRIDE_MICB3_MASK                  0x20
#define WCD9378_ANA_MICB3_PRECHARGE_CLK_SEL_MICB3_MASK                   0x18
#define WCD9378_ANA_MICB3_SDCA_BYPASS_MASK                               0x04

/* WCD9378_BIAS_CTL Fields: */
#define WCD9378_BIAS_CTL_BG_FAST_MODE_EN_MASK                            0x80
#define WCD9378_BIAS_CTL_DC_START_UP_EN_MASK                             0x20
#define WCD9378_BIAS_CTL_TRAN_START_UP_EN_MASK                           0x10
#define WCD9378_BIAS_CTL_OTA_BIAS_CTL_MASK                               0x08
#define WCD9378_BIAS_CTL_ATEST_CTL_MASK                                  0x04
#define WCD9378_BIAS_CTL_EFUSE_EN_MASK                                   0x02

/* WCD9378_BIAS_VBG_FINE_ADJ Fields: */
#define WCD9378_BIAS_VBG_FINE_ADJ_VBG_FINE_ADJ_MASK                      0xf0
#define WCD9378_BIAS_VBG_FINE_ADJ_EN_DTEST_BG_STATUS_MASK                0x08
#define WCD9378_BIAS_VBG_FINE_ADJ_PRECHARGE_TIMER_COUNT_MASK             0x07

/* WCD9378_LDOL_VDDCX_ADJUST Fields: */
#define WCD9378_LDOL_VDDCX_ADJUST_RC_ZERO_FREQ_TUNE_MASK                 0x0c
#define WCD9378_LDOL_VDDCX_ADJUST_VDDCX_ADJUST_MASK                      0x03

/* WCD9378_LDOL_DISABLE_LDOL Fields: */
#define WCD9378_LDOL_DISABLE_LDOL_DISABLE_LDOL_MASK                      0x01

/* WCD9378_MBHC_CTL_CLK Fields: */
#define WCD9378_MBHC_CTL_CLK_CLK_SEL_MASK                                0x40
#define WCD9378_MBHC_CTL_CLK_COMP_CLK_CTL_MASK                           0x30
#define WCD9378_MBHC_CTL_CLK_COMP_AZ_CTL_MASK                            0x0c
#define WCD9378_MBHC_CTL_CLK_TEST_CLK_EN_MASK                            0x02
#define WCD9378_MBHC_CTL_CLK_COMP_AVG_BYP_EN_MASK                        0x01

/* WCD9378_MBHC_CTL_ANA Fields: */
#define WCD9378_MBHC_CTL_ANA_BIAS_SEL_MASK                               0x80

/* WCD9378_MBHC_CTL_SPARE_1 Fields: */
#define WCD9378_MBHC_CTL_SPARE_1_SPARE_BITS_MASK                         0xfc
#define WCD9378_MBHC_CTL_SPARE_1_BIASGEN_RES_CTRL_MASK                   0x03

/* WCD9378_MBHC_CTL_SPARE_2 Fields: */
#define WCD9378_MBHC_CTL_SPARE_2_SPARE_BITS_MASK                         0xff

/* WCD9378_MBHC_CTL_BCS Fields: */
#define WCD9378_MBHC_CTL_BCS_FAST_INT_OVRD_EN_MASK                       0x80
#define WCD9378_MBHC_CTL_BCS_ELECT_REM_FAST_REG_OVRD_MASK                0x40
#define WCD9378_MBHC_CTL_BCS_BTN_RELEASE_FAST_REG_OVRD_MASK              0x20
#define WCD9378_MBHC_CTL_BCS_BTN_PRESS_FAST_REG_OVRD_MASK                0x10
#define WCD9378_MBHC_CTL_BCS_ANC_DET_EN_MASK                             0x02
#define WCD9378_MBHC_CTL_BCS_DEBUG_1_MASK                                0x01

/* WCD9378_MBHC_MOISTURE_DET_FSM_STATUS Fields: */
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_ELECT_IN2P_COMP_MASK        0x80
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MECH_HS_G_COMP_MASK         0x40
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MECH_HS_M_COMP_MASK         0x20
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MECH_HS_L_COMP_MASK         0x10
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MOISTURE_INTR_MASK          0x08
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MOISTURE_GTPOLLING_STATUS_MASK 0x04
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_MOISTURE_DET_STATUS_MASK    0x02
#define WCD9378_MBHC_MOISTURE_DET_FSM_STATUS_SAMPLE_CLK_LDET_MASK        0x01

/* WCD9378_MBHC_TEST_CTL Fields: */
#define WCD9378_MBHC_TEST_CTL_FAST_DBNC_TIMER_MASK                       0x30
#define WCD9378_MBHC_TEST_CTL_ATEST_MASK                                 0x0f

/* WCD9378_LDOH_MODE Fields: */
#define WCD9378_LDOH_MODE_LDOH_EN_MASK                                   0x80
#define WCD9378_LDOH_MODE_PWRDN_STATE_MASK                               0x40
#define WCD9378_LDOH_MODE_SLOWRAMP_EN_MASK                               0x20
#define WCD9378_LDOH_MODE_VOUT_ADJUST_MASK                               0x18
#define WCD9378_LDOH_MODE_VOUT_COARSE_ADJ_MASK                           0x07

/* WCD9378_LDOH_BIAS Fields: */
#define WCD9378_LDOH_BIAS_IBIAS_REF_MASK                                 0xe0
#define WCD9378_LDOH_BIAS_IBIAS_ERR_AMP_MASK                             0x18
#define WCD9378_LDOH_BIAS_IBIAS_NATIVE_DEVICE_MASK                       0x04
#define WCD9378_LDOH_BIAS_IBIAS_BUFFER_BLEED_MASK                        0x02

/* WCD9378_LDOH_STB_LOADS Fields: */
#define WCD9378_LDOH_STB_LOADS_STB_LOADS_1_UA_MASK                       0xf0
#define WCD9378_LDOH_STB_LOADS_STB_LOAD_10_UA_MASK                       0x08

/* WCD9378_LDOH_SLOWRAMP Fields: */
#define WCD9378_LDOH_SLOWRAMP_SLOWRAMP_IBIAS_MASK                        0xc0
#define WCD9378_LDOH_SLOWRAMP_SLOWRAMP_RESET_TIME_MASK                   0x30

/* WCD9378_MICB1_TEST_CTL_1 Fields: */
#define WCD9378_MICB1_TEST_CTL_1_NOISE_FILT_RES_VAL_MASK                 0xe0
#define WCD9378_MICB1_TEST_CTL_1_EN_VREFGEN_MASK                         0x10
#define WCD9378_MICB1_TEST_CTL_1_EN_LDO_MASK                             0x08
#define WCD9378_MICB1_TEST_CTL_1_LDO_BLEEDER_CTRL_MASK                   0x07

/* WCD9378_MICB1_TEST_CTL_2 Fields: */
#define WCD9378_MICB1_TEST_CTL_2_IBIAS_VREFGEN_MASK                      0xc0
#define WCD9378_MICB1_TEST_CTL_2_INRUSH_CURRENT_FIX_DIS_MASK             0x20
#define WCD9378_MICB1_TEST_CTL_2_SPARE_BITS_MASK                         0x18
#define WCD9378_MICB1_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK                   0x07

/* WCD9378_MICB1_TEST_CTL_3 Fields: */
#define WCD9378_MICB1_TEST_CTL_3_CFILT_REF_EN_MASK                       0x80
#define WCD9378_MICB1_TEST_CTL_3_RZ_LDO_VAL_MASK                         0x70
#define WCD9378_MICB1_TEST_CTL_3_IBIAS_LDO_STG3_MASK                     0x0c
#define WCD9378_MICB1_TEST_CTL_3_ATEST_CTRL_MASK                         0x03

/* WCD9378_MICB2_TEST_CTL_1 Fields: */
#define WCD9378_MICB2_TEST_CTL_1_NOISE_FILT_RES_VAL_MASK                 0xe0
#define WCD9378_MICB2_TEST_CTL_1_EN_VREFGEN_MASK                         0x10
#define WCD9378_MICB2_TEST_CTL_1_EN_LDO_MASK                             0x08
#define WCD9378_MICB2_TEST_CTL_1_LDO_BLEEDER_CTRL_MASK                   0x07

/* WCD9378_MICB2_TEST_CTL_2 Fields: */
#define WCD9378_MICB2_TEST_CTL_2_IBIAS_VREFGEN_MASK                      0xc0
#define WCD9378_MICB2_TEST_CTL_2_INRUSH_CURRENT_FIX_DIS_MASK             0x20
#define WCD9378_MICB2_TEST_CTL_2_SPARE_BITS_MASK                         0x18
#define WCD9378_MICB2_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK                   0x07

/* WCD9378_MICB2_TEST_CTL_3 Fields: */
#define WCD9378_MICB2_TEST_CTL_3_CFILT_REF_EN_MASK                       0x80
#define WCD9378_MICB2_TEST_CTL_3_RZ_LDO_VAL_MASK                         0x70
#define WCD9378_MICB2_TEST_CTL_3_IBIAS_LDO_STG3_MASK                     0x0c
#define WCD9378_MICB2_TEST_CTL_3_ATEST_CTRL_MASK                         0x03

/* WCD9378_MICB3_TEST_CTL_1 Fields: */
#define WCD9378_MICB3_TEST_CTL_1_PRECHARGE_OVERRIDE_MICB1_MASK           0x80
#define WCD9378_MICB3_TEST_CTL_1_PRECHARGE_CLK_SEL_MICB1_MASK            0x60
#define WCD9378_MICB3_TEST_CTL_1_EN_VREFGEN3_MASK                        0x10
#define WCD9378_MICB3_TEST_CTL_1_EN_LDO3_MASK                            0x08
#define WCD9378_MICB3_TEST_CTL_1_LDO_BLEEDER_CTRL3_MASK                  0x07

/* WCD9378_MICB3_TEST_CTL_2 Fields: */
#define WCD9378_MICB3_TEST_CTL_2_FILTER_POLYRES_EN_MICB2_MASK            0x80
#define WCD9378_MICB3_TEST_CTL_2_PRECHARGE_OVERRIDE_MICB2_MASK           0x40
#define WCD9378_MICB3_TEST_CTL_2_INRUSH_CURRENT_FIX_DIS3_MASK            0x20
#define WCD9378_MICB3_TEST_CTL_2_PRECHARGE_CLK_SEL_MICB2_MASK            0x18
#define WCD9378_MICB3_TEST_CTL_2_IBIAS_LDO_DRIVER_MASK                   0x07

/* WCD9378_MICB3_TEST_CTL_3 Fields: */
#define WCD9378_MICB3_TEST_CTL_3_FILTER_MOSRES_EN_MICB2_MASK             0x80
#define WCD9378_MICB3_TEST_CTL_3_RZ_LDO_VAL_MASK                         0x70
#define WCD9378_MICB3_TEST_CTL_3_IBIAS_LDO_STG3_MASK                     0x0c
#define WCD9378_MICB3_TEST_CTL_3_ATEST_CTRL_MASK                         0x03

/* WCD9378_TX_COM_ADC_VCM Fields: */
#define WCD9378_TX_COM_ADC_VCM_VCM_L2_12P288_MASK                        0x30
#define WCD9378_TX_COM_ADC_VCM_VCM_L2_9P6_MASK                           0x0c
#define WCD9378_TX_COM_ADC_VCM_VCM_DEFAULT_MASK                          0x03

/* WCD9378_TX_COM_BIAS_ATEST Fields: */
#define WCD9378_TX_COM_BIAS_ATEST_TX_CURR_EN_MASK                        0x80
#define WCD9378_TX_COM_BIAS_ATEST_SC_BIAS_EN_MASK                        0x40
#define WCD9378_TX_COM_BIAS_ATEST_SC_BIAS_VREF_SEL_MASK                  0x20
#define WCD9378_TX_COM_BIAS_ATEST_ATEST4_EN_MASK                         0x08
#define WCD9378_TX_COM_BIAS_ATEST_ATEST3_EN_MASK                         0x04
#define WCD9378_TX_COM_BIAS_ATEST_ATEST2_EN_MASK                         0x02
#define WCD9378_TX_COM_BIAS_ATEST_ATEST1_EN_MASK                         0x01

/* WCD9378_TX_COM_SPARE1 Fields: */
#define WCD9378_TX_COM_SPARE1_SPARE_BITS_7_0_MASK                        0xff

/* WCD9378_TX_COM_SPARE2 Fields: */
#define WCD9378_TX_COM_SPARE2_SPARE_BITS_7_0_MASK                        0xff

/* WCD9378_TX_COM_TXFE_DIV_CTL Fields: */
#define WCD9378_TX_COM_TXFE_DIV_CTL_SEQ_BYPASS_MASK                      0x80
#define WCD9378_TX_COM_TXFE_DIV_CTL_FB_SW_DRIVE_MASK                     0x20
#define WCD9378_TX_COM_TXFE_DIV_CTL_EN_CKGEN_INIT_MASK                   0x10
#define WCD9378_TX_COM_TXFE_DIV_CTL_N_PAUSE_MASK                         0x03

/* WCD9378_TX_COM_TXFE_DIV_START Fields: */
#define WCD9378_TX_COM_TXFE_DIV_START_DIV_MASK                           0xff

/* WCD9378_TX_COM_SPARE3 Fields: */
#define WCD9378_TX_COM_SPARE3_SPARE_BITS_7_0_MASK                        0xff

/* WCD9378_TX_COM_SPARE4 Fields: */
#define WCD9378_TX_COM_SPARE4_SPARE_BITS_7_0_MASK                        0xff

/* WCD9378_TX_1_2_TEST_EN Fields: */
#define WCD9378_TX_1_2_TEST_EN_TXFE1_EN_MASK                             0x80
#define WCD9378_TX_1_2_TEST_EN_ADC1_EN_MASK                              0x40
#define WCD9378_TX_1_2_TEST_EN_TXFE1_BYPASS_MASK                         0x20
#define WCD9378_TX_1_2_TEST_EN_TXFE1_CLK_MODE_MASK                       0x10
#define WCD9378_TX_1_2_TEST_EN_TXFE2_EN_MASK                             0x08
#define WCD9378_TX_1_2_TEST_EN_ADC2_EN_MASK                              0x04
#define WCD9378_TX_1_2_TEST_EN_TXFE2_BYPASS_MASK                         0x02
#define WCD9378_TX_1_2_TEST_EN_TXFE2_CLK_MODE_MASK                       0x01

/* WCD9378_TX_1_2_ADC_IB Fields: */
#define WCD9378_TX_1_2_ADC_IB_ADC2_DEM_MODE_MASK                         0xc0
#define WCD9378_TX_1_2_ADC_IB_ADC2_DEM_OPERATION_MASK                    0x30
#define WCD9378_TX_1_2_ADC_IB_L2_DAC_DLY_MASK                            0x0c
#define WCD9378_TX_1_2_ADC_IB_DEFAULT_DAC_DLY_MASK                       0x03

/* WCD9378_TX_1_2_ATEST_REFCTL Fields: */
#define WCD9378_TX_1_2_ATEST_REFCTL_ATEST_CTL_MASK                       0xf0
#define WCD9378_TX_1_2_ATEST_REFCTL_TXFE_INCM_REF_MASK                   0x0c
#define WCD9378_TX_1_2_ATEST_REFCTL_TXFE_HP_GAIN_MODE_MASK               0x02
#define WCD9378_TX_1_2_ATEST_REFCTL_SPARE_BITS_0_0_MASK                  0x01

/* WCD9378_TX_1_2_TEST_CTL Fields: */
#define WCD9378_TX_1_2_TEST_CTL_TXFE_HP_GAIN_MASK                        0x80
#define WCD9378_TX_1_2_TEST_CTL_REF_CAP_MASK                             0x40
#define WCD9378_TX_1_2_TEST_CTL_ADC1_DEM_MODE_MASK                       0x30
#define WCD9378_TX_1_2_TEST_CTL_ADC1_DEM_OPERATION_MASK                  0x0c
#define WCD9378_TX_1_2_TEST_CTL_SAR_ERR_DET_EN_MASK                      0x02
#define WCD9378_TX_1_2_TEST_CTL_SAR_EXT_DELAY_EN_MASK                    0x01

/* WCD9378_TX_1_2_TEST_BLK_EN1 Fields: */
#define WCD9378_TX_1_2_TEST_BLK_EN1_ADC1_INT1_EN_MASK                    0x80
#define WCD9378_TX_1_2_TEST_BLK_EN1_ADC1_INT2_EN_MASK                    0x40
#define WCD9378_TX_1_2_TEST_BLK_EN1_ADC1_SAR_EN_MASK                     0x20
#define WCD9378_TX_1_2_TEST_BLK_EN1_ADC1_CMGEN_EN_MASK                   0x10
#define WCD9378_TX_1_2_TEST_BLK_EN1_ADC1_CLKGEN_EN_MASK                  0x08
#define WCD9378_TX_1_2_TEST_BLK_EN1_REF_EN_MASK                          0x04
#define WCD9378_TX_1_2_TEST_BLK_EN1_TXFE1_CLKDIV_EN_MASK                 0x02
#define WCD9378_TX_1_2_TEST_BLK_EN1_TXFE2_CLKDIV_EN_MASK                 0x01

/* WCD9378_TX_1_2_TXFE1_CLKDIV Fields: */
#define WCD9378_TX_1_2_TXFE1_CLKDIV_DIV_MASK                             0xff

/* WCD9378_TX_1_2_SAR2_ERR Fields: */
#define WCD9378_TX_1_2_SAR2_ERR_SAR_ERR_COUNT_MASK                       0xff

/* WCD9378_TX_1_2_SAR1_ERR Fields: */
#define WCD9378_TX_1_2_SAR1_ERR_SAR_ERR_COUNT_MASK                       0xff

/* WCD9378_TX_3_TEST_EN Fields: */
#define WCD9378_TX_3_TEST_EN_TXFE3_EN_MASK                               0x80
#define WCD9378_TX_3_TEST_EN_ADC3_EN_MASK                                0x40
#define WCD9378_TX_3_TEST_EN_TXFE3_BYPASS_MASK                           0x20
#define WCD9378_TX_3_TEST_EN_TXFE3_CLK_MODE_MASK                         0x10
#define WCD9378_TX_3_TEST_EN_SPARE_BITS_3_0_MASK                         0x0f

/* WCD9378_TX_3_ADC_IB Fields: */
#define WCD9378_TX_3_ADC_IB_SPARE_BITS_3_0_MASK                          0xf0
#define WCD9378_TX_3_ADC_IB_L2_DAC_DLY_MASK                              0x0c
#define WCD9378_TX_3_ADC_IB_DEFAULT_DAC_DLY_MASK                         0x03

/* WCD9378_TX_3_ATEST_REFCTL Fields: */
#define WCD9378_TX_3_ATEST_REFCTL_ATEST_CTL_MASK                         0xf0
#define WCD9378_TX_3_ATEST_REFCTL_TXFE_INCM_REF_MASK                     0x0c
#define WCD9378_TX_3_ATEST_REFCTL_TXFE_HP_GAIN_MODE_MASK                 0x02
#define WCD9378_TX_3_ATEST_REFCTL_SPARE_BITS_0_0_MASK                    0x01

/* WCD9378_TX_3_TEST_CTL Fields: */
#define WCD9378_TX_3_TEST_CTL_TXFE_HP_GAIN_MASK                          0x80
#define WCD9378_TX_3_TEST_CTL_REF_CAP_MASK                               0x40
#define WCD9378_TX_3_TEST_CTL_ADC3_DEM_MODE_MASK                         0x30
#define WCD9378_TX_3_TEST_CTL_ADC3_DEM_OPERATION_MASK                    0x0c
#define WCD9378_TX_3_TEST_CTL_SAR_ERR_DET_EN_MASK                        0x02
#define WCD9378_TX_3_TEST_CTL_SAR_EXT_DELAY_EN_MASK                      0x01

/* WCD9378_TX_3_TEST_BLK_EN3 Fields: */
#define WCD9378_TX_3_TEST_BLK_EN3_ADC3_INT1_EN_MASK                      0x80
#define WCD9378_TX_3_TEST_BLK_EN3_ADC3_INT2_EN_MASK                      0x40
#define WCD9378_TX_3_TEST_BLK_EN3_ADC3_SAR_EN_MASK                       0x20
#define WCD9378_TX_3_TEST_BLK_EN3_ADC3_CMGEN_EN_MASK                     0x10
#define WCD9378_TX_3_TEST_BLK_EN3_ADC3_CLKGEN_EN_MASK                    0x08
#define WCD9378_TX_3_TEST_BLK_EN3_REF_EN_MASK                            0x04
#define WCD9378_TX_3_TEST_BLK_EN3_TXFE3_CLKDIV_EN_MASK                   0x02
#define WCD9378_TX_3_TEST_BLK_EN3_SPARE_BITS_0_0_MASK                    0x01

/* WCD9378_TX_3_TXFE3_CLKDIV Fields: */
#define WCD9378_TX_3_TXFE3_CLKDIV_DIV_MASK                               0xff

/* WCD9378_TX_3_SAR4_ERR Fields: */
#define WCD9378_TX_3_SAR4_ERR_SAR_ERR_COUNT_MASK                         0xff

/* WCD9378_TX_3_SAR3_ERR Fields: */
#define WCD9378_TX_3_SAR3_ERR_SAR_ERR_COUNT_MASK                         0xff

/* WCD9378_TX_3_TEST_BLK_EN2 Fields: */
#define WCD9378_TX_3_TEST_BLK_EN2_ADC2_INT1_EN_MASK                      0x80
#define WCD9378_TX_3_TEST_BLK_EN2_ADC2_INT2_EN_MASK                      0x40
#define WCD9378_TX_3_TEST_BLK_EN2_ADC2_SAR_EN_MASK                       0x20
#define WCD9378_TX_3_TEST_BLK_EN2_ADC2_CMGEN_EN_MASK                     0x10
#define WCD9378_TX_3_TEST_BLK_EN2_ADC2_CLKGEN_EN_MASK                    0x08
#define WCD9378_TX_3_TEST_BLK_EN2_ADC12_VREF_NONL2_MASK                  0x06
#define WCD9378_TX_3_TEST_BLK_EN2_TXFE2_MBHC_CLKRST_EN_MASK              0x01

/* WCD9378_TX_3_TXFE2_CLKDIV Fields: */
#define WCD9378_TX_3_TXFE2_CLKDIV_DIV_MASK                               0xff

/* WCD9378_TX_3_SPARE1 Fields: */
#define WCD9378_TX_3_SPARE1_SPARE_BITS_7_0_MASK                          0xff

/* WCD9378_TX_3_TEST_BLK_EN4 Fields: */
#define WCD9378_TX_3_TEST_BLK_EN4_SPARE_BITS_7_3_MASK                    0xf8
#define WCD9378_TX_3_TEST_BLK_EN4_ADC34_VREF_NONL2_MASK                  0x06
#define WCD9378_TX_3_TEST_BLK_EN4_SPARE_BITS_0_0_MASK                    0x01

/* WCD9378_TX_3_SPARE2 Fields: */
#define WCD9378_TX_3_SPARE2_SPARE_BITS_7_0_MASK                          0xff

/* WCD9378_TX_3_SPARE3 Fields: */
#define WCD9378_TX_3_SPARE3_SPARE_BITS_7_0_MASK                          0xff

/* WCD9378_RX_AUX_SW_CTL Fields: */
#define WCD9378_RX_AUX_SW_CTL_AUXL_SW_EN_MASK                            0x80
#define WCD9378_RX_AUX_SW_CTL_AUXR_SW_EN_MASK                            0x40
#define WCD9378_RX_AUX_SW_CTL_AUXL2R_SW_EN_MASK                          0x20

/* WCD9378_RX_PA_AUX_IN_CONN Fields: */
#define WCD9378_RX_PA_AUX_IN_CONN_HPHL_AUX_IN_MASK                       0x80
#define WCD9378_RX_PA_AUX_IN_CONN_HPHR_AUX_IN_MASK                       0x40
#define WCD9378_RX_PA_AUX_IN_CONN_EAR_AUX_IN_MASK                        0x20
#define WCD9378_RX_PA_AUX_IN_CONN_AUX_AUX_IN_MASK                        0x10

/* WCD9378_RX_TIMER_DIV Fields: */
#define WCD9378_RX_TIMER_DIV_RX_CLK_DIVIDER_OVWT_MASK                    0x80
#define WCD9378_RX_TIMER_DIV_RX_CLK_DIVIDER_MASK                         0x7f

/* WCD9378_RX_OCP_CTL Fields: */
#define WCD9378_RX_OCP_CTL_SPARE_BITS_MASK                               0xf0
#define WCD9378_RX_OCP_CTL_N_CONNECTION_ATTEMPTS_MASK                    0x0f

/* WCD9378_RX_OCP_COUNT Fields: */
#define WCD9378_RX_OCP_COUNT_RUN_N_CYCLES_MASK                           0xf0
#define WCD9378_RX_OCP_COUNT_WAIT_N_CYCLES_MASK                          0x0f

/* WCD9378_RX_BIAS_EAR_DAC Fields: */
#define WCD9378_RX_BIAS_EAR_DAC_EAR_DAC_5_UA_MASK                        0xf0
#define WCD9378_RX_BIAS_EAR_DAC_ATEST_RX_BIAS_MASK                       0x0f

/* WCD9378_RX_BIAS_EAR_AMP Fields: */
#define WCD9378_RX_BIAS_EAR_AMP_EAR_AMP_10_UA_MASK                       0xf0
#define WCD9378_RX_BIAS_EAR_AMP_EAR_AMP_5_UA_MASK                        0x0f

/* WCD9378_RX_BIAS_HPH_LDO Fields: */
#define WCD9378_RX_BIAS_HPH_LDO_HPH_NVLDO2_5_UA_MASK                     0xf0
#define WCD9378_RX_BIAS_HPH_LDO_HPH_NVLDO1_4P5_UA_MASK                   0x0f

/* WCD9378_RX_BIAS_HPH_PA Fields: */
#define WCD9378_RX_BIAS_HPH_PA_HPH_CONSTOP_5_UA_MASK                     0xf0
#define WCD9378_RX_BIAS_HPH_PA_HPH_AMP_5_UA_MASK                         0x0f

/* WCD9378_RX_BIAS_HPH_RDACBUFF_CNP2 Fields: */
#define WCD9378_RX_BIAS_HPH_RDACBUFF_CNP2_RDAC_BUF_4_UA_MASK             0xf0
#define WCD9378_RX_BIAS_HPH_RDACBUFF_CNP2_HPH_CNP_10_UA_MASK             0x0f

/* WCD9378_RX_BIAS_HPH_RDAC_LDO Fields: */
#define WCD9378_RX_BIAS_HPH_RDAC_LDO_RDAC_LDO_1P65_4_UA_MASK             0xf0
#define WCD9378_RX_BIAS_HPH_RDAC_LDO_RDAC_LDO_N1P65_4_UA_MASK            0x0f

/* WCD9378_RX_BIAS_HPH_CNP1 Fields: */
#define WCD9378_RX_BIAS_HPH_CNP1_HPH_CNP_4_UA_MASK                       0xf0
#define WCD9378_RX_BIAS_HPH_CNP1_HPH_CNP_3_UA_MASK                       0x0f

/* WCD9378_RX_BIAS_HPH_LOWPOWER Fields: */
#define WCD9378_RX_BIAS_HPH_LOWPOWER_HPH_AMP_LP_1P5_UA_MASK              0xf0
#define WCD9378_RX_BIAS_HPH_LOWPOWER_RDAC_BUF_LP_0P5_UA_MASK             0x0f

/* WCD9378_RX_BIAS_AUX_DAC Fields: */
#define WCD9378_RX_BIAS_AUX_DAC_AUX_DAC_5_UA_MASK                        0xf0

/* WCD9378_RX_BIAS_AUX_AMP Fields: */
#define WCD9378_RX_BIAS_AUX_AMP_AUX_AMP_10_UA_MASK                       0xf0
#define WCD9378_RX_BIAS_AUX_AMP_AUX_AMP_5_UA_MASK                        0x0f

/* WCD9378_RX_SPARE_1 Fields: */
#define WCD9378_RX_SPARE_1_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_2 Fields: */
#define WCD9378_RX_SPARE_2_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_3 Fields: */
#define WCD9378_RX_SPARE_3_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_4 Fields: */
#define WCD9378_RX_SPARE_4_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_5 Fields: */
#define WCD9378_RX_SPARE_5_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_6 Fields: */
#define WCD9378_RX_SPARE_6_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_RX_SPARE_7 Fields: */
#define WCD9378_RX_SPARE_7_SPARE_BITS_7_0_MASK                           0xff

/* WCD9378_HPH_L_STATUS Fields: */
#define WCD9378_HPH_L_STATUS_CMPDR_GAIN_MASK                             0xf8
#define WCD9378_HPH_L_STATUS_OCP_COMP_DETECT_MASK                        0x04
#define WCD9378_HPH_L_STATUS_OCP_LIMIT_MASK                              0x02
#define WCD9378_HPH_L_STATUS_PA_READY_MASK                               0x01

/* WCD9378_HPH_R_STATUS Fields: */
#define WCD9378_HPH_R_STATUS_CMPDR_GAIN_MASK                             0xf8
#define WCD9378_HPH_R_STATUS_OCP_COMP_DETECT_MASK                        0x04
#define WCD9378_HPH_R_STATUS_OCP_LIMIT_MASK                              0x02
#define WCD9378_HPH_R_STATUS_PA_READY_MASK                               0x01

/* WCD9378_HPH_CNP_EN Fields: */
#define WCD9378_HPH_CNP_EN_FSM_CLK_EN_MASK                               0x80
#define WCD9378_HPH_CNP_EN_FSM_RESET_MASK                                0x40
#define WCD9378_HPH_CNP_EN_CNP_IREF_SEL_MASK                             0x20
#define WCD9378_HPH_CNP_EN_FSM_OVERRIDE_EN_MASK                          0x08
#define WCD9378_HPH_CNP_EN_WG_LR_SEL_MASK                                0x04
#define WCD9378_HPH_CNP_EN_DBG_CURR_DIRECTION_R_MASK                     0x02
#define WCD9378_HPH_CNP_EN_DBG_VREF_EN_MASK                              0x01

/* WCD9378_HPH_CNP_WG_CTL Fields: */
#define WCD9378_HPH_CNP_WG_CTL_GM3_BOOST_EN_MASK                         0x80
#define WCD9378_HPH_CNP_WG_CTL_NO_PD_SEQU_MASK                           0x40
#define WCD9378_HPH_CNP_WG_CTL_VREF_TIMER_MASK                           0x38
#define WCD9378_HPH_CNP_WG_CTL_CURR_LDIV_CTL_MASK                        0x07

/* WCD9378_HPH_CNP_WG_TIME Fields: */
#define WCD9378_HPH_CNP_WG_TIME_WG_FINE_TIMER_MASK                       0xff

/* WCD9378_HPH_OCP_CTL Fields: */
#define WCD9378_HPH_OCP_CTL_OCP_CURR_LIMIT_MASK                          0xe0
#define WCD9378_HPH_OCP_CTL_OCP_FSM_EN_MASK                              0x10
#define WCD9378_HPH_OCP_CTL_SPARE_BITS_MASK                              0x08
#define WCD9378_HPH_OCP_CTL_SCD_OP_EN_MASK                               0x02

/* WCD9378_HPH_AUTO_CHOP Fields: */
#define WCD9378_HPH_AUTO_CHOP_AUTO_CHOPPER_MODE_MASK                     0x20
#define WCD9378_HPH_AUTO_CHOP_GAIN_THRESHOLD_MASK                        0x1f

/* WCD9378_HPH_CHOP_CTL Fields: */
#define WCD9378_HPH_CHOP_CTL_CHOPPER_EN_MASK                             0x80
#define WCD9378_HPH_CHOP_CTL_CLK_INV_MASK                                0x40
#define WCD9378_HPH_CHOP_CTL_DIV2_DIV_BY_2_MASK                          0x04
#define WCD9378_HPH_CHOP_CTL_DIV2_DIV_BY_2_4_6_8_MASK                    0x03

/* WCD9378_HPH_PA_CTL1 Fields: */
#define WCD9378_HPH_PA_CTL1_GM3_IBIAS_CTL_MASK                           0xf0
#define WCD9378_HPH_PA_CTL1_GM3_IB_SCALE_MASK                            0x0e

/* WCD9378_HPH_PA_CTL2 Fields: */
#define WCD9378_HPH_PA_CTL2_SPARE_BITS_MASK                              0x80
#define WCD9378_HPH_PA_CTL2_HPHPA_GND_R_MASK                             0x40
#define WCD9378_HPH_PA_CTL2_HPHPA_GND_L_MASK                             0x10

/* WCD9378_HPH_L_EN Fields: */
#define WCD9378_HPH_L_EN_CONST_SEL_L_MASK                                0xc0
#define WCD9378_HPH_L_EN_GAIN_SOURCE_SEL_MASK                            0x20
#define WCD9378_HPH_L_EN_PA_GAIN_MASK                                    0x1f

/* WCD9378_HPH_L_TEST Fields: */
#define WCD9378_HPH_L_TEST_PDN_EN_MASK                                   0x80
#define WCD9378_HPH_L_TEST_PDN_AMP2_EN_MASK                              0x40
#define WCD9378_HPH_L_TEST_PDN_AMP_EN_MASK                               0x20
#define WCD9378_HPH_L_TEST_PA_CNP_SW_CONN_MASK                           0x10
#define WCD9378_HPH_L_TEST_PA_CNP_SW_OFF_MASK                            0x08
#define WCD9378_HPH_L_TEST_PA_CNP_SW_ON_MASK                             0x04
#define WCD9378_HPH_L_TEST_OCP_DET_EN_MASK                               0x01

/* WCD9378_HPH_L_ATEST Fields: */
#define WCD9378_HPH_L_ATEST_DACL_REF_ATEST1_CONN_MASK                    0x80
#define WCD9378_HPH_L_ATEST_LDO1_L_ATEST2_CONN_MASK                      0x40
#define WCD9378_HPH_L_ATEST_LDO_L_ATEST2_CAL_MASK                        0x20
#define WCD9378_HPH_L_ATEST_LDO2_L_ATEST2_CONN_MASK                      0x10
#define WCD9378_HPH_L_ATEST_HPHPA_GND_OVR_MASK                           0x08
#define WCD9378_HPH_L_ATEST_CNP_EXD2_MASK                                0x02
#define WCD9378_HPH_L_ATEST_CNP_EXD1_MASK                                0x01

/* WCD9378_HPH_R_EN Fields: */
#define WCD9378_HPH_R_EN_CONST_SEL_R_MASK                                0xc0
#define WCD9378_HPH_R_EN_GAIN_SOURCE_SEL_MASK                            0x20
#define WCD9378_HPH_R_EN_PA_GAIN_MASK                                    0x1f

/* WCD9378_HPH_R_TEST Fields: */
#define WCD9378_HPH_R_TEST_PDN_EN_MASK                                   0x80
#define WCD9378_HPH_R_TEST_PDN_AMP2_EN_MASK                              0x40
#define WCD9378_HPH_R_TEST_PDN_AMP_EN_MASK                               0x20
#define WCD9378_HPH_R_TEST_PA_CNP_SW_CONN_MASK                           0x10
#define WCD9378_HPH_R_TEST_PA_CNP_SW_OFF_MASK                            0x08
#define WCD9378_HPH_R_TEST_PA_CNP_SW_ON_MASK                             0x04
#define WCD9378_HPH_R_TEST_OCP_DET_EN_MASK                               0x01

/* WCD9378_HPH_R_ATEST Fields: */
#define WCD9378_HPH_R_ATEST_DACR_REF_ATEST1_CONN_MASK                    0x80
#define WCD9378_HPH_R_ATEST_LDO1_R_ATEST2_CONN_MASK                      0x40
#define WCD9378_HPH_R_ATEST_LDO_R_ATEST2_CAL_MASK                        0x20
#define WCD9378_HPH_R_ATEST_LDO2_R_ATEST2_CONN_MASK                      0x10
#define WCD9378_HPH_R_ATEST_LDO_1P65V_ATEST1_CONN_MASK                   0x08
#define WCD9378_HPH_R_ATEST_HPH_GE_EFUSE_MASK                            0x04
#define WCD9378_HPH_R_ATEST_HPHPA_GND_OVR_MASK                           0x02

/* WCD9378_HPH_RDAC_CLK_CTL1 Fields: */
#define WCD9378_HPH_RDAC_CLK_CTL1_OPAMP_CHOP_CLK_EN_MASK                 0x80
#define WCD9378_HPH_RDAC_CLK_CTL1_OPAMP_CHOP_CLK_DIV_CTRL_MASK           0x70
#define WCD9378_HPH_RDAC_CLK_CTL1_SPARE_BITS_MASK                        0x0f

/* WCD9378_HPH_RDAC_CLK_CTL2 Fields: */
#define WCD9378_HPH_RDAC_CLK_CTL2_SPARE_BITS_MASK                        0xf0
#define WCD9378_HPH_RDAC_CLK_CTL2_PREREF_SC_CLK_EN_MASK                  0x08
#define WCD9378_HPH_RDAC_CLK_CTL2_PREREF_SC_CLK_DIVIDER_CTRL_MASK        0x07

/* WCD9378_HPH_RDAC_LDO_CTL Fields: */
#define WCD9378_HPH_RDAC_LDO_CTL_LDO_1P65_BYPASS_MASK                    0x80
#define WCD9378_HPH_RDAC_LDO_CTL_LDO_1P65_OUTCTL_MASK                    0x70
#define WCD9378_HPH_RDAC_LDO_CTL_N1P65V_LDO_BYPASS_MASK                  0x08
#define WCD9378_HPH_RDAC_LDO_CTL_N1P65_LDO_OUTCTL_MASK                   0x07

/* WCD9378_HPH_RDAC_CHOP_CLK_LP_CTL Fields: */
#define WCD9378_HPH_RDAC_CHOP_CLK_LP_CTL_OPAMP_CHOP_CLK_EN_LP_MASK       0x80

/* WCD9378_HPH_REFBUFF_UHQA_CTL Fields: */
#define WCD9378_HPH_REFBUFF_UHQA_CTL_OPAMP_IQ_PROG_MASK                  0xc0
#define WCD9378_HPH_REFBUFF_UHQA_CTL_SPARE_BITS_MASK                     0x3f

/* WCD9378_HPH_REFBUFF_LP_CTL Fields: */
#define WCD9378_HPH_REFBUFF_LP_CTL_SPARE_BITS_MASK                       0xc0
#define WCD9378_HPH_REFBUFF_LP_CTL_OPAMP_IQ_PROG_MASK                    0x30
#define WCD9378_HPH_REFBUFF_LP_CTL_EN_PREREF_FILT_STARTUP_CLKDIV_MASK    0x08
#define WCD9378_HPH_REFBUFF_LP_CTL_PREREF_FILT_STARTUP_CLKDIV_CTL_MASK   0x06
#define WCD9378_HPH_REFBUFF_LP_CTL_PREREF_FILT_BYPASS_MASK               0x01

/* WCD9378_HPH_L_DAC_CTL Fields: */
#define WCD9378_HPH_L_DAC_CTL_DAC_REF_EN_MASK                            0x40
#define WCD9378_HPH_L_DAC_CTL_DAC_SAMPLE_EDGE_SELECT_MASK                0x20
#define WCD9378_HPH_L_DAC_CTL_DATA_RESET_MASK                            0x10
#define WCD9378_HPH_L_DAC_CTL_INV_DATA_MASK                              0x08
#define WCD9378_HPH_L_DAC_CTL_DAC_L_EN_OV_MASK                           0x04
#define WCD9378_HPH_L_DAC_CTL_DAC_LDO_UHQA_OV_MASK                       0x02
#define WCD9378_HPH_L_DAC_CTL_DAC_LDO_POWERMODE_MASK                     0x01

/* WCD9378_HPH_R_DAC_CTL Fields: */
#define WCD9378_HPH_R_DAC_CTL_DAC_REF_EN_MASK                            0x40
#define WCD9378_HPH_R_DAC_CTL_DAC_SAMPLE_EDGE_SELECT_MASK                0x20
#define WCD9378_HPH_R_DAC_CTL_DATA_RESET_MASK                            0x10
#define WCD9378_HPH_R_DAC_CTL_INV_DATA_MASK                              0x08
#define WCD9378_HPH_R_DAC_CTL_DAC_R_EN_OV_MASK                           0x04
#define WCD9378_HPH_R_DAC_CTL_DAC_PREREF_UHQA_OV_MASK                    0x02
#define WCD9378_HPH_R_DAC_CTL_DAC_PREREF_POWERMODE_MASK                  0x01

/* WCD9378_HPH_SURGE_HPHLR_SURGE_COMP_SEL Fields: */
#define WCD9378_HPH_SURGE_HPHLR_SURGE_COMP_SEL_COMP_REF_SEL_HPHL_PSURGE_MASK 0xc0
#define WCD9378_HPH_SURGE_HPHLR_SURGE_COMP_SEL_COMP_REF_SEL_HPHL_NSURGE_MASK 0x30
#define WCD9378_HPH_SURGE_HPHLR_SURGE_COMP_SEL_COMP_REF_SEL_HPHR_PSURGE_MASK 0x0c
#define WCD9378_HPH_SURGE_HPHLR_SURGE_COMP_SEL_COMP_REF_SEL_HPHR_NSURGE_MASK 0x03

/* WCD9378_HPH_SURGE_HPHLR_SURGE_EN Fields: */
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_EN_SURGE_PROTECTION_HPHL_MASK   0x80
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_EN_SURGE_PROTECTION_HPHR_MASK   0x40
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_SEL_SURGE_COMP_IQ_MASK          0x30
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_SURGE_VOLT_MODE_SHUTOFF_EN_MASK 0x08
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_LATCH_INTR_OP_STG_HIZ_EN_MASK   0x04
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_SURGE_LATCH_REG_RESET_MASK      0x02
#define WCD9378_HPH_SURGE_HPHLR_SURGE_EN_SWTICH_VN_VNDAC_NSURGE_EN_MASK  0x01

/* WCD9378_HPH_SURGE_HPHLR_SURGE_MISC1 Fields: */
#define WCD9378_HPH_SURGE_HPHLR_SURGE_MISC1_EN_VNEG_PULLDN_MASK          0x80
#define WCD9378_HPH_SURGE_HPHLR_SURGE_MISC1_EN_OFFSET_36MV_NSURGE_RESLADDER_MASK 0x40
#define WCD9378_HPH_SURGE_HPHLR_SURGE_MISC1_EN_NMOS_LAMP_MASK            0x20
#define WCD9378_HPH_SURGE_HPHLR_SURGE_MISC1_SPARE_BITS_MASK              0x1f

/* WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS Fields: */
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHL_CLAMP_SW_STATUS_MASK   0x80
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHR_CLAMP_SW_STATUS_MASK   0x40
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHL_PSURGE_COMP_STATUS_MASK 0x20
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHL_NSURGE_COMP_STATUS_MASK 0x10
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHR_PSURGE_COMP_STATUS_MASK 0x08
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHR_NSURGE_COMP_STATUS_MASK 0x04
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHL_SURGE_DET_INTR_EN_MASK 0x02
#define WCD9378_HPH_SURGE_HPHLR_SURGE_STATUS_HPHR_SURGE_DET_INTR_EN_MASK 0x01

/* WCD9378_EAR_EAR_EN_REG Fields: */
#define WCD9378_EAR_EAR_EN_REG_EAR_DAC_DATA_RESET_MASK                   0x80
#define WCD9378_EAR_EAR_EN_REG_EAR_DAC_DATA_EN_MASK                      0x40
#define WCD9378_EAR_EAR_EN_REG_EAR_DAC_REF_EN_MASK                       0x20
#define WCD9378_EAR_EAR_EN_REG_EAR_VCM_EN_MASK                           0x10
#define WCD9378_EAR_EAR_EN_REG_EAR_AMP_EN_MASK                           0x08
#define WCD9378_EAR_EAR_EN_REG_EAR_BIAS_EN_MASK                          0x04
#define WCD9378_EAR_EAR_EN_REG_EAR_CNP_FSM_EN_MASK                       0x02
#define WCD9378_EAR_EAR_EN_REG_EAR_OUTPUT_SHORT_MASK                     0x01

/* WCD9378_EAR_EAR_PA_CON Fields: */
#define WCD9378_EAR_EAR_PA_CON_EAR_ANA_AUX_EN_MASK                       0x80
#define WCD9378_EAR_EAR_PA_CON_EAR_CMFB_SF_BYPASS_MASK                   0x40
#define WCD9378_EAR_EAR_PA_CON_EAR_SF_CURR_MASK                          0x20
#define WCD9378_EAR_EAR_PA_CON_EAR_BTI_CTL_MASK                          0x10
#define WCD9378_EAR_EAR_PA_CON_EAR_GM3_IBIAS_CTL_MASK                    0x0f

/* WCD9378_EAR_EAR_SP_CON Fields: */
#define WCD9378_EAR_EAR_SP_CON_EAR_SP_INT_EN_MASK                        0x80
#define WCD9378_EAR_EAR_SP_CON_EAR_SP_AUTO_SHT_DWN_MASK                  0x40
#define WCD9378_EAR_EAR_SP_CON_SP_LIMIT_CURR_NMOS_MASK                   0x38
#define WCD9378_EAR_EAR_SP_CON_SP_LIMIT_CURR_PMOS_MASK                   0x07

/* WCD9378_EAR_EAR_DAC_CON Fields: */
#define WCD9378_EAR_EAR_DAC_CON_DAC_SAMPLE_EDGE_SEL_MASK                 0x80
#define WCD9378_EAR_EAR_DAC_CON_REF_DBG_EN_MASK                          0x40
#define WCD9378_EAR_EAR_DAC_CON_REF_DBG_GAIN_MASK                        0x38
#define WCD9378_EAR_EAR_DAC_CON_GAIN_DAC_MASK                            0x06
#define WCD9378_EAR_EAR_DAC_CON_INV_DATA_MASK                            0x01

/* WCD9378_EAR_EAR_CNP_FSM_CON Fields: */
#define WCD9378_EAR_EAR_CNP_FSM_CON_CNP_FSM_CLK_DIV1_MASK                0xf0
#define WCD9378_EAR_EAR_CNP_FSM_CON_CNP_FSM_CLK_DIV2_MASK                0x0c
#define WCD9378_EAR_EAR_CNP_FSM_CON_SCD_FSM_DEGLITCH_SEL_MASK            0x03

/* WCD9378_EAR_TEST_CTL Fields: */
#define WCD9378_EAR_TEST_CTL_DTEST_EN_MASK                               0x80
#define WCD9378_EAR_TEST_CTL_DTEST_SEL_2_MASK                            0x40
#define WCD9378_EAR_TEST_CTL_EAR_RDAC_ATEST_EN_MASK                      0x20
#define WCD9378_EAR_TEST_CTL_EAR_PA_ATEST_SEL_MASK                       0x1f

/* WCD9378_EAR_STATUS_REG_1 Fields: */
#define WCD9378_EAR_STATUS_REG_1_SP_INT_MASK                             0x80
#define WCD9378_EAR_STATUS_REG_1_SP_ALL_OUT_MASK                         0x40
#define WCD9378_EAR_STATUS_REG_1_SP_NMOS_OUT_MASK                        0x20
#define WCD9378_EAR_STATUS_REG_1_SP_PMOS_OUT_MASK                        0x10
#define WCD9378_EAR_STATUS_REG_1_PA_READY_MASK                           0x08
#define WCD9378_EAR_STATUS_REG_1_CNP_FSM_STATUS_MASK                     0x04

/* WCD9378_EAR_STATUS_REG_2 Fields: */
#define WCD9378_EAR_STATUS_REG_2_PA_EN_MASK                              0x80
#define WCD9378_EAR_STATUS_REG_2_BIAS_EN_MASK                            0x40
#define WCD9378_EAR_STATUS_REG_2_DAC_EN_MASK                             0x20
#define WCD9378_EAR_STATUS_REG_2_VCM_EN_MASK                             0x10
#define WCD9378_EAR_STATUS_REG_2_CLK_EN_MASK                             0x08
#define WCD9378_EAR_STATUS_REG_2_SCD_EN_MASK                             0x04
#define WCD9378_EAR_STATUS_REG_2_SHORT_EN_MASK                           0x02
#define WCD9378_EAR_STATUS_REG_2_DAC_RESET_MASK                          0x01

/* WCD9378_A_PAGE Fields: */
#define WCD9378_A_PAGE_VALUE_MASK                                        0xff

/* WCD9378_HPH_NEW_ANA_HPH2 Fields: */
#define WCD9378_HPH_NEW_ANA_HPH2_LP_PWR_CTL_MASK                         0xc0
#define WCD9378_HPH_NEW_ANA_HPH2_SPARE_BITS_MASK                         0x3f

/* WCD9378_HPH_NEW_ANA_HPH3 Fields: */
#define WCD9378_HPH_NEW_ANA_HPH3_SPARE_BITS_MASK                         0xff

/* WCD9378_SLEEP_CTL Fields: */
#define WCD9378_SLEEP_CTL_BG_EN_MASK                                     0x80
#define WCD9378_SLEEP_CTL_LDOL_BG_SEL_MASK                               0x40
#define WCD9378_SLEEP_CTL_LDORT_REF_SEL_MASK                             0x30
#define WCD9378_SLEEP_CTL_BG_CTL_MASK                                    0x0e
#define WCD9378_SLEEP_CTL_DUALVIO_DTEST_EN_MASK                          0x01

/* WCD9378_SLEEP_WATCHDOG_CTL Fields: */
#define WCD9378_SLEEP_WATCHDOG_CTL_EN_WATCHDOG_MASK                      0x80
#define WCD9378_SLEEP_WATCHDOG_CTL_EN_WATCHDOG_VREFGEN_MASK              0x40
#define WCD9378_SLEEP_WATCHDOG_CTL_BYPASS_WATCHDOG_MASK                  0x20
#define WCD9378_SLEEP_WATCHDOG_CTL_ATEST_CTL_MASK                        0x1c

/* WCD9378_MBHC_NEW_ELECT_REM_CLAMP_CTL Fields: */
#define WCD9378_MBHC_NEW_ELECT_REM_CLAMP_CTL_FSM_ELECT_CLAMP_EN_MASK     0x80
#define WCD9378_MBHC_NEW_ELECT_REM_CLAMP_CTL_SLNQ_ELECT_CLAMP_EN_MASK    0x40
#define WCD9378_MBHC_NEW_ELECT_REM_CLAMP_CTL_SLNQ_FAIL_CLAMP_EN_MASK     0x20
#define WCD9378_MBHC_NEW_ELECT_REM_CLAMP_CTL_SLNQ_ELECT_REM_RST_MASK     0x10

/* WCD9378_MBHC_NEW_CTL_1 Fields: */
#define WCD9378_MBHC_NEW_CTL_1_RCO_EN_MASK                               0x80
#define WCD9378_MBHC_NEW_CTL_1_ADC_MODE_MASK                             0x40
#define WCD9378_MBHC_NEW_CTL_1_DETECTION_DONE_MASK                       0x20
#define WCD9378_MBHC_NEW_CTL_1_ADC_ENABLE_MASK                           0x10
#define WCD9378_MBHC_NEW_CTL_1_BTN_DBNC_CTL_MASK                         0x0f

/* WCD9378_MBHC_NEW_CTL_2 Fields: */
#define WCD9378_MBHC_NEW_CTL_2_MUX_CTL_MASK                              0x70
#define WCD9378_MBHC_NEW_CTL_2_M_RTH_CTL_MASK                            0x0c
#define WCD9378_MBHC_NEW_CTL_2_HS_VREF_CTL_MASK                          0x03

/* WCD9378_MBHC_NEW_PLUG_DETECT_CTL Fields: */
#define WCD9378_MBHC_NEW_PLUG_DETECT_CTL_SPARE_BITS_7_6_MASK             0xc0
#define WCD9378_MBHC_NEW_PLUG_DETECT_CTL_MIC_CLAMP_CTL_MASK              0x30
#define WCD9378_MBHC_NEW_PLUG_DETECT_CTL_INSREM_DBNC_CTL_MASK            0x0f

/* WCD9378_MBHC_NEW_ZDET_ANA_CTL Fields: */
#define WCD9378_MBHC_NEW_ZDET_ANA_CTL_AVERAGING_EN_MASK                  0x80
#define WCD9378_MBHC_NEW_ZDET_ANA_CTL_ZDET_MAXV_CTL_MASK                 0x70
#define WCD9378_MBHC_NEW_ZDET_ANA_CTL_ZDET_RANGE_CTL_MASK                0x0f

/* WCD9378_MBHC_NEW_ZDET_RAMP_CTL Fields: */
#define WCD9378_MBHC_NEW_ZDET_RAMP_CTL_ZDET_RAMP_TIME_CTL_MASK           0x0f

/* WCD9378_MBHC_NEW_FSM_STATUS Fields: */
#define WCD9378_MBHC_NEW_FSM_STATUS_ADC_TIMEOUT_MASK                     0x80
#define WCD9378_MBHC_NEW_FSM_STATUS_ADC_COMPLETE_MASK                    0x40
#define WCD9378_MBHC_NEW_FSM_STATUS_HS_M_COMP_STATUS_MASK                0x20
#define WCD9378_MBHC_NEW_FSM_STATUS_FAST_PRESS_FLAG_STATUS_MASK          0x10
#define WCD9378_MBHC_NEW_FSM_STATUS_FAST_REMOVAL_FLAG_STATUS_MASK        0x08
#define WCD9378_MBHC_NEW_FSM_STATUS_REMOVAL_FLAG_STATUS_MASK             0x04
#define WCD9378_MBHC_NEW_FSM_STATUS_ELECT_REM_RT_STATUS_MASK             0x02
#define WCD9378_MBHC_NEW_FSM_STATUS_BTN_STATUS_MASK                      0x01

/* WCD9378_MBHC_NEW_ADC_RESULT Fields: */
#define WCD9378_MBHC_NEW_ADC_RESULT_ADC_RESULT_MASK                      0xff

/* WCD9378_AUX_AUXPA Fields: */
#define WCD9378_AUX_AUXPA_AUX_PA_EN_MASK                                 0x80
#define WCD9378_AUX_AUXPA_AUX_PA_SHORT_PROT_EN_MASK                      0x40
#define WCD9378_AUX_AUXPA_AUX_PA_OUT_IMP_MASK                            0x20
#define WCD9378_AUX_AUXPA_AUX_PA_CLK_SEL_MASK                            0x10

/* WCD9378_DIE_CRACK_DIE_CRK_DET_EN Fields: */
#define WCD9378_DIE_CRACK_DIE_CRK_DET_EN_DIE_CRK_DET_EN_MASK             0x80
#define WCD9378_DIE_CRACK_DIE_CRK_DET_EN_SEL_CURR_INJCT_PT_MRING_MASK    0x40

/* WCD9378_DIE_CRACK_DIE_CRK_DET_OUT Fields: */
#define WCD9378_DIE_CRACK_DIE_CRK_DET_OUT_DIE_CRK_DET_OUT_MASK           0x80

/* WCD9378_TX_NEW_TX_CH12_MUX Fields: */
#define WCD9378_TX_NEW_TX_CH12_MUX_SPARE_BITS_MASK                       0x80
#define WCD9378_TX_NEW_TX_CH12_MUX_SYS_USAGE_BYP_MASK                    0x40
#define WCD9378_TX_NEW_TX_CH12_MUX_CH2_SEL_MASK                          0x38
#define WCD9378_TX_NEW_TX_CH12_MUX_CH1_SEL_MASK                          0x07

/* WCD9378_TX_NEW_TX_CH34_MUX Fields: */
#define WCD9378_TX_NEW_TX_CH34_MUX_SPARE_BITS_MASK                       0xf8
#define WCD9378_TX_NEW_TX_CH34_MUX_CH3_SEL_MASK                          0x07

/* WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL Fields: */
#define WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL_RDAC_GAINCTL_MASK              0xf0
#define WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL_REFBUF_CMFB2_ZERO_PROG_MASK    0x08
#define WCD9378_HPH_NEW_INT_RDAC_GAIN_CTL_SPARE_BITS_MASK                0x07

/* WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L Fields: */
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_EN_HD2_RES_DIV_L_MASK         0x80
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_HD2_RES_DIV_PULLGND_L_MASK    0x40
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_SPARE_BITS_MASK               0x20
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_SELECT_HD2_RES_DIV_L_MASK     0x10
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_L_HD2_RES_DIV_CTL_L_MASK        0x0f

/* WCD9378_HPH_NEW_INT_RDAC_VREF_CTL Fields: */
#define WCD9378_HPH_NEW_INT_RDAC_VREF_CTL_EN_REFCURRENT_RDEG_SHORT_MASK  0x80
#define WCD9378_HPH_NEW_INT_RDAC_VREF_CTL_RDAC_REFBUF_RFB_9K_MASK        0x40
#define WCD9378_HPH_NEW_INT_RDAC_VREF_CTL_LP_RDAC_REFBUF_RFB_CTL_MASK    0x30
#define WCD9378_HPH_NEW_INT_RDAC_VREF_CTL_RDAC_REFCURRENT_IREF_2UA_MASK  0x08
#define WCD9378_HPH_NEW_INT_RDAC_VREF_CTL_SPARE_BITS_MASK                0x07

/* WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL Fields: */
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_REFBUF_RFB_OVRIDE_MASK     0x80
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_REFBUF_IREF_OVRIDE_MASK    0x40
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_REFCURRENT_RDEG_CTL_OVRIDE_MASK 0x20
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_REFBUF_CMFB2_ZERO_OVRIDE_MASK 0x10
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_RDAC_IDLE_DETECT_OVERRIDE_MASK 0x08
#define WCD9378_HPH_NEW_INT_RDAC_OVERRIDE_CTL_SPARE_BITS_MASK            0x07

/* WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R Fields: */
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_EN_HD2_RES_DIV_R_MASK         0x80
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_HD2_RES_DIV_PULLGND_R_MASK    0x40
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_SPARE_BITS_MASK               0x20
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_SELECT_HD2_RES_DIV_R_MASK     0x10
#define WCD9378_HPH_NEW_INT_RDAC_HD2_CTL_R_HD2_RES_DIV_CTL_R_MASK        0x0f

/* WCD9378_HPH_NEW_INT_PA_MISC1 Fields: */
#define WCD9378_HPH_NEW_INT_PA_MISC1_EN_AUTO_CMPDR_DETECTION_MASK        0x80
#define WCD9378_HPH_NEW_INT_PA_MISC1_EN_PA_IDLE_DETECT_OVERRIDE_MASK     0x40
#define WCD9378_HPH_NEW_INT_PA_MISC1_D_PZ_INF_EN_MASK                    0x20
#define WCD9378_HPH_NEW_INT_PA_MISC1_SPARE_BITS_MASK                     0x18
#define WCD9378_HPH_NEW_INT_PA_MISC1_PA_CHOP_EN_OVERRIDE_MASK            0x04
#define WCD9378_HPH_NEW_INT_PA_MISC1_OCP_FSM_LOCK_EN_MASK                0x02
#define WCD9378_HPH_NEW_INT_PA_MISC1_AUTOCHOP_PDN_SEQ_OVERRIDE_MASK      0x01

/* WCD9378_HPH_NEW_INT_PA_MISC2 Fields: */
#define WCD9378_HPH_NEW_INT_PA_MISC2_HPHPA_HI_Z_MASK                     0x80
#define WCD9378_HPH_NEW_INT_PA_MISC2_HPH_PSRR_ENH_MASK                   0x40
#define WCD9378_HPH_NEW_INT_PA_MISC2_FORCE_IQCTRL_MASK                   0x20
#define WCD9378_HPH_NEW_INT_PA_MISC2_FORCE_PSRREH_MASK                   0x10
#define WCD9378_HPH_NEW_INT_PA_MISC2_CHOP_CLKLAP_SEL_MASK                0x08
#define WCD9378_HPH_NEW_INT_PA_MISC2_SPARE_BITS_MASK                     0x04
#define WCD9378_HPH_NEW_INT_PA_MISC2_IDLE_DETECT_L_DTEST_ENABLE_MASK     0x02
#define WCD9378_HPH_NEW_INT_PA_MISC2_IDLE_DETECT_R_DTEST_ENABLE_MASK     0x01

/* WCD9378_HPH_NEW_INT_PA_RDAC_MISC Fields: */
#define WCD9378_HPH_NEW_INT_PA_RDAC_MISC_CNP_WG_FINE_TIME_LSB_CTL_MASK   0xf0
#define WCD9378_HPH_NEW_INT_PA_RDAC_MISC_SPARE_BITS_MASK                 0x0c
#define WCD9378_HPH_NEW_INT_PA_RDAC_MISC_RDAC_PSW_REG_CTL_MASK           0x03

/* WCD9378_HPH_NEW_INT_HPH_TIMER1 Fields: */
#define WCD9378_HPH_NEW_INT_HPH_TIMER1_CURR_IDIV_CTL_CMPDR_OFF_MASK      0xe0
#define WCD9378_HPH_NEW_INT_HPH_TIMER1_CURR_IDIV_CTL_AUTOCHOP_MASK       0x1c
#define WCD9378_HPH_NEW_INT_HPH_TIMER1_AUTOCHOP_TIMER_CTL_EN_MASK        0x02
#define WCD9378_HPH_NEW_INT_HPH_TIMER1_SPARE_BITS_MASK                   0x01

/* WCD9378_HPH_NEW_INT_HPH_TIMER2 Fields: */
#define WCD9378_HPH_NEW_INT_HPH_TIMER2_VREF_TIMER_IDLESTATE_MASK         0xe0
#define WCD9378_HPH_NEW_INT_HPH_TIMER2_CNP_WG_FINE_TIME_LSB_CTL_IDLE_MASK 0x1e
#define WCD9378_HPH_NEW_INT_HPH_TIMER2_SPARE_BITS_MASK                   0x01

/* WCD9378_HPH_NEW_INT_HPH_TIMER3 Fields: */
#define WCD9378_HPH_NEW_INT_HPH_TIMER3_WG_FINE_TIMER_CMPDR_OFF_MASK      0xff

/* WCD9378_HPH_NEW_INT_HPH_TIMER4 Fields: */
#define WCD9378_HPH_NEW_INT_HPH_TIMER4_WG_FINE_TIMER_AUTOCHOP_MASK       0xff

/* WCD9378_HPH_NEW_INT_PA_RDAC_MISC2 Fields: */
#define WCD9378_HPH_NEW_INT_PA_RDAC_MISC2_SPARE_BITS_MASK                0xff

/* WCD9378_HPH_NEW_INT_PA_RDAC_MISC3 Fields: */
#define WCD9378_HPH_NEW_INT_PA_RDAC_MISC3_SPARE_BITS_MASK                0xff

/* WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_LOHIFI Fields: */
#define WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_LOHIFI_HPHPA_BIAS_LOHIFI_MASK   0xf0
#define WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_LOHIFI_HPHRDAC_BIAS_LOHIFI_MASK 0x0f

/* WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_ULP Fields: */
#define WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_ULP_SPARE_BITS_MASK             0xf0
#define WCD9378_RX_NEW_INT_HPH_RDAC_BIAS_ULP_HPHRDAC_BIAS_ULP_MASK       0x0f

/* WCD9378_RX_NEW_INT_HPH_RDAC_LDO_LP Fields: */
#define WCD9378_RX_NEW_INT_HPH_RDAC_LDO_LP_HPHRDAC_1P6VLDO_BIAS_LP_MASK  0xf0
#define WCD9378_RX_NEW_INT_HPH_RDAC_LDO_LP_HPHRDAC_N1P6VLDO_BIAS_LP_MASK 0x0f

/* WCD9378_CP_CLASSG_CP_CTRL_0 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_0_DIS_CP_LDO_MASK                      0x10
#define WCD9378_CP_CLASSG_CP_CTRL_0_EN_VPOS_CMP_MASK                     0x08
#define WCD9378_CP_CLASSG_CP_CTRL_0_EN_VPOS_CMP_OV_MASK                  0x04
#define WCD9378_CP_CLASSG_CP_CTRL_0_EN_CP_VAL_MASK                       0x02
#define WCD9378_CP_CLASSG_CP_CTRL_0_EN_CP_OV_MASK                        0x01

/* WCD9378_CP_CLASSG_CP_CTRL_1 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_1_TNOV_SEL_MASK                        0x01

/* WCD9378_CP_CLASSG_CP_CTRL_2 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_2_IB_LDO_SEL_MASK                      0xc0
#define WCD9378_CP_CLASSG_CP_CTRL_2_VGN_LDO_SEL_MASK                     0x3c
#define WCD9378_CP_CLASSG_CP_CTRL_2_SW_SIZE_MASK                         0x03

/* WCD9378_CP_CLASSG_CP_CTRL_3 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_3_IB_CMP_SEL_MASK                      0x60
#define WCD9378_CP_CLASSG_CP_CTRL_3_VHYST_CMP_SEL_MASK                   0x18
#define WCD9378_CP_CLASSG_CP_CTRL_3_VTH_CMP_SEL_MASK                     0x07

/* WCD9378_CP_CLASSG_CP_CTRL_4 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_4_DTEST_EN_MASK                        0x80
#define WCD9378_CP_CLASSG_CP_CTRL_4_DTEST_SEL_MASK                       0x70
#define WCD9378_CP_CLASSG_CP_CTRL_4_ATEST_EN_MASK                        0x08
#define WCD9378_CP_CLASSG_CP_CTRL_4_ATEST_SEL_MASK                       0x07

/* WCD9378_CP_CLASSG_CP_CTRL_5 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_5_VPOS_FILT_RSEL_MASK                  0x0c
#define WCD9378_CP_CLASSG_CP_CTRL_5_VDD_CP_LPF_R_SEL_MASK                0x03

/* WCD9378_CP_CLASSG_CP_CTRL_6 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_6_SPARE_BITS_7_0_MASK                  0xff

/* WCD9378_CP_CLASSG_CP_CTRL_7 Fields: */
#define WCD9378_CP_CLASSG_CP_CTRL_7_SPARE_BITS_7_0_MASK                  0xff

/* WCD9378_CP_VNEGDAC_CTRL_0 Fields: */
#define WCD9378_CP_VNEGDAC_CTRL_0_IB_LDO_SEL_MASK                        0xc0
#define WCD9378_CP_VNEGDAC_CTRL_0_VGN_LDO_SEL_MASK                       0x3c
#define WCD9378_CP_VNEGDAC_CTRL_0_SW_SIZE_MASK                           0x03

/* WCD9378_CP_VNEGDAC_CTRL_1 Fields: */
#define WCD9378_CP_VNEGDAC_CTRL_1_TNOV_SEL_NCP_MASK                      0x01

/* WCD9378_CP_VNEGDAC_CTRL_2 Fields: */
#define WCD9378_CP_VNEGDAC_CTRL_2_SPARE_BITS_7_0_MASK                    0xff

/* WCD9378_CP_VNEGDAC_CTRL_3 Fields: */
#define WCD9378_CP_VNEGDAC_CTRL_3_SPARE_BITS_7_0_MASK                    0xff

/* WCD9378_CP_CP_DTOP_CTRL_0 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_0_SWR_CLK_RATE_MASK                      0x80
#define WCD9378_CP_CP_DTOP_CTRL_0_CP_CLK_EDGE_SEL_MASK                   0x40
#define WCD9378_CP_CP_DTOP_CTRL_0_TEST_INT_CLK_MASK                      0x20
#define WCD9378_CP_CP_DTOP_CTRL_0_NCP_CLK_EDGE_SEL_MASK                  0x10
#define WCD9378_CP_CP_DTOP_CTRL_0_NCP_TEST_INT_CLK_MASK                  0x08
#define WCD9378_CP_CP_DTOP_CTRL_0_OVERRIDE_SWR_CLK_RATE_MASK             0x04

/* WCD9378_CP_CP_DTOP_CTRL_1 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_1_VTH_1_SEL_MASK                         0x38
#define WCD9378_CP_CP_DTOP_CTRL_1_VTH_0_SEL_MASK                         0x07

/* WCD9378_CP_CP_DTOP_CTRL_2 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_2_VTH_3_SEL_MASK                         0x38
#define WCD9378_CP_CP_DTOP_CTRL_2_VTH_2_SEL_MASK                         0x07

/* WCD9378_CP_CP_DTOP_CTRL_3 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_3_VTH_5_SEL_MASK                         0x38
#define WCD9378_CP_CP_DTOP_CTRL_3_VTH_4_SEL_MASK                         0x07

/* WCD9378_CP_CP_DTOP_CTRL_4 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_4_VTH_7_SEL_MASK                         0x38
#define WCD9378_CP_CP_DTOP_CTRL_4_VTH_6_SEL_MASK                         0x07

/* WCD9378_CP_CP_DTOP_CTRL_5 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_5_VTH_9_SEL_MASK                         0x38
#define WCD9378_CP_CP_DTOP_CTRL_5_VTH_8_SEL_MASK                         0x07

/* WCD9378_CP_CP_DTOP_CTRL_6 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_6_VTH_11_SEL_MASK                        0x38
#define WCD9378_CP_CP_DTOP_CTRL_6_VTH_10_SEL_MASK                        0x07

/* WCD9378_CP_CP_DTOP_CTRL_7 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_7_VTH_12_SEL_MASK                        0x07

/* WCD9378_CP_CP_DTOP_CTRL_8 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_8_TPW_G0P5_PH1_SEL_MASK                  0xf0
#define WCD9378_CP_CP_DTOP_CTRL_8_TPW_G0P5_PH2_SEL_MASK                  0x0f

/* WCD9378_CP_CP_DTOP_CTRL_9 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_9_TPW_G1_PH1_SEL_MASK                    0xf0
#define WCD9378_CP_CP_DTOP_CTRL_9_DISABLE_TWAIT_MASK                     0x08
#define WCD9378_CP_CP_DTOP_CTRL_9_TWAIT_G1_STEP3_MASK                    0x07

/* WCD9378_CP_CP_DTOP_CTRL_10 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_10_TWAIT_G1_STEP2_MASK                   0x38
#define WCD9378_CP_CP_DTOP_CTRL_10_TWAIT_G1_STEP1_MASK                   0x07

/* WCD9378_CP_CP_DTOP_CTRL_11 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_11_TWAIT_G0P5_STEP3_MASK                 0x07

/* WCD9378_CP_CP_DTOP_CTRL_12 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_12_TWAIT_G0P5_STEP2_MASK                 0x38
#define WCD9378_CP_CP_DTOP_CTRL_12_TWAIT_G0P5_STEP1_MASK                 0x07

/* WCD9378_CP_CP_DTOP_CTRL_13 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_13_INVERT_CP_CLKS_MASK                   0x80
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S10P_MASK                    0x40
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S6P_MASK                     0x20
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S5N_MASK                     0x10
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S4N_MASK                     0x08
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S3P_MASK                     0x04
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S2N_MASK                     0x02
#define WCD9378_CP_CP_DTOP_CTRL_13_GATE_OFF_S1P_MASK                     0x01

/* WCD9378_CP_CP_DTOP_CTRL_14 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_VREF_MASK                    0x80
#define WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_FSW_VAL_MASK                 0x78
#define WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_FSW_MASK                     0x04
#define WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_G_VAL_MASK                   0x02
#define WCD9378_CP_CP_DTOP_CTRL_14_OVERRIDE_G_MASK                       0x01

/* WCD9378_CP_CP_DTOP_CTRL_15 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_15_OVERRIDE_VREF_VAL_MASK                0xff

/* WCD9378_CP_CP_DTOP_CTRL_16 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_16_OVERRIDE_SWSIZE_VAL_MASK              0x06
#define WCD9378_CP_CP_DTOP_CTRL_16_OVERRIDE_SWSIZE_MASK                  0x01

/* WCD9378_CP_CP_DTOP_CTRL_17 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_17_TPW_PH1_SEL_MASK                      0x0f

/* WCD9378_CP_CP_DTOP_CTRL_18 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_18_OVERRIDE_NCP_FSW_VAL_MASK             0x78
#define WCD9378_CP_CP_DTOP_CTRL_18_OVERRIDE_NCP_FSW_MASK                 0x04
#define WCD9378_CP_CP_DTOP_CTRL_18_INVERT_CLKS_MASK                      0x02
#define WCD9378_CP_CP_DTOP_CTRL_18_GATE_OFF_NCP_CLK_MASK                 0x01

/* WCD9378_CP_CP_DTOP_CTRL_19 Fields: */
#define WCD9378_CP_CP_DTOP_CTRL_19_SPARE_BITS_7_0_MASK                   0xff

/* WCD9378_MBHC_NEW_INT_MOISTURE_DET_DC_CTRL Fields: */
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_DC_CTRL_ONCOUNT_MASK           0x60
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_DC_CTRL_OFFCOUNT_MASK          0x1f

/* WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL Fields: */
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL_HPHL_PA_EN_MASK   0x40
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL_DTEST_EN_MASK     0x30
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL_MOISTURE_OVRD_POLLING_MASK 0x08
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL_MOISTURE_EN_POLLING_MASK 0x04
#define WCD9378_MBHC_NEW_INT_MOISTURE_DET_POLLING_CTRL_MOISTURE_DBNC_TIME_MASK 0x03

/* WCD9378_MBHC_NEW_INT_MECH_DET_CURRENT Fields: */
#define WCD9378_MBHC_NEW_INT_MECH_DET_CURRENT_HSDET_PULLUP_CTL_MASK      0x1f

/* WCD9378_MBHC_NEW_INT_SPARE_2 Fields: */
#define WCD9378_MBHC_NEW_INT_SPARE_2_ZDET_TIMER_MASK                     0x80
#define WCD9378_MBHC_NEW_INT_SPARE_2_SPARE_BITS_6_0_MASK                 0x7f

/* WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON Fields: */
#define WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON_EAR_CHOPPER_EN_MASK          0x80
#define WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON_EAR_CHOPPER_CLK_DIV_MASK     0x78
#define WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON_EAR_CHOPPER_CLK_INV_MASK     0x04
#define WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON_EAR_CHOPPER_CLK_OVERLAP_MASK 0x02
#define WCD9378_EAR_INT_NEW_EAR_CHOPPER_CON_SCD_SHTDWN_FAST_PATH_DIS_MASK 0x01

/* WCD9378_EAR_INT_NEW_CNP_VCM_CON1 Fields: */
#define WCD9378_EAR_INT_NEW_CNP_VCM_CON1_SCD_EN_TIME_SEL_MASK            0x80
#define WCD9378_EAR_INT_NEW_CNP_VCM_CON1_NO_DYN_BIAS_DURING_STARTUP_MASK 0x40
#define WCD9378_EAR_INT_NEW_CNP_VCM_CON1_CNP_VCM_GEN_START_MASK          0x3f

/* WCD9378_EAR_INT_NEW_CNP_VCM_CON2 Fields: */
#define WCD9378_EAR_INT_NEW_CNP_VCM_CON2_DTEST_SEL_MASK                  0xc0
#define WCD9378_EAR_INT_NEW_CNP_VCM_CON2_CNP_VCM_GEN_STOP_MASK           0x3f

/* WCD9378_EAR_INT_NEW_EAR_DYNAMIC_BIAS Fields: */
#define WCD9378_EAR_INT_NEW_EAR_DYNAMIC_BIAS_EAR_DYN_BIAS_SEL_MASK       0xe0
#define WCD9378_EAR_INT_NEW_EAR_DYNAMIC_BIAS_EAR_BIAS_CURR_MASK          0x1f

/* WCD9378_AUX_INT_EN_REG Fields: */
#define WCD9378_AUX_INT_EN_REG_DAC_DATA_RESET_MASK                       0x80
#define WCD9378_AUX_INT_EN_REG_DAC_DATA_EN_MASK                          0x40
#define WCD9378_AUX_INT_EN_REG_DAC_REF_EN_MASK                           0x20
#define WCD9378_AUX_INT_EN_REG_AMP_EN_MASK                               0x10
#define WCD9378_AUX_INT_EN_REG_BIAS_EN_MASK                              0x08
#define WCD9378_AUX_INT_EN_REG_OUTPUT_SHORT_MASK                         0x04
#define WCD9378_AUX_INT_EN_REG_CNP_FSM_RESET_MASK                        0x02
#define WCD9378_AUX_INT_EN_REG_REG_OVERRIDE_EN_MASK                      0x01

/* WCD9378_AUX_INT_PA_CTRL Fields: */
#define WCD9378_AUX_INT_PA_CTRL_SPARE_BITS_7_6_MASK                      0xc0
#define WCD9378_AUX_INT_PA_CTRL_CMFB_LSF_CURR_MASK                       0x20
#define WCD9378_AUX_INT_PA_CTRL_BTI_CTL_MASK                             0x10
#define WCD9378_AUX_INT_PA_CTRL_GM3_IBIAS_CTL_MASK                       0x0f

/* WCD9378_AUX_INT_SP_CTRL Fields: */
#define WCD9378_AUX_INT_SP_CTRL_SP_INT_EN_MASK                           0x80
#define WCD9378_AUX_INT_SP_CTRL_SP_AUTO_SHUT_DOWN_MASK                   0x40
#define WCD9378_AUX_INT_SP_CTRL_SP_LIMIT_CURR_NMOS_MASK                  0x38
#define WCD9378_AUX_INT_SP_CTRL_SP_LIMIT_CURR_PMOS_MASK                  0x07

/* WCD9378_AUX_INT_DAC_CTRL Fields: */
#define WCD9378_AUX_INT_DAC_CTRL_DAC_SAMPLE_EDGE_SEL_MASK                0x80
#define WCD9378_AUX_INT_DAC_CTRL_REF_DBG_EN_MASK                         0x40
#define WCD9378_AUX_INT_DAC_CTRL_REF_DBG_GAIN_MASK                       0x38
#define WCD9378_AUX_INT_DAC_CTRL_GAIN_DAC_MASK                           0x06
#define WCD9378_AUX_INT_DAC_CTRL_INV_DATA_MASK                           0x01

/* WCD9378_AUX_INT_CLK_CTRL Fields: */
#define WCD9378_AUX_INT_CLK_CTRL_GNDSW_TIMER_MASK                        0xe0
#define WCD9378_AUX_INT_CLK_CTRL_SCD_DEGLITCH_SEL_MASK                   0x18
#define WCD9378_AUX_INT_CLK_CTRL_SPARE_BITS_2_0_MASK                     0x07

/* WCD9378_AUX_INT_TEST_CTRL Fields: */
#define WCD9378_AUX_INT_TEST_CTRL_SPARE_BITS_7_5_MASK                    0xe0
#define WCD9378_AUX_INT_TEST_CTRL_DAC_ATEST_EN_MASK                      0x10
#define WCD9378_AUX_INT_TEST_CTRL_PA_ATEST_EN_MASK                       0x08
#define WCD9378_AUX_INT_TEST_CTRL_PA_ATEST_SEL_MASK                      0x07

/* WCD9378_AUX_INT_STATUS_REG Fields: */
#define WCD9378_AUX_INT_STATUS_REG_SP_INT_MASK                           0x80
#define WCD9378_AUX_INT_STATUS_REG_SP_OUT_MASK                           0x40
#define WCD9378_AUX_INT_STATUS_REG_SP_NMOS_OUT_MASK                      0x20
#define WCD9378_AUX_INT_STATUS_REG_SP_PMOS_OUT_MASK                      0x10
#define WCD9378_AUX_INT_STATUS_REG_PA_READY_MASK                         0x08
#define WCD9378_AUX_INT_STATUS_REG_SPARE_BITS_2_0_MASK                   0x07

/* WCD9378_AUX_INT_MISC Fields: */
#define WCD9378_AUX_INT_MISC_SPARE_BITS_7_4_MASK                         0xf0
#define WCD9378_AUX_INT_MISC_PA_GAIN_MASK                                0x0f

/* WCD9378_SLEEP_INT_WATCHDOG_CTL_1 Fields: */
#define WCD9378_SLEEP_INT_WATCHDOG_CTL_1_VREF_HI_CTL_MASK                0x1f

/* WCD9378_SLEEP_INT_WATCHDOG_CTL_2 Fields: */
#define WCD9378_SLEEP_INT_WATCHDOG_CTL_2_VREF_LO_CTL_MASK                0x1f

/* WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1 Fields: */
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1_SEL_EDGE_DET_MASK         0xc0
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1_EN_RINGM_ATEST_MASK       0x20
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1_EN_RINGP_ATEST_MASK       0x10
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1_RING_CURR_SEL_MASK        0x0e
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT1_EN_VREF_ATEST_MASK        0x01

/* WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT2 Fields: */
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT2_REF_CURR_SEL_MASK         0xe0
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT2_COMP_STG1_IBIAS_MASK      0x18
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT2_COMP_STG2_IBIAS_MASK      0x06
#define WCD9378_DIE_CRACK_INT_DIE_CRK_DET_INT2_EN_ATEST_MASK             0x01

/* WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L2 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L2_DIV_L2_MASK               0xff

/* WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L1 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L1_DIV_L1_MASK               0xff

/* WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L0 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_DIVSTOP_L0_DIV_L0_MASK               0xff

/* WCD9378_TX_COM_NEW_INT_SPARE1 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE1_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_SPARE2 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE2_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L2 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L2_NINIT_L2_MASK               0xc0
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L2_SPARE_BITS_4_0_MASK         0x1f

/* WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L1 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L1_NINIT_L1_MASK               0xc0
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L1_SPARE_BITS_4_0_MASK         0x1f

/* WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L0 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L0_NINIT_L0_MASK               0xc0
#define WCD9378_TX_COM_NEW_INT_TXFE_NINIT_L0_SPARE_BITS_4_0_MASK         0x1f

/* WCD9378_TX_COM_NEW_INT_SPARE3 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE3_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_SPARE4 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE4_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_SPARE5 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE5_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_SPARE6 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE6_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_SPARE7 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE7_SPARE_BITS_7_0_MASK                0xff

/* WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L2L1 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L2L1_ICTRL_SCBIAS_L2_MASK    0xf0
#define WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L2L1_ICTRL_SCBIAS_L1_MASK    0x0f

/* WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L0 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L0_ICTRL_SCBIAS_L0_MASK      0xf0
#define WCD9378_TX_COM_NEW_INT_TXADC_SCBIAS_L0_SPARE_BITS_3_0_MASK       0x0f

/* WCD9378_TX_COM_NEW_INT_TXADC_INT_L2 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L2_INT1_L2_MASK                 0xf0
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L2_INT2_L2_MASK                 0x0f

/* WCD9378_TX_COM_NEW_INT_TXADC_INT_L1 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L1_INT1_L1_MASK                 0xf0
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L1_INT2_L1_MASK                 0x0f

/* WCD9378_TX_COM_NEW_INT_TXADC_INT_L0 Fields: */
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L0_INT1_L0_MASK                 0xf0
#define WCD9378_TX_COM_NEW_INT_TXADC_INT_L0_INT2_L0_MASK                 0x0f

/* WCD9378_TX_COM_NEW_INT_SPARE8 Fields: */
#define WCD9378_TX_COM_NEW_INT_SPARE8_SPARE_BITS_7_0_MASK                0xff


/* WCD9378_TAMBORA_PAGE Fields: */
#define WCD9378_TAMBORA_PAGE_PAG_REG_MASK                                0xff

/* WCD9378_CHIP_ID0 Fields: */
#define WCD9378_CHIP_ID0_BYTE_0_MASK                                     0xff

/* WCD9378_CHIP_ID1 Fields: */
#define WCD9378_CHIP_ID1_BYTE_1_MASK                                     0xff

/* WCD9378_CHIP_ID2 Fields: */
#define WCD9378_CHIP_ID2_BYTE_2_MASK                                     0xff

/* WCD9378_CHIP_ID3 Fields: */
#define WCD9378_CHIP_ID3_BYTE_3_MASK                                     0xff

/* WCD9378_SWR_TX_CLK_RATE Fields: */
#define WCD9378_SWR_TX_CLK_RATE_CLK_RATE_BK_1_MASK                       0xf0
#define WCD9378_SWR_TX_CLK_RATE_CLK_RATE_BK_0_MASK                       0x0f

/* WCD9378_CDC_RST_CTL Fields: */
#define WCD9378_CDC_RST_CTL_ANA_SW_RST_N_MASK                            0x02
#define WCD9378_CDC_RST_CTL_DIG_SW_RST_N_MASK                            0x01

/* WCD9378_TOP_CLK_CFG Fields: */
#define WCD9378_TOP_CLK_CFG_RX_CLK_CFG_MASK                              0x06
#define WCD9378_TOP_CLK_CFG_TX_CLK_CFG_MASK                              0x01

/* WCD9378_CDC_ANA_CLK_CTL Fields: */
#define WCD9378_CDC_ANA_CLK_CTL_ANA_RX_DIV4_CLK_EN_MASK                  0x04
#define WCD9378_CDC_ANA_CLK_CTL_ANA_RX_DIV2_CLK_EN_MASK                  0x02
#define WCD9378_CDC_ANA_CLK_CTL_ANA_RX_CLK_EN_MASK                       0x01

/* WCD9378_CDC_DIG_CLK_CTL Fields: */
#define WCD9378_CDC_DIG_CLK_CTL_TXD2_CLK_EN_MASK                         0x40
#define WCD9378_CDC_DIG_CLK_CTL_TXD1_CLK_EN_MASK                         0x20
#define WCD9378_CDC_DIG_CLK_CTL_TXD0_CLK_EN_MASK                         0x10
#define WCD9378_CDC_DIG_CLK_CTL_RXD2_CLK_EN_MASK                         0x04
#define WCD9378_CDC_DIG_CLK_CTL_RXD1_CLK_EN_MASK                         0x02
#define WCD9378_CDC_DIG_CLK_CTL_RXD0_CLK_EN_MASK                         0x01

/* WCD9378_SWR_RST_EN Fields: */
#define WCD9378_SWR_RST_EN_RX_RESET_SYNC_LOST_EN_MASK                    0x20
#define WCD9378_SWR_RST_EN_RX_RESET_SWR_BUS_EN_MASK                      0x10
#define WCD9378_SWR_RST_EN_RX_RESET_SWR_REG_EN_MASK                      0x08
#define WCD9378_SWR_RST_EN_TX_RESET_SYNC_LOST_EN_MASK                    0x04
#define WCD9378_SWR_RST_EN_TX_RESET_SWR_BUS_EN_MASK                      0x02
#define WCD9378_SWR_RST_EN_TX_RESET_SWR_REG_EN_MASK                      0x01

/* WCD9378_CDC_PATH_MODE Fields: */
#define WCD9378_CDC_PATH_MODE_RX2_CLK_RATE_MASK                          0x40

/* WCD9378_CDC_RX_RST Fields: */
#define WCD9378_CDC_RX_RST_RX2_SOFT_RST_MASK                             0x04
#define WCD9378_CDC_RX_RST_RX1_SOFT_RST_MASK                             0x02
#define WCD9378_CDC_RX_RST_RX0_SOFT_RST_MASK                             0x01

/* WCD9378_CDC_RX0_CTL Fields: */
#define WCD9378_CDC_RX0_CTL_DSM_DITHER_ENABLE_MASK                       0x80
#define WCD9378_CDC_RX0_CTL_DEM_DITHER_ENABLE_MASK                       0x40
#define WCD9378_CDC_RX0_CTL_DEM_MID_ENABLE_MASK                          0x20
#define WCD9378_CDC_RX0_CTL_DEM_MOD_SWITCHING_BLOCK_ENABLE_MASK          0x10
#define WCD9378_CDC_RX0_CTL_DEM_SWITCHING_BLOCK_ENABLE_MASK              0x08
#define WCD9378_CDC_RX0_CTL_DEM_SEGMENTING_BLOCK_ENABLE_MASK             0x04
#define WCD9378_CDC_RX0_CTL_DEM_BYPASS_MASK                              0x02

/* WCD9378_CDC_RX1_CTL Fields: */
#define WCD9378_CDC_RX1_CTL_DSM_DITHER_ENABLE_MASK                       0x80
#define WCD9378_CDC_RX1_CTL_DEM_DITHER_ENABLE_MASK                       0x40
#define WCD9378_CDC_RX1_CTL_DEM_MID_ENABLE_MASK                          0x20
#define WCD9378_CDC_RX1_CTL_DEM_MOD_SWITCHING_BLOCK_ENABLE_MASK          0x10
#define WCD9378_CDC_RX1_CTL_DEM_SWITCHING_BLOCK_ENABLE_MASK              0x08
#define WCD9378_CDC_RX1_CTL_DEM_SEGMENTING_BLOCK_ENABLE_MASK             0x04
#define WCD9378_CDC_RX1_CTL_DEM_BYPASS_MASK                              0x02

/* WCD9378_CDC_RX2_CTL Fields: */
#define WCD9378_CDC_RX2_CTL_DSM_DITHER_ENABLE_MASK                       0x80
#define WCD9378_CDC_RX2_CTL_DEM_DITHER_ENABLE_MASK                       0x40
#define WCD9378_CDC_RX2_CTL_DEM_MID_ENABLE_MASK                          0x20
#define WCD9378_CDC_RX2_CTL_DEM_MOD_SWITCHING_BLOCK_ENABLE_MASK          0x10
#define WCD9378_CDC_RX2_CTL_DEM_SWITCHING_BLOCK_ENABLE_MASK              0x08
#define WCD9378_CDC_RX2_CTL_DEM_SEGMENTING_BLOCK_ENABLE_MASK             0x04
#define WCD9378_CDC_RX2_CTL_DEM_BYPASS_MASK                              0x02

/* WCD9378_CDC_TX_ANA_MODE_0_1 Fields: */
#define WCD9378_CDC_TX_ANA_MODE_0_1_TXD1_MODE_MASK                       0xf0
#define WCD9378_CDC_TX_ANA_MODE_0_1_TXD0_MODE_MASK                       0x0f

/* WCD9378_CDC_TX_ANA_MODE_2_3 Fields: */
#define WCD9378_CDC_TX_ANA_MODE_2_3_TXD2_MODE_MASK                       0x0f

/* WCD9378_CDC_COMP_CTL_0 Fields: */
#define WCD9378_CDC_COMP_CTL_0_EAR_COMP_EN_MASK                          0x04
#define WCD9378_CDC_COMP_CTL_0_HPHL_COMP_EN_MASK                         0x02
#define WCD9378_CDC_COMP_CTL_0_HPHR_COMP_EN_MASK                         0x01

/* WCD9378_CDC_ANA_TX_CLK_CTL Fields: */
#define WCD9378_CDC_ANA_TX_CLK_CTL_ANA_TX2_ADC_CLK_EN_MASK               0x08
#define WCD9378_CDC_ANA_TX_CLK_CTL_ANA_TX1_ADC_CLK_EN_MASK               0x04
#define WCD9378_CDC_ANA_TX_CLK_CTL_ANA_TX0_ADC_CLK_EN_MASK               0x02
#define WCD9378_CDC_ANA_TX_CLK_CTL_ANA_TXSCBIAS_CLK_EN_MASK              0x01

/* WCD9378_CDC_HPH_DSM_A1_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A1_0_COEF_A1_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A1_1 Fields: */
#define WCD9378_CDC_HPH_DSM_A1_1_COEF_A1_MASK                            0x01

/* WCD9378_CDC_HPH_DSM_A2_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A2_0_COEF_A2_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A2_1 Fields: */
#define WCD9378_CDC_HPH_DSM_A2_1_COEF_A2_MASK                            0x0f

/* WCD9378_CDC_HPH_DSM_A3_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A3_0_COEF_A3_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A3_1 Fields: */
#define WCD9378_CDC_HPH_DSM_A3_1_COEF_A3_MASK                            0x07

/* WCD9378_CDC_HPH_DSM_A4_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A4_0_COEF_A4_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A4_1 Fields: */
#define WCD9378_CDC_HPH_DSM_A4_1_COEF_A4_MASK                            0x03

/* WCD9378_CDC_HPH_DSM_A5_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A5_0_COEF_A5_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A5_1 Fields: */
#define WCD9378_CDC_HPH_DSM_A5_1_COEF_A5_MASK                            0x03

/* WCD9378_CDC_HPH_DSM_A6_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A6_0_COEF_A6_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_A7_0 Fields: */
#define WCD9378_CDC_HPH_DSM_A7_0_COEF_A7_MASK                            0xff

/* WCD9378_CDC_HPH_DSM_C_0 Fields: */
#define WCD9378_CDC_HPH_DSM_C_0_COEF_C3_MASK                             0xf0
#define WCD9378_CDC_HPH_DSM_C_0_COEF_C2_MASK                             0x0f

/* WCD9378_CDC_HPH_DSM_C_1 Fields: */
#define WCD9378_CDC_HPH_DSM_C_1_COEF_C5_MASK                             0xf0
#define WCD9378_CDC_HPH_DSM_C_1_COEF_C4_MASK                             0x0f

/* WCD9378_CDC_HPH_DSM_C_2 Fields: */
#define WCD9378_CDC_HPH_DSM_C_2_COEF_C7_MASK                             0xf0
#define WCD9378_CDC_HPH_DSM_C_2_COEF_C6_MASK                             0x0f

/* WCD9378_CDC_HPH_DSM_C_3 Fields: */
#define WCD9378_CDC_HPH_DSM_C_3_COEF_C7_MASK                             0x3f

/* WCD9378_CDC_HPH_DSM_R1 Fields: */
#define WCD9378_CDC_HPH_DSM_R1_SAT_LIMIT_R1_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R2 Fields: */
#define WCD9378_CDC_HPH_DSM_R2_SAT_LIMIT_R2_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R3 Fields: */
#define WCD9378_CDC_HPH_DSM_R3_SAT_LIMIT_R3_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R4 Fields: */
#define WCD9378_CDC_HPH_DSM_R4_SAT_LIMIT_R4_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R5 Fields: */
#define WCD9378_CDC_HPH_DSM_R5_SAT_LIMIT_R5_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R6 Fields: */
#define WCD9378_CDC_HPH_DSM_R6_SAT_LIMIT_R6_MASK                         0xff

/* WCD9378_CDC_HPH_DSM_R7 Fields: */
#define WCD9378_CDC_HPH_DSM_R7_SAT_LIMIT_R7_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_A1_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A1_0_COEF_A1_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A1_1 Fields: */
#define WCD9378_CDC_AUX_DSM_A1_1_COEF_A1_MASK                            0x01

/* WCD9378_CDC_AUX_DSM_A2_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A2_0_COEF_A2_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A2_1 Fields: */
#define WCD9378_CDC_AUX_DSM_A2_1_COEF_A2_MASK                            0x0f

/* WCD9378_CDC_AUX_DSM_A3_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A3_0_COEF_A3_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A3_1 Fields: */
#define WCD9378_CDC_AUX_DSM_A3_1_COEF_A3_MASK                            0x07

/* WCD9378_CDC_AUX_DSM_A4_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A4_0_COEF_A4_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A4_1 Fields: */
#define WCD9378_CDC_AUX_DSM_A4_1_COEF_A4_MASK                            0x03

/* WCD9378_CDC_AUX_DSM_A5_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A5_0_COEF_A5_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A5_1 Fields: */
#define WCD9378_CDC_AUX_DSM_A5_1_COEF_A5_MASK                            0x03

/* WCD9378_CDC_AUX_DSM_A6_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A6_0_COEF_A6_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_A7_0 Fields: */
#define WCD9378_CDC_AUX_DSM_A7_0_COEF_A7_MASK                            0xff

/* WCD9378_CDC_AUX_DSM_C_0 Fields: */
#define WCD9378_CDC_AUX_DSM_C_0_COEF_C3_MASK                             0xf0
#define WCD9378_CDC_AUX_DSM_C_0_COEF_C2_MASK                             0x0f

/* WCD9378_CDC_AUX_DSM_C_1 Fields: */
#define WCD9378_CDC_AUX_DSM_C_1_COEF_C5_MASK                             0xf0
#define WCD9378_CDC_AUX_DSM_C_1_COEF_C4_MASK                             0x0f

/* WCD9378_CDC_AUX_DSM_C_2 Fields: */
#define WCD9378_CDC_AUX_DSM_C_2_COEF_C7_MASK                             0xf0
#define WCD9378_CDC_AUX_DSM_C_2_COEF_C6_MASK                             0x0f

/* WCD9378_CDC_AUX_DSM_C_3 Fields: */
#define WCD9378_CDC_AUX_DSM_C_3_COEF_C7_MASK                             0x3f

/* WCD9378_CDC_AUX_DSM_R1 Fields: */
#define WCD9378_CDC_AUX_DSM_R1_SAT_LIMIT_R1_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R2 Fields: */
#define WCD9378_CDC_AUX_DSM_R2_SAT_LIMIT_R2_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R3 Fields: */
#define WCD9378_CDC_AUX_DSM_R3_SAT_LIMIT_R3_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R4 Fields: */
#define WCD9378_CDC_AUX_DSM_R4_SAT_LIMIT_R4_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R5 Fields: */
#define WCD9378_CDC_AUX_DSM_R5_SAT_LIMIT_R5_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R6 Fields: */
#define WCD9378_CDC_AUX_DSM_R6_SAT_LIMIT_R6_MASK                         0xff

/* WCD9378_CDC_AUX_DSM_R7 Fields: */
#define WCD9378_CDC_AUX_DSM_R7_SAT_LIMIT_R7_MASK                         0xff

/* WCD9378_CDC_HPH_GAIN_RX_0 Fields: */
#define WCD9378_CDC_HPH_GAIN_RX_0_GAIN_RX_MASK                           0xff

/* WCD9378_CDC_HPH_GAIN_RX_1 Fields: */
#define WCD9378_CDC_HPH_GAIN_RX_1_GAIN_RX_MASK                           0xff

/* WCD9378_CDC_HPH_GAIN_DSD_0 Fields: */
#define WCD9378_CDC_HPH_GAIN_DSD_0_GAIN_DSD_MASK                         0xff

/* WCD9378_CDC_HPH_GAIN_DSD_1 Fields: */
#define WCD9378_CDC_HPH_GAIN_DSD_1_GAIN_DSD_MASK                         0xff

/* WCD9378_CDC_HPH_GAIN_DSD_2 Fields: */
#define WCD9378_CDC_HPH_GAIN_DSD_2_GAIN_LATCH_MASK                       0x02
#define WCD9378_CDC_HPH_GAIN_DSD_2_GAIN_DSD_MASK                         0x01

/* WCD9378_CDC_AUX_GAIN_DSD_0 Fields: */
#define WCD9378_CDC_AUX_GAIN_DSD_0_GAIN_DSD_MASK                         0xff

/* WCD9378_CDC_AUX_GAIN_DSD_1 Fields: */
#define WCD9378_CDC_AUX_GAIN_DSD_1_GAIN_DSD_MASK                         0xff

/* WCD9378_CDC_AUX_GAIN_DSD_2 Fields: */
#define WCD9378_CDC_AUX_GAIN_DSD_2_GAIN_LATCH_MASK                       0x02
#define WCD9378_CDC_AUX_GAIN_DSD_2_GAIN_DSD_MASK                         0x01

/* WCD9378_CDC_HPH_GAIN_CTL Fields: */
#define WCD9378_CDC_HPH_GAIN_CTL_HPH_STEREO_EN_MASK                      0x10
#define WCD9378_CDC_HPH_GAIN_CTL_HPHR_RX_EN_MASK                         0x08
#define WCD9378_CDC_HPH_GAIN_CTL_HPHL_RX_EN_MASK                         0x04
#define WCD9378_CDC_HPH_GAIN_CTL_HPHR_DSD_EN_MASK                        0x02
#define WCD9378_CDC_HPH_GAIN_CTL_HPHL_DSD_EN_MASK                        0x01

/* WCD9378_CDC_AUX_GAIN_CTL Fields: */
#define WCD9378_CDC_AUX_GAIN_CTL_AUX_EN_MASK                             0x01

/* WCD9378_CDC_PATH_CTL Fields: */
#define WCD9378_CDC_PATH_CTL_AUX_MUX_SEL_MASK                            0x10
#define WCD9378_CDC_PATH_CTL_EAR_1BIT_MODE_MASK                          0x02
#define WCD9378_CDC_PATH_CTL_EAR_MUX_SEL_MASK                            0x01

/* WCD9378_CDC_SWR_CLG Fields: */
#define WCD9378_CDC_SWR_CLG_CLG_CTL_MASK                                 0xff

/* WCD9378_SWR_CLG_BYP Fields: */
#define WCD9378_SWR_CLG_BYP_SWR_CLG_BYP_MASK                             0x01

/* WCD9378_CDC_TX0_CTL Fields: */
#define WCD9378_CDC_TX0_CTL_REQ_FB_SEL_MASK                              0x40
#define WCD9378_CDC_TX0_CTL_TX_DITHER_EN_MASK                            0x20
#define WCD9378_CDC_TX0_CTL_RANDOM_REGION_MASK                           0x1f

/* WCD9378_CDC_TX1_CTL Fields: */
#define WCD9378_CDC_TX1_CTL_REQ_FB_SEL_MASK                              0x40
#define WCD9378_CDC_TX1_CTL_TX_DITHER_EN_MASK                            0x20
#define WCD9378_CDC_TX1_CTL_RANDOM_REGION_MASK                           0x1f

/* WCD9378_CDC_TX2_CTL Fields: */
#define WCD9378_CDC_TX2_CTL_REQ_FB_SEL_MASK                              0x40
#define WCD9378_CDC_TX2_CTL_TX_DITHER_EN_MASK                            0x20
#define WCD9378_CDC_TX2_CTL_RANDOM_REGION_MASK                           0x1f

/* WCD9378_CDC_TX_RST Fields: */
#define WCD9378_CDC_TX_RST_TX2_SOFT_RST_MASK                             0x04
#define WCD9378_CDC_TX_RST_TX1_SOFT_RST_MASK                             0x02
#define WCD9378_CDC_TX_RST_TX0_SOFT_RST_MASK                             0x01

/* WCD9378_CDC_REQ_CTL Fields: */
#define WCD9378_CDC_REQ_CTL_TX2_WIDE_BAND_MASK                           0x10
#define WCD9378_CDC_REQ_CTL_TX1_WIDE_BAND_MASK                           0x08
#define WCD9378_CDC_REQ_CTL_TX0_WIDE_BAND_MASK                           0x04
#define WCD9378_CDC_REQ_CTL_FS_RATE_4P8_MASK                             0x02
#define WCD9378_CDC_REQ_CTL_DEM_BYPASS_MASK                              0x01

/* WCD9378_CDC_RST Fields: */
#define WCD9378_CDC_RST_TX_SOFT_RST_MASK                                 0x02
#define WCD9378_CDC_RST_RX_SOFT_RST_MASK                                 0x01

/* WCD9378_CDC_AMIC_CTL Fields: */
#define WCD9378_CDC_AMIC_CTL_AMIC4_IN_SEL_MASK                           0x04
#define WCD9378_CDC_AMIC_CTL_AMIC3_IN_SEL_MASK                           0x02
#define WCD9378_CDC_AMIC_CTL_AMIC1_IN_SEL_MASK                           0x01

/* WCD9378_CDC_DMIC_CTL Fields: */
#define WCD9378_CDC_DMIC_CTL_DMIC_LEGACY_SW_MODE_MASK                    0x08
#define WCD9378_CDC_DMIC_CTL_DMIC_DIV_BAK_EN_MASK                        0x04
#define WCD9378_CDC_DMIC_CTL_CLK_SCALE_EN_MASK                           0x02
#define WCD9378_CDC_DMIC_CTL_SOFT_RESET_MASK                             0x01

/* WCD9378_CDC_DMIC1_CTL Fields: */
#define WCD9378_CDC_DMIC1_CTL_DMIC_CLK_SCALE_SEL_MASK                    0x70
#define WCD9378_CDC_DMIC1_CTL_DMIC_CLK_EN_MASK                           0x08
#define WCD9378_CDC_DMIC1_CTL_DMIC_CLK_SEL_MASK                          0x07

/* WCD9378_CDC_DMIC2_CTL Fields: */
#define WCD9378_CDC_DMIC2_CTL_DMIC_LEFT_EN_MASK                          0x80
#define WCD9378_CDC_DMIC2_CTL_DMIC_CLK_SCALE_SEL_MASK                    0x70
#define WCD9378_CDC_DMIC2_CTL_DMIC_CLK_EN_MASK                           0x08
#define WCD9378_CDC_DMIC2_CTL_DMIC_CLK_SEL_MASK                          0x07

/* WCD9378_CDC_DMIC3_CTL Fields: */
#define WCD9378_CDC_DMIC3_CTL_DMIC_CLK_SCALE_SEL_MASK                    0x70
#define WCD9378_CDC_DMIC3_CTL_DMIC_CLK_EN_MASK                           0x08
#define WCD9378_CDC_DMIC3_CTL_DMIC_CLK_SEL_MASK                          0x07

/* WCD9378_EFUSE_PRG_CTL Fields: */
#define WCD9378_EFUSE_PRG_CTL_PRG_ADDR_MASK                              0xff

/* WCD9378_EFUSE_CTL Fields: */
#define WCD9378_EFUSE_CTL_EFUSE_ST_CNT_MASK                              0x3c
#define WCD9378_EFUSE_CTL_EFUSE_SOFT_RST_N_MASK                          0x02
#define WCD9378_EFUSE_CTL_EFUSE_EN_MASK                                  0x01

/* WCD9378_CDC_DMIC_RATE_1_2 Fields: */
#define WCD9378_CDC_DMIC_RATE_1_2_DMIC2_RATE_MASK                        0xf0
#define WCD9378_CDC_DMIC_RATE_1_2_DMIC1_RATE_MASK                        0x0f

/* WCD9378_CDC_DMIC_RATE_3_4 Fields: */
#define WCD9378_CDC_DMIC_RATE_3_4_DMIC3_RATE_MASK                        0x0f

/* WCD9378_PDM_WD_EN_OVRD Fields: */
#define WCD9378_PDM_WD_EN_OVRD_RX2_MASK                                  0x10
#define WCD9378_PDM_WD_EN_OVRD_RX1_MASK                                  0x0c
#define WCD9378_PDM_WD_EN_OVRD_RX0_MASK                                  0x03

/* WCD9378_PDM_WD_CTL0 Fields: */
#define WCD9378_PDM_WD_CTL0_HOLD_OFF_MASK                                0x80
#define WCD9378_PDM_WD_CTL0_TIME_OUT_SEL_DSD_MASK                        0x60
#define WCD9378_PDM_WD_CTL0_TIME_OUT_SEL_PCM_MASK                        0x18
#define WCD9378_PDM_WD_CTL0_PDM_WD_EN_MASK                               0x07

/* WCD9378_PDM_WD_CTL1 Fields: */
#define WCD9378_PDM_WD_CTL1_HOLD_OFF_MASK                                0x80
#define WCD9378_PDM_WD_CTL1_TIME_OUT_SEL_DSD_MASK                        0x60
#define WCD9378_PDM_WD_CTL1_TIME_OUT_SEL_PCM_MASK                        0x18
#define WCD9378_PDM_WD_CTL1_PDM_WD_EN_MASK                               0x07

/* WCD9378_PDM_WD_CTL2 Fields: */
#define WCD9378_PDM_WD_CTL2_HOLD_OFF_MASK                                0x08
#define WCD9378_PDM_WD_CTL2_TIME_OUT_SEL_MASK                            0x06
#define WCD9378_PDM_WD_CTL2_PDM_WD_EN_MASK                               0x01

/* WCD9378_RAMP_CTL Fields: */
#define WCD9378_RAMP_CTL_RX2_RAMP_EN_MASK                                0x04
#define WCD9378_RAMP_CTL_RX1_RAMP_EN_MASK                                0x02
#define WCD9378_RAMP_CTL_RX0_RAMP_EN_MASK                                0x01

/* WCD9378_ACT_DET_CTL Fields: */
#define WCD9378_ACT_DET_CTL_RX2_ACT_DET_EN_MASK                          0x04
#define WCD9378_ACT_DET_CTL_RX1_ACT_DET_EN_MASK                          0x02
#define WCD9378_ACT_DET_CTL_RX0_ACT_DET_EN_MASK                          0x01

/* WCD9378_ACT_DET_HOOKUP0 Fields: */
#define WCD9378_ACT_DET_HOOKUP0_RX2_INPUT_MUTE_MASK                      0x04
#define WCD9378_ACT_DET_HOOKUP0_RX1_INPUT_MUTE_MASK                      0x02
#define WCD9378_ACT_DET_HOOKUP0_RX0_INPUT_MUTE_MASK                      0x01

/* WCD9378_ACT_DET_HOOKUP1 Fields: */
#define WCD9378_ACT_DET_HOOKUP1_RX2_OUTPUT_MUTE_MASK                     0x04
#define WCD9378_ACT_DET_HOOKUP1_RX1_OUTPUT_MUTE_MASK                     0x02
#define WCD9378_ACT_DET_HOOKUP1_RX0_OUTPUT_MUTE_MASK                     0x01

/* WCD9378_ACT_DET_HOOKUP2 Fields: */
#define WCD9378_ACT_DET_HOOKUP2_RX2_DSD_GAIN_CSR_EN_MASK                 0x10
#define WCD9378_ACT_DET_HOOKUP2_RX1_DSD_GAIN_CSR_EN_MASK                 0x08
#define WCD9378_ACT_DET_HOOKUP2_RX1_PCM_GAIN_CSR_EN_MASK                 0x04
#define WCD9378_ACT_DET_HOOKUP2_RX0_DSD_GAIN_CSR_EN_MASK                 0x02
#define WCD9378_ACT_DET_HOOKUP2_RX0_PCM_GAIN_CSR_EN_MASK                 0x01

/* WCD9378_ACT_DET_DLY_BUF_EN Fields: */
#define WCD9378_ACT_DET_DLY_BUF_EN_RX2_DSD_DLY_BUF_EN_MASK               0x10
#define WCD9378_ACT_DET_DLY_BUF_EN_RX1_DSD_DLY_BUF_EN_MASK               0x08
#define WCD9378_ACT_DET_DLY_BUF_EN_RX1_PCM_DLY_BUF_EN_MASK               0x04
#define WCD9378_ACT_DET_DLY_BUF_EN_RX0_DSD_DLY_BUF_EN_MASK               0x02
#define WCD9378_ACT_DET_DLY_BUF_EN_RX0_PCM_DLY_BUF_EN_MASK               0x01

/* WCD9378_INTR_MODE Fields: */
#define WCD9378_INTR_MODE_SWR_PULSE_CLR_MASK                             0x20
#define WCD9378_INTR_MODE_SWR_RX_INT_OUT_EN_MASK                         0x10
#define WCD9378_INTR_MODE_GPIO_1_INT_OUT_EN_MASK                         0x08
#define WCD9378_INTR_MODE_GPIO_0_INT_OUT_EN_MASK                         0x04
#define WCD9378_INTR_MODE_SWR_INTR_LEVEL_MASK                            0x02
#define WCD9378_INTR_MODE_INT_POLARITY_MASK                              0x01

/* WCD9378_INTR_STATUS_0 Fields: */
#define WCD9378_INTR_STATUS_0_HPHL_OCP_INT_MASK                          0x80
#define WCD9378_INTR_STATUS_0_HPHR_CNP_INT_MASK                          0x40
#define WCD9378_INTR_STATUS_0_HPHR_OCP_INT_MASK                          0x20
#define WCD9378_INTR_STATUS_0_MBHC_SW_INT_MASK                           0x10
#define WCD9378_INTR_STATUS_0_MBHC_ELECT_INS_REM_LEG_INT_MASK            0x08
#define WCD9378_INTR_STATUS_0_MBHC_ELECT_INS_REM_INT_MASK                0x04
#define WCD9378_INTR_STATUS_0_MBHC_BTN_RELEASE_INT_MASK                  0x02
#define WCD9378_INTR_STATUS_0_MBHC_BTN_PRESS_INT_MASK                    0x01

/* WCD9378_INTR_STATUS_1 Fields: */
#define WCD9378_INTR_STATUS_1_AUX_PDM_WD_INT_MASK                        0x80
#define WCD9378_INTR_STATUS_1_HPHR_PDM_WD_INT_MASK                       0x40
#define WCD9378_INTR_STATUS_1_HPHL_PDM_WD_INT_MASK                       0x20
#define WCD9378_INTR_STATUS_1_AUX_SCD_INT_MASK                           0x10
#define WCD9378_INTR_STATUS_1_AUX_CNP_INT_MASK                           0x08
#define WCD9378_INTR_STATUS_1_EAR_SCD_INT_MASK                           0x04
#define WCD9378_INTR_STATUS_1_EAR_CNP_INT_MASK                           0x02
#define WCD9378_INTR_STATUS_1_HPHL_CNP_INT_MASK                          0x01

/* WCD9378_INTR_STATUS_2 Fields: */
#define WCD9378_INTR_STATUS_2_HIDTX_CUR_OWNER_CHG_MASK                   0x80
#define WCD9378_INTR_STATUS_2_SAPU_PROT_MODE_CHG_MASK                    0x40
#define WCD9378_INTR_STATUS_2_GPIO_1_INT_MASK                            0x20
#define WCD9378_INTR_STATUS_2_GPIO_0_INT_MASK                            0x10
#define WCD9378_INTR_STATUS_2_HPHL_SURGE_DET_INT_MASK                    0x08
#define WCD9378_INTR_STATUS_2_HPHR_SURGE_DET_INT_MASK                    0x04
#define WCD9378_INTR_STATUS_2_MBHC_MOISTRUE_INT_MASK                     0x02
#define WCD9378_INTR_STATUS_2_LDORT_SCD_INT_MASK                         0x01

/* WCD9378_INTR_STATUS_3 Fields: */
#define WCD9378_INTR_STATUS_3_SM2_STAT_ALERT_MASK                        0x20
#define WCD9378_INTR_STATUS_3_SM1_STAT_ALERT_MASK                        0x10
#define WCD9378_INTR_STATUS_3_SM0_STAT_ALERT_MASK                        0x08
#define WCD9378_INTR_STATUS_3_SJ_STAT_ALERT_MASK                         0x04
#define WCD9378_INTR_STATUS_3_SA_STAT_ALERT_MASK                         0x02

/* WCD9378_INTR_MASK_0 Fields: */
#define WCD9378_INTR_MASK_0_HPHL_OCP_INT_MASK                            0x80
#define WCD9378_INTR_MASK_0_HPHR_CNP_INT_MASK                            0x40
#define WCD9378_INTR_MASK_0_HPHR_OCP_INT_MASK                            0x20
#define WCD9378_INTR_MASK_0_MBHC_SW_INT_MASK                             0x10
#define WCD9378_INTR_MASK_0_MBHC_ELECT_INS_REM_LEG_INT_MASK              0x08
#define WCD9378_INTR_MASK_0_MBHC_ELECT_INS_REM_INT_MASK                  0x04
#define WCD9378_INTR_MASK_0_MBHC_BTN_RELEASE_INT_MASK                    0x02
#define WCD9378_INTR_MASK_0_MBHC_BTN_PRESS_INT_MASK                      0x01

/* WCD9378_INTR_MASK_1 Fields: */
#define WCD9378_INTR_MASK_1_AUX_PDM_WD_INT_MASK                          0x80
#define WCD9378_INTR_MASK_1_HPHR_PDM_WD_INT_MASK                         0x40
#define WCD9378_INTR_MASK_1_HPHL_PDM_WD_INT_MASK                         0x20
#define WCD9378_INTR_MASK_1_AUX_SCD_INT_MASK                             0x10
#define WCD9378_INTR_MASK_1_AUX_CNP_INT_MASK                             0x08
#define WCD9378_INTR_MASK_1_EAR_SCD_INT_MASK                             0x04
#define WCD9378_INTR_MASK_1_EAR_CNP_INT_MASK                             0x02
#define WCD9378_INTR_MASK_1_HPHL_CNP_INT_MASK                            0x01

/* WCD9378_INTR_MASK_2 Fields: */
#define WCD9378_INTR_MASK_2_HIDTX_CUR_OWNER_CHG_MASK                     0x80
#define WCD9378_INTR_MASK_2_SAPU_PROT_MODE_CHG_MASK                      0x40
#define WCD9378_INTR_MASK_2_GPIO_1_INT_MASK                              0x20
#define WCD9378_INTR_MASK_2_GPIO_0_INT_MASK                              0x10
#define WCD9378_INTR_MASK_2_HPHL_SURGE_DET_INT_MASK                      0x08
#define WCD9378_INTR_MASK_2_HPHR_SURGE_DET_INT_MASK                      0x04
#define WCD9378_INTR_MASK_2_MBHC_MOISTRUE_INT_MASK                       0x02
#define WCD9378_INTR_MASK_2_LDORT_SCD_INT_MASK                           0x01

/* WCD9378_INTR_MASK_3 Fields: */
#define WCD9378_INTR_MASK_3_SM2_STAT_ALERT_MASK                          0x20
#define WCD9378_INTR_MASK_3_SM1_STAT_ALERT_MASK                          0x10
#define WCD9378_INTR_MASK_3_SM0_STAT_ALERT_MASK                          0x08
#define WCD9378_INTR_MASK_3_SJ_STAT_ALERT_MASK                           0x04
#define WCD9378_INTR_MASK_3_SA_STAT_ALERT_MASK                           0x02

/* WCD9378_INTR_SET_0 Fields: */
#define WCD9378_INTR_SET_0_HPHL_OCP_INT_MASK                             0x80
#define WCD9378_INTR_SET_0_HPHR_CNP_INT_MASK                             0x40
#define WCD9378_INTR_SET_0_HPHR_OCP_INT_MASK                             0x20
#define WCD9378_INTR_SET_0_MBHC_SW_INT_MASK                              0x10
#define WCD9378_INTR_SET_0_MBHC_ELECT_INS_REM_LEG_INT_MASK               0x08
#define WCD9378_INTR_SET_0_MBHC_ELECT_INS_REM_INT_MASK                   0x04
#define WCD9378_INTR_SET_0_MBHC_BTN_RELEASE_INT_MASK                     0x02
#define WCD9378_INTR_SET_0_MBHC_BTN_PRESS_INT_MASK                       0x01

/* WCD9378_INTR_SET_1 Fields: */
#define WCD9378_INTR_SET_1_AUX_PDM_WD_INT_MASK                           0x80
#define WCD9378_INTR_SET_1_HPHR_PDM_WD_INT_MASK                          0x40
#define WCD9378_INTR_SET_1_HPHL_PDM_WD_INT_MASK                          0x20
#define WCD9378_INTR_SET_1_AUX_SCD_INT_MASK                              0x10
#define WCD9378_INTR_SET_1_AUX_CNP_INT_MASK                              0x08
#define WCD9378_INTR_SET_1_EAR_SCD_INT_MASK                              0x04
#define WCD9378_INTR_SET_1_EAR_CNP_INT_MASK                              0x02
#define WCD9378_INTR_SET_1_HPHL_CNP_INT_MASK                             0x01

/* WCD9378_INTR_SET_2 Fields: */
#define WCD9378_INTR_SET_2_HIDTX_CUR_OWNER_CHG_MASK                      0x80
#define WCD9378_INTR_SET_2_SAPU_PROT_MODE_CHG_MASK                       0x40
#define WCD9378_INTR_SET_2_GPIO_1_INT_MASK                               0x20
#define WCD9378_INTR_SET_2_GPIO_0_INT_MASK                               0x10
#define WCD9378_INTR_SET_2_HPHL_SURGE_DET_INT_MASK                       0x08
#define WCD9378_INTR_SET_2_HPHR_SURGE_DET_INT_MASK                       0x04
#define WCD9378_INTR_SET_2_MBHC_MOISTRUE_INT_MASK                        0x02
#define WCD9378_INTR_SET_2_LDORT_SCD_INT_MASK                            0x01

/* WCD9378_INTR_SET_3 Fields: */
#define WCD9378_INTR_SET_3_SM2_STAT_ALERT_MASK                           0x20
#define WCD9378_INTR_SET_3_SM1_STAT_ALERT_MASK                           0x10
#define WCD9378_INTR_SET_3_SM0_STAT_ALERT_MASK                           0x08
#define WCD9378_INTR_SET_3_SJ_STAT_ALERT_MASK                            0x04
#define WCD9378_INTR_SET_3_SA_STAT_ALERT_MASK                            0x02

/* WCD9378_INTR_TEST_0 Fields: */
#define WCD9378_INTR_TEST_0_HPHL_OCP_INT_MASK                            0x80
#define WCD9378_INTR_TEST_0_HPHR_CNP_INT_MASK                            0x40
#define WCD9378_INTR_TEST_0_HPHR_OCP_INT_MASK                            0x20
#define WCD9378_INTR_TEST_0_MBHC_SW_INT_MASK                             0x10
#define WCD9378_INTR_TEST_0_MBHC_ELECT_INS_REM_LEG_INT_MASK              0x08
#define WCD9378_INTR_TEST_0_MBHC_ELECT_INS_REM_INT_MASK                  0x04
#define WCD9378_INTR_TEST_0_MBHC_BTN_RELEASE_INT_MASK                    0x02
#define WCD9378_INTR_TEST_0_MBHC_BTN_PRESS_INT_MASK                      0x01

/* WCD9378_INTR_TEST_1 Fields: */
#define WCD9378_INTR_TEST_1_AUX_PDM_WD_INT_MASK                          0x80
#define WCD9378_INTR_TEST_1_HPHR_PDM_WD_INT_MASK                         0x40
#define WCD9378_INTR_TEST_1_HPHL_PDM_WD_INT_MASK                         0x20
#define WCD9378_INTR_TEST_1_AUX_SCD_INT_MASK                             0x10
#define WCD9378_INTR_TEST_1_AUX_CNP_INT_MASK                             0x08
#define WCD9378_INTR_TEST_1_EAR_SCD_INT_MASK                             0x04
#define WCD9378_INTR_TEST_1_EAR_CNP_INT_MASK                             0x02
#define WCD9378_INTR_TEST_1_HPHL_CNP_INT_MASK                            0x01

/* WCD9378_INTR_TEST_2 Fields: */
#define WCD9378_INTR_TEST_2_HIDTX_CUR_OWNER_CHG_MASK                     0x80
#define WCD9378_INTR_TEST_2_SAPU_PROT_MODE_CHG_MASK                      0x40
#define WCD9378_INTR_TEST_2_GPIO_1_INT_MASK                              0x20
#define WCD9378_INTR_TEST_2_GPIO_0_INT_MASK                              0x10
#define WCD9378_INTR_TEST_2_HPHL_SURGE_DET_INT_MASK                      0x08
#define WCD9378_INTR_TEST_2_HPHR_SURGE_DET_INT_MASK                      0x04
#define WCD9378_INTR_TEST_2_MBHC_MOISTRUE_INT_MASK                       0x02
#define WCD9378_INTR_TEST_2_LDORT_SCD_INT_MASK                           0x01

/* WCD9378_INTR_TEST_3 Fields: */
#define WCD9378_INTR_TEST_3_SM2_STAT_ALERT_MASK                          0x20
#define WCD9378_INTR_TEST_3_SM1_STAT_ALERT_MASK                          0x10
#define WCD9378_INTR_TEST_3_SM0_STAT_ALERT_MASK                          0x08
#define WCD9378_INTR_TEST_3_SJ_STAT_ALERT_MASK                           0x04
#define WCD9378_INTR_TEST_3_SA_STAT_ALERT_MASK                           0x02

/* WCD9378_TX_MODE_DBG_EN Fields: */
#define WCD9378_TX_MODE_DBG_EN_TXD2_MODE_DBG_EN_MASK                     0x04
#define WCD9378_TX_MODE_DBG_EN_TXD1_MODE_DBG_EN_MASK                     0x02
#define WCD9378_TX_MODE_DBG_EN_TXD0_MODE_DBG_EN_MASK                     0x01

/* WCD9378_TX_MODE_DBG_0_1 Fields: */
#define WCD9378_TX_MODE_DBG_0_1_TXD1_MODE_DBG_MASK                       0xf0
#define WCD9378_TX_MODE_DBG_0_1_TXD0_MODE_DBG_MASK                       0x0f

/* WCD9378_TX_MODE_DBG_2_3 Fields: */
#define WCD9378_TX_MODE_DBG_2_3_TXD2_MODE_DBG_MASK                       0x0f

/* WCD9378_LB_IN_SEL_CTL Fields: */
#define WCD9378_LB_IN_SEL_CTL_AUX_LB_IN_SEL_MASK                         0x0c
#define WCD9378_LB_IN_SEL_CTL_HPH_LB_IN_SEL_MASK                         0x03

/* WCD9378_LOOP_BACK_MODE Fields: */
#define WCD9378_LOOP_BACK_MODE_TX_DATA_EDGE_MASK                         0x10
#define WCD9378_LOOP_BACK_MODE_RX_DATA_EDGE_MASK                         0x08
#define WCD9378_LOOP_BACK_MODE_LOOPBACK_MODE_MASK                        0x07

/* WCD9378_SWR_DAC_TEST Fields: */
#define WCD9378_SWR_DAC_TEST_SWR_DAC_TEST_MASK                           0x01

/* WCD9378_SWR_HM_TEST_RX_0 Fields: */
#define WCD9378_SWR_HM_TEST_RX_0_ALT_MODE_MASK                           0x80
#define WCD9378_SWR_HM_TEST_RX_0_IO_MODE_MASK                            0x40
#define WCD9378_SWR_HM_TEST_RX_0_LN2_T_DATA_OE_MASK                      0x20
#define WCD9378_SWR_HM_TEST_RX_0_LN2_T_DATA_OUT_MASK                     0x10
#define WCD9378_SWR_HM_TEST_RX_0_LN2_T_KEEPER_EN_MASK                    0x08
#define WCD9378_SWR_HM_TEST_RX_0_LN1_T_DATA_OE_MASK                      0x04
#define WCD9378_SWR_HM_TEST_RX_0_LN1_T_DATA_OUT_MASK                     0x02
#define WCD9378_SWR_HM_TEST_RX_0_LN1_T_KEEPER_EN_MASK                    0x01

/* WCD9378_SWR_HM_TEST_TX_0 Fields: */
#define WCD9378_SWR_HM_TEST_TX_0_ALT_MODE_MASK                           0x80
#define WCD9378_SWR_HM_TEST_TX_0_IO_MODE_MASK                            0x40
#define WCD9378_SWR_HM_TEST_TX_0_LN2_T_DATA_OE_MASK                      0x20
#define WCD9378_SWR_HM_TEST_TX_0_LN2_T_DATA_OUT_MASK                     0x10
#define WCD9378_SWR_HM_TEST_TX_0_LN2_T_KEEPER_EN_MASK                    0x08
#define WCD9378_SWR_HM_TEST_TX_0_LN1_T_DATA_OE_MASK                      0x04
#define WCD9378_SWR_HM_TEST_TX_0_LN1_T_DATA_OUT_MASK                     0x02
#define WCD9378_SWR_HM_TEST_TX_0_LN1_T_KEEPER_EN_MASK                    0x01

/* WCD9378_SWR_HM_TEST_RX_1 Fields: */
#define WCD9378_SWR_HM_TEST_RX_1_DTEST_SEL_MASK                          0x1c
#define WCD9378_SWR_HM_TEST_RX_1_LN2_DLY_CELL_TEST_EN_MASK               0x02
#define WCD9378_SWR_HM_TEST_RX_1_LN1_DLY_CELL_TEST_EN_MASK               0x01

/* WCD9378_SWR_HM_TEST_TX_1 Fields: */
#define WCD9378_SWR_HM_TEST_TX_1_DTEST_SEL_MASK                          0x3c
#define WCD9378_SWR_HM_TEST_TX_1_LN2_DLY_CELL_TEST_EN_MASK               0x02
#define WCD9378_SWR_HM_TEST_TX_1_LN1_DLY_CELL_TEST_EN_MASK               0x01

/* WCD9378_SWR_HM_TEST_0 Fields: */
#define WCD9378_SWR_HM_TEST_0_TX_LN2_T_DATA_IN_MASK                      0x80
#define WCD9378_SWR_HM_TEST_0_TX_LN2_T_CLK_IN_MASK                       0x40
#define WCD9378_SWR_HM_TEST_0_TX_LN1_T_DATA_IN_MASK                      0x20
#define WCD9378_SWR_HM_TEST_0_TX_LN1_T_CLK_IN_MASK                       0x10
#define WCD9378_SWR_HM_TEST_0_RX_LN2_T_DATA_IN_MASK                      0x08
#define WCD9378_SWR_HM_TEST_0_RX_LN2_T_CLK_IN_MASK                       0x04
#define WCD9378_SWR_HM_TEST_0_RX_LN1_T_DATA_IN_MASK                      0x02
#define WCD9378_SWR_HM_TEST_0_RX_LN1_T_CLK_IN_MASK                       0x01

/* WCD9378_PAD_CTL_SWR_0 Fields: */
#define WCD9378_PAD_CTL_SWR_0_SWR_SLEW_PRG_MASK                          0xf0
#define WCD9378_PAD_CTL_SWR_0_SWR_DRIVE_PRG_MASK                         0x0f

/* WCD9378_PAD_CTL_SWR_1 Fields: */
#define WCD9378_PAD_CTL_SWR_1_SWR_TDZ_PRG_MASK                           0x0f

/* WCD9378_I2C_CTL Fields: */
#define WCD9378_I2C_CTL_ACTIVE_MODE_MASK                                 0x01

/* WCD9378_LEGACY_SW_MODE Fields: */
#define WCD9378_LEGACY_SW_MODE_USE_LOCAL_INTR_CTRL_MASK                  0x08
#define WCD9378_LEGACY_SW_MODE_CDC_LEGACY_ACCESS_MASK                    0x04
#define WCD9378_LEGACY_SW_MODE_MIPI_SWR_V1P1_MASK                        0x02
#define WCD9378_LEGACY_SW_MODE_CDC_TX_TANGGU_SW_MODE_MASK                0x01

/* WCD9378_EFUSE_TEST_CTL_0 Fields: */
#define WCD9378_EFUSE_TEST_CTL_0_EFUSE_TEST_CTL_LSB_MASK                 0xff

/* WCD9378_EFUSE_TEST_CTL_1 Fields: */
#define WCD9378_EFUSE_TEST_CTL_1_EFUSE_TEST_CTL_MSB_MASK                 0xff

/* WCD9378_EFUSE_T_DATA_0 Fields: */
#define WCD9378_EFUSE_T_DATA_0_EFUSE_DATA_MASK                           0xff

/* WCD9378_PAD_CTL_PDM_RX0 Fields: */
#define WCD9378_PAD_CTL_PDM_RX0_PDM_SLEW_PRG_MASK                        0xf0
#define WCD9378_PAD_CTL_PDM_RX0_PDM_DRIVE_PRG_MASK                       0x0f

/* WCD9378_PAD_CTL_PDM_RX1 Fields: */
#define WCD9378_PAD_CTL_PDM_RX1_PDM_SLEW_PRG_MASK                        0xf0
#define WCD9378_PAD_CTL_PDM_RX1_PDM_DRIVE_PRG_MASK                       0x0f

/* WCD9378_PAD_CTL_PDM_TX0 Fields: */
#define WCD9378_PAD_CTL_PDM_TX0_PDM_SLEW_PRG_MASK                        0xf0
#define WCD9378_PAD_CTL_PDM_TX0_PDM_DRIVE_PRG_MASK                       0x0f

/* WCD9378_PAD_CTL_PDM_TX1 Fields: */
#define WCD9378_PAD_CTL_PDM_TX1_PDM_SLEW_PRG_MASK                        0xf0
#define WCD9378_PAD_CTL_PDM_TX1_PDM_DRIVE_PRG_MASK                       0x0f

/* WCD9378_PAD_INP_DIS_0 Fields: */
#define WCD9378_PAD_INP_DIS_0_DMIC3_CLK_MASK                             0x20
#define WCD9378_PAD_INP_DIS_0_DMIC3_DATA_MASK                            0x10
#define WCD9378_PAD_INP_DIS_0_DMIC2_CLK_MASK                             0x08
#define WCD9378_PAD_INP_DIS_0_DMIC2_DATA_MASK                            0x04
#define WCD9378_PAD_INP_DIS_0_DMIC1_CLK_MASK                             0x02
#define WCD9378_PAD_INP_DIS_0_DMIC1_DATA_MASK                            0x01

/* WCD9378_DRIVE_STRENGTH_0 Fields: */
#define WCD9378_DRIVE_STRENGTH_0_DS_DMIC2_CLK_MASK                       0xc0
#define WCD9378_DRIVE_STRENGTH_0_DS_DMIC2_DATA_MASK                      0x30
#define WCD9378_DRIVE_STRENGTH_0_DS_DMIC1_CLK_MASK                       0x0c
#define WCD9378_DRIVE_STRENGTH_0_DS_DMIC1_DATA_MASK                      0x03

/* WCD9378_DRIVE_STRENGTH_1 Fields: */
#define WCD9378_DRIVE_STRENGTH_1_DS_DMIC3_CLK_MASK                       0x0c
#define WCD9378_DRIVE_STRENGTH_1_DS_DMIC3_DATA_MASK                      0x03

/* WCD9378_RX_DATA_EDGE_CTL Fields: */
#define WCD9378_RX_DATA_EDGE_CTL_HPH_CLH_EDGE_MASK                       0x20
#define WCD9378_RX_DATA_EDGE_CTL_AUX_DOUT_EDGE_MASK                      0x10
#define WCD9378_RX_DATA_EDGE_CTL_HPHR_DOUT_EDGE_MASK                     0x08
#define WCD9378_RX_DATA_EDGE_CTL_HPHL_DOUT_EDGE_MASK                     0x04
#define WCD9378_RX_DATA_EDGE_CTL_HPHR_GAIN_EDGE_MASK                     0x02
#define WCD9378_RX_DATA_EDGE_CTL_HPHL_GAIN_EDGE_MASK                     0x01

/* WCD9378_TX_DATA_EDGE_CTL Fields: */
#define WCD9378_TX_DATA_EDGE_CTL_TX_WE_DLY_MASK                          0x18
#define WCD9378_TX_DATA_EDGE_CTL_TX2_DIN_EDGE_MASK                       0x04
#define WCD9378_TX_DATA_EDGE_CTL_TX1_DIN_EDGE_MASK                       0x02
#define WCD9378_TX_DATA_EDGE_CTL_TX0_DIN_EDGE_MASK                       0x01

/* WCD9378_GPIO_MODE Fields: */
#define WCD9378_GPIO_MODE_GPIO_3_EN_MASK                                 0x10
#define WCD9378_GPIO_MODE_GPIO_2_EN_MASK                                 0x08
#define WCD9378_GPIO_MODE_GPIO_1_EN_MASK                                 0x04
#define WCD9378_GPIO_MODE_GPIO_0_EN_MASK                                 0x02
#define WCD9378_GPIO_MODE_TEST_MODE_MASK                                 0x01

/* WCD9378_PIN_CTL_OE Fields: */
#define WCD9378_PIN_CTL_OE_TEST_PIN_CTL_OE_MASK                          0x10
#define WCD9378_PIN_CTL_OE_GPIO_3_PIN_CTL_OE_MASK                        0x08
#define WCD9378_PIN_CTL_OE_GPIO_2_PIN_CTL_OE_MASK                        0x04
#define WCD9378_PIN_CTL_OE_GPIO_1_PIN_CTL_OE_MASK                        0x02
#define WCD9378_PIN_CTL_OE_GPIO_0_PIN_CTL_OE_MASK                        0x01

/* WCD9378_PIN_CTL_DATA_0 Fields: */
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC3_CLK_MASK                        0x20
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC3_DATA_MASK                       0x10
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC2_CLK_MASK                        0x08
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC2_DATA_MASK                       0x04
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC1_CLK_MASK                        0x02
#define WCD9378_PIN_CTL_DATA_0_PAD_DMIC1_DATA_MASK                       0x01

/* WCD9378_PIN_STATUS_0 Fields: */
#define WCD9378_PIN_STATUS_0_PAD_DMIC3_CLK_MASK                          0x20
#define WCD9378_PIN_STATUS_0_PAD_DMIC3_DATA_MASK                         0x10
#define WCD9378_PIN_STATUS_0_PAD_DMIC2_CLK_MASK                          0x08
#define WCD9378_PIN_STATUS_0_PAD_DMIC2_DATA_MASK                         0x04
#define WCD9378_PIN_STATUS_0_PAD_DMIC1_CLK_MASK                          0x02
#define WCD9378_PIN_STATUS_0_PAD_DMIC1_DATA_MASK                         0x01

/* WCD9378_DIG_DEBUG_CTL Fields: */
#define WCD9378_DIG_DEBUG_CTL_DIG_DEBUG_CTL_MASK                         0xff

/* WCD9378_DIG_DEBUG_EN Fields: */
#define WCD9378_DIG_DEBUG_EN_TX_DBG_MODE_MASK                            0x02
#define WCD9378_DIG_DEBUG_EN_RX_DBG_MODE_MASK                            0x01

/* WCD9378_ANA_CSR_DBG_ADD Fields: */
#define WCD9378_ANA_CSR_DBG_ADD_ADD_MASK                                 0xff

/* WCD9378_ANA_CSR_DBG_CTL Fields: */
#define WCD9378_ANA_CSR_DBG_CTL_WR_VALUE_MASK                            0xc0
#define WCD9378_ANA_CSR_DBG_CTL_RD_VALUE_MASK                            0x38
#define WCD9378_ANA_CSR_DBG_CTL_DBG_PAGE_SEL_MASK                        0x06
#define WCD9378_ANA_CSR_DBG_CTL_DBG_EN_MASK                              0x01

/* WCD9378_SSP_DBG Fields: */
#define WCD9378_SSP_DBG_RX_SSP_DBG_MASK                                  0x02
#define WCD9378_SSP_DBG_TX_SSP_DBG_MASK                                  0x01

/* WCD9378_MODE_STATUS_0 Fields: */
#define WCD9378_MODE_STATUS_0_ATE_7_MASK                                 0x80
#define WCD9378_MODE_STATUS_0_ATE_6_MASK                                 0x40
#define WCD9378_MODE_STATUS_0_ATE_5_MASK                                 0x20
#define WCD9378_MODE_STATUS_0_ATE_4_MASK                                 0x10
#define WCD9378_MODE_STATUS_0_ATE_3_MASK                                 0x08
#define WCD9378_MODE_STATUS_0_ATE_2_MASK                                 0x04
#define WCD9378_MODE_STATUS_0_ATE_1_MASK                                 0x02
#define WCD9378_MODE_STATUS_0_SWR_TEST_MASK                              0x01

/* WCD9378_MODE_STATUS_1 Fields: */
#define WCD9378_MODE_STATUS_1_SWR_PAD_TEST_MASK                          0x02
#define WCD9378_MODE_STATUS_1_EFUSE_MODE_MASK                            0x01

/* WCD9378_SPARE_0 Fields: */
#define WCD9378_SPARE_0_SPARE_REG_0_MASK                                 0xffff

/* WCD9378_SPARE_1 Fields: */
#define WCD9378_SPARE_1_SPARE_REG_1_MASK                                 0xffffff

/* WCD9378_SPARE_2 Fields: */
#define WCD9378_SPARE_2_SPARE_REG_2_MASK                                 0xffffffff

/* WCD9378_EFUSE_REG_0 Fields: */
#define WCD9378_EFUSE_REG_0_SPARE_BITS_MASK                              0xe0
#define WCD9378_EFUSE_REG_0_WCD9378_ID_MASK                              0x1e
#define WCD9378_EFUSE_REG_0_EFUSE_BLOWN_MASK                             0x01

/* WCD9378_EFUSE_REG_1 Fields: */
#define WCD9378_EFUSE_REG_1_LOT_ID_0_MASK                                0xff

/* WCD9378_EFUSE_REG_2 Fields: */
#define WCD9378_EFUSE_REG_2_LOT_ID_1_MASK                                0xff

/* WCD9378_EFUSE_REG_3 Fields: */
#define WCD9378_EFUSE_REG_3_LOT_ID_2_MASK                                0xff

/* WCD9378_EFUSE_REG_4 Fields: */
#define WCD9378_EFUSE_REG_4_LOT_ID_3_MASK                                0xff

/* WCD9378_EFUSE_REG_5 Fields: */
#define WCD9378_EFUSE_REG_5_LOT_ID_4_MASK                                0xff

/* WCD9378_EFUSE_REG_6 Fields: */
#define WCD9378_EFUSE_REG_6_LOT_ID_5_MASK                                0xff

/* WCD9378_EFUSE_REG_7 Fields: */
#define WCD9378_EFUSE_REG_7_LOT_ID_6_MASK                                0xff

/* WCD9378_EFUSE_REG_8 Fields: */
#define WCD9378_EFUSE_REG_8_LOT_ID_7_MASK                                0xff

/* WCD9378_EFUSE_REG_9 Fields: */
#define WCD9378_EFUSE_REG_9_LOT_ID_8_MASK                                0xff

/* WCD9378_EFUSE_REG_10 Fields: */
#define WCD9378_EFUSE_REG_10_LOT_ID_9_MASK                               0xff

/* WCD9378_EFUSE_REG_11 Fields: */
#define WCD9378_EFUSE_REG_11_LOT_ID_10_MASK                              0xff

/* WCD9378_EFUSE_REG_12 Fields: */
#define WCD9378_EFUSE_REG_12_LOT_ID_11_MASK                              0xff

/* WCD9378_EFUSE_REG_13 Fields: */
#define WCD9378_EFUSE_REG_13_WAFER_ID_MASK                               0xff

/* WCD9378_EFUSE_REG_14 Fields: */
#define WCD9378_EFUSE_REG_14_X_DIE_LOCATION_MASK                         0xff

/* WCD9378_EFUSE_REG_15 Fields: */
#define WCD9378_EFUSE_REG_15_Y_DIE_LOCATION_MASK                         0xff

/* WCD9378_EFUSE_REG_16 Fields: */
#define WCD9378_EFUSE_REG_16_FAB_ID_MASK                                 0xff

/* WCD9378_EFUSE_REG_17 Fields: */
#define WCD9378_EFUSE_REG_17_TEST_PROGRAM_REV_MASK                       0xff

/* WCD9378_EFUSE_REG_18 Fields: */
#define WCD9378_EFUSE_REG_18_DIE_REVISION_MASK                           0xff

/* WCD9378_EFUSE_REG_19 Fields: */
#define WCD9378_EFUSE_REG_19_MFG_ID_SPARE_MASK                           0xff

/* WCD9378_EFUSE_REG_20 Fields: */
#define WCD9378_EFUSE_REG_20_I2C_SLV_ID_BLOWN_MASK                       0x80
#define WCD9378_EFUSE_REG_20_I2C_SLAVE_ID_MASK                           0x7f

/* WCD9378_EFUSE_REG_21 Fields: */
#define WCD9378_EFUSE_REG_21_MBHC_IMP_DET_0_MASK                         0xff

/* WCD9378_EFUSE_REG_22 Fields: */
#define WCD9378_EFUSE_REG_22_MBHC_IMP_DET_1_MASK                         0xff

/* WCD9378_EFUSE_REG_23 Fields: */
#define WCD9378_EFUSE_REG_23_SWR_PAD_DRIVE_PRG_1P8V_MASK                 0xf0
#define WCD9378_EFUSE_REG_23_SWR_SLEW_PRG_1P8V_MASK                      0x0f

/* WCD9378_EFUSE_REG_24 Fields: */
#define WCD9378_EFUSE_REG_24_SPARE_BITS_MASK                             0xe0
#define WCD9378_EFUSE_REG_24_SWR_PAD_BLOWN_MASK                          0x10
#define WCD9378_EFUSE_REG_24_SWR_TDZ_DELAY_PRG_1P8V_MASK                 0x0f

/* WCD9378_EFUSE_REG_25 Fields: */
#define WCD9378_EFUSE_REG_25_MBHC_IMP_DET_2_MASK                         0xff

/* WCD9378_EFUSE_REG_26 Fields: */
#define WCD9378_EFUSE_REG_26_MBHC_IMP_DET_3_MASK                         0xff

/* WCD9378_EFUSE_REG_27 Fields: */
#define WCD9378_EFUSE_REG_27_HPH_DSD_DIS_MASK                            0x80
#define WCD9378_EFUSE_REG_27_BG_TUNE_BLOWN_MASK                          0x40
#define WCD9378_EFUSE_REG_27_BG_TUNE_MASK                                0x30
#define WCD9378_EFUSE_REG_27_EFUSE_HPH_MASK                              0x0f

/* WCD9378_EFUSE_REG_28 Fields: */
#define WCD9378_EFUSE_REG_28_SPARE_BITS_MASK                             0xff

/* WCD9378_EFUSE_REG_29 Fields: */
#define WCD9378_EFUSE_REG_29_SPARE_BITS_MASK                             0xe0
#define WCD9378_EFUSE_REG_29_TX_LP_DIS_MASK                              0x10
#define WCD9378_EFUSE_REG_29_TX_HP_DIS_MASK                              0x08
#define WCD9378_EFUSE_REG_29_DMIC_DIS_MASK                               0x04
#define WCD9378_EFUSE_REG_29_PLATFORM_MASK                               0x02
#define WCD9378_EFUSE_REG_29_PLATFORM_BLOWN_MASK                         0x01

/* WCD9378_EFUSE_REG_30 Fields: */
#define WCD9378_EFUSE_REG_30_SPARE_BITS_MASK                             0xf0
#define WCD9378_EFUSE_REG_30_SWR_SLEW_PRG_1P2V_MASK                      0x0f

/* WCD9378_EFUSE_REG_31 Fields: */
#define WCD9378_EFUSE_REG_31_SWR_PAD_DRIVE_PRG_1P2V_MASK                 0xf0
#define WCD9378_EFUSE_REG_31_SWR_TDZ_DELAY_PRG_1P2V_MASK                 0x0f

/* WCD9378_TX_REQ_FB_CTL_2 Fields: */
#define WCD9378_TX_REQ_FB_CTL_2_L0_FB_T2_MASK                            0xf0
#define WCD9378_TX_REQ_FB_CTL_2_L0_FB_T1_MASK                            0x0f

/* WCD9378_TX_REQ_FB_CTL_3 Fields: */
#define WCD9378_TX_REQ_FB_CTL_3_L1_FB_T2_MASK                            0xf0
#define WCD9378_TX_REQ_FB_CTL_3_L1_FB_T1_MASK                            0x0f

/* WCD9378_TX_REQ_FB_CTL_4 Fields: */
#define WCD9378_TX_REQ_FB_CTL_4_L2_FB_T2_MASK                            0xf0
#define WCD9378_TX_REQ_FB_CTL_4_L2_FB_T1_MASK                            0x0f

/* WCD9378_DEM_BYPASS_DATA0 Fields: */
#define WCD9378_DEM_BYPASS_DATA0_DEM_BYPASS_DATA0_MASK                   0xff

/* WCD9378_DEM_BYPASS_DATA1 Fields: */
#define WCD9378_DEM_BYPASS_DATA1_DEM_BYPASS_DATA0_MASK                   0xff

/* WCD9378_DEM_BYPASS_DATA2 Fields: */
#define WCD9378_DEM_BYPASS_DATA2_DEM_BYPASS_DATA0_MASK                   0xff

/* WCD9378_DEM_BYPASS_DATA3 Fields: */
#define WCD9378_DEM_BYPASS_DATA3_DEM_BYPASS_DATA0_MASK                   0x03

/* WCD9378_RX0_PCM_RAMP_STEP Fields: */
#define WCD9378_RX0_PCM_RAMP_STEP_RX0_RAMP_STEP_MASK                     0xff

/* WCD9378_RX0_DSD_RAMP_STEP Fields: */
#define WCD9378_RX0_DSD_RAMP_STEP_RX0_RAMP_STEP_MASK                     0xff

/* WCD9378_RX1_PCM_RAMP_STEP Fields: */
#define WCD9378_RX1_PCM_RAMP_STEP_RX1_RAMP_STEP_MASK                     0xff

/* WCD9378_RX1_DSD_RAMP_STEP Fields: */
#define WCD9378_RX1_DSD_RAMP_STEP_RX1_RAMP_STEP_MASK                     0xff

/* WCD9378_RX2_RAMP_STEP Fields: */
#define WCD9378_RX2_RAMP_STEP_RX2_RAMP_STEP_MASK                         0xff

/* WCD9378_PLATFORM_CTL Fields: */
#define WCD9378_PLATFORM_CTL_MODE_MASK                                   0x01

/* WCD9378_CLK_DIV_CFG Fields: */
#define WCD9378_CLK_DIV_CFG_TX_DIV_EN_MASK                               0x02
#define WCD9378_CLK_DIV_CFG_RX_DIV_EN_MASK                               0x01

/* WCD9378_DRE_DLY_VAL Fields: */
#define WCD9378_DRE_DLY_VAL_SWR_HPHR_MASK                                0xf0
#define WCD9378_DRE_DLY_VAL_SWR_HPHL_MASK                                0x0f


/* WCD9378_SYS_USAGE_CTRL Fields: */
#define WCD9378_SYS_USAGE_CTRL_SYS_USAGE_CTRL_MASK                       0x0f

/* WCD9378_SURGE_CTL Fields: */
#define WCD9378_SURGE_CTL_SURGE_EN_MASK                                  0x01

/* WCD9378_SEQ_CTL Fields: */
#define WCD9378_SEQ_CTL_TX2_SEQ_SOFT_RST_MASK                            0x10
#define WCD9378_SEQ_CTL_TX1_SEQ_SOFT_RST_MASK                            0x08
#define WCD9378_SEQ_CTL_TX0_SEQ_SOFT_RST_MASK                            0x04
#define WCD9378_SEQ_CTL_SA_SEQ_SOFT_RST_MASK                             0x02
#define WCD9378_SEQ_CTL_SJ_SEQ_SOFT_RST_MASK                             0x01

/* WCD9378_HPH_UP_T0 Fields: */
#define WCD9378_HPH_UP_T0_HPH_UP_T0_MASK                                 0x07

/* WCD9378_HPH_UP_T1 Fields: */
#define WCD9378_HPH_UP_T1_HPH_UP_T1_MASK                                 0x07

/* WCD9378_HPH_UP_T2 Fields: */
#define WCD9378_HPH_UP_T2_HPH_UP_T2_MASK                                 0x07

/* WCD9378_HPH_UP_T3 Fields: */
#define WCD9378_HPH_UP_T3_HPH_UP_T3_MASK                                 0x07

/* WCD9378_HPH_UP_T4 Fields: */
#define WCD9378_HPH_UP_T4_HPH_UP_T4_MASK                                 0x07

/* WCD9378_HPH_UP_T5 Fields: */
#define WCD9378_HPH_UP_T5_HPH_UP_T5_MASK                                 0x07

/* WCD9378_HPH_UP_T6 Fields: */
#define WCD9378_HPH_UP_T6_HPH_UP_T6_MASK                                 0x07

/* WCD9378_HPH_UP_T7 Fields: */
#define WCD9378_HPH_UP_T7_HPH_UP_T7_MASK                                 0x07

/* WCD9378_HPH_UP_T8 Fields: */
#define WCD9378_HPH_UP_T8_HPH_UP_T8_MASK                                 0x07

/* WCD9378_HPH_UP_T9 Fields: */
#define WCD9378_HPH_UP_T9_HPH_UP_T9_MASK                                 0x07

/* WCD9378_HPH_UP_T10 Fields: */
#define WCD9378_HPH_UP_T10_HPH_UP_T10_MASK                               0x07

/* WCD9378_HPH_DN_T0 Fields: */
#define WCD9378_HPH_DN_T0_HPH_DN_T0_MASK                                 0x07

/* WCD9378_HPH_DN_T1 Fields: */
#define WCD9378_HPH_DN_T1_HPH_DN_T1_MASK                                 0x07

/* WCD9378_HPH_DN_T2 Fields: */
#define WCD9378_HPH_DN_T2_HPH_DN_T2_MASK                                 0x07

/* WCD9378_HPH_DN_T3 Fields: */
#define WCD9378_HPH_DN_T3_HPH_DN_T3_MASK                                 0x07

/* WCD9378_HPH_DN_T4 Fields: */
#define WCD9378_HPH_DN_T4_HPH_DN_T4_MASK                                 0x07

/* WCD9378_HPH_DN_T5 Fields: */
#define WCD9378_HPH_DN_T5_HPH_DN_T5_MASK                                 0x07

/* WCD9378_HPH_DN_T6 Fields: */
#define WCD9378_HPH_DN_T6_HPH_DN_T6_MASK                                 0x07

/* WCD9378_HPH_DN_T7 Fields: */
#define WCD9378_HPH_DN_T7_HPH_DN_T7_MASK                                 0x07

/* WCD9378_HPH_DN_T8 Fields: */
#define WCD9378_HPH_DN_T8_HPH_DN_T8_MASK                                 0x07

/* WCD9378_HPH_DN_T9 Fields: */
#define WCD9378_HPH_DN_T9_HPH_DN_T9_MASK                                 0x07

/* WCD9378_HPH_DN_T10 Fields: */
#define WCD9378_HPH_DN_T10_HPH_DN_T10_MASK                               0x07

/* WCD9378_HPH_UP_STAGE_LOC_0 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_0_HPH_UP_STAGE_LOC_0_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_1 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_1_HPH_UP_STAGE_LOC_1_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_2 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_2_HPH_UP_STAGE_LOC_2_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_3 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_3_HPH_UP_STAGE_LOC_3_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_4 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_4_HPH_UP_STAGE_LOC_4_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_5 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_5_HPH_UP_STAGE_LOC_5_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_6 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_6_HPH_UP_STAGE_LOC_6_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_7 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_7_HPH_UP_STAGE_LOC_7_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_8 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_8_HPH_UP_STAGE_LOC_8_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_9 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_9_HPH_UP_STAGE_LOC_9_MASK               0x0f

/* WCD9378_HPH_UP_STAGE_LOC_10 Fields: */
#define WCD9378_HPH_UP_STAGE_LOC_10_HPH_UP_STAGE_LOC_10_MASK             0x0f

/* WCD9378_HPH_DN_STAGE_LOC_0 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_0_HPH_DN_STAGE_LOC_0_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_1 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_1_HPH_DN_STAGE_LOC_1_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_2 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_2_HPH_DN_STAGE_LOC_2_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_3 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_3_HPH_DN_STAGE_LOC_3_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_4 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_4_HPH_DN_STAGE_LOC_4_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_5 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_5_HPH_DN_STAGE_LOC_5_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_6 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_6_HPH_DN_STAGE_LOC_6_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_7 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_7_HPH_DN_STAGE_LOC_7_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_8 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_8_HPH_DN_STAGE_LOC_8_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_9 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_9_HPH_DN_STAGE_LOC_9_MASK               0x0f

/* WCD9378_HPH_DN_STAGE_LOC_10 Fields: */
#define WCD9378_HPH_DN_STAGE_LOC_10_HPH_DN_STAGE_LOC_10_MASK             0x0f

/* WCD9378_SA_UP_T0 Fields: */
#define WCD9378_SA_UP_T0_SA_UP_T0_MASK                                   0x07

/* WCD9378_SA_UP_T1 Fields: */
#define WCD9378_SA_UP_T1_SA_UP_T1_MASK                                   0x07

/* WCD9378_SA_UP_T2 Fields: */
#define WCD9378_SA_UP_T2_SA_UP_T2_MASK                                   0x07

/* WCD9378_SA_UP_T3 Fields: */
#define WCD9378_SA_UP_T3_SA_UP_T3_MASK                                   0x07

/* WCD9378_SA_UP_T4 Fields: */
#define WCD9378_SA_UP_T4_SA_UP_T4_MASK                                   0x07

/* WCD9378_SA_UP_T5 Fields: */
#define WCD9378_SA_UP_T5_SA_UP_T5_MASK                                   0x07

/* WCD9378_SA_UP_T6 Fields: */
#define WCD9378_SA_UP_T6_SA_UP_T6_MASK                                   0x07

/* WCD9378_SA_UP_T7 Fields: */
#define WCD9378_SA_UP_T7_SA_UP_T7_MASK                                   0x07

/* WCD9378_SA_DN_T0 Fields: */
#define WCD9378_SA_DN_T0_SA_DN_T0_MASK                                   0x07

/* WCD9378_SA_DN_T1 Fields: */
#define WCD9378_SA_DN_T1_SA_DN_T1_MASK                                   0x07

/* WCD9378_SA_DN_T2 Fields: */
#define WCD9378_SA_DN_T2_SA_DN_T2_MASK                                   0x07

/* WCD9378_SA_DN_T3 Fields: */
#define WCD9378_SA_DN_T3_SA_DN_T3_MASK                                   0x07

/* WCD9378_SA_DN_T4 Fields: */
#define WCD9378_SA_DN_T4_SA_DN_T4_MASK                                   0x07

/* WCD9378_SA_DN_T5 Fields: */
#define WCD9378_SA_DN_T5_SA_DN_T5_MASK                                   0x07

/* WCD9378_SA_DN_T6 Fields: */
#define WCD9378_SA_DN_T6_SA_DN_T6_MASK                                   0x07

/* WCD9378_SA_DN_T7 Fields: */
#define WCD9378_SA_DN_T7_SA_DN_T7_MASK                                   0x07

/* WCD9378_SA_UP_STAGE_LOC_0 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_0_SA_UP_STAGE_LOC_0_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_1 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_1_SA_UP_STAGE_LOC_1_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_2 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_2_SA_UP_STAGE_LOC_2_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_3 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_3_SA_UP_STAGE_LOC_3_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_4 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_4_SA_UP_STAGE_LOC_4_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_5 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_5_SA_UP_STAGE_LOC_5_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_6 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_6_SA_UP_STAGE_LOC_6_MASK                 0x07

/* WCD9378_SA_UP_STAGE_LOC_7 Fields: */
#define WCD9378_SA_UP_STAGE_LOC_7_SA_UP_STAGE_LOC_7_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_0 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_0_SA_DN_STAGE_LOC_0_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_1 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_1_SA_DN_STAGE_LOC_1_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_2 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_2_SA_DN_STAGE_LOC_2_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_3 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_3_SA_DN_STAGE_LOC_3_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_4 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_4_SA_DN_STAGE_LOC_4_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_5 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_5_SA_DN_STAGE_LOC_5_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_6 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_6_SA_DN_STAGE_LOC_6_MASK                 0x07

/* WCD9378_SA_DN_STAGE_LOC_7 Fields: */
#define WCD9378_SA_DN_STAGE_LOC_7_SA_DN_STAGE_LOC_7_MASK                 0x07

/* WCD9378_TX0_UP_T0 Fields: */
#define WCD9378_TX0_UP_T0_TX0_UP_T0_MASK                                 0x07

/* WCD9378_TX0_UP_T1 Fields: */
#define WCD9378_TX0_UP_T1_TX0_UP_T1_MASK                                 0x07

/* WCD9378_TX0_UP_T2 Fields: */
#define WCD9378_TX0_UP_T2_TX0_UP_T2_MASK                                 0x07

/* WCD9378_TX0_UP_T3 Fields: */
#define WCD9378_TX0_UP_T3_TX0_UP_T3_MASK                                 0x07

/* WCD9378_TX0_DN_T0 Fields: */
#define WCD9378_TX0_DN_T0_TX0_DN_T0_MASK                                 0x07

/* WCD9378_TX0_DN_T1 Fields: */
#define WCD9378_TX0_DN_T1_TX0_DN_T1_MASK                                 0x07

/* WCD9378_TX0_DN_T2 Fields: */
#define WCD9378_TX0_DN_T2_TX0_DN_T2_MASK                                 0x07

/* WCD9378_TX0_DN_T3 Fields: */
#define WCD9378_TX0_DN_T3_TX0_DN_T3_MASK                                 0x07

/* WCD9378_TX0_UP_STAGE_LOC_0 Fields: */
#define WCD9378_TX0_UP_STAGE_LOC_0_TX0_UP_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX0_UP_STAGE_LOC_1 Fields: */
#define WCD9378_TX0_UP_STAGE_LOC_1_TX0_UP_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX0_UP_STAGE_LOC_2 Fields: */
#define WCD9378_TX0_UP_STAGE_LOC_2_TX0_UP_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX0_UP_STAGE_LOC_3 Fields: */
#define WCD9378_TX0_UP_STAGE_LOC_3_TX0_UP_STAGE_LOC_3_MASK               0x03

/* WCD9378_TX0_DN_STAGE_LOC_0 Fields: */
#define WCD9378_TX0_DN_STAGE_LOC_0_TX0_DN_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX0_DN_STAGE_LOC_1 Fields: */
#define WCD9378_TX0_DN_STAGE_LOC_1_TX0_DN_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX0_DN_STAGE_LOC_2 Fields: */
#define WCD9378_TX0_DN_STAGE_LOC_2_TX0_DN_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX0_DN_STAGE_LOC_3 Fields: */
#define WCD9378_TX0_DN_STAGE_LOC_3_TX0_DN_STAGE_LOC_3_MASK               0x03

/* WCD9378_TX1_UP_T0 Fields: */
#define WCD9378_TX1_UP_T0_TX1_UP_T0_MASK                                 0x07

/* WCD9378_TX1_UP_T1 Fields: */
#define WCD9378_TX1_UP_T1_TX1_UP_T1_MASK                                 0x07

/* WCD9378_TX1_UP_T2 Fields: */
#define WCD9378_TX1_UP_T2_TX1_UP_T2_MASK                                 0x07

/* WCD9378_TX1_UP_T3 Fields: */
#define WCD9378_TX1_UP_T3_TX1_UP_T3_MASK                                 0x07

/* WCD9378_TX1_DN_T0 Fields: */
#define WCD9378_TX1_DN_T0_TX1_DN_T0_MASK                                 0x07

/* WCD9378_TX1_DN_T1 Fields: */
#define WCD9378_TX1_DN_T1_TX1_DN_T1_MASK                                 0x07

/* WCD9378_TX1_DN_T2 Fields: */
#define WCD9378_TX1_DN_T2_TX1_DN_T2_MASK                                 0x07

/* WCD9378_TX1_DN_T3 Fields: */
#define WCD9378_TX1_DN_T3_TX1_DN_T3_MASK                                 0x07

/* WCD9378_TX1_UP_STAGE_LOC_0 Fields: */
#define WCD9378_TX1_UP_STAGE_LOC_0_TX1_UP_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX1_UP_STAGE_LOC_1 Fields: */
#define WCD9378_TX1_UP_STAGE_LOC_1_TX1_UP_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX1_UP_STAGE_LOC_2 Fields: */
#define WCD9378_TX1_UP_STAGE_LOC_2_TX1_UP_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX1_UP_STAGE_LOC_3 Fields: */
#define WCD9378_TX1_UP_STAGE_LOC_3_TX1_UP_STAGE_LOC_3_MASK               0x03

/* WCD9378_TX1_DN_STAGE_LOC_0 Fields: */
#define WCD9378_TX1_DN_STAGE_LOC_0_TX1_DN_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX1_DN_STAGE_LOC_1 Fields: */
#define WCD9378_TX1_DN_STAGE_LOC_1_TX1_DN_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX1_DN_STAGE_LOC_2 Fields: */
#define WCD9378_TX1_DN_STAGE_LOC_2_TX1_DN_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX1_DN_STAGE_LOC_3 Fields: */
#define WCD9378_TX1_DN_STAGE_LOC_3_TX1_DN_STAGE_LOC_3_MASK               0x03

/* WCD9378_TX2_UP_T0 Fields: */
#define WCD9378_TX2_UP_T0_TX2_UP_T0_MASK                                 0x07

/* WCD9378_TX2_UP_T1 Fields: */
#define WCD9378_TX2_UP_T1_TX2_UP_T1_MASK                                 0x07

/* WCD9378_TX2_UP_T2 Fields: */
#define WCD9378_TX2_UP_T2_TX2_UP_T2_MASK                                 0x07

/* WCD9378_TX2_UP_T3 Fields: */
#define WCD9378_TX2_UP_T3_TX2_UP_T3_MASK                                 0x07

/* WCD9378_TX2_DN_T0 Fields: */
#define WCD9378_TX2_DN_T0_TX2_DN_T0_MASK                                 0x07

/* WCD9378_TX2_DN_T1 Fields: */
#define WCD9378_TX2_DN_T1_TX2_DN_T1_MASK                                 0x07

/* WCD9378_TX2_DN_T2 Fields: */
#define WCD9378_TX2_DN_T2_TX2_DN_T2_MASK                                 0x07

/* WCD9378_TX2_DN_T3 Fields: */
#define WCD9378_TX2_DN_T3_TX2_DN_T3_MASK                                 0x07

/* WCD9378_TX2_UP_STAGE_LOC_0 Fields: */
#define WCD9378_TX2_UP_STAGE_LOC_0_TX2_UP_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX2_UP_STAGE_LOC_1 Fields: */
#define WCD9378_TX2_UP_STAGE_LOC_1_TX2_UP_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX2_UP_STAGE_LOC_2 Fields: */
#define WCD9378_TX2_UP_STAGE_LOC_2_TX2_UP_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX2_UP_STAGE_LOC_3 Fields: */
#define WCD9378_TX2_UP_STAGE_LOC_3_TX2_UP_STAGE_LOC_3_MASK               0x03

/* WCD9378_TX2_DN_STAGE_LOC_0 Fields: */
#define WCD9378_TX2_DN_STAGE_LOC_0_TX2_DN_STAGE_LOC_0_MASK               0x03

/* WCD9378_TX2_DN_STAGE_LOC_1 Fields: */
#define WCD9378_TX2_DN_STAGE_LOC_1_TX2_DN_STAGE_LOC_1_MASK               0x03

/* WCD9378_TX2_DN_STAGE_LOC_2 Fields: */
#define WCD9378_TX2_DN_STAGE_LOC_2_TX2_DN_STAGE_LOC_2_MASK               0x03

/* WCD9378_TX2_DN_STAGE_LOC_3 Fields: */
#define WCD9378_TX2_DN_STAGE_LOC_3_TX2_DN_STAGE_LOC_3_MASK               0x03

/* WCD9378_SEQ_HPH_STAT Fields: */
#define WCD9378_SEQ_HPH_STAT_HPH_FUNC_FAULTY_MASK                        0x04
#define WCD9378_SEQ_HPH_STAT_HPH_PWR_UP_RDY_MASK                         0x02
#define WCD9378_SEQ_HPH_STAT_HPH_PWR_DN_RDY_MASK                         0x01

/* WCD9378_SEQ_SA_STAT Fields: */
#define WCD9378_SEQ_SA_STAT_SA_PWR_UP_RDY_MASK                           0x02
#define WCD9378_SEQ_SA_STAT_SA_PWR_DN_RDY_MASK                           0x01

/* WCD9378_SEQ_TX0_STAT Fields: */
#define WCD9378_SEQ_TX0_STAT_TX0_PWR_UP_RDY_MASK                         0x02
#define WCD9378_SEQ_TX0_STAT_TX0_PWR_DN_RDY_MASK                         0x01

/* WCD9378_SEQ_TX1_STAT Fields: */
#define WCD9378_SEQ_TX1_STAT_TX1_FUNC_FAULTY_MASK                        0x04
#define WCD9378_SEQ_TX1_STAT_TX1_PWR_UP_RDY_MASK                         0x02
#define WCD9378_SEQ_TX1_STAT_TX1_PWR_DN_RDY_MASK                         0x01

/* WCD9378_SEQ_TX2_STAT Fields: */
#define WCD9378_SEQ_TX2_STAT_TX2_PWR_UP_RDY_MASK                         0x02
#define WCD9378_SEQ_TX2_STAT_TX2_PWR_DN_RDY_MASK                         0x01

/* WCD9378_MICB_REMAP_TABLE_VAL_0 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_0_MICB_REMAP_TABLE_VAL_0_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_1 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_1_MICB_REMAP_TABLE_VAL_1_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_2 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_2_MICB_REMAP_TABLE_VAL_2_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_3 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_3_MICB_REMAP_TABLE_VAL_3_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_4 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_4_MICB_REMAP_TABLE_VAL_4_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_5 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_5_MICB_REMAP_TABLE_VAL_5_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_6 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_6_MICB_REMAP_TABLE_VAL_6_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_7 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_7_MICB_REMAP_TABLE_VAL_7_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_8 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_8_MICB_REMAP_TABLE_VAL_8_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_9 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_9_MICB_REMAP_TABLE_VAL_9_MASK       0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_10 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_10_MICB_REMAP_TABLE_VAL_10_MASK     0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_11 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_11_MICB_REMAP_TABLE_VAL_11_MASK     0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_12 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_12_MICB_REMAP_TABLE_VAL_12_MASK     0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_13 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_13_MICB_REMAP_TABLE_VAL_13_MASK     0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_14 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_14_MICB_REMAP_TABLE_VAL_14_MASK     0xff

/* WCD9378_MICB_REMAP_TABLE_VAL_15 Fields: */
#define WCD9378_MICB_REMAP_TABLE_VAL_15_MICB_REMAP_TABLE_VAL_15_MASK     0xff

/* WCD9378_SM0_MB_SEL Fields: */
#define WCD9378_SM0_MB_SEL_SM0_MB_SEL_MASK                               0x03

/* WCD9378_SM1_MB_SEL Fields: */
#define WCD9378_SM1_MB_SEL_SM1_MB_SEL_MASK                               0x03

/* WCD9378_SM2_MB_SEL Fields: */
#define WCD9378_SM2_MB_SEL_SM2_MB_SEL_MASK                               0x03

/* WCD9378_MB_PULLUP_EN Fields: */
#define WCD9378_MB_PULLUP_EN_MB3_1P8V_OR_PULLUP_SEL_MASK                 0x04
#define WCD9378_MB_PULLUP_EN_MB2_1P8V_OR_PULLUP_SEL_MASK                 0x02
#define WCD9378_MB_PULLUP_EN_MB1_1P8V_OR_PULLUP_SEL_MASK                 0x01

/* WCD9378_BYP_EN_CTL0 Fields: */
#define WCD9378_BYP_EN_CTL0_TX2_SEQ_BYP_EN_MASK                          0x10
#define WCD9378_BYP_EN_CTL0_TX1_SEQ_BYP_EN_MASK                          0x08
#define WCD9378_BYP_EN_CTL0_TX0_SEQ_BYP_EN_MASK                          0x04
#define WCD9378_BYP_EN_CTL0_SA_SEQ_BYP_EN_MASK                           0x02
#define WCD9378_BYP_EN_CTL0_HPH_SEQ_BYP_EN_MASK                          0x01

/* WCD9378_BYP_EN_CTL1 Fields: */
#define WCD9378_BYP_EN_CTL1_SYS_USAGE_BYP_EN_MASK                        0x04
#define WCD9378_BYP_EN_CTL1_SDCA_BYP_EN_MASK                             0x02
#define WCD9378_BYP_EN_CTL1_DIG_SEQ_BYP_EN_MASK                          0x01

/* WCD9378_BYP_EN_CTL2 Fields: */
#define WCD9378_BYP_EN_CTL2_RX_CLK_BYP_EN_MASK                           0x80
#define WCD9378_BYP_EN_CTL2_TX_CLK_BANK1_BYP_EN_MASK                     0x40
#define WCD9378_BYP_EN_CTL2_TX_CLK_BANK0_BYP_EN_MASK                     0x20
#define WCD9378_BYP_EN_CTL2_HPH_VALID_CFG_BYP_EN_MASK                    0x10
#define WCD9378_BYP_EN_CTL2_SA_VALID_CFG_BYP_EN_MASK                     0x08
#define WCD9378_BYP_EN_CTL2_TX2_VALID_CFG_BYP_EN_MASK                    0x04
#define WCD9378_BYP_EN_CTL2_TX1_VALID_CFG_BYP_EN_MASK                    0x02
#define WCD9378_BYP_EN_CTL2_TX0_VALID_CFG_BYP_EN_MASK                    0x01

/* WCD9378_SEQ_OVRRIDE_CTL0 Fields: */
#define WCD9378_SEQ_OVRRIDE_CTL0_HPHR_COMP_EN_OVR_MASK                   0x80
#define WCD9378_SEQ_OVRRIDE_CTL0_HPHL_COMP_EN_OVR_MASK                   0x40
#define WCD9378_SEQ_OVRRIDE_CTL0_CLASSAB_EN_OVR_MASK                     0x20
#define WCD9378_SEQ_OVRRIDE_CTL0_TX2_SEQ_EN_OVR_MASK                     0x10
#define WCD9378_SEQ_OVRRIDE_CTL0_TX1_SEQ_EN_OVR_MASK                     0x08
#define WCD9378_SEQ_OVRRIDE_CTL0_TX0_SEQ_EN_OVR_MASK                     0x04
#define WCD9378_SEQ_OVRRIDE_CTL0_SA_SEQ_EN_OVR_MASK                      0x02
#define WCD9378_SEQ_OVRRIDE_CTL0_HPH_SEQ_EN_OVR_MASK                     0x01

/* WCD9378_SEQ_OVRRIDE_CTL1 Fields: */
#define WCD9378_SEQ_OVRRIDE_CTL1_RX2_MUTE_OVR_MASK                       0x80
#define WCD9378_SEQ_OVRRIDE_CTL1_RX1_MUTE_OVR_MASK                       0x40
#define WCD9378_SEQ_OVRRIDE_CTL1_RX0_MUTE_OVR_MASK                       0x20
#define WCD9378_SEQ_OVRRIDE_CTL1_TX2_SEQ_TRIGGER_OVR_MASK                0x10
#define WCD9378_SEQ_OVRRIDE_CTL1_TX1_SEQ_TRIGGER_OVR_MASK                0x08
#define WCD9378_SEQ_OVRRIDE_CTL1_TX0_SEQ_TRIGGER_OVR_MASK                0x04
#define WCD9378_SEQ_OVRRIDE_CTL1_SA_SEQ_TRIGGER_OVR_MASK                 0x02
#define WCD9378_SEQ_OVRRIDE_CTL1_HPH_SEQ_TRIGGER_OVR_MASK                0x01

/* WCD9378_SEQ_OVRRIDE_CTL2 Fields: */
#define WCD9378_SEQ_OVRRIDE_CTL2_TX2_VALID_CFG_OVR_MASK                  0x40
#define WCD9378_SEQ_OVRRIDE_CTL2_TX1_VALID_CFG_OVR_MASK                  0x20
#define WCD9378_SEQ_OVRRIDE_CTL2_TX0_VALID_CFG_OVR_MASK                  0x10
#define WCD9378_SEQ_OVRRIDE_CTL2_SA_VALID_CFG_OVR_MASK                   0x08
#define WCD9378_SEQ_OVRRIDE_CTL2_HPH_VALID_CFG_OVR_MASK                  0x04
#define WCD9378_SEQ_OVRRIDE_CTL2_SJ_USAGE_OVR_MASK                       0x03

/* WCD9378_HPH_SEQ_OVRRIDE_CTL0 Fields: */
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_ANA_CLKS_EN_MASK                    0x80
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_DIG_CLKS_EN_MASK                    0x40
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_RX_BIAS_EN_MASK                     0x20
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_NCP_EN_MASK                         0x10
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_CLASSG_CP_EN_MASK                   0x08
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_ACT_DET_EN_MASK                     0x04
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_PAS_EN_MASK                         0x02
#define WCD9378_HPH_SEQ_OVRRIDE_CTL0_ACTUAL_PS_MASK                      0x01

/* WCD9378_HPH_SEQ_OVRRIDE_CTL1 Fields: */
#define WCD9378_HPH_SEQ_OVRRIDE_CTL1_HREF_EN_MASK                        0x04
#define WCD9378_HPH_SEQ_OVRRIDE_CTL1_SET_POWER_LEVEL_MASK                0x02
#define WCD9378_HPH_SEQ_OVRRIDE_CTL1_AUTOCHOP_TIMER_CTL_EN_MASK          0x01

/* WCD9378_SA_SEQ_OVRRIDE_CTL Fields: */
#define WCD9378_SA_SEQ_OVRRIDE_CTL_ANA_CLKS_EN_MASK                      0x80
#define WCD9378_SA_SEQ_OVRRIDE_CTL_DIG_CLKS_EN_MASK                      0x40
#define WCD9378_SA_SEQ_OVRRIDE_CTL_RX_BIAS_EN_MASK                       0x20
#define WCD9378_SA_SEQ_OVRRIDE_CTL_NCP_EN_MASK                           0x10
#define WCD9378_SA_SEQ_OVRRIDE_CTL_CLASSG_CP_EN_MASK                     0x08
#define WCD9378_SA_SEQ_OVRRIDE_CTL_ACT_DET_EN_MASK                       0x04
#define WCD9378_SA_SEQ_OVRRIDE_CTL_PAS_EN_MASK                           0x02
#define WCD9378_SA_SEQ_OVRRIDE_CTL_ACTUAL_PS_MASK                        0x01

/* WCD9378_TX0_SEQ_OVRRIDE_CTL Fields: */
#define WCD9378_TX0_SEQ_OVRRIDE_CTL_TX0_TXDN_CLK_EN_MASK                 0x08
#define WCD9378_TX0_SEQ_OVRRIDE_CTL_TX0_SET_POWER_LEVEL_MASK             0x04
#define WCD9378_TX0_SEQ_OVRRIDE_CTL_TX0_HPF_INIT_MASK                    0x02
#define WCD9378_TX0_SEQ_OVRRIDE_CTL_ACTUAL_PS_MASK                       0x01

/* WCD9378_TX1_SEQ_OVRRIDE_CTL Fields: */
#define WCD9378_TX1_SEQ_OVRRIDE_CTL_TX1_TXDN_CLK_EN_MASK                 0x08
#define WCD9378_TX1_SEQ_OVRRIDE_CTL_TX1_SET_POWER_LEVEL_MASK             0x04
#define WCD9378_TX1_SEQ_OVRRIDE_CTL_TX1_HPF_INIT_MASK                    0x02
#define WCD9378_TX1_SEQ_OVRRIDE_CTL_ACTUAL_PS_MASK                       0x01

/* WCD9378_TX2_SEQ_OVRRIDE_CTL Fields: */
#define WCD9378_TX2_SEQ_OVRRIDE_CTL_TX2_TXDN_CLK_EN_MASK                 0x08
#define WCD9378_TX2_SEQ_OVRRIDE_CTL_TX2_SET_POWER_LEVEL_MASK             0x04
#define WCD9378_TX2_SEQ_OVRRIDE_CTL_TX2_HPF_INIT_MASK                    0x02
#define WCD9378_TX2_SEQ_OVRRIDE_CTL_ACTUAL_PS_MASK                       0x01

/* WCD9378_FORCE_CTL Fields: */
#define WCD9378_FORCE_CTL_FORCE_CLASSG_CP_EN_MASK                        0x20
#define WCD9378_FORCE_CTL_FORCE_NCP_EN_MASK                              0x10
#define WCD9378_FORCE_CTL_FORCE_RX_BIAS_EN_MASK                          0x08
#define WCD9378_FORCE_CTL_FORCE_ANA_DIV4_EN_MASK                         0x04
#define WCD9378_FORCE_CTL_FORCE_ANA_DIV2_EN_MASK                         0x02
#define WCD9378_FORCE_CTL_FORCE_ANA_DIV1_EN_MASK                         0x01


/* WCD9378_DEVICE_DET Fields: */
#define WCD9378_DEVICE_DET_ACCESSORY_TYPE_MASK                           0x07

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_0_TYPE0_WRAP_OSCNX_TPRESS_MIN_0_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_0_TYPE0_WRAP_OSCNX_TPRESS_MAX_0_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_0_TYPE0_WRAP_OSCNX_TRELEASE_MIN_0_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_0_TYPE0_WRAP_OSCNX_TRELEASE_MAX_0_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_0_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_0_MASK 0x0f

/* WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_0 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_0_TYPE0_WRAP_OSCNX_OUTPUT_SEL_0_MASK 0x01

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_1_TYPE0_WRAP_OSCNX_TPRESS_MIN_1_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_1_TYPE0_WRAP_OSCNX_TPRESS_MAX_1_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_1_TYPE0_WRAP_OSCNX_TRELEASE_MIN_1_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_1_TYPE0_WRAP_OSCNX_TRELEASE_MAX_1_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_1_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_1_MASK 0x0f

/* WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_1 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_1_TYPE0_WRAP_OSCNX_OUTPUT_SEL_1_MASK 0x01

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_2_TYPE0_WRAP_OSCNX_TPRESS_MIN_2_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_2_TYPE0_WRAP_OSCNX_TPRESS_MAX_2_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_2_TYPE0_WRAP_OSCNX_TRELEASE_MIN_2_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_2_TYPE0_WRAP_OSCNX_TRELEASE_MAX_2_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_2_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_2_MASK 0x0f

/* WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_2 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_2_TYPE0_WRAP_OSCNX_OUTPUT_SEL_2_MASK 0x01

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MIN_3_TYPE0_WRAP_OSCNX_TPRESS_MIN_3_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TPRESS_MAX_3_TYPE0_WRAP_OSCNX_TPRESS_MAX_3_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MIN_3_TYPE0_WRAP_OSCNX_TRELEASE_MIN_3_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_TRELEASE_MAX_3_TYPE0_WRAP_OSCNX_TRELEASE_MAX_3_MASK 0xff

/* WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_3_TYPE0_WRAP_OSCNX_HDL_BT_ASSIGN_3_MASK 0x0f

/* WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_3 Fields: */
#define WCD9378_TYPE0_WRAP_OSCNX_OUTPUT_SEL_3_TYPE0_WRAP_OSCNX_OUTPUT_SEL_3_MASK 0x01

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_0_TYPE1_WRAP_OSCNX_TPRESS_MIN_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_0_TYPE1_WRAP_OSCNX_TPRESS_MAX_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_0_TYPE1_WRAP_OSCNX_TRELEASE_MIN_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_0_TYPE1_WRAP_OSCNX_TRELEASE_MAX_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_0_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_0_MASK 0x0f

/* WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_0 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_0_TYPE1_WRAP_OSCNX_OUTPUT_SEL_0_MASK 0x01

/* WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_0 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_0_TYPE1_WRAP_HOLD_TPRESS_MIN_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_0 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_0_TYPE1_WRAP_HOLD_TRELEASE_MIN_0_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_0 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_0_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_0_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_0 Fields: */
#define WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_0_TYPE1_WRAP_RO_TDEBOUNCE_0_MASK 0x1f

/* WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_0 Fields: */
#define WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_0_TYPE1_WRAP_RO_HDL_BT_ASSIGN_0_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RTC_OOC_SEL_0 Fields: */
#define WCD9378_TYPE1_WRAP_RTC_OOC_SEL_0_TYPE1_WRAP_RTC_OOC_SEL_0_MASK   0x01

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_1_TYPE1_WRAP_OSCNX_TPRESS_MIN_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_1_TYPE1_WRAP_OSCNX_TPRESS_MAX_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_1_TYPE1_WRAP_OSCNX_TRELEASE_MIN_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_1_TYPE1_WRAP_OSCNX_TRELEASE_MAX_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_1_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_1_MASK 0x0f

/* WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_1 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_1_TYPE1_WRAP_OSCNX_OUTPUT_SEL_1_MASK 0x01

/* WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_1 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_1_TYPE1_WRAP_HOLD_TPRESS_MIN_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_1 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_1_TYPE1_WRAP_HOLD_TRELEASE_MIN_1_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_1 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_1_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_1_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_1 Fields: */
#define WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_1_TYPE1_WRAP_RO_TDEBOUNCE_1_MASK 0x1f

/* WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_1 Fields: */
#define WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_1_TYPE1_WRAP_RO_HDL_BT_ASSIGN_1_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RTC_OOC_SEL_1 Fields: */
#define WCD9378_TYPE1_WRAP_RTC_OOC_SEL_1_TYPE1_WRAP_RTC_OOC_SEL_1_MASK   0x01

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_2_TYPE1_WRAP_OSCNX_TPRESS_MIN_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_2_TYPE1_WRAP_OSCNX_TPRESS_MAX_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_2_TYPE1_WRAP_OSCNX_TRELEASE_MIN_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_2_TYPE1_WRAP_OSCNX_TRELEASE_MAX_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_2_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_2_MASK 0x0f

/* WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_2 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_2_TYPE1_WRAP_OSCNX_OUTPUT_SEL_2_MASK 0x01

/* WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_2 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_2_TYPE1_WRAP_HOLD_TPRESS_MIN_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_2 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_2_TYPE1_WRAP_HOLD_TRELEASE_MIN_2_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_2 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_2_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_2_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_2 Fields: */
#define WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_2_TYPE1_WRAP_RO_TDEBOUNCE_2_MASK 0x1f

/* WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_2 Fields: */
#define WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_2_TYPE1_WRAP_RO_HDL_BT_ASSIGN_2_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RTC_OOC_SEL_2 Fields: */
#define WCD9378_TYPE1_WRAP_RTC_OOC_SEL_2_TYPE1_WRAP_RTC_OOC_SEL_2_MASK   0x01

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MIN_3_TYPE1_WRAP_OSCNX_TPRESS_MIN_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TPRESS_MAX_3_TYPE1_WRAP_OSCNX_TPRESS_MAX_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MIN_3_TYPE1_WRAP_OSCNX_TRELEASE_MIN_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_TRELEASE_MAX_3_TYPE1_WRAP_OSCNX_TRELEASE_MAX_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_3_TYPE1_WRAP_OSCNX_HDL_BT_ASSIGN_3_MASK 0x0f

/* WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_3 Fields: */
#define WCD9378_TYPE1_WRAP_OSCNX_OUTPUT_SEL_3_TYPE1_WRAP_OSCNX_OUTPUT_SEL_3_MASK 0x01

/* WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_3 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TPRESS_MIN_3_TYPE1_WRAP_HOLD_TPRESS_MIN_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_3 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_TRELEASE_MIN_3_TYPE1_WRAP_HOLD_TRELEASE_MIN_3_MASK 0xff

/* WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_3 Fields: */
#define WCD9378_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_3_TYPE1_WRAP_HOLD_HDL_BT_ASSIGN_3_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_3 Fields: */
#define WCD9378_TYPE1_WRAP_RO_TDEBOUNCE_3_TYPE1_WRAP_RO_TDEBOUNCE_3_MASK 0x1f

/* WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_3 Fields: */
#define WCD9378_TYPE1_WRAP_RO_HDL_BT_ASSIGN_3_TYPE1_WRAP_RO_HDL_BT_ASSIGN_3_MASK 0x0f

/* WCD9378_TYPE1_WRAP_RTC_OOC_SEL_3 Fields: */
#define WCD9378_TYPE1_WRAP_RTC_OOC_SEL_3_TYPE1_WRAP_RTC_OOC_SEL_3_MASK   0x01

/* WCD9378_SDCA_MESSAGE_GATE Fields: */
#define WCD9378_SDCA_MESSAGE_GATE_CLAMP_ELEC_SEL_MASK                    0x01

/* WCD9378_MBHC_DATA_IN_EDGE Fields: */
#define WCD9378_MBHC_DATA_IN_EDGE_RISE_EDGE_EN_MASK                      0x01

/* WCD9378_MBHC_RESET Fields: */
#define WCD9378_MBHC_RESET_SOFT_RST_MASK                                 0x01

/* WCD9378_MBHC_DEBUG Fields: */
#define WCD9378_MBHC_DEBUG_UMP_WR_NO_STOP_EN_MASK                        0x02
#define WCD9378_MBHC_DEBUG_UMP_RD_ALL_EN_MASK                            0x01

/* WCD9378_MBHC_DEBUG_UMP_0 Fields: */
#define WCD9378_MBHC_DEBUG_UMP_0_UMP_DATA_IN_7_0_MASK                    0xff

/* WCD9378_MBHC_DEBUG_UMP_1 Fields: */
#define WCD9378_MBHC_DEBUG_UMP_1_UMP_DATA_IN_15_8_MASK                   0xff

/* WCD9378_MBHC_DEBUG_UMP_2 Fields: */
#define WCD9378_MBHC_DEBUG_UMP_2_UMP_DATA_IN_23_16_MASK                  0xff


/* WCD9378_HID_FUNC_EXT_ID_0 Fields: */
#define WCD9378_HID_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK                     0xff

/* WCD9378_HID_FUNC_EXT_ID_1 Fields: */
#define WCD9378_HID_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK                     0xff

/* WCD9378_HID_FUNC_EXT_VER Fields: */
#define WCD9378_HID_FUNC_EXT_VER_FUNC_EXT_VER_MASK                       0xff

/* WCD9378_HID_FUNC_STAT Fields: */
#define WCD9378_HID_FUNC_STAT_FUNC_STAT_MASK                             0xff

/* WCD9378_HID_CUR_OWNER Fields: */
#define WCD9378_HID_CUR_OWNER_HID_CUR_OWNER_MASK                         0x01

/* WCD9378_HID_MSG_OFFSET Fields: */
#define WCD9378_HID_MSG_OFFSET_HID_MSG_OFFSET_MASK                       0xffffffff

/* WCD9378_HID_MSG_LENGTH Fields: */
#define WCD9378_HID_MSG_LENGTH_HID_MSG_LENGTH_MASK                       0xffffffff

/* WCD9378_HID_DEV_MANU_ID_0 Fields: */
#define WCD9378_HID_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK                     0xff

/* WCD9378_HID_DEV_MANU_ID_1 Fields: */
#define WCD9378_HID_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK                     0xff

/* WCD9378_HID_DEV_PART_ID_0 Fields: */
#define WCD9378_HID_DEV_PART_ID_0_DEV_PART_ID_0_MASK                     0xff

/* WCD9378_HID_DEV_PART_ID_1 Fields: */
#define WCD9378_HID_DEV_PART_ID_1_DEV_PART_ID_1_MASK                     0xff

/* WCD9378_HID_DEV_VER Fields: */
#define WCD9378_HID_DEV_VER_DEV_VER_MASK                                 0xff


/* WCD9378_SMP_AMP_FUNC_EXT_ID_0 Fields: */
#define WCD9378_SMP_AMP_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK                 0xff

/* WCD9378_SMP_AMP_FUNC_EXT_ID_1 Fields: */
#define WCD9378_SMP_AMP_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK                 0xff

/* WCD9378_SMP_AMP_FUNC_EXT_VER Fields: */
#define WCD9378_SMP_AMP_FUNC_EXT_VER_FUNC_EXT_VER_MASK                   0xff

/* WCD9378_XU22_BYP Fields: */
#define WCD9378_XU22_BYP_XU22_BYP_MASK                                   0x01

/* WCD9378_PDE22_REQ_PS Fields: */
#define WCD9378_PDE22_REQ_PS_PDE22_REQ_PS_MASK                           0xff

/* WCD9378_FU23_MUTE Fields: */
#define WCD9378_FU23_MUTE_FU23_MUTE_MASK                                 0x01

/* WCD9378_PDE23_REQ_PS Fields: */
#define WCD9378_PDE23_REQ_PS_PDE23_REQ_PS_MASK                           0xff

/* WCD9378_SMP_AMP_FUNC_STAT Fields: */
#define WCD9378_SMP_AMP_FUNC_STAT_FUNC_STAT_MASK                         0xff

/* WCD9378_FUNC_ACT Fields: */
#define WCD9378_FUNC_ACT_FUNC_ACT_MASK                                   0x01

/* WCD9378_PDE22_ACT_PS Fields: */
#define WCD9378_PDE22_ACT_PS_PDE22_ACT_PS_MASK                           0xff

/* WCD9378_SAPU29_PROT_MODE Fields: */
#define WCD9378_SAPU29_PROT_MODE_SAPU29_PROT_MODE_MASK                   0xff

/* WCD9378_SAPU29_PROT_STAT Fields: */
#define WCD9378_SAPU29_PROT_STAT_SAPU29_PROT_STAT_MASK                   0xff

/* WCD9378_PDE23_ACT_PS Fields: */
#define WCD9378_PDE23_ACT_PS_PDE23_ACT_PS_MASK                           0xff

/* WCD9378_SMP_AMP_DEV_MANU_ID_0 Fields: */
#define WCD9378_SMP_AMP_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK                 0xff

/* WCD9378_SMP_AMP_DEV_MANU_ID_1 Fields: */
#define WCD9378_SMP_AMP_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK                 0xff

/* WCD9378_SMP_AMP_DEV_PART_ID_0 Fields: */
#define WCD9378_SMP_AMP_DEV_PART_ID_0_DEV_PART_ID_0_MASK                 0xff

/* WCD9378_SMP_AMP_DEV_PART_ID_1 Fields: */
#define WCD9378_SMP_AMP_DEV_PART_ID_1_DEV_PART_ID_1_MASK                 0xff

/* WCD9378_SMP_AMP_DEV_VER Fields: */
#define WCD9378_SMP_AMP_DEV_VER_DEV_VER_MASK                             0xff


/* WCD9378_CMT_GRP_MASK Fields: */
#define WCD9378_CMT_GRP_MASK_CMT_GRP_MASK_MASK                           0xff

/* WCD9378_SMP_JACK_FUNC_EXT_ID_0 Fields: */
#define WCD9378_SMP_JACK_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK                0xff

/* WCD9378_SMP_JACK_FUNC_EXT_ID_1 Fields: */
#define WCD9378_SMP_JACK_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK                0xff

/* WCD9378_SMP_JACK_FUNC_EXT_VER Fields: */
#define WCD9378_SMP_JACK_FUNC_EXT_VER_FUNC_EXT_VER_MASK                  0xff

/* WCD9378_IT41_USAGE Fields: */
#define WCD9378_IT41_USAGE_IT41_USAGE_MASK                               0xff

/* WCD9378_XU42_BYP Fields: */
#define WCD9378_XU42_BYP_XU42_BYP_MASK                                   0x01

/* WCD9378_PDE42_REQ_PS Fields: */
#define WCD9378_PDE42_REQ_PS_PDE42_REQ_PS_MASK                           0xff

/* WCD9378_FU42_MUTE_CH1 Fields: */
#define WCD9378_FU42_MUTE_CH1_FU42_MUTE_CH1_MASK                         0x01

/* WCD9378_FU42_MUTE_CH2 Fields: */
#define WCD9378_FU42_MUTE_CH2_FU42_MUTE_CH2_MASK                         0x01

/* WCD9378_FU42_CH_VOL_CH1 Fields: */
#define WCD9378_FU42_CH_VOL_CH1_FU42_CH_VOL_CH1_MASK                     0xffff

/* WCD9378_FU42_CH_VOL_CH2 Fields: */
#define WCD9378_FU42_CH_VOL_CH2_FU42_CH_VOL_CH2_MASK                     0xffff

/* WCD9378_SU43_SELECTOR Fields: */
#define WCD9378_SU43_SELECTOR_SU43_SELECTOR_MASK                         0x01

/* WCD9378_SU45_SELECTOR Fields: */
#define WCD9378_SU45_SELECTOR_SU45_SELECTOR_MASK                         0x01

/* WCD9378_PDE47_REQ_PS Fields: */
#define WCD9378_PDE47_REQ_PS_PDE47_REQ_PS_MASK                           0xff

/* WCD9378_GE35_SEL_MODE Fields: */
#define WCD9378_GE35_SEL_MODE_GE35_SEL_MODE_MASK                         0xff

/* WCD9378_GE35_DET_MODE Fields: */
#define WCD9378_GE35_DET_MODE_GE35_DET_MODE_MASK                         0xff

/* WCD9378_IT31_MICB Fields: */
#define WCD9378_IT31_MICB_IT31_MICB_MASK                                 0xff

/* WCD9378_IT31_USAGE Fields: */
#define WCD9378_IT31_USAGE_IT31_USAGE_MASK                               0xff

/* WCD9378_PDE34_REQ_PS Fields: */
#define WCD9378_PDE34_REQ_PS_PDE34_REQ_PS_MASK                           0xff

/* WCD9378_SU45_TX_SELECTOR Fields: */
#define WCD9378_SU45_TX_SELECTOR_SU45_TX_SELECTOR_MASK                   0x01

/* WCD9378_XU36_BYP Fields: */
#define WCD9378_XU36_BYP_XU36_BYP_MASK                                   0x01

/* WCD9378_PDE36_REQ_PS Fields: */
#define WCD9378_PDE36_REQ_PS_PDE36_REQ_PS_MASK                           0xff

/* WCD9378_OT36_USAGE Fields: */
#define WCD9378_OT36_USAGE_OT36_USAGE_MASK                               0xff

/* WCD9378_SMP_JACK_FUNC_STAT Fields: */
#define WCD9378_SMP_JACK_FUNC_STAT_FUNC_STAT_MASK                        0xff

/* WCD9378_SMP_JACK_FUNC_ACT Fields: */
#define WCD9378_SMP_JACK_FUNC_ACT_FUNC_ACT_MASK                          0x01

/* WCD9378_PDE42_ACT_PS Fields: */
#define WCD9378_PDE42_ACT_PS_PDE42_ACT_PS_MASK                           0xff

/* WCD9378_PDE47_ACT_PS Fields: */
#define WCD9378_PDE47_ACT_PS_PDE47_ACT_PS_MASK                           0xff

/* WCD9378_PDE34_ACT_PS Fields: */
#define WCD9378_PDE34_ACT_PS_PDE34_ACT_PS_MASK                           0xff

/* WCD9378_PDE36_ACT_PS Fields: */
#define WCD9378_PDE36_ACT_PS_PDE36_ACT_PS_MASK                           0xff

/* WCD9378_SMP_JACK_DEV_MANU_ID_0 Fields: */
#define WCD9378_SMP_JACK_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK                0xff

/* WCD9378_SMP_JACK_DEV_MANU_ID_1 Fields: */
#define WCD9378_SMP_JACK_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK                0xff

/* WCD9378_SMP_JACK_DEV_PART_ID_0 Fields: */
#define WCD9378_SMP_JACK_DEV_PART_ID_0_DEV_PART_ID_0_MASK                0xff

/* WCD9378_SMP_JACK_DEV_PART_ID_1 Fields: */
#define WCD9378_SMP_JACK_DEV_PART_ID_1_DEV_PART_ID_1_MASK                0xff

/* WCD9378_SMP_JACK_DEV_VER Fields: */
#define WCD9378_SMP_JACK_DEV_VER_DEV_VER_MASK                            0xff


/* WCD9378_SMP_MIC_CTRL0_FUNC_EXT_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL0_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_FUNC_EXT_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL0_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_FUNC_EXT_VER Fields: */
#define WCD9378_SMP_MIC_CTRL0_FUNC_EXT_VER_FUNC_EXT_VER_MASK             0xff

/* WCD9378_IT11_MICB Fields: */
#define WCD9378_IT11_MICB_IT11_MICB_MASK                                 0xff

/* WCD9378_IT11_USAGE Fields: */
#define WCD9378_IT11_USAGE_IT11_USAGE_MASK                               0xff

/* WCD9378_PDE11_REQ_PS Fields: */
#define WCD9378_PDE11_REQ_PS_PDE11_REQ_PS_MASK                           0xff

/* WCD9378_OT10_USAGE Fields: */
#define WCD9378_OT10_USAGE_OT10_USAGE_MASK                               0xff

/* WCD9378_SMP_MIC_CTRL0_FUNC_STAT Fields: */
#define WCD9378_SMP_MIC_CTRL0_FUNC_STAT_FUNC_STAT_MASK                   0xff

/* WCD9378_SMP_MIC_CTRL0_FUNC_ACT Fields: */
#define WCD9378_SMP_MIC_CTRL0_FUNC_ACT_FUNC_ACT_MASK                     0x01

/* WCD9378_PDE11_ACT_PS Fields: */
#define WCD9378_PDE11_ACT_PS_PDE11_ACT_PS_MASK                           0xff

/* WCD9378_SMP_MIC_CTRL0_DEV_MANU_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL0_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_DEV_MANU_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL0_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_DEV_PART_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL0_DEV_PART_ID_0_DEV_PART_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_DEV_PART_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL0_DEV_PART_ID_1_DEV_PART_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL0_DEV_VER Fields: */
#define WCD9378_SMP_MIC_CTRL0_DEV_VER_DEV_VER_MASK                       0xff


/* WCD9378_SMP_MIC_CTRL1_FUNC_EXT_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL1_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_FUNC_EXT_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL1_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_FUNC_EXT_VER Fields: */
#define WCD9378_SMP_MIC_CTRL1_FUNC_EXT_VER_FUNC_EXT_VER_MASK             0xff

/* WCD9378_SMP_MIC_CTRL1_IT11_MICB Fields: */
#define WCD9378_SMP_MIC_CTRL1_IT11_MICB_IT11_MICB_MASK                   0xff

/* WCD9378_SMP_MIC_CTRL1_IT11_USAGE Fields: */
#define WCD9378_SMP_MIC_CTRL1_IT11_USAGE_IT11_USAGE_MASK                 0xff

/* WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS Fields: */
#define WCD9378_SMP_MIC_CTRL1_PDE11_REQ_PS_PDE11_REQ_PS_MASK             0xff

/* WCD9378_SMP_MIC_CTRL1_OT10_USAGE Fields: */
#define WCD9378_SMP_MIC_CTRL1_OT10_USAGE_OT10_USAGE_MASK                 0xff

/* WCD9378_SMP_MIC_CTRL1_FUNC_STAT Fields: */
#define WCD9378_SMP_MIC_CTRL1_FUNC_STAT_FUNC_STAT_MASK                   0xff

/* WCD9378_SMP_MIC_CTRL1_FUNC_ACT Fields: */
#define WCD9378_SMP_MIC_CTRL1_FUNC_ACT_FUNC_ACT_MASK                     0x01

/* WCD9378_SMP_MIC_CTRL1_PDE11_ACT_PS Fields: */
#define WCD9378_SMP_MIC_CTRL1_PDE11_ACT_PS_PDE11_ACT_PS_MASK             0xff

/* WCD9378_SMP_MIC_CTRL1_DEV_MANU_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL1_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_DEV_MANU_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL1_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_DEV_PART_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL1_DEV_PART_ID_0_DEV_PART_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_DEV_PART_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL1_DEV_PART_ID_1_DEV_PART_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL1_DEV_VER Fields: */
#define WCD9378_SMP_MIC_CTRL1_DEV_VER_DEV_VER_MASK                       0xff


/* WCD9378_SMP_MIC_CTRL2_FUNC_EXT_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL2_FUNC_EXT_ID_0_FUNC_EXT_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_FUNC_EXT_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL2_FUNC_EXT_ID_1_FUNC_EXT_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_FUNC_EXT_VER Fields: */
#define WCD9378_SMP_MIC_CTRL2_FUNC_EXT_VER_FUNC_EXT_VER_MASK             0xff

/* WCD9378_SMP_MIC_CTRL2_IT11_MICB Fields: */
#define WCD9378_SMP_MIC_CTRL2_IT11_MICB_IT11_MICB_MASK                   0xff

/* WCD9378_SMP_MIC_CTRL2_IT11_USAGE Fields: */
#define WCD9378_SMP_MIC_CTRL2_IT11_USAGE_IT11_USAGE_MASK                 0xff

/* WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS Fields: */
#define WCD9378_SMP_MIC_CTRL2_PDE11_REQ_PS_PDE11_REQ_PS_MASK             0xff

/* WCD9378_SMP_MIC_CTRL2_OT10_USAGE Fields: */
#define WCD9378_SMP_MIC_CTRL2_OT10_USAGE_OT10_USAGE_MASK                 0xff

/* WCD9378_SMP_MIC_CTRL2_FUNC_STAT Fields: */
#define WCD9378_SMP_MIC_CTRL2_FUNC_STAT_FUNC_STAT_MASK                   0xff

/* WCD9378_SMP_MIC_CTRL2_FUNC_ACT Fields: */
#define WCD9378_SMP_MIC_CTRL2_FUNC_ACT_FUNC_ACT_MASK                     0x01

/* WCD9378_SMP_MIC_CTRL2_PDE11_ACT_PS Fields: */
#define WCD9378_SMP_MIC_CTRL2_PDE11_ACT_PS_PDE11_ACT_PS_MASK             0xff

/* WCD9378_SMP_MIC_CTRL2_DEV_MANU_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL2_DEV_MANU_ID_0_DEV_MANU_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_DEV_MANU_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL2_DEV_MANU_ID_1_DEV_MANU_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_DEV_PART_ID_0 Fields: */
#define WCD9378_SMP_MIC_CTRL2_DEV_PART_ID_0_DEV_PART_ID_0_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_DEV_PART_ID_1 Fields: */
#define WCD9378_SMP_MIC_CTRL2_DEV_PART_ID_1_DEV_PART_ID_1_MASK           0xff

/* WCD9378_SMP_MIC_CTRL2_DEV_VER Fields: */
#define WCD9378_SMP_MIC_CTRL2_DEV_VER_DEV_VER_MASK                       0xff


/* WCD9378_REPORT_ID Fields: */
#define WCD9378_REPORT_ID_REPORT_ID_MASK                                 0xff

/* WCD9378_MESSAGE0 Fields: */
#define WCD9378_MESSAGE0_MESSAGE0_MASK                                   0xff

/* WCD9378_MESSAGE1 Fields: */
#define WCD9378_MESSAGE1_MESSAGE1_MASK                                   0xff

/* WCD9378_MESSAGE2 Fields: */
#define WCD9378_MESSAGE2_MESSAGE2_MASK                                   0xff


#endif /* WCD9378_REG_MASKS_H */

