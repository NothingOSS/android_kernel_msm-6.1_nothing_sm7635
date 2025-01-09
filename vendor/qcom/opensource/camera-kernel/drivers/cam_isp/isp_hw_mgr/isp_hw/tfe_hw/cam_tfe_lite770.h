/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */


#ifndef _CAM_TFE_LITE770_H_
#define _CAM_TFE_LITE770_H_
#include "cam_tfe_core.h"
#include "cam_tfe_bus.h"


/* throtle cfg register not used, diag sensor frame cnt status1 */
static struct cam_tfe_top_reg_offset_common  tfe_lite770_top_commong_reg  = {
	.hw_version                             = 0x00001800,
	.hw_capability                          = 0x00001804,
	.lens_feature                           = 0x00001808,
	.stats_feature                          = 0x0000180C,
	.zoom_feature                           = 0x00001810,
	.global_reset_cmd                       = 0x00001814,
	.core_cgc_ctrl_0                        = 0x00001818,
	.core_cgc_ctrl_1                        = 0x0000181C,
	.ahb_cgc_ctrl                           = 0x00001820,
	.core_cfg_0                             = 0x00001824,
	.reg_update_cmd                         = 0x0000182C,
	.diag_config                            = 0x00001860,
	.diag_sensor_status_0                   = 0x00001864,
	.diag_sensor_status_1                   = 0x00001868,
	.diag_sensor_frame_cnt_status           = 0x0000186C,
	.violation_status                       = 0x00001870,
	.stats_throttle_cnt_cfg_0               = 0x00001874,
	.stats_throttle_cnt_cfg_1               = 0x00001878,
	.num_debug_reg                          = 12,
	.debug_reg = {
		0x000018A0,
		0x000018A4,
		0x000018A8,
		0x000018AC,
		0x000018B0,
		0x000018B4,
		0x000018B8,
		0x000018BC,
		0x000018C0,
		0x000018C4,
		0x000018C8,
		0x000018CC,
	},
	.debug_cfg                              = 0x000018DC,
	.num_perf_cfg                           = 2,
	.perf_cfg = {
		{
			.perf_cnt_cfg           = 0x000018E0,
			.perf_pixel_count       = 0x000018E4,
			.perf_line_count        = 0x000018E8,
			.perf_stall_count       = 0x000018EC,
			.perf_always_count      = 0x000018F0,
			.perf_count_status      = 0x000018F4,
		},
		{
			.perf_cnt_cfg           = 0x000018F8,
			.perf_pixel_count       = 0x000018FC,
			.perf_line_count        = 0x00001900,
			.perf_stall_count       = 0x00001904,
			.perf_always_count      = 0x00001908,
			.perf_count_status      = 0x0000190C,
		},
	},
	.diag_sensor_frame_cnt_status_1         = 0x00001920,
	.diag_min_hbi_error_shift               = 15,
	.diag_neq_hbi_shift                     = 14,
	.diag_sensor_hbi_mask                   = 0x3FFF,
	.serializer_supported                   = true,
	.height_shift                           = 16,
	.epoch_shift_val                        = 16,
};

static struct cam_tfe_rdi_reg  tfe_lite770_rdi0_reg = {
	.rdi_hw_version              = 0x00001E00,
	.rdi_hw_status               = 0x00001E04,
	.rdi_module_config           = 0x00001E60,
	.rdi_skip_period             = 0x00001E68,
	.rdi_irq_subsample_pattern   = 0x00001E6C,
	.rdi_epoch_irq               = 0x00001E70,
	.rdi_debug_1                 = 0x00001FF0,
	.rdi_debug_0                 = 0x00001FF4,
	.rdi_test_bus_ctrl           = 0x00001FF8,
	.rdi_spare                   = 0x00001FFC,
	.reg_update_cmd              = 0x0000182C,
};

static struct cam_tfe_rdi_reg_data tfe_lite770_rdi0_reg_data = {
	.reg_update_cmd_data         = 0x2,
	.epoch_line_cfg              = 0x00140014,
	.pixel_pattern_shift         = 24,
	.pixel_pattern_mask          = 0x07000000,
	.rdi_out_enable_shift        = 0,

	.sof_irq_mask                = 0x00000010,
	.epoch0_irq_mask             = 0x00000040,
	.epoch1_irq_mask             = 0x00000080,
	.eof_irq_mask                = 0x00000020,
	.error_irq_mask0             = 0x00020200,
	.error_irq_mask2             = 0x00000004,
	.subscribe_irq_mask          = {
		0x00000000,
		0x00000030,
		0x00000000,
	},
	.enable_diagnostic_hw        = 0x1,
	.diag_sensor_sel             = 0x1,
	.diag_sensor_shift           = 0x1,
};

static struct cam_tfe_rdi_reg  tfe_lite770_rdi1_reg = {
	.rdi_hw_version              = 0x00002000,
	.rdi_hw_status               = 0x00002004,
	.rdi_module_config           = 0x00002060,
	.rdi_skip_period             = 0x00002068,
	.rdi_irq_subsample_pattern   = 0x0000206C,
	.rdi_epoch_irq               = 0x00002070,
	.rdi_debug_1                 = 0x000021F0,
	.rdi_debug_0                 = 0x000021F4,
	.rdi_test_bus_ctrl           = 0x000021F8,
	.rdi_spare                   = 0x000021FC,
	.reg_update_cmd              = 0x0000182C,
};

static struct cam_tfe_rdi_reg_data tfe_lite770_rdi1_reg_data = {
	.reg_update_cmd_data         = 0x4,
	.epoch_line_cfg              = 0x00140014,
	.pixel_pattern_shift         = 24,
	.pixel_pattern_mask          = 0x07000000,
	.rdi_out_enable_shift        = 0,

	.sof_irq_mask                = 0x00000100,
	.epoch0_irq_mask             = 0x00000400,
	.epoch1_irq_mask             = 0x00000800,
	.eof_irq_mask                = 0x00000200,
	.error_irq_mask0             = 0x00040400,
	.error_irq_mask2             = 0x00000008,
	.subscribe_irq_mask          = {
		0x00000000,
		0x00000300,
		0x00000000,
	},
	.enable_diagnostic_hw        = 0x1,
	.diag_sensor_sel             = 0x2,
	.diag_sensor_shift           = 0x1,
};

static struct cam_tfe_rdi_reg  tfe_lite770_rdi2_reg = {
	.rdi_hw_version              = 0x00002200,
	.rdi_hw_status               = 0x00002204,
	.rdi_module_config           = 0x00002260,
	.rdi_skip_period             = 0x00002268,
	.rdi_irq_subsample_pattern   = 0x0000226C,
	.rdi_epoch_irq               = 0x00002270,
	.rdi_debug_1                 = 0x000023F0,
	.rdi_debug_0                 = 0x000023F4,
	.rdi_test_bus_ctrl           = 0x000023F8,
	.rdi_spare                   = 0x000023FC,
	.reg_update_cmd              = 0x0000182C,
};

static struct cam_tfe_rdi_reg_data tfe_lite770_rdi2_reg_data = {
	.reg_update_cmd_data         = 0x8,
	.epoch_line_cfg              = 0x00140014,
	.pixel_pattern_shift         = 24,
	.pixel_pattern_mask          = 0x07000000,
	.rdi_out_enable_shift        = 0,

	.sof_irq_mask                = 0x00001000,
	.epoch0_irq_mask             = 0x00004000,
	.epoch1_irq_mask             = 0x00008000,
	.eof_irq_mask                = 0x00002000,
	.error_irq_mask0             = 0x00080800,
	.error_irq_mask2             = 0x00000004,
	.subscribe_irq_mask          = {
		0x00000000,
		0x00003000,
		0x00000000,
	},
	.enable_diagnostic_hw        = 0x1,
	.diag_sensor_sel             = 0x3,
	.diag_sensor_shift           = 0x1,

};

static struct cam_tfe_clc_hw_status  tfe_lite770_clc_hw_info[CAM_TFE_LITE_MAX_CLC] = {
	{
		.name = "CLC_RDI0_CAMIF",
		.hw_status_reg = 0x1E04,
	},
	{
		.name = "CLC_RDI1_CAMIF",
		.hw_status_reg = 0x2004,
	},
	{
		.name = "CLC_RDI2_CAMIF",
		.hw_status_reg = 0x2204,
	},
};

static struct  cam_tfe_top_hw_info tfe_lite770_top_hw_info = {
	.common_reg = &tfe_lite770_top_commong_reg,
	.rdi_hw_info  = {
		{
			.rdi_reg  = &tfe_lite770_rdi0_reg,
			.reg_data = &tfe_lite770_rdi0_reg_data,
		},
		{
			.rdi_reg  = &tfe_lite770_rdi1_reg,
			.reg_data = &tfe_lite770_rdi1_reg_data,
		},
		{
			.rdi_reg  = &tfe_lite770_rdi2_reg,
			.reg_data = &tfe_lite770_rdi2_reg_data,
		},
	},
	.in_port = {
		CAM_TFE_RDI_VER_1_0,
		CAM_TFE_RDI_VER_1_0,
		CAM_TFE_RDI_VER_1_0
	},
	.reg_dump_data  = {
		.num_reg_dump_entries    = 19,
		.num_lut_dump_entries    = 0,
		.bus_start_addr          = 0x2000,
		.bus_write_top_end_addr  = 0x2120,
		.bus_client_start_addr   = 0x2200,
		.bus_client_offset       = 0x100,
		.num_bus_clients         = 10,
		.reg_entry = {
			{
				.start_offset = 0x1000,
				.end_offset   = 0x10F4,
			},
			{
				.start_offset = 0x1260,
				.end_offset   = 0x1280,
			},
			{
				.start_offset = 0x13F0,
				.end_offset   = 0x13FC,
			},
			{
				.start_offset = 0x1460,
				.end_offset   = 0x1470,
			},
			{
				.start_offset = 0x15F0,
				.end_offset   = 0x15FC,
			},
			{
				.start_offset = 0x1660,
				.end_offset   = 0x1670,
			},
			{
				.start_offset = 0x17F0,
				.end_offset   = 0x17FC,
			},
			{
				.start_offset = 0x1860,
				.end_offset   = 0x1870,
			},
			{
				.start_offset = 0x19F0,
				.end_offset   = 0x19FC,
			},
			{
				.start_offset = 0x2660,
				.end_offset   = 0x2694,
			},
			{
				.start_offset = 0x2860,
				.end_offset   = 0x2884,
			},
			{
				.start_offset = 0x2A60,
				.end_offset   = 0X2B34,
			},
			{
				.start_offset = 0x2C60,
				.end_offset   = 0X2C80,
			},
			{
				.start_offset = 0x2E60,
				.end_offset   = 0X2E7C,
			},
			{
				.start_offset = 0x3060,
				.end_offset   = 0X3110,
			},
			{
				.start_offset = 0x3260,
				.end_offset   = 0X3278,
			},
			{
				.start_offset = 0x3460,
				.end_offset   = 0X3478,
			},
			{
				.start_offset = 0x3660,
				.end_offset   = 0X3684,
			},
			{
				.start_offset = 0x3860,
				.end_offset   = 0X3884,
			},
		},
		.lut_entry = {
			{
				.lut_word_size = 1,
				.lut_bank_sel  = 0x40,
				.lut_addr_size = 180,
				.dmi_reg_offset = 0x2800,
			},
			{
				.lut_word_size = 1,
				.lut_bank_sel  = 0x41,
				.lut_addr_size = 180,
				.dmi_reg_offset = 0x3000,
			},
		},
	},
};

static struct cam_tfe_bus_hw_info  tfe_lite770_bus_hw_info = {
	.common_reg = {
		.hw_version  = 0x00003000,
		.cgc_ovd     = 0x00003008,
		.comp_cfg_0  = 0x0000300C,
		.comp_cfg_1  = 0x00003010,
		.frameheader_cfg  = {
			0x00003034,
			0x00003038,
			0x0000303C,
			0x00003040,
		},
		.pwr_iso_cfg = 0x0000305C,
		.overflow_status_clear = 0x00003060,
		.ccif_violation_status = 0x00003064,
		.overflow_status       = 0x00003068,
		.image_size_violation_status = 0x00003070,
		.perf_count_cfg = {
			0x00003074,
			0x00003078,
			0x0000307C,
			0x00003080,
			0x00003084,
			0x00003088,
			0x0000308C,
			0x00003090,
		},
		.perf_count_val = {
			0x00003094,
			0x00003098,
			0x0000309C,
			0x000030A0,
			0x000030A4,
			0x000030A8,
			0x000030AC,
			0x000030B0,
		},
		.perf_count_status = 0x000030B4,
		.debug_status_top_cfg = 0x000030D4,
		.debug_status_top = 0x000030D8,
		.test_bus_ctrl = 0x000030DC,
		.irq_mask = {
			0x00003018,
			0x0000301C,
		},
		.irq_clear = {
			0x00003020,
			0x00003024,
		},
		.irq_status = {
			0x00003028,
			0x0000302C,
		},
		.irq_cmd = 0x00003030,
		.cons_violation_shift = 28,
		.violation_shift  = 30,
		.image_size_violation = 31,
	},
	.num_client = 3,
	.bus_client_reg = {
		{
			.cfg                   = 0x00003900,
			.image_addr            = 0x00003904,
			.frame_incr            = 0x00003908,
			.image_cfg_0           = 0x0000390C,
			.image_cfg_1           = 0x00003910,
			.image_cfg_2           = 0x00003914,
			.packer_cfg            = 0x00003918,
			.bw_limit              = 0x0000391C,
			.frame_header_addr     = 0x00003920,
			.frame_header_incr     = 0x00003924,
			.frame_header_cfg      = 0x00003928,
			.line_done_cfg         = 0x00000000,
			.irq_subsample_period  = 0x00003930,
			.irq_subsample_pattern = 0x00003934,
			.framedrop_period      = 0x00003938,
			.framedrop_pattern     = 0x0000393C,
			.system_cache_cfg      = 0x00003960,
			.addr_status_0         = 0x00003968,
			.addr_status_1         = 0x0000396C,
			.addr_status_2         = 0x00003970,
			.addr_status_3         = 0x00003974,
			.debug_status_cfg      = 0x00003978,
			.debug_status_0        = 0x0000397C,
			.debug_status_1        = 0x00003980,
			.comp_group            = CAM_TFE_BUS_COMP_GRP_5,
			.client_name           = "RDI0",
		},
		/* BUS Client 8 RDI1 */
		{
			.cfg                   = 0x00003A00,
			.image_addr            = 0x00003A04,
			.frame_incr            = 0x00003A08,
			.image_cfg_0           = 0x00003A0C,
			.image_cfg_1           = 0x00003A10,
			.image_cfg_2           = 0x00003A14,
			.packer_cfg            = 0x00003A18,
			.bw_limit              = 0x00003A1C,
			.frame_header_addr     = 0x00003A20,
			.frame_header_incr     = 0x00003A24,
			.frame_header_cfg      = 0x00003A28,
			.line_done_cfg         = 0x00000000,
			.irq_subsample_period  = 0x00003A30,
			.irq_subsample_pattern = 0x00003A34,
			.framedrop_period      = 0x00003A38,
			.framedrop_pattern     = 0x00003A3C,
			.system_cache_cfg      = 0x00003A60,
			.addr_status_0         = 0x00003A68,
			.addr_status_1         = 0x00003A6C,
			.addr_status_2         = 0x00003A70,
			.addr_status_3         = 0x00003A74,
			.debug_status_cfg      = 0x00003A78,
			.debug_status_0        = 0x00003A7C,
			.debug_status_1        = 0x00003A80,
			.comp_group            = CAM_TFE_BUS_COMP_GRP_6,
			.client_name           = "RDI1",
		},
		/* BUS Client 9 RDI2 */
		{
			.cfg                   = 0x00003B00,
			.image_addr            = 0x00003B04,
			.frame_incr            = 0x00003B08,
			.image_cfg_0           = 0x00003B0C,
			.image_cfg_1           = 0x00003B10,
			.image_cfg_2           = 0x00003B14,
			.packer_cfg            = 0x00003B18,
			.bw_limit              = 0x00003B1C,
			.frame_header_addr     = 0x00003B20,
			.frame_header_incr     = 0x00003B24,
			.frame_header_cfg      = 0x00003B28,
			.line_done_cfg         = 0x00000000,
			.irq_subsample_period  = 0x00003B30,
			.irq_subsample_pattern = 0x00003B34,
			.framedrop_period      = 0x00003B38,
			.framedrop_pattern     = 0x00003B3C,
			.system_cache_cfg      = 0x00003B60,
			.addr_status_0         = 0x00003B68,
			.addr_status_1         = 0x00003B6C,
			.addr_status_2         = 0x00003B70,
			.addr_status_3         = 0x00003B74,
			.debug_status_cfg      = 0x00003B78,
			.debug_status_0        = 0x00003B7C,
			.debug_status_1        = 0x00003B80,
			.comp_group            = CAM_TFE_BUS_COMP_GRP_7,
			.client_name           = "RDI2",
		},
	},
	.num_out  = 3,
	.tfe_out_hw_info = {
		{
			.tfe_out_id       = CAM_TFE_BUS_TFE_OUT_RDI0,
			.max_width        = -1,
			.max_height       = -1,
			.composite_group  = CAM_TFE_BUS_COMP_GRP_0,
			.rup_group_id     = CAM_TFE_BUS_RUP_GRP_1,
			.mid[0]              = 4,
		},
		{
			.tfe_out_id       = CAM_TFE_BUS_TFE_OUT_RDI1,
			.max_width        = -1,
			.max_height       = -1,
			.composite_group  = CAM_TFE_BUS_COMP_GRP_1,
			.rup_group_id     = CAM_TFE_BUS_RUP_GRP_2,
			.mid[0]              = 5,
		},
		{
			.tfe_out_id       = CAM_TFE_BUS_TFE_OUT_RDI2,
			.max_width        = -1,
			.max_height       = -1,
			.composite_group  = CAM_TFE_BUS_COMP_GRP_2,
			.rup_group_id     = CAM_TFE_BUS_RUP_GRP_3,
			.mid[0]              = 6,
		},
	},
	.num_comp_grp             = 3,
	.max_wm_per_comp_grp      = 3,
	.comp_done_shift          = 8,
	.top_bus_wr_irq_shift     = 1,
	.comp_buf_done_mask = 0xE000,
	.comp_rup_done_mask = 0xF,
	.bus_irq_error_mask = {
		0xD0000000,
		0x00000000,
	},
	.support_consumed_addr = true,
	.pdaf_rdi2_mux_en = false,
	.rdi_width = 128,
	.mode_cfg_shift = 16,
	.height_shift = 16,
};

struct cam_tfe_hw_info cam_tfe_lite770 = {
	.top_irq_mask = {
		0x00001834,
		0x00001838,
		0x0000183C,
	},
	.top_irq_clear = {
		0x00001840,
		0x00001844,
		0x00001848,
	},
	.top_irq_status = {
		0x0000184C,
		0x00001850,
		0x00001854,
	},
	.top_irq_cmd                       = 0x00001830,
	.global_clear_bitmask              = 0x00000001,
	.bus_irq_mask = {
		0x00003018,
		0x0000301C,
	},
	.bus_irq_clear = {
		0x00003020,
		0x00003024,
	},
	.bus_irq_status = {
		0x00003028,
		0x0000302C,
	},
	.bus_irq_cmd = 0x00003030,
	.bus_violation_reg = 0x00003064,
	.bus_overflow_reg = 0x00003068,
	.bus_image_size_vilation_reg = 0x3070,
	.bus_overflow_clear_cmd = 0x3060,
	.debug_status_top = 0x30D8,

	.reset_irq_mask = {
		0x00000001,
		0x00000000,
		0x00000000,
	},
	.error_irq_mask = {
		0x001F1F00,
		0x00000000,
		0x0000003F,
	},
	.non_fatal_error_irq_mask = {
		0x00200000,
		0x00000000,
		0x00000000,
	},
	.bus_reg_irq_mask = {
		0x00000002,
		0x00000000,
	},
	.bus_error_irq_mask = {
		0xC0000000,
		0x00000000,
	},

	.num_clc = 3,
	.clc_hw_status_info            = tfe_lite770_clc_hw_info,
	.bus_version                   = CAM_TFE_BUS_1_0,
	.bus_hw_info                   = &tfe_lite770_bus_hw_info,

	.top_version                   = CAM_TFE_TOP_1_0,
	.top_hw_info                   = &tfe_lite770_top_hw_info,
};

#endif /* _CAM_TFE_LITE770__H_ */
