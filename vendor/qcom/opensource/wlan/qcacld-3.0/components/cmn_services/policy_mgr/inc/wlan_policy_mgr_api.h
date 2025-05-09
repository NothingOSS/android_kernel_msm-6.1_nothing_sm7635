/*
 * Copyright (c) 2012-2021 The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __WLAN_POLICY_MGR_API_H
#define __WLAN_POLICY_MGR_API_H

/**
 * DOC: wlan_policy_mgr_api.h
 *
 * Concurrenct Connection Management entity
 */

/* Include files */
#include "qdf_types.h"
#include "qdf_status.h"
#include "wlan_objmgr_psoc_obj.h"
#include "wlan_mlo_mgr_public_structs.h"
#include "wlan_policy_mgr_public_struct.h"
#include "wlan_cm_roam_public_struct.h"
#include "wlan_utility.h"
#include "sir_types.h"

struct target_psoc_info;

typedef const enum policy_mgr_pcl_type
	pm_dbs_pcl_second_connection_table_type
	[PM_MAX_ONE_CONNECTION_MODE][PM_MAX_NUM_OF_MODE]
	[PM_MAX_CONC_PRIORITY_MODE];

typedef const enum policy_mgr_pcl_type
	pm_dbs_pcl_third_connection_table_type
	[PM_MAX_TWO_CONNECTION_MODE][PM_MAX_NUM_OF_MODE]
	[PM_MAX_CONC_PRIORITY_MODE];

typedef const enum policy_mgr_conc_next_action
	policy_mgr_next_action_two_connection_table_type
	[PM_MAX_ONE_CONNECTION_MODE][POLICY_MGR_MAX_BAND];

typedef const enum policy_mgr_conc_next_action
	policy_mgr_next_action_three_connection_table_type
	[PM_MAX_TWO_CONNECTION_MODE][POLICY_MGR_MAX_BAND];

#define PM_FW_MODE_STA_STA_BIT_POS       0
#define PM_FW_MODE_STA_P2P_BIT_POS       1

#define PM_FW_MODE_STA_STA_BIT_MASK      (0x1 << PM_FW_MODE_STA_STA_BIT_POS)
#define PM_FW_MODE_STA_P2P_BIT_MASK      (0x1 << PM_FW_MODE_STA_P2P_BIT_POS)

#define PM_CHANNEL_SELECT_LOGIC_STA_STA_GET(channel_select_logic_conc)  \
	((channel_select_logic_conc & PM_FW_MODE_STA_STA_BIT_MASK) >>   \
	 PM_FW_MODE_STA_STA_BIT_POS)
#define PM_CHANNEL_SELECT_LOGIC_STA_P2P_GET(channel_select_logic_conc)  \
	((channel_select_logic_conc & PM_FW_MODE_STA_P2P_BIT_MASK) >>   \
	 PM_FW_MODE_STA_P2P_BIT_POS)

/**
 * enum PM_AP_DFS_MASTER_MODE - AP dfs master mode
 * @PM_STA_SAP_ON_DFS_DEFAULT: Disallow STA+SAP SCC on DFS channel
 * @PM_STA_SAP_ON_DFS_MASTER_MODE_DISABLED: Allow STA+SAP SCC
 *        on DFS channel with master mode disabled
 * @PM_STA_SAP_ON_DFS_MASTER_MODE_FLEX: enhance
 *        "PM_STA_SAP_ON_DFS_MASTER_MODE_DISABLED" with below requirement:
 *	 a. Allow single SAP (GO) start on DFS channel.
 *	 b. Allow CAC process on DFS channel in single SAP (GO) mode
 *	 c. Allow DFS radar event process in single SAP (GO) mode
 *	 d. Disallow CAC and radar event process in SAP (GO) + STA mode.
 *
 * This enum value will be used to set to INI g_sta_sap_scc_on_dfs_chan to
 * config the sta+sap on dfs channel behaviour expected by user.
 */
enum PM_AP_DFS_MASTER_MODE {
	PM_STA_SAP_ON_DFS_DEFAULT,
	PM_STA_SAP_ON_DFS_MASTER_MODE_DISABLED,
	PM_STA_SAP_ON_DFS_MASTER_MODE_FLEX,
};

static inline const char *pcl_type_to_string(uint32_t idx)
{
	switch (idx) {
	CASE_RETURN_STRING(PM_NONE);
	CASE_RETURN_STRING(PM_24G);
	CASE_RETURN_STRING(PM_5G);
	CASE_RETURN_STRING(PM_SCC_CH);
	CASE_RETURN_STRING(PM_MCC_CH);
	CASE_RETURN_STRING(PM_SCC_CH_24G);
	CASE_RETURN_STRING(PM_SCC_CH_5G);
	CASE_RETURN_STRING(PM_24G_SCC_CH);
	CASE_RETURN_STRING(PM_5G_SCC_CH);
	CASE_RETURN_STRING(PM_SCC_ON_5_CH_5G);
	CASE_RETURN_STRING(PM_SCC_ON_5_SCC_ON_24_24G);
	CASE_RETURN_STRING(PM_SCC_ON_5_SCC_ON_24_5G);
	CASE_RETURN_STRING(PM_SCC_ON_5_5G_24G);
	CASE_RETURN_STRING(PM_SCC_ON_5_5G_SCC_ON_24G);
	CASE_RETURN_STRING(PM_SCC_ON_24_SCC_ON_5_24G);
	CASE_RETURN_STRING(PM_SCC_ON_24_SCC_ON_5_5G);
	CASE_RETURN_STRING(PM_SCC_ON_24_CH_24G);
	CASE_RETURN_STRING(PM_SCC_ON_5_SCC_ON_24);
	CASE_RETURN_STRING(PM_SCC_ON_24_SCC_ON_5);
	CASE_RETURN_STRING(PM_MCC_CH_24G);
	CASE_RETURN_STRING(PM_MCC_CH_5G);
	CASE_RETURN_STRING(PM_24G_MCC_CH);
	CASE_RETURN_STRING(PM_5G_MCC_CH);
	CASE_RETURN_STRING(PM_SBS_CH);
	CASE_RETURN_STRING(PM_SBS_CH_5G);
	CASE_RETURN_STRING(PM_24G_SCC_CH_SBS_CH);
	CASE_RETURN_STRING(PM_24G_SCC_CH_SBS_CH_5G);
	CASE_RETURN_STRING(PM_24G_SBS_CH_MCC_CH);
	/* New PCL type for DBS-SBS HW */
	CASE_RETURN_STRING(PM_SBS_CH_24G_SCC_CH);
	CASE_RETURN_STRING(PM_SBS_CH_SCC_CH_24G);
	CASE_RETURN_STRING(PM_SCC_CH_SBS_CH_24G);
	CASE_RETURN_STRING(PM_SBS_CH_SCC_CH_5G_24G);
	CASE_RETURN_STRING(PM_SCC_CH_MCC_CH_SBS_CH_24G);
	CASE_RETURN_STRING(PM_SBS_CH_2G);
	CASE_RETURN_STRING(PM_SCC_ON_5G_LOW_5G_LOW_PLUS_SHARED_2G);
	CASE_RETURN_STRING(PM_SCC_ON_5G_HIGH_5G_HIGH_PLUS_SHARED_2G);
	CASE_RETURN_STRING(PM_SCC_ON_5G_HIGH_5G_HIGH_SCC_ON_5G_LOW_PLUS_SHARED_2G);
	CASE_RETURN_STRING(PM_SBS_CH_MCC_CH);
	CASE_RETURN_STRING(PM_SBS_5G_MCC_24G);
	default:
		return "Unknown";
	}
}

static inline const char *device_mode_to_string(uint32_t idx)
{
	switch (idx) {
	CASE_RETURN_STRING(PM_STA_MODE);
	CASE_RETURN_STRING(PM_SAP_MODE);
	CASE_RETURN_STRING(PM_P2P_CLIENT_MODE);
	CASE_RETURN_STRING(PM_P2P_GO_MODE);
	CASE_RETURN_STRING(PM_NDI_MODE);
	CASE_RETURN_STRING(PM_NAN_DISC_MODE);
	CASE_RETURN_STRING(PM_LL_LT_SAP_MODE);
	default:
		return "Unknown";
	}
};

/**
 * struct trim_chan_info - trim channel info
 * @band_capability: band capability
 * @sap_count: sap count
 * @trim: enum trim channel list
 */
struct trim_chan_info {
	uint32_t band_capability;
	uint32_t sap_count;
	uint16_t trim;
};

/**
 * policy_mgr_get_allow_mcc_go_diff_bi() - to get information on whether GO
 *						can have diff BI than STA in MCC
 * @psoc: pointer to psoc
 * @allow_mcc_go_diff_bi: value to be filled
 *
 * This API is used to find out whether GO's BI can different than STA in MCC
 * scenario
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_allow_mcc_go_diff_bi(struct wlan_objmgr_psoc *psoc,
				    uint8_t *allow_mcc_go_diff_bi);
/**
 * policy_mgr_get_dual_mac_feature() - to find out if DUAL MAC feature is
 *				       enabled
 * @psoc: pointer to psoc
 * @dual_mac_feature: value to be filled
 *
 * This API is used to find out whether dual mac (dual radio) specific feature
 * is enabled or not
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_dual_mac_feature(struct wlan_objmgr_psoc *psoc,
					   uint8_t *dual_mac_feature);

/**
 * policy_mgr_allow_multiple_sta_connections() - to find out if STA+STA feature
 * is enabled.
 * @psoc: pointer to psoc
 *
 * This API is used to find out whether STA+STA specific feature is enabled
 * or not
 *
 * Return: true if supports else false.
 */
bool policy_mgr_allow_multiple_sta_connections(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_dual_mac_feature() - to set the dual mac feature value
 * @psoc: pointer to psoc
 * @dual_mac_feature: value to be updated
 *
 * This API is used to update the dual mac (dual radio) specific feature value
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_set_dual_mac_feature(struct wlan_objmgr_psoc *psoc,
					   uint8_t dual_mac_feature);

/**
 * policy_mgr_get_force_1x1() - to find out if 1x1 connection is enforced
 *
 * @psoc: pointer to psoc
 * @force_1x1: value to be filled
 *
 * This API is used to find out if 1x1 connection is enforced.
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_force_1x1(struct wlan_objmgr_psoc *psoc,
				    uint8_t *force_1x1);

/**
 * policy_mgr_get_max_conc_cxns() - to get configured max concurrent active
 * connection count
 *
 * @psoc: pointer to psoc
 *
 * This API is used to query the configured max concurrent active connection
 * count.
 *
 * Return: max active connection count
 */
uint32_t policy_mgr_get_max_conc_cxns(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_max_conc_cxns() - to set supported max concurrent active
 * connection count to policy mgr
 *
 * @psoc: pointer to psoc
 * @max_conc_cxns: max active connection count
 *
 * This API is used to update the max concurrent active connection
 * count to policy mgr
 *
 * Return: QDF_STATUS_SUCCESS if set successfully
 */
QDF_STATUS policy_mgr_set_max_conc_cxns(struct wlan_objmgr_psoc *psoc,
					uint32_t max_conc_cxns);

/**
 * policy_mgr_set_sta_sap_scc_on_dfs_chnl() - to set sta_sap_scc_on_dfs_chnl
 * @psoc: pointer to psoc
 * @sta_sap_scc_on_dfs_chnl: value to be set
 *
 * This API is used to set sta_sap_scc_on_dfs_chnl
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_set_sta_sap_scc_on_dfs_chnl(struct wlan_objmgr_psoc *psoc,
				       uint8_t sta_sap_scc_on_dfs_chnl);

/**
 * policy_mgr_get_sta_sap_scc_on_dfs_chnl() - to find out if STA and SAP
 *						   SCC is allowed on DFS channel
 * @psoc: pointer to psoc
 * @sta_sap_scc_on_dfs_chnl: value to be filled
 *
 * This API is used to find out whether STA and SAP SCC is allowed on
 * DFS channels
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_sta_sap_scc_on_dfs_chnl(struct wlan_objmgr_psoc *psoc,
				       uint8_t *sta_sap_scc_on_dfs_chnl);

/**
 * policy_mgr_set_multi_sap_allowed_on_same_band() - to set
 * multi_sap_allowed_on_same_band
 * @psoc: pointer to psoc
 * @multi_sap_allowed_on_same_band: value to be set
 *
 * This API is used to set multi_sap_allowed_on_same_band
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_set_multi_sap_allowed_on_same_band(struct wlan_objmgr_psoc *psoc,
				bool multi_sap_allowed_on_same_band);

/**
 * policy_mgr_get_sta_sap_scc_allowed_on_indoor_chnl() - Get if STA-SAP scc is
 * allowed on indoor channel
 * @psoc: Global psoc pointer
 *
 * Return: true if STA-SAP SCC on indoor channel is allowed
 */
bool policy_mgr_get_sta_sap_scc_allowed_on_indoor_chnl(
				struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_multi_sap_allowed_on_same_band() - to find out if multi sap
 * is allowed on same band
 * @psoc: pointer to psoc
 * @multi_sap_allowed_on_same_band: value to be filled
 *
 * This API is used to find out whether multi sap is allowed on same band
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_multi_sap_allowed_on_same_band(struct wlan_objmgr_psoc *psoc,
				bool *multi_sap_allowed_on_same_band);

/**
 * policy_mgr_set_original_bw_for_sap_restart() - to set use_sap_original_bw
 * @psoc: pointer to psoc
 * @use_sap_original_bw: value to be set
 *
 * This API is used to set use_sap_original_bw
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_set_original_bw_for_sap_restart(struct wlan_objmgr_psoc *psoc,
					   bool use_sap_original_bw);

/**
 * policy_mgr_get_original_bw_for_sap_restart() - to find out if sap original
 * bw is used as default BW when do sap restart
 * @psoc: pointer to psoc
 * @use_sap_original_bw: value to be filled
 *
 * This API is used to find out whether sap original BW is used as default BW
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_original_bw_for_sap_restart(struct wlan_objmgr_psoc *psoc,
					   bool *use_sap_original_bw);

/**
 * policy_mgr_get_dfs_sta_sap_go_scc_movement() - returns SAP / GO's movement
 * in STA+SAP DFS SCC concurrency to whether SAP / GO should be moved first
 * or not.
 * @psoc: pointer to psoc
 * @move_sap_go_first: value to be filled
 *
 * In STA+SAP DFS SCC concurrency, this API returns config on whether to move
 * SAP / GO first on getting CSA STA side or not.
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_dfs_sta_sap_go_scc_movement(struct wlan_objmgr_psoc *psoc,
					   bool *move_sap_go_first);

 /**
  * policy_mgr_nss_update_cb() - callback from SME confirming nss
  * update
  * @psoc: psoc handle
  * @tx_status: tx completion status for updated beacon with new
  *              nss value
  * @vdev_id: vdev id for the specific connection
  * @next_action: next action to happen at policy mgr after
  *              beacon update
  * @reason: Reason for nss update
  * @original_vdev_id: original request hwmode change vdev id
  * @request_id: request ID
  *
  * This function is the callback registered with SME at nss
  * update request time
  *
  * Return: None
  */

void policy_mgr_nss_update_cb(struct wlan_objmgr_psoc *psoc,
			      uint8_t tx_status, uint8_t vdev_id,
			      uint8_t next_action,
			      enum policy_mgr_conn_update_reason reason,
			      uint32_t original_vdev_id, uint32_t request_id);
/*
 * policy_mgr_get_connected_vdev_band_mask() - to get the connected vdev band
 * mask
 * @vdev: pointer to vdev
 *
 * This API is used to get band of the frequency.
 *
 * Return: band mask of the frequency associated with the vdev
 */
uint32_t policy_mgr_get_connected_vdev_band_mask(struct wlan_objmgr_vdev *vdev);

/**
 * policy_mgr_get_dfs_master_dynamic_enabled() - support dfs master or not
 * on AP interface when STA+SAP(GO) concurrency
 * @psoc: pointer to psoc
 * @vdev_id: sap vdev id
 *
 * This API is used to check AP dfs master functionality enabled or not when
 * STA+SAP(GO) concurrency.
 * If g_sta_sap_scc_on_dfs_chan is non-zero, the STA+SAP(GO) concurrency
 * is allowed on DFS channel SCC and the SAP's DFS master functionality
 * should be enable/disable according to:
 * 1. g_sta_sap_scc_on_dfs_chan is 0: function return true - dfs master
 *     capability enabled.
 * 2. g_sta_sap_scc_on_dfs_chan is 1: function return false - dfs master
 *     capability disabled.
 * 3. g_sta_sap_scc_on_dfs_chan is 2: dfs master capability based on STA on
 *     5G or not:
 *      a. 5G STA active - return false
 *      b. no 5G STA active -return true
 *
 * Return: true if dfs master functionality should be enabled.
 */
bool
policy_mgr_get_dfs_master_dynamic_enabled(struct wlan_objmgr_psoc *psoc,
					  uint8_t vdev_id);

/**
 * policy_mgr_get_can_skip_radar_event - Can skip DFS Radar event or not
 * @psoc: soc obj
 * @vdev_id: sap vdev id
 *
 * This API is used by dfs component to get decision whether to ignore
 * the radar event or not.
 *
 * Return: true if Radar event should be ignored.
 */
bool
policy_mgr_get_can_skip_radar_event(struct wlan_objmgr_psoc *psoc,
				    uint8_t vdev_id);

/**
 * policy_mgr_get_sta_sap_scc_lte_coex_chnl() - to find out if STA & SAP
 *						     SCC is allowed on LTE COEX
 * @psoc: pointer to psoc
 * @sta_sap_scc_lte_coex: value to be filled
 *
 * This API is used to find out whether STA and SAP scc is allowed on LTE COEX
 * channel
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_sta_sap_scc_lte_coex_chnl(struct wlan_objmgr_psoc *psoc,
					 uint8_t *sta_sap_scc_lte_coex);
/**
 * policy_mgr_get_sap_mandt_chnl() - to find out if SAP mandatory channel
 *					  support is enabled
 * @psoc: pointer to psoc
 * @sap_mandt_chnl: value to be filled
 *
 * This API is used to find out whether SAP's mandatory channel support
 * is enabled
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_sap_mandt_chnl(struct wlan_objmgr_psoc *psoc,
					 uint8_t *sap_mandt_chnl);
/**
 * policy_mgr_get_indoor_chnl_marking() - to get if indoor channel can be
 *						marked as disabled
 * @psoc: pointer to psoc
 * @indoor_chnl_marking: value to be filled
 *
 * This API is used to find out whether indoor channel can be marked as disabled
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS
policy_mgr_get_indoor_chnl_marking(struct wlan_objmgr_psoc *psoc,
				   uint8_t *indoor_chnl_marking);
/**
 * policy_mgr_get_mcc_scc_switch() - To mcc to scc switch setting from INI
 * @psoc: pointer to psoc
 * @mcc_scc_switch: value to be filled
 *
 * This API pulls mcc to scc switch setting which is given as part of INI and
 * stored in policy manager's CFGs.
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_mcc_scc_switch(struct wlan_objmgr_psoc *psoc,
					 uint8_t *mcc_scc_switch);
/**
 * policy_mgr_get_sys_pref() - to get system preference
 * @psoc: pointer to psoc
 * @sys_pref: value to be filled
 *
 * This API pulls the system preference for policy manager to provide
 * PCL
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_sys_pref(struct wlan_objmgr_psoc *psoc,
				   uint8_t *sys_pref);
/**
 * policy_mgr_set_sys_pref() - to set system preference
 * @psoc: pointer to psoc
 * @sys_pref: value to be applied as new INI setting
 *
 * This API is meant to override original INI setting for system pref
 * with new value which is used by policy manager to provide PCL
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_set_sys_pref(struct wlan_objmgr_psoc *psoc,
				   uint8_t sys_pref);

/**
 * policy_mgr_get_conc_rule1() - to find out if conc rule1 is enabled
 * @psoc: pointer to psoc
 * @conc_rule1: value to be filled
 *
 * This API is used to find out if conc rule-1 is enabled by user
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_conc_rule1(struct wlan_objmgr_psoc *psoc,
				     uint8_t *conc_rule1);
/**
 * policy_mgr_get_conc_rule2() - to find out if conc rule2 is enabled
 * @psoc: pointer to psoc
 * @conc_rule2: value to be filled
 *
 * This API is used to find out if conc rule-2 is enabled by user
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_conc_rule2(struct wlan_objmgr_psoc *psoc,
				     uint8_t *conc_rule2);

/**
 * policy_mgr_get_chnl_select_plcy() - to get channel selection policy
 * @psoc: pointer to psoc
 * @chnl_select_plcy: value to be filled
 *
 * This API is used to find out which channel selection policy has been
 * configured
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_chnl_select_plcy(struct wlan_objmgr_psoc *psoc,
					   uint32_t *chnl_select_plcy);

/**
 * policy_mgr_set_ch_select_plcy() - to set channel selection policy
 * @psoc: pointer to psoc
 * @ch_select_policy: value to be set
 *
 * This API is used to set the ch selection policy.
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_set_ch_select_plcy(struct wlan_objmgr_psoc *psoc,
					 uint32_t ch_select_policy);

/**
 * policy_mgr_get_dynamic_mcc_adaptive_sch() - to get dynamic mcc adaptive
 *                                             scheduler
 * @psoc: pointer to psoc
 * @dynamic_mcc_adaptive_sched: value to be filled
 *
 * This API is used to get dynamic mcc adaptive scheduler
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_dynamic_mcc_adaptive_sch(
				struct wlan_objmgr_psoc *psoc,
				bool *dynamic_mcc_adaptive_sched);

/**
 * policy_mgr_set_dynamic_mcc_adaptive_sch() - to set dynamic mcc adaptive
 *                                             scheduler
 * @psoc: pointer to psoc
 * @dynamic_mcc_adaptive_sched: value to be set
 *
 * This API is used to set dynamic mcc adaptive scheduler
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_set_dynamic_mcc_adaptive_sch(
				struct wlan_objmgr_psoc *psoc,
				bool dynamic_mcc_adaptive_sched);

/**
 * policy_mgr_get_mcc_adaptive_sch() - to get mcc adaptive scheduler
 * @psoc: pointer to psoc
 * @enable_mcc_adaptive_sch: value to be filled
 *
 * This API is used to find out if mcc adaptive scheduler enabled or disabled
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_mcc_adaptive_sch(struct wlan_objmgr_psoc *psoc,
					   bool *enable_mcc_adaptive_sch);

/**
 * policy_mgr_get_sta_cxn_5g_band() - to get STA's connection in 5G config
 *
 * @psoc: pointer to psoc
 * @enable_sta_cxn_5g_band: value to be filled
 *
 * This API is used to find out if STA connection in 5G band is allowed or
 * disallowed.
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
QDF_STATUS policy_mgr_get_sta_cxn_5g_band(struct wlan_objmgr_psoc *psoc,
					  uint8_t *enable_sta_cxn_5g_band);
/**
 * policy_mgr_set_concurrency_mode() - To set concurrency mode
 * @psoc: PSOC object data
 * @mode: device mode
 *
 * This routine is called to set the concurrency mode
 *
 * Return: NONE
 */
void policy_mgr_set_concurrency_mode(struct wlan_objmgr_psoc *psoc,
				     enum QDF_OPMODE mode);

/**
 * policy_mgr_clear_concurrency_mode() - To clear concurrency mode
 * @psoc: PSOC object data
 * @mode: device mode
 *
 * This routine is called to clear the concurrency mode
 *
 * Return: NONE
 */
void policy_mgr_clear_concurrency_mode(struct wlan_objmgr_psoc *psoc,
				       enum QDF_OPMODE mode);

/**
 * policy_mgr_get_connection_count() - provides the count of
 * current connections
 * @psoc: PSOC object information
 *
 * This function provides the count of current connections
 *
 * Return: connection count
 */
uint32_t policy_mgr_get_connection_count(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_connection_count_with_mlo() - provides the count of
 * current connections
 * @psoc: PSOC object information
 *
 * This function provides the count of current connections, MLD dev count
 * 1 connection no matter how many links connection.
 *
 * Return: connection count
 */
uint32_t
policy_mgr_get_connection_count_with_mlo(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_concurrency_mode() - return concurrency mode
 * @psoc: PSOC object information
 *
 * This routine is used to retrieve concurrency mode
 *
 * Return: uint32_t value of concurrency mask
 */
uint32_t policy_mgr_get_concurrency_mode(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_chnl_in_diff_band() - to check that given channel
 * is in diff band from existing channel or not
 * @psoc: pointer to psoc
 * @ch_freq: given channel frequency
 *
 * This API will check that if the passed channel is in diff band than the
 * already existing connections or not.
 *
 * Return: true if channel is in diff band
 */
bool policy_mgr_is_chnl_in_diff_band(struct wlan_objmgr_psoc *psoc,
				     uint32_t ch_freq);

/**
 * policy_mgr_is_pcl_weightage_required() - to check that PCL weightage req or
 * not
 * @psoc: pointer to psoc
 *
 * This API will check that whether PCL weightage need to consider in best
 * candidate selection or not. If some APs are in PCL list, those AP will get
 * additional weightage.
 *
 * Return: true if pcl weightage is not required
 */
bool policy_mgr_is_pcl_weightage_required(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_check_for_session_conc() - Check if concurrency is
 * allowed for a session
 * @psoc: PSOC object information
 * @vdev_id: Vdev ID
 * @ch_freq: Channel frequency
 *
 * Checks if connection is allowed for a given session_id
 *
 * True if the concurrency is allowed, false otherwise
 */
bool policy_mgr_check_for_session_conc(struct wlan_objmgr_psoc *psoc,
				       uint8_t vdev_id, uint32_t ch_freq);

/**
 * policy_mgr_handle_conc_multiport() - to handle multiport concurrency
 * @psoc: PSOC object information
 * @vdev_id: Vdev ID
 * @ch_freq: Channel frequency
 * @reason: reason for connection update
 * @request_id: Request id provided by the requester, can be used while
 * calling callback to the requester
 *
 * This routine will handle STA side concurrency when policy manager
 * is enabled.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_handle_conc_multiport(struct wlan_objmgr_psoc *psoc,
				 uint8_t vdev_id, uint32_t ch_freq,
				 enum policy_mgr_conn_update_reason reason,
				 uint32_t request_id);

#ifdef FEATURE_WLAN_MCC_TO_SCC_SWITCH
/**
 * policy_mgr_check_concurrent_intf_and_restart_sap() - Check
 * concurrent change intf
 * @psoc: PSOC object information
 * @is_acs_mode: Indicates whether SAP is started in ACS mode
 *
 * Checks the concurrent change interface and restarts SAP
 *
 * Return: None
 */
void policy_mgr_check_concurrent_intf_and_restart_sap(
		struct wlan_objmgr_psoc *psoc, bool is_acs_mode);
#else
void policy_mgr_check_concurrent_intf_and_restart_sap(
		struct wlan_objmgr_psoc *psoc, bool is_acs_mode)
{

}
#endif /* FEATURE_WLAN_MCC_TO_SCC_SWITCH */

/**
 * policy_mgr_is_chan_switch_in_progress() - Check if any SAP/GO
 * CSA is in progress or not
 * @psoc: PSOC object information
 *
 * Return: true if AP CSA is in progress
 */
bool policy_mgr_is_chan_switch_in_progress(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_wait_chan_switch_complete_evt() - Wait for SAP/GO CSA complete
 * event
 * @psoc: PSOC object information
 *
 * Return: QDF_STATUS_SUCCESS if CSA complete
 */
QDF_STATUS policy_mgr_wait_chan_switch_complete_evt(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_ap_start_in_progress() - Check if any SAP/GO
 * start is in progress or not
 * @psoc: PSOC object information
 *
 * Return: true if AP starting is in progress
 */
bool policy_mgr_is_ap_start_in_progress(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_conc_vdev_on_same_mac() - Function to get concurrent
 *                                          vdev on same mac
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @mac_id: mac id
 *
 * This function is used to get the concurrent vdev on same mac
 *
 * Return: vdev id of the concurrent interface running on same mac
 *
 */
uint32_t policy_mgr_get_conc_vdev_on_same_mac(struct wlan_objmgr_psoc *psoc,
					      uint32_t vdev_id, uint8_t mac_id);

#ifdef WLAN_FEATURE_SR
/**
 * policy_mgr_sr_same_mac_conc_enabled() - Function to check same MAC
 *					   concurrency support in Spatial Reuse
 * @psoc: PSOC object information
 *
 * This function is used to check whether concurrency is supported
 * on same mac or not with Spatial Reuse enabled.
 *
 * Return: True if same MAC concurrency is supported with Spatial Reuse
 *	   else False.
 */
bool policy_mgr_sr_same_mac_conc_enabled(struct wlan_objmgr_psoc *psoc);
#else
static inline
bool policy_mgr_sr_same_mac_conc_enabled(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

#endif

/**
 * policy_mgr_is_mcc_in_24G() - Function to check for MCC in 2.4GHz
 * @psoc: PSOC object information
 *
 * This function is used to check for MCC operation in 2.4GHz band.
 * STA, P2P and SAP adapters are only considered.
 *
 * Return: True if mcc is detected in 2.4 Ghz, false otherwise
 *
 */
bool policy_mgr_is_mcc_in_24G(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_change_mcc_go_beacon_interval() - Change MCC beacon interval
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @dev_mode: device mode
 *
 * Updates the beacon parameters of the GO in MCC scenario
 *
 * Return: Success or Failure depending on the overall function behavior
 */
QDF_STATUS policy_mgr_change_mcc_go_beacon_interval(
		struct wlan_objmgr_psoc *psoc,
		uint8_t vdev_id, enum QDF_OPMODE dev_mode);

#if defined(FEATURE_WLAN_MCC_TO_SCC_SWITCH)
/**
 * policy_mgr_check_bw_with_unsafe_chan_freq() - valid SAP channel bw against
 *                                               unsafe channel list
 * @psoc: PSOC object information
 * @center_freq: SAP channel center frequency
 * @ch_width: SAP channel width
 *
 * Return: true if no unsafe channel fall in SAP channel bandwidth range,
 *         false otherwise
 */
bool policy_mgr_check_bw_with_unsafe_chan_freq(struct wlan_objmgr_psoc *psoc,
					       qdf_freq_t center_freq,
					       enum phy_ch_width ch_width);
/**
 * policy_mgr_change_sap_channel_with_csa() - Move SAP channel using (E)CSA
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 * @ch_freq: Channel frequency to change
 * @ch_width: channel width to change
 * @forced: Force to switch channel, ignore SCC/MCC check
 *
 * Invoke the callback function to change SAP channel using (E)CSA
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
policy_mgr_change_sap_channel_with_csa(struct wlan_objmgr_psoc *psoc,
				       uint8_t vdev_id, uint32_t ch_freq,
				       uint32_t ch_width, bool forced);

#else
static inline QDF_STATUS
policy_mgr_change_sap_channel_with_csa(struct wlan_objmgr_psoc *psoc,
				       uint8_t vdev_id, uint32_t ch_freq,
				       uint32_t ch_width, bool forced)
{
	return QDF_STATUS_SUCCESS;
}
#endif

/**
 * policy_mgr_sta_sap_dfs_scc_conc_check() - validate and Move SAP channel
 * using (E)CSA
 *
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 * @csa_event: Pointer to CSA IE Received event data
 *
 * Invoke the function to change SAP channel using (E)CSA for STA+GO / SAP
 * SCC scenario only. This function will move P2P-GO / SAP first and then STA
 * will follow.
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
policy_mgr_sta_sap_dfs_scc_conc_check(struct wlan_objmgr_psoc *psoc,
				      uint8_t vdev_id,
				      struct csa_offload_params *csa_event);

/**
 * policy_mgr_sta_sap_dfs_enforce_scc() - validate and enforce SCC
 * using (E)CSA upon receiving 1st beacon
 *
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 *
 * Invoke the function to enforce SCC upon receiving 1st beacon. SAP / GO
 * movement will be triggered using (E)CSA for STA+GO / SAP DFS scenario only.
 * The pre-requisite for this function is SAP / GO shall already moved to new
 * channel by policy_mgr_sta_sap_dfs_scc_conc_check() function.
 *
 * Return: void
 */
void policy_mgr_sta_sap_dfs_enforce_scc(struct wlan_objmgr_psoc *psoc,
					uint8_t vdev_id);

#ifdef WLAN_FEATURE_P2P_P2P_STA
/**
 * policy_mgr_is_p2p_p2p_conc_supported() - p2p concurrency support
 * @psoc: pointer to psoc
 *
 * This API is used to check whether firmware supports p2p concurrency
 *
 * Return: QDF_STATUS_SUCCESS up on success and any other status for failure.
 */
bool
policy_mgr_is_p2p_p2p_conc_supported(struct wlan_objmgr_psoc *psoc);
#else
static inline bool
policy_mgr_is_p2p_p2p_conc_supported(struct wlan_objmgr_psoc *psoc)
{
	return false;
}
#endif

/**
 * policy_mgr_fetch_existing_con_info() - check if another vdev
 * is present and find mode, freq , vdev id and chan width
 *
 * @psoc: psoc object
 * @vdev_id: vdev id
 * @curr_go_freq: frequency
 * @mode: existing vdev mode
 * @con_freq: existing connection freq
 * @ch_width: ch_width of existing connection
 *
 * This function checks if another vdev is there and fetch connection
 * info for that vdev.This is mainly for force SCC implementation of GO+GO ,
 * GO+SAP or GO+STA where we fetch other existing GO, STA, SAP on the same
 * band with MCC.
 *
 * Return: vdev_id
 */
uint8_t
policy_mgr_fetch_existing_con_info(struct wlan_objmgr_psoc *psoc,
				   uint8_t vdev_id, uint32_t curr_go_freq,
				   enum policy_mgr_con_mode *mode,
				   uint32_t *con_freq,
				   enum phy_ch_width *ch_width);

#define GO_FORCE_SCC_DISABLE 0
#define GO_FORCE_SCC_STRICT 1
#define GO_FORCE_SCC_LIBERAL 2
/*
 * Stay in MCC for 1 second, in case of first p2p go channel
 * needs to be moved to curr go channel
 */
#define WAIT_BEFORE_GO_FORCESCC_RESTART (1000)

#ifdef WLAN_FEATURE_P2P_P2P_STA
/**
 * policy_mgr_is_go_scc_strict() - Get GO force SCC enabled or not
 * @psoc: psoc object
 *
 * This function checks if force SCC logic should be used on GO interface
 * as a strict mode.
 *
 * Return: True if p2p needs o be start on provided channel only.
 */
bool policy_mgr_is_go_scc_strict(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_process_forcescc_for_go () - start work queue to move first p2p go
 * to new p2p go's channel
 *
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 * @ch_freq: Channel frequency to change
 * @ch_width: channel width to change
 * @mode: existing vdev mode
 *
 * starts delayed work queue of 1 second to move first p2p go to new
 * p2p go's channel.
 *
 * Return: None
 */
void policy_mgr_process_forcescc_for_go(
		struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
		uint32_t ch_freq, uint32_t ch_width,
		enum policy_mgr_con_mode mode);

/**
 * policy_mgr_do_go_plus_go_force_scc() - First p2p go
 * to new p2p go's channel
 *
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 * @ch_freq: Channel frequency to change
 * @ch_width: channel width to change
 *
 * Move first p2p go to new
 * p2p go's channel.
 *
 * Return: None
 */
void policy_mgr_do_go_plus_go_force_scc(
		struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
		uint32_t ch_freq, uint32_t ch_width);
#else
static inline
bool policy_mgr_is_go_scc_strict(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline
void policy_mgr_process_forcescc_for_go(
		struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
		uint32_t ch_freq, uint32_t ch_width,
		enum policy_mgr_con_mode mode)
{}

static inline
void policy_mgr_do_go_plus_go_force_scc(
		struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
		uint32_t ch_freq, uint32_t ch_width)
{}
#endif

/**
 * policy_mgr_process_force_scc_for_nan () - force SAP scc on nan freq
 *
 * @psoc: PSOC object information
 *
 * Return: None
 */
void policy_mgr_process_force_scc_for_nan(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_check_sap_go_force_scc() - Check SAP GO MCC and save interface
 * information
 * @psoc: pointer to psoc
 * @vdev: initiator vdev
 * @reason_code: CSA reason code
 *
 * This API will check SAP and GO are coexistent on same band MCC or not. If
 * it is, the interface ids will be saved and a delayed workqueue will be
 * scheduled. The workqueue will handle the new channel selection and change
 * the channel of second interface to avoid MCC.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_check_sap_go_force_scc(struct wlan_objmgr_psoc *psoc,
				  struct wlan_objmgr_vdev *vdev,
				  enum sap_csa_reason_code reason_code);

/**
 * policy_mgr_set_pcl_for_existing_combo() - SET PCL for existing combo
 * @psoc: PSOC object information
 * @mode: Adapter mode
 * @vdev_id: Vdev Id
 *
 * Return: None
 */
void policy_mgr_set_pcl_for_existing_combo(struct wlan_objmgr_psoc *psoc,
					   enum policy_mgr_con_mode mode,
					   uint8_t vdev_id);

/**
 * policy_mgr_set_pcl_for_connected_vdev() - Set the PCL for connected vdevs
 * @psoc: PSOC object information
 * @vdev_id: Vdev Id
 * @clear_pcl: option to clear the PCL first before setting the new one
 *
 * This API will set the preferred channel list for other connected vdevs aside
 * from the calling function's vdev
 *
 * Context: Any kernel thread
 * Return: None
 */
void policy_mgr_set_pcl_for_connected_vdev(struct wlan_objmgr_psoc *psoc,
					   uint8_t vdev_id, bool clear_pcl);

/**
 * policy_mgr_set_pcl() - Set preferred channel list in the FW
 * @psoc: PSOC object information
 * @msg: message containing preferred channel list information
 * @vdev_id: Vdev Id
 * @clear_vdev_pcl: clear PCL flag
 *
 * Sends the set pcl command and PCL info to FW
 *
 * Context: Any kernel thread
 * Return: QDF_STATUS_SUCCESS on successful posting, fail status in any other
 *	   case
 */
QDF_STATUS policy_mgr_set_pcl(struct wlan_objmgr_psoc *psoc,
			      struct policy_mgr_pcl_list *msg,
			      uint8_t vdev_id,
			      bool clear_vdev_pcl);

/**
 * policy_mgr_incr_active_session() - increments the number of active sessions
 * @psoc: PSOC object information
 * @mode:	Adapter mode
 * @session_id: session ID for the connection session
 *
 * This function increments the number of active sessions maintained per device
 * mode. In the case of STA/P2P CLI/IBSS upon connection indication it is
 * incremented; In the case of SAP/P2P GO upon bss start it is incremented
 *
 * Return: None
 */
void policy_mgr_incr_active_session(struct wlan_objmgr_psoc *psoc,
		enum QDF_OPMODE mode, uint8_t session_id);

/**
 * policy_mgr_decr_active_session() - decrements the number of active sessions
 * @psoc: PSOC object information
 * @mode: Adapter mode
 * @session_id: session ID for the connection session
 *
 * This function decrements the number of active sessions maintained per device
 * mode. In the case of STA/P2P CLI/IBSS upon disconnection it is decremented
 * In the case of SAP/P2P GO upon bss stop it is decremented
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_decr_active_session(struct wlan_objmgr_psoc *psoc,
		enum QDF_OPMODE mode, uint8_t session_id);

/**
 * policy_mgr_decr_session_set_pcl() - Decrement session count and set PCL
 * @psoc: PSOC object information
 * @mode: Adapter mode
 * @session_id: Session id
 *
 * Decrements the active session count and sets the PCL if a STA connection
 * exists
 *
 * Return: None
 */
void policy_mgr_decr_session_set_pcl(struct wlan_objmgr_psoc *psoc,
		enum QDF_OPMODE mode, uint8_t session_id);

/**
 * polic_mgr_send_pcl_to_fw() - Send PCL to fw
 * @psoc: PSOC object information
 * @mode: Adapter mode
 *
 * Loop through all existing connections, stop RSO, send PCL to firmware
 * and start RSO. If Roaming is in progress on any of the interface,
 * avoid PCL updation as PCL gets updated post roaming.
 *
 * Return: None
 */
void
polic_mgr_send_pcl_to_fw(struct wlan_objmgr_psoc *psoc,
			 enum QDF_OPMODE mode);

#ifdef WLAN_FEATURE_11BE_MLO
/**
 * policy_mgr_mlo_sta_set_nlink() - Set link mode for MLO STA
 * by link id bitmap
 * @psoc: psoc object
 * @vdev_id: vdev id
 * @reason: reason to set
 * @mode: mode to set
 * @link_num: number of link, valid for mode:
 * MLO_LINK_FORCE_MODE_ACTIVE_NUM, MLO_LINK_FORCE_MODE_INACTIVE_NUM
 * @link_bitmap: link bitmap, valid for mode:
 * MLO_LINK_FORCE_MODE_ACTIVE, MLO_LINK_FORCE_MODE_INACTIVE,
 * MLO_LINK_FORCE_MODE_ACTIVE_NUM, MLO_LINK_FORCE_MODE_INACTIVE_NUM
 * MLO_LINK_FORCE_MODE_NO_FORCE.
 * @link_bitmap2: inactive link bitmap, only valid for mode
 * MLO_LINK_FORCE_MODE_ACTIVE_INACTIVE
 * @link_control_flags: bitmap of enum link_control_flags.
 *
 * Interface to set link mode for MLO STA
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_mlo_sta_set_nlink(struct wlan_objmgr_psoc *psoc,
			     uint8_t vdev_id,
			     enum mlo_link_force_reason reason,
			     enum mlo_link_force_mode mode,
			     uint8_t link_num,
			     uint16_t link_bitmap,
			     uint16_t link_bitmap2,
			     uint32_t link_control_flags);

/**
 * policy_mgr_mlo_sta_set_link() - Set link mode for MLO STA
 * @psoc: psoc object
 * @reason: reason to set
 * @mode: mode to set
 * @num_mlo_vdev: number of vdevs
 * @mlo_vdev_lst: vdev list
 *
 * Interface to set link mode for MLO STA
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_mlo_sta_set_link(struct wlan_objmgr_psoc *psoc,
			    enum mlo_link_force_reason reason,
			    enum mlo_link_force_mode mode,
			    uint8_t num_mlo_vdev, uint8_t *mlo_vdev_lst);

/**
 * policy_mgr_is_ml_vdev_id() - check if vdev id is part of ML
 * @psoc: PSOC object information
 * @vdev_id: vdev id to check
 *
 * Return: true if vdev is part of ml
 */
bool policy_mgr_is_ml_vdev_id(struct wlan_objmgr_psoc *psoc, uint8_t vdev_id);

/**
 * policy_mgr_get_disabled_ml_links_count() - provides the count of
 * disabled ml links
 * @psoc: PSOC object information
 *
 * This function provides the count of disabled ml links
 *
 * Return: disabled ml links count
 */
uint32_t policy_mgr_get_disabled_ml_links_count(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_move_vdev_from_disabled_to_connection_tbl() - re-enable a ml link
 * and move it from disabled link table to pm_conc_connection_list
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 *
 * Return: None
 */
void policy_mgr_move_vdev_from_disabled_to_connection_tbl(
						struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id);

/**
 * policy_mgr_move_vdev_from_connection_to_disabled_tbl() - Add/move the link
 * vdev to disable a ml link table
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 *
 * Return: None
 */
void policy_mgr_move_vdev_from_connection_to_disabled_tbl(
						struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id);

/**
 * policy_mgr_ml_link_vdev_need_to_be_disabled() - check if ml link need to be
 * disabled during connection.
 * @psoc: psoc
 * @vdev: vdev
 * @peer_assoc: check peer assoc command
 *
 * Check the vdev need to be moved to disabled policy mgr table.
 * If peer_assoc = false, the API will check the forced inactive link bitmap
 * as well. Vdev will be disabled if vdev's link id is forced inactive(includes
 * dynamic inactive)
 *
 * Return: true if STA link is need to be disabled else false.
 */
bool
policy_mgr_ml_link_vdev_need_to_be_disabled(struct wlan_objmgr_psoc *psoc,
					    struct wlan_objmgr_vdev *vdev,
					    bool peer_assoc);

/**
 * policy_mgr_is_set_link_in_progress() - Check set link in progress or not
 * @psoc: psoc pointer
 *
 * Return: true if set link in progress
 */
bool policy_mgr_is_set_link_in_progress(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_wait_for_set_link_update() - Wait for set/clear link response
 * @psoc: psoc pointer
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_wait_for_set_link_update(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_active_vdev_bitmap() - to get active ML STA vdev bitmap
 * @psoc: PSOC object information
 *
 * This API will fetch the active ML STA vdev bitmap.
 *
 * Return: vdev bitmap value
 */
uint32_t
policy_mgr_get_active_vdev_bitmap(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_emlsr_sta_concurrency_present() - Check whether eMLSR
 * concurrency is present or not.
 * @psoc: PSOC object information
 *
 * This API is to check if any other concurrency is present when an eMLSR
 * STA connection is about to complete(i.e. when first link is connected
 * and second link is coming up). This helps to let the eMLSR connection
 * happen but not let firmware enter into eMLSR hw mode by sending
 * mlo_force_link_inactive=1 in peer_assoc of link when other concurrency is
 * present.
 *
 * Host driver shall disable the one link post connection anyway if concurrency
 * is present. Once the concurrency is gone, policy_mgr shall evaluate and
 * re-enable links to let firmware go to eMLSR hw mode.
 *
 * Return: true is it's allow otherwise false
 */
bool policy_mgr_is_emlsr_sta_concurrency_present(struct wlan_objmgr_psoc *psoc);
#else
static inline bool
policy_mgr_is_ml_vdev_id(struct wlan_objmgr_psoc *psoc, uint8_t vdev_id)
{
	return false;
}

static inline uint32_t
policy_mgr_get_disabled_ml_links_count(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

static inline void
policy_mgr_move_vdev_from_disabled_to_connection_tbl(
						struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id) {}

static inline bool
policy_mgr_ml_link_vdev_need_to_be_disabled(struct wlan_objmgr_psoc *psoc,
					    struct wlan_objmgr_vdev *vdev,
					    bool peer_assoc)
{
	return false;
}

static inline void
policy_mgr_move_vdev_from_connection_to_disabled_tbl(
						struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id) {}

static inline bool
policy_mgr_is_set_link_in_progress(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline QDF_STATUS
policy_mgr_wait_for_set_link_update(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline uint32_t
policy_mgr_get_active_vdev_bitmap(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

static inline bool
policy_mgr_is_emlsr_sta_concurrency_present(struct wlan_objmgr_psoc *psoc)
{
	return false;
}
#endif

/**
 * policy_mgr_skip_dfs_ch() - skip dfs channel or not
 * @psoc: pointer to soc
 * @skip_dfs_channel: pointer to result
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_skip_dfs_ch(struct wlan_objmgr_psoc *psoc,
				  bool *skip_dfs_channel);

/**
 * policy_mgr_get_channel() - provide channel number of given mode and vdevid
 * @psoc: PSOC object information
 * @mode: given  mode
 * @vdev_id: pointer to vdev_id
 *
 * This API will provide channel frequency value of matching mode and vdevid.
 * If vdev_id is NULL then it will match only mode
 * If vdev_id is not NULL the it will match both mode and vdev_id
 *
 * Return: channel frequency value
 */
uint32_t policy_mgr_get_channel(struct wlan_objmgr_psoc *psoc,
				enum policy_mgr_con_mode mode,
				uint32_t *vdev_id);

/**
 * policy_mgr_get_pcl() - provides the preferred channel list for
 * new connection
 * @psoc: PSOC object information
 * @mode: Device mode
 * @pcl_channels: Preferred channel freq list
 * @len: length of the PCL
 * @pcl_weight: Weights of the PCL
 * @weight_len: Max length of the weights list
 * @vdev_id: Vdev id
 *
 * This function provides the preferred channel list on which
 * policy manager wants the new connection to come up. Various
 * connection decision making entities will using this function
 * to query the PCL info
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_pcl(struct wlan_objmgr_psoc *psoc,
			      enum policy_mgr_con_mode mode,
			      uint32_t *pcl_channels, uint32_t *len,
			      uint8_t *pcl_weight, uint32_t weight_len,
			      uint8_t vdev_id);

/**
 * policy_mgr_init_chan_avoidance() - init channel avoidance in policy manager.
 * @psoc: PSOC object information
 * @chan_freq_list: channel frequency list
 * @chan_cnt: channel count
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_init_chan_avoidance(struct wlan_objmgr_psoc *psoc,
					  qdf_freq_t *chan_freq_list,
					  uint16_t chan_cnt);

/**
 * policy_mgr_update_with_safe_channel_list() - provides the safe
 * channel list
 * @psoc: PSOC object information
 * @pcl_channels: channel freq list
 * @len: length of the list
 * @weight_list: Weights of the PCL
 * @weight_len: Max length of the weights list
 *
 * This function provides the safe channel list from the list
 * provided after consulting the channel avoidance list
 *
 * Return: None
 */
void policy_mgr_update_with_safe_channel_list(struct wlan_objmgr_psoc *psoc,
					      uint32_t *pcl_channels,
					      uint32_t *len,
					      uint8_t *weight_list,
					      uint32_t weight_len);

/**
 * policy_mgr_get_nondfs_preferred_channel() - to get non-dfs preferred channel
 *                                           for given mode
 * @psoc: PSOC object information
 * @mode: mode for which preferred non-dfs channel is requested
 * @for_existing_conn: flag to indicate if preferred channel is requested
 *                     for existing connection
 * @vdev_id: Vdev Id
 *
 * this routine will return non-dfs channel
 * 1) for getting non-dfs preferred channel, first we check if there are any
 *    other connection exist whose channel is non-dfs. if yes then return that
 *    channel so that we can accommodate upto 3 mode concurrency.
 * 2) if there no any other connection present then query concurrency module
 *    to give preferred channel list. once we get preferred channel list, loop
 *    through list to find first non-dfs channel from ascending order.
 *
 * Return: uint32_t non-dfs channel frequency
 */
uint32_t
policy_mgr_get_nondfs_preferred_channel(struct wlan_objmgr_psoc *psoc,
					enum policy_mgr_con_mode mode,
					bool for_existing_conn,
					uint8_t vdev_id);

/**
 * policy_mgr_is_any_nondfs_chnl_present() - Find any non-dfs
 * channel from conc table
 * @psoc: PSOC object information
 * @ch_freq: pointer to channel frequency which needs to be filled
 *
 * In-case if any connection is already present whose channel is none dfs then
 * return that channel
 *
 * Return: true up-on finding non-dfs channel else false
 */
bool policy_mgr_is_any_nondfs_chnl_present(struct wlan_objmgr_psoc *psoc,
					   uint32_t *ch_freq);

/**
 * policy_mgr_get_dfs_beaconing_session_id() - to find the
 * first DFS session id
 * @psoc: PSOC object information
 *
 * Return: If any beaconing session such as SAP or GO present and it is on
 * DFS channel then this function will return its session id
 *
 */
uint32_t policy_mgr_get_dfs_beaconing_session_id(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_any_dfs_beaconing_session_present() - to find
 * if any DFS session
 * @psoc: PSOC object information
 * @ch_freq: pointer to channel frequency that needs to filled
 * @ch_width: pointer to channel width for the beaconing session
 *
 * If any beaconing session such as SAP or GO present and it is on DFS channel
 * then this function will return true
 *
 * Return: true if session is on DFS or false if session is on non-dfs channel
 */
bool policy_mgr_is_any_dfs_beaconing_session_present(
		struct wlan_objmgr_psoc *psoc, qdf_freq_t *ch_freq,
		enum hw_mode_bandwidth *ch_width);

/**
 * policy_mgr_allow_concurrency() - Check for allowed concurrency
 * combination consulting the PCL
 * @psoc: PSOC object information
 * @mode:	new connection mode
 * @ch_freq: channel frequency on which new connection is coming up
 * @bw: Bandwidth requested by the connection (optional)
 * @ext_flags: extended flags for concurrency check (union conc_ext_flag)
 * @vdev_id: vdev id
 *
 * When a new connection is about to come up check if current
 * concurrency combination including the new connection is
 * allowed or not based on the HW capability
 *
 * Return: True/False based on concurrency support
 */
bool policy_mgr_allow_concurrency(struct wlan_objmgr_psoc *psoc,
				  enum policy_mgr_con_mode mode,
				  uint32_t ch_freq,
				  enum hw_mode_bandwidth bw,
				  uint32_t ext_flags, uint8_t vdev_id);

/**
 * policy_mgr_check_scc_channel() - Check if SAP/GO freq need to be updated
 * as per exiting concurrency
 * @psoc: PSOC object information
 * @intf_ch_freq: Channel frequency of existing concurrency
 * @sap_ch_freq: Given SAP/GO channel frequency
 * @vdev_id: Vdev id of the SAP/GO
 * @cc_mode: concurrent switch mode
 *
 * When SAP/GO is starting or re-starting, check SAP/GO freq need to be
 * aligned with the existing concurrencies. i.e. Forced to be on same freq as
 * exiting concurrency.
 *
 * Return: True/False
 */
void policy_mgr_check_scc_channel(struct wlan_objmgr_psoc *psoc,
				  qdf_freq_t *intf_ch_freq,
				  qdf_freq_t sap_ch_freq,
				  uint8_t vdev_id, uint8_t cc_mode);

/**
 * policy_mgr_handle_go_sap_fav_channel() - Get preferred force SCC
 * channel frequency using favorite mandatory channel list for GO+SAP
 * concurrency
 * @psoc: Pointer to Psoc
 * @vdev_id: vdev id
 * @sap_ch_freq: sap/go channel starting channel frequency
 * @intf_ch_freq: prefer force scc frequency
 *
 * SAP should move to 2.4 GHz if P2P GO is on 5G/6G. SAP should move to user
 * configured channel after P2P GO is stopped
 *
 * Return: QDF_STATUS_SUCCESS if a valid favorite SAP channel is found
 */
QDF_STATUS
policy_mgr_handle_go_sap_fav_channel(struct wlan_objmgr_psoc *psoc,
				     uint8_t vdev_id, qdf_freq_t sap_ch_freq,
				     qdf_freq_t *intf_ch_freq);

/**
 * policy_mgr_nan_sap_pre_enable_conc_check() - Check if NAN+SAP SCC is
 *                                              allowed in given ch
 * @psoc: PSOC object information
 * @mode: Connection mode
 * @ch_freq: channel frequency to check
 *
 * Return: True if allowed else false
 */
bool
policy_mgr_nan_sap_pre_enable_conc_check(struct wlan_objmgr_psoc *psoc,
					 enum policy_mgr_con_mode mode,
					 uint32_t ch_freq);

/**
 * policy_mgr_allow_concurrency_csa() - Check for allowed concurrency
 * combination when channel switch
 * @psoc:	PSOC object information
 * @mode:	connection mode
 * @ch_freq:	target channel frequency to switch
 * @bw:	target channel bandwidth
 * @vdev_id:	vdev id of channel switch interface
 * @forced:	forced to chan switch.
 * @reason:	request reason of CSA
 *
 * There is already existing SAP+GO combination but due to upper layer
 * notifying LTE-COEX event or sending command to move one of the connections
 * to different channel. In such cases before moving existing connection to new
 * channel, check if new channel can co-exist with the other existing
 * connection. For example, one SAP1 is on channel-6 and second SAP2 is on
 * channel-36 and lets say they are doing DBS, and lets say upper layer sends
 * LTE-COEX to move SAP1 from channel-6 to channel-149. In this case, SAP1 and
 * SAP2 will end up doing MCC which may not be desirable result. such cases
 * will be prevented with this API.
 *
 * Return: True/False
 */
bool
policy_mgr_allow_concurrency_csa(struct wlan_objmgr_psoc *psoc,
				 enum policy_mgr_con_mode mode,
				 uint32_t ch_freq, enum hw_mode_bandwidth bw,
				 uint32_t vdev_id, bool forced,
				 enum sap_csa_reason_code reason);

/**
 * policy_mgr_get_bw() - Convert phy_ch_width to hw_mode_bandwidth.
 * @chan_width: phy_ch_width
 *
 * Return: hw_mode_bandwidth
 */
enum hw_mode_bandwidth policy_mgr_get_bw(enum phy_ch_width chan_width);

/**
 * policy_mgr_get_first_connection_pcl_table_index() - provides the
 * row index to firstConnectionPclTable to get to the correct
 * pcl
 * @psoc: PSOC object information
 *
 * This function provides the row index to
 * firstConnectionPclTable. The index is the preference config.
 *
 * Return: table index
 */
enum policy_mgr_conc_priority_mode
	policy_mgr_get_first_connection_pcl_table_index(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_second_connection_pcl_table_index() - provides the
 * row index to secondConnectionPclTable to get to the correct
 * pcl
 * @psoc: PSOC object information
 *
 * This function provides the row index to
 * secondConnectionPclTable. The index is derived based on
 * current connection, band on which it is on & chain mask it is
 * using, as obtained from pm_conc_connection_list.
 *
 * Return: table index
 */
enum policy_mgr_one_connection_mode
	policy_mgr_get_second_connection_pcl_table_index(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_third_connection_pcl_table_index() - provides the
 * row index to thirdConnectionPclTable to get to the correct
 * pcl
 * @psoc: PSOC object information
 *
 * This function provides the row index to
 * thirdConnectionPclTable. The index is derived based on
 * current connection, band on which it is on & chain mask it is
 * using, as obtained from pm_conc_connection_list.
 *
 * Return: table index
 */
enum policy_mgr_two_connection_mode
	policy_mgr_get_third_connection_pcl_table_index(
		struct wlan_objmgr_psoc *psoc);

#ifdef FEATURE_FOURTH_CONNECTION
/**
 * policy_mgr_get_fourth_connection_pcl_table_index() - provides the
 * row index to fourthConnectionPclTable to get to the correct
 * pcl
 * @psoc: PSOC object information
 *
 * This function provides the row index to
 * fourthConnectionPclTable. The index is derived based on
 * current connection, band on which it is on & chain mask it is
 * using, as obtained from pm_conc_connection_list.
 *
 * Return: table index
 */
enum policy_mgr_three_connection_mode
	policy_mgr_get_fourth_connection_pcl_table_index(
		struct wlan_objmgr_psoc *psoc);
#endif

/**
 * policy_mgr_incr_connection_count() - adds the new connection to
 * the current connections list
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @mode: Operating mode
 *
 * This function adds the new connection to the current
 * connections list
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_incr_connection_count(struct wlan_objmgr_psoc *psoc,
					    uint32_t vdev_id,
					    enum QDF_OPMODE mode);

/**
 * policy_mgr_update_connection_info() - updates the existing
 * connection in the current connections list
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 *
 *
 * This function adds the new connection to the current
 * connections list
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_update_connection_info(struct wlan_objmgr_psoc *psoc,
		uint32_t vdev_id);

/**
 * policy_mgr_decr_connection_count() - remove the old connection
 * from the current connections list
 * @psoc: PSOC object information
 * @vdev_id: vdev id of the old connection
 *
 *
 * This function removes the old connection from the current
 * connections list
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_decr_connection_count(struct wlan_objmgr_psoc *psoc,
		uint32_t vdev_id);

/**
 * policy_mgr_current_connections_update() - initiates actions
 * needed on current connections once channel has been decided
 * for the new connection
 * @psoc: PSOC object information
 * @session_id: Session id
 * @ch_freq: Channel frequency on which new connection will be
 * @reason: Reason for which connection update is required
 * @request_id: Request id provided by the requester, can be used while
 * calling callback to the requester
 *
 * This function initiates initiates actions
 * needed on current connections once channel has been decided
 * for the new connection. Notifies UMAC & FW as well
 *
 * Return: QDF_STATUS enum
 */
QDF_STATUS
policy_mgr_current_connections_update(struct wlan_objmgr_psoc *psoc,
				      uint32_t session_id, uint32_t ch_freq,
				      enum policy_mgr_conn_update_reason reason,
				      uint32_t request_id);

/**
 * policy_mgr_change_hw_mode_sta_connect() - Change HW mode for STA connect
 * @psoc: psoc object
 * @scan_list: candidates for connection
 * @vdev_id: vdev id for STA/CLI
 * @connect_id: connect id of the connect request
 *
 * When a new connection is about to come up, change hw mode for STA/CLI
 * based upon the scan results and hw type.
 *
 * Return: status if set HW mode is fail or already taken care of.
 */
QDF_STATUS
policy_mgr_change_hw_mode_sta_connect(struct wlan_objmgr_psoc *psoc,
				      qdf_list_t *scan_list, uint8_t vdev_id,
				      uint32_t connect_id);

/**
 * policy_mgr_is_dbs_allowed_for_concurrency() - If dbs is allowed for current
 * concurreny
 * @psoc: PSOC object information
 * @new_conn_mode: new connection mode
 *
 * When a new connection is about to come up, check if dbs is allowed for
 * STA+STA or STA+P2P
 *
 * Return: true if dbs is allowed for STA+STA or STA+P2P else false
 */
bool policy_mgr_is_dbs_allowed_for_concurrency(
		struct wlan_objmgr_psoc *psoc, enum QDF_OPMODE new_conn_mode);

#ifndef WLAN_FEATURE_LL_LT_SAP
/**
 * policy_mgr_get_pcl_chlist_for_ll_sap() - Get pcl channel list for LL SAP
 * @psoc: PSOC object information
 * @len: length of the PCL
 * @pcl_channels: Preferred channel freq list
 * @pcl_weight: Weights of the PCL
 *
 * This function provides the preferred channel list on which the ll sap
 * can come.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_get_pcl_chlist_for_ll_sap(struct wlan_objmgr_psoc *psoc,
				     uint32_t *len, uint32_t *pcl_channels,
				     uint8_t *pcl_weight);

/**
 * policy_mgr_get_pcl_ch_for_sap_go_with_ll_sap_present() - Get pcl channel
 * list for SAP/GO when LL SAP is present
 * @psoc: PSOC object information
 * @len: length of the PCL
 * @pcl_channels: Preferred channel freq list
 * @pcl_weight: Weights of the PCL
 *
 * This function provides the preferred channel list for SAP/GO when LL SAP
 * is present
 *
 * Return: QDF_STATUS
 */

QDF_STATUS
policy_mgr_get_pcl_ch_for_sap_go_with_ll_sap_present(
					struct wlan_objmgr_psoc *psoc,
					uint32_t *len, uint32_t *pcl_channels,
					uint8_t *pcl_weight);

/**
 * policy_mgr_get_pcl_channel_for_ll_sap_concurrency() - Get pcl channel list
 * for LL SAP concurrency
 * @psoc: PSOC object information
 * @vdev_id: Vdev id
 * @pcl_channels: Preferred channel freq list
 * @pcl_weight: Weights of the PCL
 * @len: length of the PCL
 *
 * Return: QDF_STATUS
 */

QDF_STATUS
policy_mgr_get_pcl_channel_for_ll_sap_concurrency(
					struct wlan_objmgr_psoc *psoc,
					uint32_t vdev_id,
					uint32_t *pcl_channels,
					uint8_t *pcl_weight, uint32_t *len);
#else
static inline QDF_STATUS
policy_mgr_get_pcl_chlist_for_ll_sap(struct wlan_objmgr_psoc *psoc,
				     uint32_t *len, uint32_t *pcl_channels,
				     uint8_t *pcl_weight)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
policy_mgr_get_pcl_ch_for_sap_go_with_ll_sap_present(
					struct wlan_objmgr_psoc *psoc,
					uint32_t *len, uint32_t *pcl_channels,
					uint8_t *pcl_weight)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
policy_mgr_get_pcl_channel_for_ll_sap_concurrency(
					struct wlan_objmgr_psoc *psoc,
					uint32_t vdev_id,
					uint32_t *pcl_channels,
					uint8_t *pcl_weight, uint32_t *len)
{
	return QDF_STATUS_SUCCESS;
}
#endif

/**
 * policy_mgr_is_vdev_ll_sap() - Check whether given vdev is LL SAP or not
 * @psoc: psoc object
 * @vdev_id: vdev id
 *
 * Return: true if it's present otherwise false
 */
bool
policy_mgr_is_vdev_ll_sap(struct wlan_objmgr_psoc *psoc,
			  uint32_t vdev_id);

/**
 * policy_mgr_is_vdev_ll_ht_sap() - Check whether given vdev is HT LL SAP or not
 * @psoc: psoc object
 * @vdev_id: vdev id
 *
 * Based on vdev id ap profile set via vendor command is get and compared with
 * ll_ht_type AP type and is return true if profile set is throghput sensitive.
 *
 * Return: true if it's present otherwise false
 */
bool
policy_mgr_is_vdev_ll_ht_sap(struct wlan_objmgr_psoc *psoc,
			     uint32_t vdev_id);

/**
 * policy_mgr_is_vdev_ll_lt_sap() - Check whether given vdev is LL_LT_SAP or not
 * @psoc: psoc object
 * @vdev_id: vdev id
 *
 * Based on vdev id ap profile set via vendor command is get and compared with
 * lt_ll_type AP and is return true if profile set is gaming or losless audio
 * where latency matters.
 *
 * Return: true if it's present otherwise false
 */
bool
policy_mgr_is_vdev_ll_lt_sap(struct wlan_objmgr_psoc *psoc,
			     uint32_t vdev_id);

/**
 * policy_mgr_get_preferred_dbs_action_table() - get dbs action table type
 * @psoc: Pointer to psoc
 * @vdev_id: vdev Id
 * @ch_freq: channel frequency of vdev.
 * @reason: reason of request
 *
 * 1. Based on band preferred and vdev priority setting to choose the preferred
 * dbs action.
 * 2. This routine will be used to get DBS switching action tables.
 * In Genoa, two action tables for DBS1 (2x2 5G + 1x1 2G), DBS2
 *  (2x2 2G + 1x1 5G).
 * 3. It can be used in mode change case in CSA channel switching or Roaming,
 * opportunistic upgrade. If needs switch to DBS, we needs to query this
 * function to get preferred DBS mode.
 * 4. This is mainly used for dual dbs mode HW. For Legacy HW, there is
 * only single DBS mode. This function will return PM_NOP.
 *
 * return : PM_NOP, PM_DBS1, PM_DBS2
 */
enum policy_mgr_conc_next_action
policy_mgr_get_preferred_dbs_action_table(
	struct wlan_objmgr_psoc *psoc,
	uint32_t vdev_id,
	uint32_t ch_freq,
	enum policy_mgr_conn_update_reason reason);

/**
 * policy_mgr_get_conn_info() - get the current connections list
 * @len: length of the list
 *
 * This function returns a pointer to the current connections
 * list
 *
 * Return: pointer to connection list
 */
struct policy_mgr_conc_connection_info *policy_mgr_get_conn_info(
		uint32_t *len);

/**
 * policy_mgr_qdf_opmode_to_pm_con_mode() - provides the
 * type translation from QDF to policy manager type
 * @psoc: psoc
 * @device_mode: Generic connection mode type
 * @vdev_id: Vdev id
 *
 *
 * This function provides the type translation
 *
 * Return: policy_mgr_con_mode enum
 */
enum policy_mgr_con_mode
policy_mgr_qdf_opmode_to_pm_con_mode(struct wlan_objmgr_psoc *psoc,
				     enum QDF_OPMODE device_mode,
				     uint8_t vdev_id);

/**
 * policy_mgr_get_qdf_mode_from_pm - provides the
 * type translation from policy manager type
 * to generic connection mode type
 * @device_mode: policy manager mode type
 *
 *
 * This function provides the type translation
 *
 * Return: QDF_OPMODE enum
 */
enum QDF_OPMODE policy_mgr_get_qdf_mode_from_pm(
			enum policy_mgr_con_mode device_mode);

/**
 * policy_mgr_check_n_start_opportunistic_timer - check single mac upgrade
 * needed or not, if needed start the oppurtunistic timer.
 * @psoc: pointer to SOC
 *
 * This function starts the oppurtunistic timer if hw_mode change is needed
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_check_n_start_opportunistic_timer(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_pdev_set_hw_mode() - Set HW mode command to FW
 * @psoc: PSOC object information
 * @session_id: Session ID
 * @mac0_ss: MAC0 spatial stream configuration
 * @mac0_bw: MAC0 bandwidth configuration
 * @mac1_ss: MAC1 spatial stream configuration
 * @mac1_bw: MAC1 bandwidth configuration
 * @mac0_band_cap: mac0 band capability requirement
 *     (0: Don't care, 1: 2.4G, 2: 5G)
 * @dbs: HW DBS capability
 * @dfs: HW Agile DFS capability
 * @sbs: HW SBS capability
 * @reason: Reason for connection update
 * @next_action: next action to happen at policy mgr after
 *		HW mode change
 * @action: action to be applied before hw mode change
 *
 * @request_id: Request id provided by the requester, can be used while
 * calling callback to the requester
 *
 * Sends the set hw mode request to FW
 *
 * e.g.: To configure 2x2_80
 *       mac0_ss = HW_MODE_SS_2x2, mac0_bw = HW_MODE_80_MHZ
 *       mac1_ss = HW_MODE_SS_0x0, mac1_bw = HW_MODE_BW_NONE
 *       mac0_band_cap = HW_MODE_MAC_BAND_NONE,
 *       dbs = HW_MODE_DBS_NONE, dfs = HW_MODE_AGILE_DFS_NONE,
 *       sbs = HW_MODE_SBS_NONE
 * e.g.: To configure 1x1_80_1x1_40 (DBS)
 *       mac0_ss = HW_MODE_SS_1x1, mac0_bw = HW_MODE_80_MHZ
 *       mac1_ss = HW_MODE_SS_1x1, mac1_bw = HW_MODE_40_MHZ
 *       mac0_band_cap = HW_MODE_MAC_BAND_NONE,
 *       dbs = HW_MODE_DBS, dfs = HW_MODE_AGILE_DFS_NONE,
 *       sbs = HW_MODE_SBS_NONE
 * e.g.: To configure 1x1_80_1x1_40 (Agile DFS)
 *       mac0_ss = HW_MODE_SS_1x1, mac0_bw = HW_MODE_80_MHZ
 *       mac1_ss = HW_MODE_SS_1x1, mac1_bw = HW_MODE_40_MHZ
 *       mac0_band_cap = HW_MODE_MAC_BAND_NONE,
 *       dbs = HW_MODE_DBS, dfs = HW_MODE_AGILE_DFS,
 *       sbs = HW_MODE_SBS_NONE
 * e.g.: To configure 2x2_5g_80+1x1_2g_40
 *       mac0_ss = HW_MODE_SS_2x2, mac0_bw = HW_MODE_80_MHZ
 *       mac1_ss = HW_MODE_SS_1x1, mac1_bw = HW_MODE_40_MHZ
 *       mac0_band_cap = HW_MODE_MAC_BAND_5G
 *       dbs = HW_MODE_DBS, dfs = HW_MODE_AGILE_DFS_NONE,
 *       sbs = HW_MODE_SBS_NONE
 * e.g.: To configure 2x2_2g_40+1x1_5g_40
 *       mac0_ss = HW_MODE_SS_2x2, mac0_bw = HW_MODE_40_MHZ
 *       mac1_ss = HW_MODE_SS_1x1, mac1_bw = HW_MODE_40_MHZ
 *       mac0_band_cap = HW_MODE_MAC_BAND_2G
 *       dbs = HW_MODE_DBS, dfs = HW_MODE_AGILE_DFS_NONE,
 *       sbs = HW_MODE_SBS_NONE
 *
 * Return: Success if the message made it down to the next layer
 */
QDF_STATUS policy_mgr_pdev_set_hw_mode(struct wlan_objmgr_psoc *psoc,
		uint32_t session_id,
		enum hw_mode_ss_config mac0_ss,
		enum hw_mode_bandwidth mac0_bw,
		enum hw_mode_ss_config mac1_ss,
		enum hw_mode_bandwidth mac1_bw,
		enum hw_mode_mac_band_cap mac0_band_cap,
		enum hw_mode_dbs_capab dbs,
		enum hw_mode_agile_dfs_capab dfs,
		enum hw_mode_sbs_capab sbs,
		enum policy_mgr_conn_update_reason reason,
		uint8_t next_action, enum policy_mgr_conc_next_action action,
		uint32_t request_id);

/**
 * typedef policy_mgr_pdev_set_hw_mode_cback() - callback invoked by
 * other component to provide set HW mode request status
 * @status: status of the request
 * @cfgd_hw_mode_index: new HW mode index
 * @num_vdev_mac_entries: Number of mac entries
 * @vdev_mac_map: The table of vdev to mac mapping
 * @next_action: next action to happen at policy mgr after
 *		beacon update
 * @reason: Reason for set HW mode
 * @session_id: vdev id on which the request was made
 * @context: PSOC object information
 * @request_id: Request id provided by the requester, can be used while
 * calling callback to the requester
 *
 * This function is the callback registered with SME at set HW
 * mode request time
 *
 * Return: None
 */
typedef void (*policy_mgr_pdev_set_hw_mode_cback)(uint32_t status,
				uint32_t cfgd_hw_mode_index,
				uint32_t num_vdev_mac_entries,
				struct policy_mgr_vdev_mac_map *vdev_mac_map,
				uint8_t next_action,
				enum policy_mgr_conn_update_reason reason,
				uint32_t session_id, void *context,
				uint32_t request_id);

/**
 * typedef policy_mgr_nss_update_cback() - callback invoked by other
 * component to provide nss update request status
 * @psoc: PSOC object information
 * @tx_status: tx completion status for updated beacon with new
 *		nss value
 * @vdev_id: vdev id for the specific connection
 * @next_action: next action to happen at policy mgr after
 *		beacon update
 * @reason: Reason for nss update
 * @original_vdev_id: original request hwmode change vdev id
 * @request_id: cm req id
 *
 * This function is the callback registered with SME at nss
 * update request time
 *
 * Return: None
 */
typedef void (*policy_mgr_nss_update_cback)(struct wlan_objmgr_psoc *psoc,
		uint8_t tx_status,
		uint8_t vdev_id,
		uint8_t next_action,
		enum policy_mgr_conn_update_reason reason,
		uint32_t original_vdev_id, uint32_t request_id);

/**
 * struct policy_mgr_sme_cbacks - SME Callbacks to be invoked
 * from policy manager
 * @sme_get_nss_for_vdev: Get the allowed nss value for the vdev
 * @sme_soc_set_dual_mac_config: Set the dual MAC scan & FW
 *                             config
 * @sme_pdev_set_hw_mode: Set the new HW mode to FW
 * @sme_nss_update_request: Update NSS value to FW
 * @sme_change_mcc_beacon_interval: Set MCC beacon interval to FW
 * @sme_rso_start_cb: Enable roaming offload callback
 * @sme_rso_stop_cb: Disable roaming offload callback
 * @sme_change_sap_csa_count: Change CSA count for SAP/GO, only one
 *			      time, needs to set again if used once.
 * @sme_sap_update_ch_width: Update sap ch_width to fw to handle SAP 320MHz
 *                           concurrencies
 */
struct policy_mgr_sme_cbacks {
	void (*sme_get_nss_for_vdev)(enum QDF_OPMODE,
				     uint8_t *nss_2g, uint8_t *nss_5g);
	QDF_STATUS (*sme_soc_set_dual_mac_config)(
		struct policy_mgr_dual_mac_config msg);
	QDF_STATUS (*sme_pdev_set_hw_mode)(struct policy_mgr_hw_mode msg);
	QDF_STATUS (*sme_nss_update_request)(uint32_t vdev_id,
		uint8_t new_nss, uint8_t ch_width,
		policy_mgr_nss_update_cback cback,
		uint8_t next_action, struct wlan_objmgr_psoc *psoc,
		enum policy_mgr_conn_update_reason reason,
		uint32_t original_vdev_id, uint32_t request_id);
	QDF_STATUS (*sme_change_mcc_beacon_interval)(uint8_t session_id);
	QDF_STATUS (*sme_rso_start_cb)(
		mac_handle_t mac_handle, uint8_t vdev_id,
		uint8_t reason, enum wlan_cm_rso_control_requestor requestor);
	QDF_STATUS (*sme_rso_stop_cb)(
		mac_handle_t mac_handle, uint8_t vdev_id,
		uint8_t reason, enum wlan_cm_rso_control_requestor requestor);
	QDF_STATUS (*sme_change_sap_csa_count)(uint8_t count);
	QDF_STATUS (*sme_sap_update_ch_width)(struct wlan_objmgr_psoc *psoc,
			uint8_t vdev_id,
			enum phy_ch_width ch_width,
			enum policy_mgr_conn_update_reason reason,
			uint8_t conc_vdev_id, uint32_t request_id);
};

/**
 * struct policy_mgr_hdd_cbacks - HDD Callbacks to be invoked
 * from policy manager
 * @sap_restart_chan_switch_cb: Restart SAP
 * @wlan_hdd_get_channel_for_sap_restart: Get channel to restart
 *                      SAP
 * @get_mode_for_non_connected_vdev: Get the mode for a non
 *                                 connected vdev
 * @hdd_get_device_mode: Get QDF_OPMODE type for session id (vdev id)
 * @hdd_is_chan_switch_in_progress: Check if in any adapter channel switch is in
 * progress
 * @hdd_is_cac_in_progress: Check if in any adapter CAC is in progress
 * @wlan_hdd_set_sap_csa_reason: Set the sap csa reason in cases like NAN.
 * @hdd_get_ap_6ghz_capable: get ap vdev 6ghz capable info from hdd ap adapter.
 * @wlan_hdd_indicate_active_ndp_cnt: indicate active ndp cnt to hdd
 * @wlan_get_ap_prefer_conc_ch_params: get prefer ap channel bw parameters
 *  based on target channel frequency and concurrent connections.
 * @wlan_get_sap_acs_band: get acs band from sap config
 * @wlan_check_cc_intf_cb: get interference frequency of input SAP/GO interface
 * @wlan_set_tx_rx_nss_cb: set NSS dynamically for STA
 */
struct policy_mgr_hdd_cbacks {
	QDF_STATUS (*sap_restart_chan_switch_cb)(struct wlan_objmgr_psoc *psoc,
						 uint8_t vdev_id,
						 uint32_t ch_freq,
						 uint32_t channel_bw,
						 bool forced);
	QDF_STATUS (*wlan_hdd_get_channel_for_sap_restart)(
				struct wlan_objmgr_psoc *psoc,
				uint8_t vdev_id, uint32_t *ch_freq);
	enum policy_mgr_con_mode (*get_mode_for_non_connected_vdev)(
				struct wlan_objmgr_psoc *psoc,
				uint8_t vdev_id);
	enum QDF_OPMODE (*hdd_get_device_mode)(uint32_t session_id);
	bool (*hdd_is_chan_switch_in_progress)(void);
	bool (*hdd_is_cac_in_progress)(void);
	void (*wlan_hdd_set_sap_csa_reason)(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id, uint8_t reason);
	uint32_t (*hdd_get_ap_6ghz_capable)(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id);
	void (*wlan_hdd_indicate_active_ndp_cnt)(struct wlan_objmgr_psoc *psoc,
						 uint8_t vdev_id, uint8_t cnt);
	QDF_STATUS (*wlan_get_ap_prefer_conc_ch_params)(
			struct wlan_objmgr_psoc *psoc,
			uint8_t vdev_id, uint32_t chan_freq,
			struct ch_params *ch_params);
	uint32_t (*wlan_get_sap_acs_band)(struct wlan_objmgr_psoc *psoc,
					  uint8_t vdev_id, uint32_t *acs_band);
	QDF_STATUS (*wlan_check_cc_intf_cb)(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id,
					    uint32_t *ch_freq);
	QDF_STATUS (*wlan_set_tx_rx_nss_cb)(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id, uint8_t tx_nss,
					    uint8_t rx_nss);
};

/**
 * struct policy_mgr_conc_cbacks - lim Callbacks to be invoked
 * from policy manager
 * @connection_info_update: check and update params based on STA/SAP
 *                          concurrency.such as EDCA params and RTS threshold.
 *                          If updated, it will also send the updated parameters
 *                          to FW.
 */

struct policy_mgr_conc_cbacks {
	void (*connection_info_update)(void);
};

/**
 * struct policy_mgr_tdls_cbacks - TDLS Callbacks to be invoked
 * from policy manager
 * @tdls_notify_increment_session:
 * @tdls_notify_decrement_session:
 */
struct policy_mgr_tdls_cbacks {
	void (*tdls_notify_increment_session)(struct wlan_objmgr_psoc *psoc);
	void (*tdls_notify_decrement_session)(struct wlan_objmgr_psoc *psoc);
};

/**
 * struct policy_mgr_cdp_cbacks - CDP Callbacks to be invoked
 * from policy manager
 * @cdp_update_mac_id: update mac_id for vdev
 */
struct policy_mgr_cdp_cbacks {
	void (*cdp_update_mac_id)(struct wlan_objmgr_psoc *soc,
		uint8_t vdev_id, uint8_t mac_id);
};

/**
 * struct policy_mgr_dp_cbacks - CDP Callbacks to be invoked
 * from policy manager
 * @hdd_disable_rx_ol_in_concurrency: Callback to disable LRO/GRO offloads
 * @hdd_set_rx_mode_rps_cb: Callback to set RPS
 * @hdd_ipa_set_mcc_mode_cb: Callback to set mcc mode for ipa module
 * @hdd_v2_flow_pool_map: Callback to create vdev flow pool
 * @hdd_v2_flow_pool_unmap: Callback to delete vdev flow pool
 * @hdd_ipa_set_perf_level_bw: Callback to set ipa perf level based on BW
 */
struct policy_mgr_dp_cbacks {
	void (*hdd_disable_rx_ol_in_concurrency)(bool);
	void (*hdd_set_rx_mode_rps_cb)(bool);
	void (*hdd_ipa_set_mcc_mode_cb)(bool);
	void (*hdd_v2_flow_pool_map)(int);
	void (*hdd_v2_flow_pool_unmap)(int);
	void (*hdd_ipa_set_perf_level_bw)(enum hw_mode_bandwidth bw);
};

/**
 * struct policy_mgr_wma_cbacks - WMA Callbacks to be invoked
 * from policy manager
 * @wma_get_connection_info: Get the connection related info
 *                         from wma table
 */
struct policy_mgr_wma_cbacks {
	QDF_STATUS (*wma_get_connection_info)(uint8_t vdev_id,
		struct policy_mgr_vdev_entry_info *conn_table_entry);
};

/**
* policy_mgr_need_opportunistic_upgrade - check whether needs to change current
* HW mode to single mac 2x2 or the other DBS mode(for Dual DBS HW only).
* @psoc: PSOC object information
* @reason: enum policy_mgr_conn_update_reason
*
*  This function is to check whether needs to change to single Mac mode.
*  when opportunistic timer fired.  But a special case for Dual DBS HW, this
*  function will check DBS to DBS change is required or not:
*  1. For Dual DBS HW, if user set vdev priority list, we may need to do
*	 DBS to DBS switching.
*	 eg. P2P GO (2g) < SAP (5G) < STA (2g) in DBS2.
*	 If STA down, we need to switch to DBS1: P2P GO (2g) < SAP (5g).
*	 So, for opportunistic checking, we need to add DBS ->DBS checking
*            as well.
*  2. Reason code :
*	   DBS -> Single MAC : POLICY_MGR_UPDATE_REASON_OPPORTUNISTIC
*	   DBS -> DBS : POLICY_MGR_UPDATE_REASON_PRI_VDEV_CHANGE
*
*  return: PM_NOP, upgrade is not needed, otherwise new action type
*             and reason code be returned.
*/
enum policy_mgr_conc_next_action policy_mgr_need_opportunistic_upgrade(
		struct wlan_objmgr_psoc *psoc,
		enum policy_mgr_conn_update_reason *reason);

/**
 * policy_mgr_next_actions() - initiates actions needed on current
 * connections once channel has been decided for the new
 * connection
 * @psoc: PSOC object information
 * @session_id: Session id
 * @action: action to be executed
 * @reason: Reason for connection update
 * @request_id: Request id provided by the requester, can be used while
 * calling callback to the requester
 *
 * This function initiates initiates actions
 * needed on current connections once channel has been decided
 * for the new connection. Notifies UMAC & FW as well
 *
 * Return: QDF_STATUS enum
 */
QDF_STATUS policy_mgr_next_actions(struct wlan_objmgr_psoc *psoc,
		uint32_t session_id,
		enum policy_mgr_conc_next_action action,
		enum policy_mgr_conn_update_reason reason,
		uint32_t request_id);

/**
 * policy_mgr_validate_dbs_switch() - Check DBS action valid or not
 * @psoc: Pointer to psoc
 * @action: action requested
 *
 * This routine will check the current hw mode with requested action.
 * If we are already in the mode, the caller will do nothing.
 * This will be called by policy_mgr_next_actions to check the action needed
 * or not.
 *
 * return : QDF_STATUS_SUCCESS, action is allowed.
 *          QDF_STATUS_E_ALREADY, action is not needed.
 *          QDF_STATUS_E_FAILURE, error happens.
 *          QDF_STATUS_E_NOSUPPORT, the requested mode not supported.
 */
QDF_STATUS
policy_mgr_validate_dbs_switch(struct wlan_objmgr_psoc *psoc,
			       enum policy_mgr_conc_next_action action);

/**
 * policy_mgr_set_dual_mac_scan_config() - Set the dual MAC scan config
 * @psoc: PSOC object information
 * @dbs_val: Value of DBS bit
 * @dbs_plus_agile_scan_val: Value of DBS plus agile scan bit
 * @single_mac_scan_with_dbs_val: Value of Single MAC scan with DBS
 *
 * Set the values of scan config. For FW mode config, the existing values
 * will be retained
 *
 * Return: None
 */
void policy_mgr_set_dual_mac_scan_config(struct wlan_objmgr_psoc *psoc,
		uint8_t dbs_val,
		uint8_t dbs_plus_agile_scan_val,
		uint8_t single_mac_scan_with_dbs_val);

/**
 * policy_mgr_set_dual_mac_fw_mode_config() - Set the dual mac FW mode config
 * @psoc: PSOC object information
 * @dbs: DBS bit
 * @dfs: Agile DFS bit
 *
 * Set the values of fw mode config. For scan config, the existing values
 * will be retain.
 *
 * Return: None
 */
void policy_mgr_set_dual_mac_fw_mode_config(struct wlan_objmgr_psoc *psoc,
		uint8_t dbs, uint8_t dfs);

/**
 * policy_mgr_is_scc_with_this_vdev_id() - Check if this vdev_id has SCC with
 * other vdev_id's
 * @psoc: PSOC object information
 * @vdev_id: vdev_id
 *
 * This function checks if the given vdev_id has SCC with any other vdev's
 * or not.
 *
 * Return: true if SCC exists, false otherwise
 */
bool policy_mgr_is_scc_with_this_vdev_id(struct wlan_objmgr_psoc *psoc,
					 uint8_t vdev_id);

/**
 * policy_mgr_is_mcc_with_this_vdev_id() - Is current vdev having MCC
 * with any other vdev.
 * @psoc: Pointer to PSOC object
 * @vdev_id: vdev id
 * @mcc_vdev_id: Concurrent MCC vdev id
 *
 * Return: true if MCC exists, false otherwise
 */
bool policy_mgr_is_mcc_with_this_vdev_id(struct wlan_objmgr_psoc *psoc,
					 uint8_t vdev_id, uint8_t *mcc_vdev_id);

/**
 * policy_mgr_is_mcc_on_any_sta_vdev() - Check if any sta vdev is in MCC
 * @psoc: Pointer to PSOC object
 *
 * Return: true if STA vdev is in MCC false otherwise
 */
bool policy_mgr_is_mcc_on_any_sta_vdev(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_soc_set_dual_mac_cfg_cb() - Callback for set dual mac config
 * @status: Status of set dual mac config
 * @scan_config: Current scan config whose status is the first param
 * @fw_mode_config: Current FW mode config whose status is the first param
 *
 * Callback on setting the dual mac configuration
 *
 * Return: None
 */
void policy_mgr_soc_set_dual_mac_cfg_cb(enum set_hw_mode_status status,
		uint32_t scan_config, uint32_t fw_mode_config);

/**
 * policy_mgr_mode_specific_num_open_sessions() - to get number of open sessions
 *                                                for a specific mode
 * @psoc: PSOC object information
 * @mode: device mode
 * @num_sessions: to store num open sessions
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_mode_specific_num_open_sessions(
		struct wlan_objmgr_psoc *psoc, enum QDF_OPMODE mode,
		uint8_t *num_sessions);

/**
 * policy_mgr_mode_specific_num_active_sessions() - to get number of active
 *               sessions for a specific mode
 * @psoc: PSOC object information
 * @mode: device mode
 * @num_sessions: to store num active sessions
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_mode_specific_num_active_sessions(
		struct wlan_objmgr_psoc *psoc, enum QDF_OPMODE mode,
		uint8_t *num_sessions);

/**
 * policy_mgr_concurrent_open_sessions_running() - Checks for
 * concurrent open session
 * @psoc: PSOC object information
 *
 * Checks if more than one open session is running for all the allowed modes
 * in the driver
 *
 * Return: True if more than one open session exists, False otherwise
 */
bool policy_mgr_concurrent_open_sessions_running(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_max_concurrent_connections_reached() - Check if
 * max conccurrency is reached
 * @psoc: PSOC object information
 * Checks for presence of concurrency where more than one connection exists
 *
 * Return: True if the max concurrency is reached, False otherwise
 *
 * Example:
 *    STA + STA (wlan0 and wlan1 are connected) - returns true
 *    STA + STA (wlan0 connected and wlan1 disconnected) - returns false
 *    DUT with P2P-GO + P2P-CLIENT connection) - returns true
 *
 */
bool policy_mgr_max_concurrent_connections_reached(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_clear_concurrent_session_count() - Clear active session count
 * @psoc: PSOC object information
 * Clears the active session count for all modes
 *
 * Return: None
 */
void policy_mgr_clear_concurrent_session_count(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_multiple_active_sta_sessions() - Check for
 * multiple STA connections
 * @psoc: PSOC object information
 *
 * Checks if multiple active STA connection are in the driver
 *
 * Return: True if multiple STA sessions are present, False otherwise
 *
 */
bool policy_mgr_is_multiple_active_sta_sessions(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_sta_active_connection_exists() - Check if a STA
 * connection is active
 * @psoc: PSOC object information
 *
 * Checks if there is atleast one active STA connection in the driver
 *
 * Return: True if an active STA session is present, False otherwise
 */
bool policy_mgr_is_sta_active_connection_exists(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_concurrent_beaconing_sessions_running() - Checks
 * for concurrent beaconing entities
 * @psoc: PSOC object information
 *
 * Checks if multiple beaconing sessions are running i.e., if SAP or GO or IBSS
 * are beaconing together
 *
 * Return: True if multiple entities are beaconing together, False otherwise
 */
bool policy_mgr_concurrent_beaconing_sessions_running(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_wait_for_connection_update() - Wait for hw mode
 * command to get processed
 * @psoc: PSOC object information
 * Waits for CONNECTION_UPDATE_TIMEOUT duration until the set hw mode
 * response sets the event connection_update_done_evt
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_wait_for_connection_update(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_reset_connection_update() - Reset connection
 * update event
 * @psoc: PSOC object information
 * Resets the concurrent connection update event
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_reset_connection_update(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_reset_hw_mode_change() - Reset the hw mode change.
 * @psoc: Pointer to PSOC object
 *
 * Return: none
 */
void policy_mgr_reset_hw_mode_change(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_connection_update() - Set connection update
 * event
 * @psoc: PSOC object information
 * Sets the concurrent connection update event
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_set_connection_update(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_chan_switch_complete_evt() - set channel
 * switch completion event
 * @psoc: PSOC object information
 * Sets the channel switch completion event.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_set_chan_switch_complete_evt(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_reset_chan_switch_complete_evt() - reset channel
 * switch completion event
 * @psoc: PSOC object information
 * Resets the channel switch completion event.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_reset_chan_switch_complete_evt(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_opportunistic_update() - Set opportunistic
 * update event
 * @psoc: PSOC object information
 * Sets the opportunistic update event
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_set_opportunistic_update(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_stop_opportunistic_timer() - Stops opportunistic timer
 * @psoc: PSOC object information
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_stop_opportunistic_timer(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_restart_opportunistic_timer() - Restarts opportunistic timer
 * @psoc: PSOC object information
 * @check_state: check timer state if this flag is set, else restart
 *               irrespective of state
 *
 * Restarts opportunistic timer for DBS_OPPORTUNISTIC_TIME seconds.
 * Check if current state is RUNNING if check_state is set, else
 * restart the timer irrespective of state.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_restart_opportunistic_timer(
		struct wlan_objmgr_psoc *psoc, bool check_state);

/**
 * policy_mgr_modify_sap_pcl_based_on_mandatory_channel() -
 * Modify SAPs PCL based on mandatory channel list
 * @psoc: PSOC object information
 * @pcl_list_org: Pointer to the preferred channel freq list to be trimmed
 * @weight_list_org: Pointer to the weights of the preferred channel list
 * @pcl_len_org: Pointer to the length of the preferred channel list
 *
 * Modifies the preferred channel list of SAP based on the mandatory channel
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_modify_sap_pcl_based_on_mandatory_channel(
		struct wlan_objmgr_psoc *psoc, uint32_t *pcl_list_org,
		uint8_t *weight_list_org, uint32_t *pcl_len_org);

/**
 * policy_mgr_update_and_wait_for_connection_update() - Update and wait for
 * connection update
 * @psoc: PSOC object information
 * @session_id: Session id
 * @ch_freq: Channel frequency
 * @reason: Reason for connection update
 *
 * Update the connection to either single MAC or dual MAC and wait for the
 * update to complete
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_update_and_wait_for_connection_update(
		struct wlan_objmgr_psoc *psoc, uint8_t session_id,
		uint32_t ch_freq, enum policy_mgr_conn_update_reason reason);

/**
 * policy_mgr_is_sap_mandatory_channel_set() - Checks if SAP
 * mandatory channel is set
 * @psoc: PSOC object information
 * Checks if any mandatory channel is set for SAP operation
 *
 * Return: True if mandatory channel is set, false otherwise
 */
bool policy_mgr_is_sap_mandatory_channel_set(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_list_has_24GHz_channel() - Check if list contains 2.4GHz channels
 * @ch_freq_list: Channel frequency list
 * @list_len: Length of the channel list
 *
 * Checks if the channel list contains atleast one 2.4GHz channel
 *
 * Return: True if 2.4GHz channel is present, false otherwise
 */
bool policy_mgr_list_has_24GHz_channel(uint32_t *ch_freq_list,
				       uint32_t list_len);

/**
 * policy_mgr_get_valid_chans_from_range() - get valid channel from given range
 * @psoc: PSOC object information
 * @ch_list: Pointer to the channel frequency list
 * @ch_cnt: Pointer to the length of the channel list
 * @mode: Device mode
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_valid_chans_from_range(struct wlan_objmgr_psoc *psoc,
						 uint32_t *ch_list,
						 uint32_t *ch_cnt,
						 enum policy_mgr_con_mode mode);
/**
 * policy_mgr_get_valid_chans() - Get the valid channel list
 * @psoc: PSOC object information
 * @ch_freq_list: Pointer to the valid channel frequency list
 * @list_len: Pointer to the length of the valid channel list
 *
 * Gets the valid channel list filtered by band
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_valid_chans(struct wlan_objmgr_psoc *psoc,
				      uint32_t *ch_freq_list,
				      uint32_t *list_len);

/**
 * policy_mgr_get_nss_for_vdev() - Get the allowed nss value for the
 * vdev
 * @psoc: PSOC object information
 * @mode: connection type.
 * @nss_2g: Pointer to the 2G Nss parameter.
 * @nss_5g: Pointer to the 5G Nss parameter.
 *
 * Fills the 2G and 5G Nss values based on connection type.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_nss_for_vdev(struct wlan_objmgr_psoc *psoc,
		enum policy_mgr_con_mode mode,
		uint8_t *nss_2g, uint8_t *nss_5g);

/**
 * policy_mgr_get_sap_mandatory_channel() - Get the mandatory channel for SAP
 * @psoc: PSOC object information
 * @sap_ch_freq: sap current frequency in MHz
 * @intf_ch_freq: input/out interference channel frequency to sap
 * @sap_vdev_id: SAP vdev id
 *
 * Gets the mandatory channel for SAP operation
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_get_sap_mandatory_channel(struct wlan_objmgr_psoc *psoc,
				     uint32_t sap_ch_freq,
				     uint32_t *intf_ch_freq,
				     uint8_t sap_vdev_id);

/**
 * policy_mgr_set_sap_mandatory_channels() - Set the mandatory channel for SAP
 * @psoc: PSOC object information
 * @ch_freq_list: Channel frequency list to be set
 * @len: Length of the channel list
 *
 * Sets the channels for the mandatory channel list along with the length of
 * of the channel list.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_set_sap_mandatory_channels(struct wlan_objmgr_psoc *psoc,
						 uint32_t *ch_freq_list,
						 uint32_t len);

/**
 * policy_mgr_is_any_mode_active_on_band_along_with_session() -
 * Check if any connection mode is active on a band along with
 * the given session
 * @psoc: PSOC object information
 * @session_id: Session along which active sessions are looked for
 * @band: Operating frequency band of the connection
 * POLICY_MGR_BAND_24: Looks for active connection on 2.4 GHz only
 * POLICY_MGR_BAND_5: Looks for active connection on 5 GHz only
 *
 * Checks if any of the connection mode is active on a given frequency band
 *
 * Return: True if any connection is active on a given band, false otherwise
 */
bool policy_mgr_is_any_mode_active_on_band_along_with_session(
		struct wlan_objmgr_psoc *psoc, uint8_t session_id,
		enum policy_mgr_band band);

/**
 * policy_mgr_get_bw_by_session_id() - Get channel width for a given session ID
 * @psoc: PSOC object information
 * @session_id: Session ID
 *
 * Return: channel width of the session
 */
enum phy_ch_width
policy_mgr_get_bw_by_session_id(struct wlan_objmgr_psoc *psoc,
				uint8_t session_id);

/**
 * policy_mgr_get_chan_by_session_id() - Get channel for a given session ID
 * @psoc: PSOC object information
 * @session_id: Session ID
 * @ch_freq: Pointer to the channel frequency
 *
 * Gets the channel for a given session ID
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_chan_by_session_id(struct wlan_objmgr_psoc *psoc,
					     uint8_t session_id,
					     uint32_t *ch_freq);

/**
 * policy_mgr_get_sap_go_count_on_mac() - Provide the count of sap and go on
 * given mac
 * @psoc: PSOC object information
 * @list: To provide the vdev_id of the satisfied sap and go (optional)
 * @mac_id: MAC ID
 *
 * This function provides the count of the matched sap and go
 *
 * Return: count of the satisfied sap and go
 */
uint32_t policy_mgr_get_sap_go_count_on_mac(struct wlan_objmgr_psoc *psoc,
					    uint32_t *list, uint8_t mac_id);

/**
 * policy_mgr_is_sta_present_on_dfs_channel() - to find whether any DFS STA is
 *                                              present
 * @psoc: PSOC object information
 * @vdev_id: pointer to vdev_id. It will be filled with the vdev_id of DFS STA
 * @ch_freq: pointer to channel frequency on which DFS STA is present
 * @ch_width: pointer channel width on which DFS STA is connected
 * If any STA is connected on DFS channel then this function will return true
 *
 * Return: true if session is on DFS or false if session is on non-dfs channel
 */
bool policy_mgr_is_sta_present_on_dfs_channel(struct wlan_objmgr_psoc *psoc,
					      uint8_t *vdev_id,
					      qdf_freq_t *ch_freq,
					      enum hw_mode_bandwidth *ch_width);

/**
 * policy_mgr_is_sta_present_on_freq() - Checks whether sta is present on the
 *                                       given frequency
 * @psoc: PSOC object information
 * @vdev_id: pointer to vdev_id. It will be filled with the vdev_id of DFS STA
 * @ch_freq: channel for which this checks is needed
 * @ch_width: pointer channel width on which DFS STA is connected
 *
 * Return: true if STA is found in ch_freq
 */
bool policy_mgr_is_sta_present_on_freq(struct wlan_objmgr_psoc *psoc,
				       uint8_t *vdev_id, qdf_freq_t ch_freq,
				       enum hw_mode_bandwidth *ch_width);

/**
 * policy_mgr_is_sta_gc_active_on_mac() - Is there active sta/gc for a
 * given mac id
 * @psoc: PSOC object information
 * @mac_id: MAC ID
 *
 * Checks if there is active sta/gc for a given mac id
 *
 * Return: true if there is active sta/gc for a given mac id, false otherwise
 */
bool policy_mgr_is_sta_gc_active_on_mac(struct wlan_objmgr_psoc *psoc,
					uint8_t mac_id);

/**
 * policy_mgr_get_mac_id_by_session_id() - Get MAC ID for a given session ID
 * @psoc: PSOC object information
 * @session_id: Session ID
 * @mac_id: Pointer to the MAC ID
 *
 * Gets the MAC ID for a given session ID
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_mac_id_by_session_id(struct wlan_objmgr_psoc *psoc,
		uint8_t session_id, uint8_t *mac_id);

/**
 * policy_mgr_get_mcc_operating_channel() - Get the MCC channel
 * @psoc: PSOC object information
 * @session_id: Session ID with which MCC is being done
 *
 * Gets the MCC channel for a given session ID.
 *
 * Return: '0' (INVALID_CHANNEL_ID) or valid channel frequency
 */
uint32_t policy_mgr_get_mcc_operating_channel(struct wlan_objmgr_psoc *psoc,
					      uint8_t session_id);

/**
 * policy_mgr_get_pcl_for_existing_conn() - Get PCL for existing connection
 * @psoc: PSOC object information
 * @mode: Connection mode of type 'policy_mgr_con_mode'
 * @pcl_ch: Pointer to the PCL
 * @len: Pointer to the length of the PCL
 * @pcl_weight: Pointer to the weights of the PCL
 * @weight_len: Max length of the weights list
 * @all_matching_cxn_to_del: Need remove all entries before getting pcl
 * @vdev_id: Vdev Id
 *
 * Get the PCL for an existing connection
 *
 * Return: None
 */
QDF_STATUS policy_mgr_get_pcl_for_existing_conn(
		struct wlan_objmgr_psoc *psoc,
		enum policy_mgr_con_mode mode,
		uint32_t *pcl_ch, uint32_t *len,
		uint8_t *pcl_weight, uint32_t weight_len,
		bool all_matching_cxn_to_del,
		uint8_t vdev_id);

/**
 * policy_mgr_get_pcl_for_vdev_id() - Get PCL for 1 vdev
 * @psoc: PSOC object information
 * @mode: Connection mode of type 'policy_mgr_con_mode'
 * @pcl_ch: Pointer to the PCL
 * @len: Pointer to the length of the PCL
 * @pcl_weight: Pointer to the weights of the PCL
 * @weight_len: Max length of the weights list
 * @vdev_id: vdev id to get PCL
 *
 * Get the PCL for a vdev, when vdev need move to another channel, need
 * get PCL after remove the vdev from connection list.
 *
 * Return: None
 */
QDF_STATUS policy_mgr_get_pcl_for_vdev_id(struct wlan_objmgr_psoc *psoc,
					  enum policy_mgr_con_mode mode,
					  uint32_t *pcl_ch, uint32_t *len,
					  uint8_t *pcl_weight,
					  uint32_t weight_len,
					  uint8_t vdev_id);

/**
 * policy_mgr_get_pcl_for_scc_in_same_mode() - Get PCL for vdev and other
 * connection in same mode and same frequency
 * @psoc: PSOC object information
 * @mode: Connection mode of type 'policy_mgr_con_mode'
 * @pcl_ch: Pointer to the PCL
 * @len: Pointer to the length of the PCL
 * @pcl_weight: Pointer to the weights of the PCL
 * @weight_len: Max length of the weights list
 * @vdev_id: vdev id to get PCL
 *
 * If need move connections in same mode and same frequency, need get PCL
 * after remove them from connection list.
 *
 * Return: QDF STATUS
 */
QDF_STATUS
policy_mgr_get_pcl_for_scc_in_same_mode(struct wlan_objmgr_psoc *psoc,
					enum policy_mgr_con_mode mode,
					uint32_t *pcl_ch, uint32_t *len,
					uint8_t *pcl_weight,
					uint32_t weight_len,
					uint8_t vdev_id);

/**
 * policy_mgr_get_valid_chan_weights() - Get the weightage for
 * all valid channels
 * @psoc: PSOC object information
 * @weight: Pointer to the structure containing pcl, saved channel list and
 * weighed channel list
 * @mode: Policy manager connection mode
 * @vdev: pointer to vdev on which new connection is coming up
 *
 * Provides the weightage for all valid channels. This compares the PCL list
 * with the valid channel list. The channels present in the PCL get their
 * corresponding weightage and the non-PCL channels get the default weightage
 * of WEIGHT_OF_NON_PCL_CHANNELS.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_valid_chan_weights(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_pcl_chan_weights *weight,
		enum policy_mgr_con_mode mode, struct wlan_objmgr_vdev *vdev);

/**
 * policy_mgr_set_hw_mode_on_channel_switch() - Set hw mode
 * after channel switch
 * @psoc: PSOC object information
 * @session_id: Session ID
 *
 * Sets hw mode after doing a channel switch
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_set_hw_mode_on_channel_switch(
		struct wlan_objmgr_psoc *psoc, uint8_t session_id);

/**
 * policy_mgr_check_and_set_hw_mode_for_channel_switch() - check if hw mode
 * change is required before channel switch for STA/SAP,
 * this is required if DBS mode is 2x2
 * @psoc: PSOC object information
 * @vdev_id: vdev id on which channel switch is required
 * @ch_freq: New channel frequency to which channel switch is requested
 * @reason: reason for hw mode change
 *
 * Return: QDF_STATUS, success if HW mode change is required else Failure
 */
QDF_STATUS policy_mgr_check_and_set_hw_mode_for_channel_switch(
		struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
		uint32_t ch_freq, enum policy_mgr_conn_update_reason reason);

/**
 * policy_mgr_checkn_update_hw_mode_single_mac_mode() - Set hw_mode to SMM
 * if required
 * @psoc: PSOC object information
 * @ch_freq: channel frequency for the new STA connection
 *
 * After the STA disconnection, if the hw_mode is in DBS and the new STA
 * connection is coming in the band in which existing connections are
 * present, then this function stops the dbs opportunistic timer and sets
 * the hw_mode to Single MAC mode (SMM).
 *
 * Return: None
 */
void policy_mgr_checkn_update_hw_mode_single_mac_mode(
		struct wlan_objmgr_psoc *psoc, uint32_t ch_freq);

/**
 * policy_mgr_dump_connection_status_info() - Dump the concurrency information
 * @psoc: PSOC object information
 * Prints the concurrency information such as tx/rx spatial stream, chainmask,
 * etc.
 *
 * Return: None
 */
void policy_mgr_dump_connection_status_info(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_mode_get_macid_by_vdev_id() - get macid from vdev_id
 * @psoc: PSOC object information
 * @vdev_id: vdev id to get PCL
 *
 * Return: mac id
 */
uint32_t policy_mgr_mode_get_macid_by_vdev_id(struct wlan_objmgr_psoc *psoc,
					      uint32_t vdev_id);

/**
 * policy_mgr_mode_specific_vdev_id() - provides the
 * vdev id of the pecific mode
 * @psoc: PSOC object information
 * @mode: type of connection
 *
 * This function provides vdev id for the given mode
 *
 * Return: vdev id
 */
uint32_t policy_mgr_mode_specific_vdev_id(struct wlan_objmgr_psoc *psoc,
					  enum policy_mgr_con_mode mode);

/**
 * policy_mgr_mode_specific_connection_count() - provides the
 * count of connections of specific mode
 * @psoc: PSOC object information
 * @mode: type of connection
 * @list: To provide the indices on pm_conc_connection_list
 *	(optional)
 *
 * This function provides the count of current connections
 *
 * Return: connection count of specific type
 */
uint32_t policy_mgr_mode_specific_connection_count(
		struct wlan_objmgr_psoc *psoc, enum policy_mgr_con_mode mode,
		uint32_t *list);

/**
 * policy_mgr_check_conn_with_mode_and_vdev_id() - checks if any active
 * session with specific mode and vdev_id
 * @psoc: PSOC object information
 * @mode: type of connection
 * @vdev_id: vdev_id of the connection
 *
 * This function checks if any active session with specific mode and vdev_id
 * is present
 *
 * Return: QDF STATUS with success if active session is found, else failure
 */
QDF_STATUS policy_mgr_check_conn_with_mode_and_vdev_id(
		struct wlan_objmgr_psoc *psoc, enum policy_mgr_con_mode mode,
		uint32_t vdev_id);

/**
 * policy_mgr_dump_freq_range_n_vdev_map() - Dump freq range of mac and vdev to
 * mac mapping
 * @num_vdev_mac_entries: Number of vdev-mac id mapping that follows
 * @vdev_mac_map: vdev-mac id map. This memory will be freed by the caller.
 * So, make local copy if needed.
 * @num_mac_freq: Number of pdev freq mapping that follows
 * @mac_freq_range: mac_freq_range mapping
 *
 * Return: None
 */
void
policy_mgr_dump_freq_range_n_vdev_map(uint32_t num_vdev_mac_entries,
			struct policy_mgr_vdev_mac_map *vdev_mac_map,
			uint32_t num_mac_freq,
			struct policy_mgr_pdev_mac_freq_map *mac_freq_range);

/**
 * policy_mgr_hw_mode_transition_cb() - Callback for HW mode
 * transition from FW
 * @old_hw_mode_index: Old HW mode index
 * @new_hw_mode_index: New HW mode index
 * @num_vdev_mac_entries: Number of vdev-mac id mapping that follows
 * @vdev_mac_map: vdev-mac id map. This memory will be freed by the caller.
 * So, make local copy if needed.
 * @num_mac_freq: Number of pdev freq mapping that follows
 * @mac_freq_range: mac_freq_range mapping
 * @context:
 *
 * Provides the old and new HW mode index set by the FW
 *
 * Return: None
 */
void policy_mgr_hw_mode_transition_cb(uint32_t old_hw_mode_index,
		uint32_t new_hw_mode_index,
		uint32_t num_vdev_mac_entries,
		struct policy_mgr_vdev_mac_map *vdev_mac_map,
		uint32_t num_mac_freq,
		struct policy_mgr_pdev_mac_freq_map *mac_freq_range,
		struct wlan_objmgr_psoc *context);

/**
 * policy_mgr_will_freq_lead_to_mcc() - Check if the given freq can lead to
 * MCC scenario with existing connection
 * @psoc: psoc pointer
 * @freq: freq to check with existing connections
 *
 * Return: true or false
 */
bool policy_mgr_will_freq_lead_to_mcc(struct wlan_objmgr_psoc *psoc,
				      qdf_freq_t freq);

/**
 * policy_mgr_current_concurrency_is_scc() - To check the current
 * concurrency combination if it is doing SCC
 * @psoc: PSOC object information
 * This routine is called to check if it is doing SCC
 *
 * Return: True - SCC, False - Otherwise
 */
bool policy_mgr_current_concurrency_is_scc(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_current_concurrency_is_mcc() - To check the current
 * concurrency combination if it is doing MCC
 * @psoc: PSOC object information
 * This routine is called to check if it is doing MCC
 *
 * Return: True - MCC, False - Otherwise
 */
bool policy_mgr_current_concurrency_is_mcc(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_concurrent_sta_on_different_mac() - To check if
 * sta concurrency on different mac
 * @psoc: PSOC object information
 * This routine is called to check if sta concurrency on different mac
 *
 * Return: True - sta concurrency on different mac, False - Otherwise
 */
bool policy_mgr_concurrent_sta_on_different_mac(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_sap_p2pgo_on_dfs() - check if there is a P2PGO or SAP
 * operating in a DFS channel
 * @psoc: PSOC object information
 * This routine is called to check if there is a P2PGO/SAP on DFS channel
 *
 * Return: True  - P2PGO/SAP present on DFS Channel
 * False - Otherwise
 */

bool policy_mgr_is_sap_p2pgo_on_dfs(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_register_sme_cb() - register SME callbacks
 * @psoc: PSOC object information
 * @sme_cbacks: function pointers from SME
 *
 * API, allows SME to register callbacks to be invoked by policy
 * mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_sme_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_sme_cbacks *sme_cbacks);

/**
 * policy_mgr_register_hdd_cb() - register HDD callbacks
 * @psoc: PSOC object information
 * @hdd_cbacks: function pointers from HDD
 *
 * API, allows HDD to register callbacks to be invoked by policy
 * mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_hdd_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_hdd_cbacks *hdd_cbacks);

/**
 * policy_mgr_register_conc_cb() - register Lim callbacks
 * @psoc: PSOC object information
 * @conc_cbacks: function pointers from lim
 *
 * API, allows Lim to register callbacks to be invoked by policy
 * mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */

QDF_STATUS policy_mgr_register_conc_cb(struct wlan_objmgr_psoc *psoc,
				struct policy_mgr_conc_cbacks *conc_cbacks);

/**
 * policy_mgr_deregister_hdd_cb() - Deregister HDD callbacks
 * @psoc: PSOC object information
 *
 * API, allows HDD to deregister callbacks
 *
 * Return: SUCCESS,
 *         Failure (if de-registration fails)
 */
QDF_STATUS policy_mgr_deregister_hdd_cb(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_register_tdls_cb() - register TDLS callbacks
 * @psoc: PSOC object information
 * @tdls_cbacks: function pointers from TDLS
 *
 * API, allows TDLS to register callbacks to be invoked by
 * policy mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_tdls_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_tdls_cbacks *tdls_cbacks);

/**
 * policy_mgr_register_cdp_cb() - register CDP callbacks
 * @psoc: PSOC object information
 * @cdp_cbacks: function pointers from CDP
 *
 * API, allows CDP to register callbacks to be invoked by
 * policy mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_cdp_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_cdp_cbacks *cdp_cbacks);

/**
 * policy_mgr_register_dp_cb() - register DP callbacks
 * @psoc: PSOC object information
 * @dp_cbacks: function pointers from DP
 *
 * API, allows DP to register callbacks to be invoked by
 * policy mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_dp_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_dp_cbacks *dp_cbacks);

/**
 * policy_mgr_register_wma_cb() - register WMA callbacks
 * @psoc: PSOC object information
 * @wma_cbacks: function pointers from WMA
 *
 * API, allows WMA to register callbacks to be invoked by policy
 * mgr
 *
 * Return: SUCCESS,
 *         Failure (if registration fails)
 */
QDF_STATUS policy_mgr_register_wma_cb(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_wma_cbacks *wma_cbacks);

/**
 * policy_mgr_find_if_fw_supports_dbs() - to find if FW/HW supports DBS
 * @psoc: PSOC object information
 *
 * This API checks if legacy service ready event contains DBS or no.
 * This API doesn't check service ready extension which contains actual
 * hw mode list that tells if all supported HW modes' caps.
 *
 * Return: true (if service ready indication supports DBS or no) else false
 *
 */
bool policy_mgr_find_if_fw_supports_dbs(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_dbs_enable() - Check if master DBS control is enabled
 * @psoc: PSOC object information
 * Checks if the master DBS control is enabled. This will be used
 * to override any other DBS capability
 *
 * Return: True if master DBS control is enabled
 */
bool policy_mgr_is_dbs_enable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_hw_dbs_capable() - Check if HW is DBS capable
 * @psoc: PSOC object information
 * Checks if the HW is DBS capable
 *
 * Return: true if the HW is DBS capable
 */
bool policy_mgr_is_hw_dbs_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_interband_mcc_supported() - Checks for interband MCC support
 * @psoc: PSOC object information
 * Checks if target supports interband MCC or not
 *
 * Return: True if the target supports interband MCC else False
 */
bool policy_mgr_is_interband_mcc_supported(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_dbs_scan_allowed() - Check if DBS scan is allowed or not
 * @psoc: PSOC object information
 * Checks if the DBS scan can be performed or not
 *
 * Return: true if DBS scan is allowed.
 */
bool policy_mgr_is_dbs_scan_allowed(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_hw_sbs_capable() - Check if HW is SBS capable
 * @psoc: PSOC object information
 * Checks if the HW is SBS capable
 *
 * Return: true if the HW is SBS capable
 */
bool policy_mgr_is_hw_sbs_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_hw_emlsr_capable() - Check if HW is EMLSR capable
 * @psoc: PSOC object information
 * Checks if the HW is EMLSR capable
 *
 * Return: true if the HW is EMLSR capable
 */
bool policy_mgr_is_hw_emlsr_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_2_freq_always_on_same_mac() - Function to check whether both the
 * input frequencies are on same mac in all supported mode/freq range
 * @psoc: Pointer to Psoc
 * @freq_1: Frequency 1 to check
 * @freq_2: Frequency 2 to check
 *
 * This Function check whether both the input frequency exist in the same mac
 * in all supported mode/freq range
 *
 * Return:True if both the frequency exist on the same mac in all supported
 * mode/freq range.
 */
bool policy_mgr_2_freq_always_on_same_mac(struct wlan_objmgr_psoc *psoc,
					  qdf_freq_t freq_1, qdf_freq_t freq_2);

/**
 * policy_mgr_are_2_freq_on_same_mac() - Function to check whether both the
 * input frequencies are on same mac in current freq range
 * @psoc: Pointer to Psoc
 * @freq_1: Frequency 1 to check
 * @freq_2: Frequency 2 to check
 *
 * This Function check whether both the input frequency exist in the same mac
 *
 * Return:True if both the frequency exist on the same mac.
 */
bool
policy_mgr_are_2_freq_on_same_mac(struct wlan_objmgr_psoc *psoc,
				  qdf_freq_t freq_1,
				  qdf_freq_t  freq_2);

/**
 * policy_mgr_3_freq_always_on_same_mac() - Function to check whether all three
 * input frequencies will always in same mac in all supported mode/freq range
 * @psoc: Pointer to Psoc
 * @freq_1: Frequency 1 to check
 * @freq_2: Frequency 2 to check
 * @freq_3: Frequency 3 to check
 *
 * This Function check whether all three input frequencies exist in the same
 * mac in all supported mode/freq range.
 *
 * Return:True if all three frequency exist on the same mac in all supported
 * mode/freq range
 */
bool
policy_mgr_3_freq_always_on_same_mac(struct wlan_objmgr_psoc *psoc,
				     qdf_freq_t freq_1, qdf_freq_t freq_2,
				     qdf_freq_t freq_3);

/**
 * policy_mgr_are_3_freq_on_same_mac() - Function to check whether all three
 * input frequencies are in same mac in current freq range
 * @psoc: Pointer to Psoc
 * @freq_1: Frequency 1 to check
 * @freq_2: Frequency 2 to check
 * @freq_3: Frequency 3 to check
 *
 * This Function check whether all three input frequencies exist in the same
 * mac.
 *
 * Return:True if all three frequency exist on the same mac
 */
bool
policy_mgr_are_3_freq_on_same_mac(struct wlan_objmgr_psoc *psoc,
				  qdf_freq_t freq_1, qdf_freq_t freq_2,
				  qdf_freq_t freq_3);

/**
 * policy_mgr_allow_4th_new_freq() - Function to check whether 4th freq can
 * be allowed with existing 3 vifs
 * @psoc: Pointer to Psoc
 * @ch_freq: new channel frequency
 * @mode: new device mode
 * @ext_flags: extended flags for concurrency check
 *
 * Return:True if 4th freq can be allowed with existing 3 vifs
 */
#ifdef FEATURE_FOURTH_CONNECTION
bool
policy_mgr_allow_4th_new_freq(struct wlan_objmgr_psoc *psoc,
			      qdf_freq_t ch_freq,
			      enum policy_mgr_con_mode mode,
			      uint32_t ext_flags);
#else
static inline bool
policy_mgr_allow_4th_new_freq(struct wlan_objmgr_psoc *psoc,
			      qdf_freq_t ch_freq,
			      enum policy_mgr_con_mode mode,
			      uint32_t ext_flags)
{
	return false;
}
#endif

/**
 * policy_mgr_are_sbs_chan() - Function to check whether both the
 * input frequency are in SBS frequency range
 *
 * @psoc: Pointer to Psoc
 * @freq_1: Frequency 1 to check
 * @freq_2: Frequency 2 to check
 *
 * This Function check whether both the input frequency exist in the SBS
 * frequency range.
 *
 * Return:True if both the frequency exist on the SBS frequency range.
 *
 */
bool
policy_mgr_are_sbs_chan(struct wlan_objmgr_psoc *psoc, qdf_freq_t freq_1,
			qdf_freq_t  freq_2);

/**
 * policy_mgr_is_current_hwmode_dbs() - Function to check if current HW mode is
 * DBS
 *
 * @psoc: Pointer to Psoc
 *
 * This Function checks if current HW mode is DBS
 *
 * Return:True if current HW mode is DBS.
 *
 */
bool policy_mgr_is_current_hwmode_dbs(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_current_hwmode_sbs() - Function to check if current HW mode is
 * SBS
 *
 * @psoc: Pointer to Psoc
 *
 * This Function checks if current HW mode is SBS
 *
 * Return:True if current HW mode is SBS.
 *
 */
bool policy_mgr_is_current_hwmode_sbs(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_curr_hwmode_emlsr() - Function to check if current HW mode is
 * eMLSR
 *
 * @psoc: Pointer to psoc
 *
 * This Function checks if current HW mode is eMLSR
 *
 * Return:True if current HW mode is eMLSR.
 *
 */
bool policy_mgr_is_curr_hwmode_emlsr(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_dp_hw_dbs_capable() - if hardware is capable of dbs 2x2
 * or 1X1 for Data Path (HW mode)
 * @psoc: PSOC object information
 * This API is for Data Path to get HW mode support dbs.
 *
 * Return: true - DBS capable, false - not
 */
bool policy_mgr_is_dp_hw_dbs_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_hw_dbs_2x2_capable() - if hardware is capable of dbs 2x2
 * @psoc: PSOC object information
 * This function checks if hw_modes supported are always capable of
 * DBS and there is no need for downgrading while entering DBS.
 *    true: DBS 2x2 can always be supported
 *    false: hw_modes support DBS 1x1 as well
 * Genoa DBS 2x2 + 1x1 will not be included.
 *
 * Return: true - DBS2x2, false - DBS1x1
 */
bool policy_mgr_is_hw_dbs_2x2_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_hw_dbs_required_for_band() - Check whether hardware needs DBS
 * mode to support the given band
 * @psoc: PSOC object information
 * @band: band
 *
 * The function checks whether DBS mode switching required or not to support
 * given band based on target capability.
 * Any HW which doesn't support given band on PHY A will need DBS HW mode when a
 * connection is coming up on that band.
 *
 * Return: true - DBS mode required for requested band
 */
bool policy_mgr_is_hw_dbs_required_for_band(struct wlan_objmgr_psoc *psoc,
					    enum hw_mode_mac_band_cap band);

/*
 * policy_mgr_is_2x2_1x1_dbs_capable() - check 2x2+1x1 DBS supported or not
 * @psoc: PSOC object data
 *
 * This routine is called to check 2x2 5G + 1x1 2G (DBS1) or
 * 2x2 2G + 1x1 5G (DBS2) support or not.
 * Either DBS1 or DBS2 supported
 *
 * Return: true/false
 */
bool policy_mgr_is_2x2_1x1_dbs_capable(struct wlan_objmgr_psoc *psoc);

/*
 * policy_mgr_is_2x2_5G_1x1_2G_dbs_capable() - check Genoa DBS1 enabled or not
 * @psoc: PSOC object data
 *
 * This routine is called to check support DBS1 or not.
 * Notes: DBS1: 2x2 5G + 1x1 2G.
 * This function will call policy_mgr_get_hw_mode_idx_from_dbs_hw_list to match
 * the HW mode from hw mode list. The parameters will also be matched to
 * 2x2 5G +2x2 2G HW mode. But firmware will not report 2x2 5G + 2x2 2G alone
 * with 2x2 5G + 1x1 2G at same time. So, it is safe to find DBS1 with
 * policy_mgr_get_hw_mode_idx_from_dbs_hw_list.
 *
 * Return: true/false
 */
bool policy_mgr_is_2x2_5G_1x1_2G_dbs_capable(struct wlan_objmgr_psoc *psoc);

/*
 * policy_mgr_is_2x2_2G_1x1_5G_dbs_capable() - check Genoa DBS2 enabled or not
 * @psoc: PSOC object data
 *
 * This routine is called to check support DBS2 or not.
 * Notes: DBS2: 2x2 2G + 1x1 5G
 *
 * Return: true/false
 */
bool policy_mgr_is_2x2_2G_1x1_5G_dbs_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_init() - Policy Manager component initialization
 *                 routine
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_init(void);

/**
 * policy_mgr_deinit() - Policy Manager component
 *                 de-initialization routine
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_deinit(void);

/**
 * policy_mgr_psoc_enable() - Policy Manager component
 *                 enable routine
 * @psoc: PSOC object information
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_psoc_enable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_psoc_disable() - Policy Manager component
 *                 disable routine
 * @psoc: PSOC object information
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_psoc_disable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_psoc_open() - Policy Manager component
 *                 open routine
 * @psoc: PSOC object information
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_psoc_open(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_psoc_close() - Policy Manager component
 *                 close routine
 * @psoc: PSOC object information
 *
 * Return - QDF Status
 */
QDF_STATUS policy_mgr_psoc_close(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_num_dbs_hw_modes() - Get number of HW mode
 * @psoc: PSOC object information
 * Fetches the number of DBS HW modes returned by the FW
 *
 * Return: Negative value on error or returns the number of DBS HW modes
 */
int8_t policy_mgr_get_num_dbs_hw_modes(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_dbs_hw_modes() - Get the DBS HW modes for userspace
 * @psoc: PSOC object information
 * @one_by_one_dbs: 1x1 DBS capability of HW
 * @two_by_two_dbs: 2x2 DBS capability of HW
 *
 * Provides the DBS HW mode capability such as whether
 * 1x1 DBS, 2x2 DBS is supported by the HW or not.
 *
 * Return: Failure in case of error and 0 on success
 *         one_by_one_dbs/two_by_two_dbs will be false,
 *         if they are not supported.
 *         one_by_one_dbs/two_by_two_dbs will be true,
 *         if they are supported.
 *         false values of one_by_one_dbs/two_by_two_dbs,
 *         indicate DBS is disabled
 */
QDF_STATUS policy_mgr_get_dbs_hw_modes(struct wlan_objmgr_psoc *psoc,
		bool *one_by_one_dbs, bool *two_by_two_dbs);

/**
 * policy_mgr_check_sap_restart() - Restart SAP when band/channel change
 * @psoc: Pointer to soc
 * @vdev_id: Vdev id
 *
 * Return: None
 */
void
policy_mgr_check_sap_restart(struct wlan_objmgr_psoc *psoc, uint8_t vdev_id);

/**
 * policy_mgr_check_sta_ap_concurrent_ch_intf() - Restart SAP in STA-AP case
 * @data: Pointer to STA adapter
 *
 * Restarts the SAP interface in STA-AP concurrency scenario
 *
 * Return: None
 */
void policy_mgr_check_sta_ap_concurrent_ch_intf(void *data);

/**
 * policy_mgr_get_current_hw_mode() - Get current HW mode params
 * @psoc: PSOC object information
 * @hw_mode: HW mode parameters
 *
 * Provides the current HW mode parameters if the HW mode is initialized
 * in the driver
 *
 * Return: Success if the current HW mode params are successfully populated
 */
QDF_STATUS policy_mgr_get_current_hw_mode(struct wlan_objmgr_psoc *psoc,
		struct policy_mgr_hw_mode_params *hw_mode);

/**
 * policy_mgr_get_dbs_plus_agile_scan_config() - Get DBS plus agile scan bit
 * @psoc: PSOC object information
 * Gets the DBS plus agile scan bit of concurrent_scan_config_bits
 *
 * Return: 0 or 1 to indicate the DBS plus agile scan bit
 */
bool policy_mgr_get_dbs_plus_agile_scan_config(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_single_mac_scan_with_dfs_config() - Get Single
 * MAC scan with DFS bit
 * @psoc: PSOC object information
 * Gets the Single MAC scan with DFS bit of concurrent_scan_config_bits
 *
 * Return: 0 or 1 to indicate the Single MAC scan with DFS bit
 */
bool policy_mgr_get_single_mac_scan_with_dfs_config(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_hw_mode_change_in_progress() - Set value
 * corresponding to policy_mgr_hw_mode_change that indicate if
 * HW mode change is in progress
 * @psoc: PSOC object information
 * @value: Indicate if hw mode change is in progress
 *
 * Set the value corresponding to policy_mgr_hw_mode_change that
 * indicated if hw mode change is in progress.
 *
 * Return: None
 */
void policy_mgr_set_hw_mode_change_in_progress(
	struct wlan_objmgr_psoc *psoc, enum policy_mgr_hw_mode_change value);

/**
 * policy_mgr_is_hw_mode_change_in_progress() - Check if HW mode
 * change is in progress.
 * @psoc: PSOC object information
 *
 * Returns the corresponding policy_mgr_hw_mode_change value.
 *
 * Return: policy_mgr_hw_mode_change value.
 */
enum policy_mgr_hw_mode_change policy_mgr_is_hw_mode_change_in_progress(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_hw_mode_change_from_hw_mode_index() - Get
 * matching HW mode from index
 * @psoc: PSOC object information
 * @hw_mode_index: HW mode index
 * Returns the corresponding policy_mgr_hw_mode_change HW mode.
 *
 * Return: policy_mgr_hw_mode_change value.
 */
enum policy_mgr_hw_mode_change policy_mgr_get_hw_mode_change_from_hw_mode_index(
	struct wlan_objmgr_psoc *psoc, uint32_t hw_mode_index);

/**
 * policy_mgr_is_scan_simultaneous_capable() - Check if scan
 * parallelization is supported or not
 * @psoc: PSOC object information
 * currently scan parallelization feature support is dependent on DBS but
 * it can be independent in future.
 *
 * Return: True if master DBS control is enabled
 */
bool policy_mgr_is_scan_simultaneous_capable(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_set_user_cfg() - Function to set user cfg variables
 * required by policy manager component
 * @psoc: PSOC object information
 * @user_cfg: User config valiables structure pointer
 *
 * This function sets the user cfg variables required by policy
 * manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
QDF_STATUS policy_mgr_set_user_cfg(struct wlan_objmgr_psoc *psoc,
				struct policy_mgr_user_cfg *user_cfg);

/**
 * policy_mgr_init_dbs_config() - Function to initialize DBS
 * config in policy manager component
 * @psoc: PSOC object information
 * @scan_config: DBS scan config
 * @fw_config: DBS FW config
 *
 * This function sets the DBS configurations required by policy
 * manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_init_dbs_config(struct wlan_objmgr_psoc *psoc,
		uint32_t scan_config, uint32_t fw_config);

/**
 * policy_mgr_init_sbs_fw_config() - Function to initialize SBS
 * fw mode config in policy manager component
 * @psoc: PSOC object information
 * @fw_config: FW config
 *
 * This function initialize SBS fw mode config in policy manager component
 *
 * Return: void
 *
 */
void policy_mgr_init_sbs_fw_config(struct wlan_objmgr_psoc *psoc,
				   uint32_t fw_config);

/**
 * policy_mgr_update_dbs_scan_config() - Function to update
 * DBS scan config in policy manager component
 * @psoc: PSOC object information
 *
 * This function updates the DBS scan configurations required by
 * policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_dbs_scan_config(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_update_dbs_fw_config() - Function to update DBS FW
 * config in policy manager component
 * @psoc: PSOC object information
 *
 * This function updates the DBS FW configurations required by
 * policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_dbs_fw_config(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_update_dbs_req_config() - Function to update DBS
 * request config in policy manager component
 * @psoc: PSOC object information
 * @scan_config: DBS scan config
 * @fw_mode_config: DBS FW config
 *
 * This function updates DBS request configurations required by
 * policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_dbs_req_config(struct wlan_objmgr_psoc *psoc,
		uint32_t scan_config, uint32_t fw_mode_config);

/**
 * policy_mgr_dump_dbs_hw_mode() - Function to dump DBS config
 * @psoc: PSOC object information
 *
 * This function dumps the DBS configurations
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_dump_dbs_hw_mode(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_init_dbs_hw_mode() - Function to initialize DBS HW
 * modes in policy manager component
 * @psoc: PSOC object information
 * @num_dbs_hw_modes: Number of HW modes
 * @ev_wlan_dbs_hw_mode_list: HW list
 *
 * This function to initialize the DBS HW modes in policy
 * manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_init_dbs_hw_mode(struct wlan_objmgr_psoc *psoc,
				uint32_t num_dbs_hw_modes,
				uint32_t *ev_wlan_dbs_hw_mode_list);

QDF_STATUS policy_mgr_update_sbs_freq(struct wlan_objmgr_psoc *psoc,
				      struct target_psoc_info *tgt_hdl);

/**
 * policy_mgr_get_sbs_cut_off_freq() - Function to get SBS 5g cut off freq
 *
 * @psoc: PSOC object information
 *
 * This function to get sbs cut off freq
 *
 * Return: cut of freq
 *
 */
qdf_freq_t policy_mgr_get_sbs_cut_off_freq(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_update_hw_mode_list() - Function to initialize DBS
 * HW modes in policy manager component
 * @psoc: PSOC object information
 * @tgt_hdl: Target psoc information
 *
 * This function to initialize the DBS HW modes in policy
 * manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
QDF_STATUS policy_mgr_update_hw_mode_list(struct wlan_objmgr_psoc *psoc,
					  struct target_psoc_info *tgt_hdl);

/**
 * policy_mgr_update_hw_mode_index() - Function to update
 * current HW mode in policy manager component
 * @psoc: PSOC object information
 * @new_hw_mode_index: index to new HW mode
 *
 * This function to update the current HW mode in policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_hw_mode_index(struct wlan_objmgr_psoc *psoc,
		uint32_t new_hw_mode_index);

/**
 * policy_mgr_update_old_hw_mode_index() - Function to update
 * old HW mode in policy manager component
 * @psoc: PSOC object information
 * @old_hw_mode_index: index to old HW mode
 *
 * This function to update the old HW mode in policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_old_hw_mode_index(struct wlan_objmgr_psoc *psoc,
		uint32_t old_hw_mode_index);

/**
 * policy_mgr_update_new_hw_mode_index() - Function to update
 * new HW mode in policy manager component
 * @psoc: PSOC object information
 * @new_hw_mode_index: index to new HW mode
 *
 * This function to update the new HW mode in policy manager
 *
 * Return: SUCCESS or FAILURE
 *
 */
void policy_mgr_update_new_hw_mode_index(struct wlan_objmgr_psoc *psoc,
		uint32_t new_hw_mode_index);

/**
 * policy_mgr_is_chan_ok_for_dnbs() - Function to check if a channel
 * is OK for "Do Not Break Stream"
 * @psoc: PSOC object information
 * @ch_freq: Channel frequency to check.
 * @ok: Pointer to flag in which status will be stored
 * This function checks if a channel is OK for
 * "Do Not Break Stream"
 * Return: SUCCESS or FAILURE
 */
QDF_STATUS policy_mgr_is_chan_ok_for_dnbs(struct wlan_objmgr_psoc *psoc,
					  uint32_t ch_freq, bool *ok);

/**
 * policy_mgr_get_hw_dbs_nss() - Computes DBS NSS
 * @psoc: PSOC object information
 * @nss_dbs: NSS info of both MAC0 and MAC1
 * This function computes NSS info of both MAC0 and MAC1
 *
 * Return: uint32_t value signifies supported RF chains
 */
uint32_t policy_mgr_get_hw_dbs_nss(struct wlan_objmgr_psoc *psoc,
				   struct dbs_nss *nss_dbs);

/**
 * policy_mgr_is_dnsc_set - Check if user has set
 * "Do_Not_Switch_Channel" for the vdev passed
 * @vdev: vdev pointer
 *
 * Get "Do_Not_Switch_Channel" setting for the vdev passed.
 *
 * Return: true for success, else false
 */
bool policy_mgr_is_dnsc_set(struct wlan_objmgr_vdev *vdev);

/**
 * policy_mgr_get_updated_scan_and_fw_mode_config() - Function
 * to get latest scan & fw config for DBS
 * @psoc: PSOC object information
 * @scan_config: DBS related scan config
 * @fw_mode_config: DBS related FW config
 * @dual_mac_disable_ini: DBS related ini config
 * @channel_select_logic_conc:
 *
 * This function returns the latest DBS configuration for
 * connection & scan, sent to FW
 *
 * Return: SUCCESS or FAILURE
 */
QDF_STATUS policy_mgr_get_updated_scan_and_fw_mode_config(
		struct wlan_objmgr_psoc *psoc, uint32_t *scan_config,
		uint32_t *fw_mode_config, uint32_t dual_mac_disable_ini,
		uint32_t channel_select_logic_conc);

/**
 * policy_mgr_is_safe_channel - Check if the channel is in LTE
 * coex channel avoidance list
 * @psoc: PSOC object information
 * @ch_freq: channel frequency to be checked
 *
 * Check if the channel is in LTE coex channel avoidance list.
 *
 * Return: true for success, else false
 */
bool policy_mgr_is_safe_channel(struct wlan_objmgr_psoc *psoc,
				uint32_t ch_freq);

#ifdef FEATURE_WLAN_CH_AVOID_EXT
/**
 * policy_mgr_restrict_sap_on_unsafe_chan() - Check if need check unsafe
 * channel if SAP start on fixed channel.
 * @psoc: PSOC object information
 *
 * Return: true for success, else false
 */
bool policy_mgr_restrict_sap_on_unsafe_chan(struct wlan_objmgr_psoc *psoc);
#else
static inline bool
policy_mgr_restrict_sap_on_unsafe_chan(struct wlan_objmgr_psoc *psoc)
{
	return false;
}
#endif

/**
 * policy_mgr_is_sap_freq_allowed - Check if the channel is allowed for sap
 * @psoc: PSOC object information
 * @opmode: Current op_mode, helps to check whether it's P2P_GO/SAP
 * @sap_freq: channel frequency to be checked
 *
 * Check the factors as below to decide whether the channel is allowed or not:
 * If the channel is in LTE coex channel avoidance list;
 * If it's STA+SAP SCC;
 * If STA+SAP SCC on LTE coex channel is allowed.
 *
 * Return: true for allowed, else false
 */
bool policy_mgr_is_sap_freq_allowed(struct wlan_objmgr_psoc *psoc,
				    enum QDF_OPMODE opmode,
				    uint32_t sap_freq);

/**
 * policy_mgr_get_ch_width() - Convert hw_mode_bandwidth to phy_ch_width
 * @bw: Hardware mode band width used by WMI
 *
 * Return: phy_ch_width
 */
enum phy_ch_width policy_mgr_get_ch_width(enum hw_mode_bandwidth bw);

/**
 * policy_mgr_is_force_scc() - checks if SCC needs to be
 * mandated
 * @psoc: PSOC object information
 *
 * This function checks if SCC needs to be mandated or not
 *
 * Return: True if SCC to be mandated, false otherwise
 */
bool policy_mgr_is_force_scc(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_go_scc_enforced() - Get GO force SCC enabled or not
 * @psoc: psoc object
 *
 * This function checks if force SCC logic should be used on GO interface.
 *
 * Return: True if allow GO force SCC
 */
bool policy_mgr_go_scc_enforced(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_valid_sap_conc_channel_check() - Check and update
 * the SAP channel in case of STA+SAP concurrency
 * @psoc: PSOC object information
 * @con_ch_freq: pointer to the chan freq on which sap will come up
 * @sap_ch_freq: initial channel frequency for SAP
 * @sap_vdev_id: sap vdev id.
 * @ch_params: sap channel parameters
 *
 * This function checks & updates the channel SAP to come up on in
 * case of STA+SAP concurrency
 *
 * Return: Success if SAP can come up on a channel
 */
QDF_STATUS policy_mgr_valid_sap_conc_channel_check(
	struct wlan_objmgr_psoc *psoc, uint32_t *con_ch_freq,
	uint32_t sap_ch_freq, uint8_t sap_vdev_id,
	struct ch_params *ch_params);

/**
 * policy_mgr_get_alternate_channel_for_sap() - Get an alternate
 * channel to move the SAP to
 * @psoc: PSOC object information
 * @sap_vdev_id: sap vdev id.
 * @sap_ch_freq: sap channel frequency.
 * @pref_band: Preferred band on channel is required
 *
 * This function returns an alternate channel for SAP to move to
 *
 * Return: The new channel for SAP
 */
uint32_t policy_mgr_get_alternate_channel_for_sap(
	struct wlan_objmgr_psoc *psoc, uint8_t sap_vdev_id,
	uint32_t sap_ch_freq,
	enum reg_wifi_band pref_band);

/**
 * policy_mgr_con_mode_by_vdev_id() - Get policy mgr con mode from vdev id
 * @psoc: psoc object
 * @vdev_id: vdev id
 *
 * return: enum policy_mgr_con_mode for the vdev id
 */
enum policy_mgr_con_mode
policy_mgr_con_mode_by_vdev_id(struct wlan_objmgr_psoc *psoc,
			       uint8_t vdev_id);

/**
 * policy_mgr_disallow_mcc() - Check for mcc
 *
 * @psoc: PSOC object information
 * @ch_freq: channel frequency on which new connection is coming up
 *
 * When a new connection is about to come up check if current
 * concurrency combination including the new connection is
 * causing MCC
 *
 * Return: True if it is causing MCC
 */
bool policy_mgr_disallow_mcc(struct wlan_objmgr_psoc *psoc,
			     uint32_t ch_freq);

/**
 * policy_mgr_mode_specific_get_channel() - Get channel for a
 * connection type
 * @psoc: PSOC object information
 * @mode: Connection type
 *
 * Get channel frequency for a connection type
 *
 * Return: channel frequency
 */
uint32_t policy_mgr_mode_specific_get_channel(struct wlan_objmgr_psoc *psoc,
					      enum policy_mgr_con_mode mode);

/**
 * policy_mgr_add_sap_mandatory_chan() - Add chan to SAP mandatory channel
 * list
 * @psoc: Pointer to soc
 * @ch_freq: Channel frequency to be added
 *
 * Add chan to SAP mandatory channel list
 *
 * Return: None
 */
void policy_mgr_add_sap_mandatory_chan(struct wlan_objmgr_psoc *psoc,
				       uint32_t ch_freq);

/**
 * policy_mgr_get_sap_mandatory_chan_list_len() - Return the SAP mandatory
 * channel list len
 * @psoc: Pointer to soc
 *
 * Get the SAP mandatory channel list len
 *
 * Return: Channel list length
 */
uint32_t policy_mgr_get_sap_mandatory_chan_list_len(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_init_sap_mandatory_chan() - Init 2.4G 5G 6G SAP mandatory channel
 * list
 * @psoc: Pointer to soc
 * @org_ch_freq: sap initial channel frequency MHz
 *
 * Initialize the 2.4G 5G 6G SAP mandatory channels
 *
 * Return: None
 */
void  policy_mgr_init_sap_mandatory_chan(struct wlan_objmgr_psoc *psoc,
					 uint32_t org_ch_freq);

/**
 * policy_mgr_remove_sap_mandatory_chan() - Remove channel from SAP mandatory
 * channel list
 * @psoc: Pointer to soc
 * @ch_freq: channel frequency to be removed from mandatory list
 *
 * Remove channel from SAP mandatory channel list
 *
 * Return: None
 */
void policy_mgr_remove_sap_mandatory_chan(struct wlan_objmgr_psoc *psoc,
					  uint32_t ch_freq);

/*
 * policy_set_cur_conc_system_pref - set current conc_system_pref
 * @psoc: soc pointer
 *
 * Set the current concurrency system preference.
 *
 * Return: None
 */
void policy_mgr_set_cur_conc_system_pref(struct wlan_objmgr_psoc *psoc,
		uint8_t conc_system_pref);
/**
 * policy_mgr_get_cur_conc_system_pref - Get current conc_system_pref
 * @psoc: soc pointer
 *
 * Get the current concurrent system preference.
 *
 * Return: conc_system_pref
 */
uint8_t policy_mgr_get_cur_conc_system_pref(struct wlan_objmgr_psoc *psoc);
/**
 * policy_mgr_check_and_stop_opportunistic_timer - Get current
 * state of opportunistic timer, if running, stop it and take
 * action
 * @psoc: soc pointer
 * @id: Session/vdev id
 *
 * Get the current state of opportunistic timer, if it is
 * running, stop it and take action.
 *
 * Return: None
 */
void policy_mgr_check_and_stop_opportunistic_timer(
	struct wlan_objmgr_psoc *psoc, uint8_t id);

/**
 * policy_mgr_set_weight_of_disabled_inactive_channels_to_zero() - set weight
 * of disabled and inactive channels to 0
 * @psoc: pointer to soc
 * @pcl: preferred channel freq list
 * @len: length of preferred channel list
 * @weight_list: preferred channel weight list
 * @weight_len: length of weight list
 * This function set the weight of disabled and inactive channels to 0
 *
 * Return: None
 */
void policy_mgr_set_weight_of_disabled_inactive_channels_to_zero(
		struct wlan_objmgr_psoc *psoc, uint32_t *pcl,
		uint32_t *len, uint8_t *weight_list, uint32_t weight_len);
/**
 * policy_mgr_is_sap_allowed_on_dfs_freq() - check if sap allowed on dfs freq
 * @pdev: id of objmgr pdev
 * @vdev_id: vdev id
 * @ch_freq: channel freq
 * This function is used to check if sta_sap_scc_on_dfs_chan ini is set,
 * DFS master capability is assumed disabled in the driver.
 *
 * Return: true if sap is allowed on dfs freq,
 * otherwise false
 */
bool policy_mgr_is_sap_allowed_on_dfs_freq(struct wlan_objmgr_pdev *pdev,
					   uint8_t vdev_id, qdf_freq_t ch_freq);
/**
 * policy_mgr_is_sta_sap_scc_allowed_on_dfs_chan() - check if sta+sap scc
 * allowed on dfs chan
 * @psoc: pointer to soc
 * This function is used to check if sta+sap scc allowed on dfs channel
 *
 * Return: true if sta+sap scc is allowed on dfs channel, otherwise false
 */
bool policy_mgr_is_sta_sap_scc_allowed_on_dfs_chan(
		struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_multi_sap_allowed_on_same_band() - check if multi sap allowed
 * on same band
 * @pdev: id of objmgr pdev
 * @mode: operating mode of interface to be checked
 * @ch_freq: channel freq
 * This function is used to check if multi sap can be started on the same band
 *
 * Return: true if multi sap is allowed on same band, otherwise false
 */
bool policy_mgr_is_multi_sap_allowed_on_same_band(
					struct wlan_objmgr_pdev *pdev,
					enum policy_mgr_con_mode mode,
					qdf_freq_t ch_freq);

/**
 * policy_mgr_is_special_mode_active_5g() - check if given mode active in 5g
 * @psoc: pointer to soc
 * @mode: operating mode of interface to be checked
 *
 * Return: true if given mode is active in 5g
 */
bool policy_mgr_is_special_mode_active_5g(struct wlan_objmgr_psoc *psoc,
					  enum policy_mgr_con_mode mode);

/**
 * policy_mgr_is_sta_connected_2g() - check if sta connected in 2g
 * @psoc: pointer to soc
 *
 * Return: true if sta is connected in 2g else false
 */
bool policy_mgr_is_sta_connected_2g(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_connected_sta_5g() - check if sta connected in 5 GHz
 * @psoc: pointer to soc
 * @freq: Pointer to the frequency on which sta is connected
 *
 * Return: true if sta is connected in 5 GHz else false
 */
bool policy_mgr_is_connected_sta_5g(struct wlan_objmgr_psoc *psoc,
				    qdf_freq_t *freq);

/**
 * policy_mgr_scan_trim_5g_chnls_for_dfs_ap() - check if sta scan should skip
 * 5g channel when dfs ap is present.
 *
 * @psoc: pointer to soc
 * @freq: DFS freq of concurrent SAP/GO
 *
 * Return: true if sta scan 5g chan should be skipped
 */
bool policy_mgr_scan_trim_5g_chnls_for_dfs_ap(struct wlan_objmgr_psoc *psoc,
					      qdf_freq_t *freq);

/**
 * policy_mgr_scan_trim_chnls_for_connected_ap() - check if sta scan
 * should skip 5g or 2.4g channel when AP/GO connected by clients.
 * STA + AP 5G (connected) + AP 2.4G		skip 5G scan
 * STA + AP 5G (connected)			skip 5G scan
 * STA + AP 2.4G (connected && 2.4G only)	skip 2.4G scan
 *
 * @pdev: pointer to pdev
 *
 * Return: trim_channel_list
 */
uint16_t
policy_mgr_scan_trim_chnls_for_connected_ap(struct wlan_objmgr_pdev *pdev);

/**
 * policy_mgr_is_hwmode_set_for_given_chnl() - to check for given channel
 * if the hw mode is properly set.
 * @psoc: pointer to psoc
 * @ch_freq: given channel frequency
 *
 * If HW mode is properly set for given channel then it returns true else
 * it returns false.
 * For example, when 2x2 DBS is supported and if the first connection is
 * coming up on 2G band then driver expects DBS HW mode to be set first
 * before the connection can be established. Driver can call this API to
 * find-out if HW mode is set properly.
 *
 * Return: true if HW mode is set properly else false
 */
bool policy_mgr_is_hwmode_set_for_given_chnl(struct wlan_objmgr_psoc *psoc,
					     uint32_t ch_freq);

/**
 * policy_mgr_get_connection_info() - Get info of all active connections
 * @psoc: pointer to soc
 * @info: Pointer to connection info
 *
 * Return: Connection count
 */
uint32_t policy_mgr_get_connection_info(struct wlan_objmgr_psoc *psoc,
					struct connection_info *info);
/**
 * policy_mgr_register_mode_change_cb() - Register mode change callback with
 * policy manager
 * @psoc: pointer to soc
 * @mode_change_cb: HDD callback to be registered
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_register_mode_change_cb(struct wlan_objmgr_psoc *psoc,
			send_mode_change_event_cb mode_change_cb);
/**
 * policy_mgr_deregister_mode_change_cb() - Deregister mode change callback with
 * policy manager
 * @psoc: pointer to soc
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_deregister_mode_change_cb(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_allow_sap_go_concurrency() - check whether SAP/GO concurrency is
 * allowed.
 * @psoc: pointer to soc
 * @mode: operating mode of interface to be checked
 * @ch_freq: new operating channel of the interface to be checked
 * @vdev_id: vdev id of the connection to be checked, 0xff for new connection
 *
 * Checks whether new channel SAP/GO can co-exist with the channel of existing
 * SAP/GO connection. This API mainly used for two purposes:
 *
 * 1) When new GO/SAP session is coming up and needs to check if this session's
 * channel can co-exist with existing existing GO/SAP sessions. For example,
 * when single radio platform comes, MCC for SAP/GO+SAP/GO is not supported, in
 * such case this API should prevent bringing the second connection.
 *
 * 2) There is already existing SAP+GO combination but due to upper layer
 * notifying LTE-COEX event or sending command to move one of the connections
 * to different channel. In such cases before moving existing connection to new
 * channel, check if new channel can co-exist with the other existing
 * connection. For example, one SAP1 is on channel-6 and second SAP2 is on
 * channel-36 and lets say they are doing DBS, and lets say upper layer sends
 * LTE-COEX to move SAP1 from channel-6 to channel-149. In this case, SAP1 and
 * SAP2 will end up doing MCC which may not be desirable result. such cases
 * will be prevented with this API.
 *
 * Return: true or false
 */
bool policy_mgr_allow_sap_go_concurrency(struct wlan_objmgr_psoc *psoc,
					 enum policy_mgr_con_mode mode,
					 uint32_t ch_freq,
					 uint32_t vdev_id);

/**
 * policy_mgr_dual_beacon_on_single_mac_scc_capable() - get capability that
 * whether support dual beacon on same channel on single MAC
 * @psoc: pointer to soc
 *
 *  Return: bool: capable
 */
bool policy_mgr_dual_beacon_on_single_mac_scc_capable(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_dual_beacon_on_single_mac_mcc_capable() - get capability that
 * whether support dual beacon on different channel on single MAC
 * @psoc: pointer to soc
 *
 *  Return: bool: capable
 */
bool policy_mgr_dual_beacon_on_single_mac_mcc_capable(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_sta_sap_scc_on_lte_coex_chan() - get capability that
 * whether support sta sap scc on lte coex chan
 * @psoc: pointer to soc
 *
 *  Return: bool: capable
 */
bool policy_mgr_sta_sap_scc_on_lte_coex_chan(
	struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_user_config_sap_freq() - Get the user configured channel
 *
 * @psoc: pointer to psoc
 * @vdev_id: vdev id
 *
 * Return: user configured frequency
 */
qdf_freq_t policy_mgr_get_user_config_sap_freq(struct wlan_objmgr_psoc *psoc,
					       uint8_t vdev_id);

/**
 * policy_mgr_nan_sap_post_enable_conc_check() - Do concurrency operations
 *                                               post nan/sap enable
 * @psoc: pointer to psoc
 *
 * Return: QDF_STATUS
 **/
QDF_STATUS
policy_mgr_nan_sap_post_enable_conc_check(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_nan_sap_post_disable_conc_check() - Do concurrency related
 *                                                operation post nan/sap disable
 * @psoc: pointer to psoc
 *
 * Return: void
 **/
void policy_mgr_nan_sap_post_disable_conc_check(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_sap_restart_required_after_sta_disconnect() - is sap restart
 * required
 * after sta disconnection
 * @psoc: psoc object data
 * @sap_vdev_id: sap vdev id
 * @intf_ch_freq: sap channel frequency
 * @is_acs_mode: Indicates whether SAP is started in ACS mode or fixed channel
 *
 * Check if SAP should be moved to a non dfs channel after STA disconnection.
 * This API applicable only for STA+SAP SCC and ini 'sta_sap_scc_on_dfs_chan'
 * or 'sta_sap_scc_on_lte_coex_chan' is enabled.
 *
 * Return: true if sap restart is required, otherwise false
 */
bool policy_mgr_is_sap_restart_required_after_sta_disconnect(
			struct wlan_objmgr_psoc *psoc, uint32_t sap_vdev_id,
			uint32_t *intf_ch_freq, bool is_acs_mode);

/**
 * policy_mgr_is_sta_sap_scc() - check whether SAP is doing SCC with
 * STA
 * @psoc: pointer to psoc
 * @sap_ch_freq: operating channel frequency of SAP interface
 * This function checks whether SAP is doing SCC with STA
 *
 * Return: true or false
 */
bool policy_mgr_is_sta_sap_scc(struct wlan_objmgr_psoc *psoc,
			       uint32_t sap_ch_freq);

/**
 * policy_mgr_nan_sap_scc_on_unsafe_ch_chk() - check whether SAP is doing SCC
 *                                             with NAN
 * @psoc: pointer to psoc
 * @sap_freq: operating channel frequency of SAP interface
 *
 * Return: true or false
 */
bool policy_mgr_nan_sap_scc_on_unsafe_ch_chk(struct wlan_objmgr_psoc *psoc,
					     uint32_t sap_freq);

/**
 * policy_mgr_get_hw_mode_from_idx() - Get HW mode based on index
 * @psoc: psoc object
 * @idx: HW mode id
 * @hw_mode: HW mode params
 *
 * Fetches the HW mode parameters
 *
 * Return: Success if hw mode is obtained and the hw mode params
 */
QDF_STATUS policy_mgr_get_hw_mode_from_idx(
		struct wlan_objmgr_psoc *psoc,
		uint32_t idx,
		struct policy_mgr_hw_mode_params *hw_mode);

#if defined(CONFIG_BAND_6GHZ) && defined(WLAN_FEATURE_11AX)
/**
 * policy_mgr_is_6ghz_conc_mode_supported() - Check connection mode supported
 * on 6ghz or not
 * @psoc: Pointer to soc
 * @mode: new connection mode
 *
 * Current PORed 6ghz connection modes are STA, SAP, P2P.
 *
 * Return: true if supports else false.
 */
bool policy_mgr_is_6ghz_conc_mode_supported(
	struct wlan_objmgr_psoc *psoc, enum policy_mgr_con_mode mode);

/**
 * policy_mgr_init_ap_6ghz_capable - Init 6Ghz capable flags
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @ap_6ghz_capable: vdev 6ghz capable flag
 *
 * Init 6Ghz capable flags for active connection in policy mgr conn table
 *
 * Return: void
 */
void policy_mgr_init_ap_6ghz_capable(struct wlan_objmgr_psoc *psoc,
				     uint8_t vdev_id,
				     enum conn_6ghz_flag ap_6ghz_capable);

/**
 * policy_mgr_set_ap_6ghz_capable - Set 6Ghz capable flags to connection list
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @set: set or clear
 * @ap_6ghz_capable: vdev 6ghz capable flag
 *
 * Set/Clear 6Ghz capable flags for active connection in policy mgr conn table
 *
 * Return: void
 */
void policy_mgr_set_ap_6ghz_capable(struct wlan_objmgr_psoc *psoc,
				    uint8_t vdev_id,
				    bool set,
				    enum conn_6ghz_flag ap_6ghz_capable);

/**
 * policy_mgr_get_ap_6ghz_capable - Get 6Ghz capable info for a vdev
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 * @conn_flag: output conntion flags
 *
 * Get 6Ghz capable flag for ap vdev (SAP). When SAP on 5G, for same reason
 * the AP needs to be moved to 6G and this API will be called to check whether
 * AP is 6Ghz capable or not.
 * AP is allowed on 6G band only when all of below statements are true:
 * a. SAP config includes WPA3 security - SAE,OWE,SuiteB.
 * b. SAP is configured by ACS range which includes any 6G channel or
 *    configured by 6G Fixed channel.
 * c. SAP has no legacy clients (client doesn't support 6G band).
 *    legacy client (non 6ghz capable): association request frame has no
 *    6G band global operating Class.
 *
 * Return: true if AP is 6ghz capable
 */
bool policy_mgr_get_ap_6ghz_capable(
	struct wlan_objmgr_psoc *psoc, uint8_t vdev_id, uint32_t *conn_flag);
#else
static inline bool policy_mgr_is_6ghz_conc_mode_supported(
	struct wlan_objmgr_psoc *psoc, enum policy_mgr_con_mode mode)
{
	return false;
}

static inline void policy_mgr_init_ap_6ghz_capable(
	struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
	enum conn_6ghz_flag ap_6ghz_capable)
{}

static inline
void policy_mgr_set_ap_6ghz_capable(struct wlan_objmgr_psoc *psoc,
				    uint8_t vdev_id,
				    bool set,
				    enum conn_6ghz_flag ap_6ghz_capable)
{}

static inline bool policy_mgr_get_ap_6ghz_capable(
	struct wlan_objmgr_psoc *psoc, uint8_t vdev_id, uint32_t *conn_flag)
{
	return false;
}

#endif

/**
 * policy_mgr_update_nan_vdev_mac_info() - Update the NAN vdev id and MAC id in
 * policy manager
 * @psoc: psoc object
 * @nan_vdev_id: NAN Discovery pseudo vdev id
 * @mac_id: NAN Discovery MAC ID
 *
 * Stores NAN Discovery related vdev and MAC id in policy manager
 *
 * Return: QDF Success
 */
QDF_STATUS policy_mgr_update_nan_vdev_mac_info(struct wlan_objmgr_psoc *psoc,
					       uint8_t nan_vdev_id,
					       uint8_t mac_id);

/**
 * policy_mgr_get_mode_specific_conn_info() - Get active mode specific
 * channel and vdev id
 * @psoc: PSOC object information
 * @ch_freq_list: Mode specific channel freq list
 * @vdev_id: Mode specific vdev id (list)
 * @mode: Connection Mode
 *
 * Get active mode specific channel and vdev id
 *
 * Return: number of connection found as per given mode
 */
uint32_t policy_mgr_get_mode_specific_conn_info(struct wlan_objmgr_psoc *psoc,
						uint32_t *ch_freq_list,
						uint8_t *vdev_id,
						enum policy_mgr_con_mode mode);

/*
 * policy_mgr_get_ml_and_non_ml_sta_count() - get ML and non ML STA count
 * also fills the freq and non ML/ML list
 * @psoc: Objmgr psoc
 * @num_ml: num ML as output
 * @ml_idx: ML vdev index as output
 * @num_non_ml: num non ML as output
 * @non_ml_idx: non ML vdev index as output
 * @freq_list: freq list of each sta vdev
 * @vdev_id_list: vdev id list
 *
 * Return: void
 */
void policy_mgr_get_ml_and_non_ml_sta_count(struct wlan_objmgr_psoc *psoc,
					    uint8_t *num_ml, uint8_t *ml_idx,
					    uint8_t *num_non_ml,
					    uint8_t *non_ml_idx,
					    qdf_freq_t *freq_list,
					    uint8_t *vdev_id_list);

/**
 * policy_mgr_is_sap_go_on_2g() - check if sap/go is on 2g
 * @psoc: PSOC object information
 *
 * Return: true or false
 */
bool policy_mgr_is_sap_go_on_2g(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_dump_channel_list() - Print channel list
 * @len: Length of pcl list
 * @pcl_channels: pcl channels list
 * @pcl_weight: pcl weight list
 *
 *
 * Return: True or false
 */
bool policy_mgr_dump_channel_list(uint32_t len,
				  uint32_t *pcl_channels,
				  uint8_t *pcl_weight);

/**
 * policy_mgr_filter_passive_ch() -filter out passive channels from the list
 * @pdev: Pointer to pdev
 * @ch_freq_list: pointer to channel frequency list
 * @ch_cnt: number of channels in list
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_filter_passive_ch(struct wlan_objmgr_pdev *pdev,
					uint32_t *ch_freq_list,
					uint32_t *ch_cnt);

/**
 * policy_mgr_is_restart_sap_required() - check whether sap need restart
 * @psoc: psoc pointer
 * @vdev_id: vdev id
 * @freq: sap current freq
 * @scc_mode: mcc to scc switch mode
 *
 * If there is no STA/P2P CLI on same MAC of SAP/P2P GO,
 * SAP/P2P Go needn't switch channel to force scc.
 *
 * Return: True or false
 */
bool policy_mgr_is_restart_sap_required(struct wlan_objmgr_psoc *psoc,
					uint8_t vdev_id,
					qdf_freq_t freq,
					tQDF_MCC_TO_SCC_SWITCH_MODE scc_mode);

/**
 * policy_mgr_get_roam_enabled_sta_session_id() - get the session id of the sta
 * on which roaming is enabled.
 * @psoc: pointer to psoc object
 * @vdev_id: vdev id of the requestor
 *
 * The function checks if any sta(other than the provided vdev_id) is present
 * and has roaming enabled and return the session id of the sta with roaming
 * enabled else if roaming is not enabled on any STA return
 * WLAN_UMAC_VDEV_ID_MAX.
 *
 * Return: session id of STA on which roaming is enabled
 */
uint8_t policy_mgr_get_roam_enabled_sta_session_id(
						struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id);

/**
 * policy_mgr_is_sta_mon_concurrency() - check if MONITOR and STA concurrency
 * is UP.
 * @psoc: pointer to psoc object
 *
 * Return: True - if STA and monitor concurrency is there, else False
 *
 */
bool policy_mgr_is_sta_mon_concurrency(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_check_mon_concurrency() - Checks if monitor intf can be added.
 * @psoc: pointer to psoc object
 *
 * Return: QDF_STATUS_SUCCESS if allowed, else send failure
 *
 */
QDF_STATUS policy_mgr_check_mon_concurrency(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_hw_dbs_max_bw() - Computes DBS BW
 * @psoc: PSOC object information
 * @bw_dbs: BW info of both MAC0 and MAC1
 * This function computes BW info of both MAC0 and MAC1
 *
 * Return: void
 */
void policy_mgr_get_hw_dbs_max_bw(struct wlan_objmgr_psoc *psoc,
				  struct dbs_bw *bw_dbs);

/**
 * policy_mgr_get_radio_combinations() - Query the supported radio combinations
 * @psoc: soc object
 * @comb: combination buffer
 * @comb_max: max combination number can be saved to comb buffer
 * @comb_num: returned combination number
 *
 * This function returns the radio combination information supported by target.
 *
 * Return: QDF_STATUS_SUCCESS if query successfully
 */
QDF_STATUS policy_mgr_get_radio_combinations(struct wlan_objmgr_psoc *psoc,
					     struct radio_combination *comb,
					     uint32_t comb_max,
					     uint32_t *comb_num);

/**
 * policy_mgr_is_mlo_sta_disconnected() - Check all STA in mlo are disconnected
 * @psoc: PSOC object information
 * @vdev_id: vdev id for sta
 *
 * if any link is associated the API will return false.
 *
 * Return: True if all sta links are disconnected
 */
bool policy_mgr_is_mlo_sta_disconnected(struct wlan_objmgr_psoc *psoc,
					uint8_t vdev_id);

#ifdef WLAN_FEATURE_11BE_MLO
/**
 * policy_mgr_is_ml_sta_links_in_mcc() - Check ML links are in MCC or not
 * @psoc: psoc ctx
 * @ml_freq_lst: ML STA freq list
 * @ml_vdev_lst: ML STA vdev id list
 * @ml_linkid_lst: ML STA link id list
 * @num_ml_sta: Number of total ML STA links
 * @affected_linkid_bitmap: link id bitmap which home channels are in MCC
 * with each other
 *
 * Return: true if ML link in MCC else false
 */
bool
policy_mgr_is_ml_sta_links_in_mcc(struct wlan_objmgr_psoc *psoc,
				  qdf_freq_t *ml_freq_lst,
				  uint8_t *ml_vdev_lst,
				  uint8_t *ml_linkid_lst,
				  uint8_t num_ml_sta,
				  uint32_t *affected_linkid_bitmap);

/**
 * policy_mgr_is_ml_links_in_mcc_allowed() - Check ML links are in MCC or not
 * @psoc: psoc ctx
 * @vdev: Pointer to vdev object
 * @ml_sta_vdev_lst: ML STA vdev id list
 * @num_ml_sta: Number of total ML STA links
 *
 * Return: QDF_STATUS_SUCCESS if ML link in MCC is allowed
 */
QDF_STATUS
policy_mgr_is_ml_links_in_mcc_allowed(struct wlan_objmgr_psoc *psoc,
				      struct wlan_objmgr_vdev *vdev,
				      uint8_t *ml_sta_vdev_lst,
				      uint8_t *num_ml_sta);

/**
 * policy_mgr_is_vdev_high_tput_or_low_latency() - Check vdev has
 * high througput or low latency flag
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 *
 * Return: true if vdev has high throughput or low latency flag
 */
bool
policy_mgr_is_vdev_high_tput_or_low_latency(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id);

/**
 * policy_mgr_check_2ghz_only_sap_affected_link() - Check force inactive
 * link is needed for 2.4 GHz only sap
 * @psoc: PSOC object information
 * @sap_vdev_id: sap vdev id
 * @sap_ch_freq: sap channel frequency
 * @ml_ch_freq_num: ML STA link num
 * @ml_freq_lst: ML STA link frequency list
 *
 * Return: true if 2.4 GHz only sap present and need to force inactive
 * ML link
 */
bool
policy_mgr_check_2ghz_only_sap_affected_link(
			struct wlan_objmgr_psoc *psoc,
			uint8_t sap_vdev_id,
			qdf_freq_t sap_ch_freq,
			uint8_t ml_ch_freq_num,
			qdf_freq_t *ml_freq_lst);

/**
 * policy_mgr_vdev_is_force_inactive() - Check force inactive or not
 * for the vdev id
 * @psoc: PSOC object information
 * @vdev_id: vdev id
 *
 * Return: true if the vdev is in force inactive
 */
bool policy_mgr_vdev_is_force_inactive(struct wlan_objmgr_psoc *psoc,
				       uint8_t vdev_id);

/**
 * policy_mgr_get_legacy_conn_info() - Get legacy connection info
 * @psoc: PSOC object information
 * @vdev_lst: vdev id list
 * @freq_lst: channel frequency list
 * @mode_lst: vdev mode list
 * @lst_sz: array size of above parameters
 *
 * This API will return the legacy STA/SAP/P2P connection info.
 * If a connection want to avoid MCC with ML STA, that connection
 * will be put in head of array list. And in 3 Port concurrency
 * case (ML STA + 2 legacy Connections), usually we can only meet
 * the high priority connection's MCC avoidance, so this API will
 * return sorted lists based on the priority. Right now we don't
 * clear requirement on which legacy interface has higher priority,
 * here we follow this order: STA, SAP, P2P.
 *
 * Return: number of legacy connection count
 */
uint8_t
policy_mgr_get_legacy_conn_info(struct wlan_objmgr_psoc *psoc,
				uint8_t *vdev_lst,
				qdf_freq_t *freq_lst,
				enum policy_mgr_con_mode *mode_lst,
				uint8_t lst_sz);

/*
 * policy_mgr_get_ml_sta_info_psoc() - Get number of ML STA vdev ids and
 * freq list
 * @pm_ctx: pm_ctx ctx
 * @num_ml_sta: Return number of ML STA present
 * @num_disabled_ml_sta: Return number of disabled ML STA links
 * @ml_vdev_lst: Return ML STA vdev id list
 * @ml_freq_lst: Return ML STA freq list
 * @num_non_ml: Return number of non-ML STA present
 * @non_ml_vdev_lst: Return non-ML STA vdev id list
 * @non_ml_freq_lst: Return non-ML STA freq list
 *
 * Return: void
 */
void
policy_mgr_get_ml_sta_info_psoc(struct wlan_objmgr_psoc *psoc,
				uint8_t *num_ml_sta,
				uint8_t *num_disabled_ml_sta,
				uint8_t *ml_vdev_lst,
				qdf_freq_t *ml_freq_lst,
				uint8_t *num_non_ml,
				uint8_t *non_ml_vdev_lst,
				qdf_freq_t *non_ml_freq_lst);

/**
 * policy_mgr_handle_link_removal_on_vdev() - Handle AP link removal for
 * MLO STA
 * @vdev: objmgr vdev
 *
 * Handle link removal for STA vdev:
 * Send force link command to target if MLO STA link number > 1.
 * Select other inactive link to active if possible.
 *
 * Return: void
 */
void policy_mgr_handle_link_removal_on_vdev(struct wlan_objmgr_vdev *vdev);

/**
 * policy_mgr_handle_link_removal_on_standby() - Handle AP link removal for
 * MLO STA standby links
 * @vdev: objmgr vdev
 * @reconfig_info: link reconfig info
 *
 * Handle link removal for ML STA standby links:
 * Send force link command to target with link removal reason code
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_handle_link_removal_on_standby(struct wlan_objmgr_vdev *vdev,
					  struct ml_rv_info *reconfig_info);

/**
 * policy_mgr_is_mlo_sap_concurrency_allowed() - Check for mlo sap allowed
 *                                               concurrency combination
 * @psoc: PSOC object information
 * @is_new_vdev_mlo: Is new vdev a mlo device or not
 * @new_vdev_id: new vdev id which need concurrency check
 *
 * When a new connection is about to come up check if current
 * concurrency combination including the new connection is
 * allowed or not. Currently no concurrency support for mlo sap
 *
 * Return: True if concurrency is supported, otherwise false.
 */
bool policy_mgr_is_mlo_sap_concurrency_allowed(struct wlan_objmgr_psoc *psoc,
					       bool is_new_vdev_mlo,
					       uint8_t new_vdev_id);

/**
 * policy_mgr_get_conc_ext_flags() - get extended flags for concurrency check
 * @vdev: pointer to vdev on which new connection is coming up
 * @force_mlo: true means it's a MLO connection, false means uncertain
 *
 * In some scenario the flag WLAN_VDEV_FEXT2_MLO may not set for vdev when
 * checking concurrency, then caller can set force_mlo accordingly to get
 * proper extended flags.
 *
 * Return: extended flags for concurrency check
 */
uint32_t
policy_mgr_get_conc_ext_flags(struct wlan_objmgr_vdev *vdev, bool force_mlo);

/**
 * policy_mgr_is_non_ml_sta_present() - Check whether Non-ML STA is present
 * @psoc: PSOC object information
 *
 * Return: True if non-ML STA is present, otherwise false.
 */
bool policy_mgr_is_non_ml_sta_present(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_mlo_sta_present() - Check whether MLO STA is present
 * @psoc: PSOC object information
 *
 * Return: True if MLO STA is present, otherwise false.
 */
bool policy_mgr_is_mlo_sta_present(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_is_mlo_in_mode_sbs() - Check whether MLO present is SBS (with both
 * links on 5/6 ghz band)
 * @psoc: PSOC object information
 * @mode: mlo mode to check
 * @mlo_vdev_lst: Pointer to mlo vdev list, this function will fill this with
 *                list of mlo vdev
 * @num_mlo: Pointer to number of mlo link, this function will fill this with
 *           number of mlo links
 *
 * Return: True if MLO is present with both links on 5 and 6ghz band
 */
bool policy_mgr_is_mlo_in_mode_sbs(struct wlan_objmgr_psoc *psoc,
				   enum policy_mgr_con_mode mode,
				   uint8_t *mlo_vdev_lst, uint8_t *num_mlo);

/**
 * policy_mgr_is_mlo_in_mode_dbs() - Check whether MLO present is DBS
 * @psoc: PSOC object information
 * @mode: mlo mode to check
 * @mlo_vdev_lst: Pointer to mlo vdev list, this function will fill this with
 *                list of mlo vdev
 * @num_mlo: Pointer to number of mlo link, this function will fill this with
 *           number of mlo links
 *
 * Return: True if MLO one link is on 2 GHz band and other links on
 * 5/6 GHz band
 */
bool policy_mgr_is_mlo_in_mode_dbs(struct wlan_objmgr_psoc *psoc,
				   enum policy_mgr_con_mode mode,
				   uint8_t *mlo_vdev_lst, uint8_t *num_mlo);

/**
 * policy_mgr_is_mlo_in_mode_emlsr() - Check whether current connection is eMLSR
 * @psoc: PSOC object information
 * @mlo_vdev_lst: Pointer to mlo vdev list, this function will fill this with
 *                list of mlo vdev
 * @num_mlo: Pointer to number of mlo link, this function will fill this with
 *           number of mlo links
 *
 * Return: True if current connection is in eMLSR mode i.e. Both STA and AP
 *         support eMLSR connection along with vendor command selection
 */
bool policy_mgr_is_mlo_in_mode_emlsr(struct wlan_objmgr_psoc *psoc,
				     uint8_t *mlo_vdev_lst, uint8_t *num_mlo);

/**
 * policy_mgr_handle_ml_sta_links_on_vdev_up_csa() - Handle enable/disable
 * link on vdev UP and channel change
 * @psoc: objmgr psoc
 * @mode: mode of vdev that went UP or changed channel
 * @vdev_id: vdev_id which went UP or changed channel
 *
 * Return: void
 */
void
policy_mgr_handle_ml_sta_links_on_vdev_up_csa(struct wlan_objmgr_psoc *psoc,
					      enum QDF_OPMODE mode,
					      uint8_t vdev_id);

/**
 * policy_mgr_handle_ml_sta_link_on_traffic_type_change() - Handle
 * enable/disable link on vdev traffic type change on SAP/P2P vdev
 * @psoc: objmgr psoc
 * @vdev: vdev on which traffic type change
 *
 * Context: Should be called only from north bound context and never from
 * schedular thread as it has wait for completed.
 *
 * Return: void
 */
void policy_mgr_handle_ml_sta_link_on_traffic_type_change(
						struct wlan_objmgr_psoc *psoc,
						struct wlan_objmgr_vdev *vdev);

/**
 * policy_mgr_handle_ml_sta_links_on_vdev_down() - Handle enable
 * link on any vdev down
 * @psoc: objmgr psoc
 * @mode: mode of vdev that went down
 * @vdev_id: vdev_id which went down
 *
 * Return: void
 */
void policy_mgr_handle_ml_sta_links_on_vdev_down(struct wlan_objmgr_psoc *psoc,
						 enum QDF_OPMODE mode,
						 uint8_t vdev_id);

/**
 * policy_mgr_handle_emlsr_sta_concurrency() - Handle concurrency scenarios with
 * EMLSR STA.
 * @psoc: objmgr psoc
 * @conc_con_coming_up: Indicates if any concurrent connection is coming up
 * @emlsr_sta_coming_up: Carries true when eMLSR STA is coming up.
 *			 Carries true when an unsupported concurrency is
 *			 gone, so that host can let firmware go to eMLSR mode.
 *
 * The API handles concurrency scenarios with existing EMLSR connection when a
 * new connection request is received OR with an existing legacy connection when
 * an EMLSR sta comes up.
 *
 * Return: none
 */
void policy_mgr_handle_emlsr_sta_concurrency(struct wlan_objmgr_psoc *psoc,
					     bool conc_con_coming_up,
					     bool emlsr_sta_coming_up);

/**
 * policy_mgr_clear_ml_links_settings_in_fw() - Process
 * QCA_WLAN_VENDOR_ATTR_LINK_STATE_CONTROL_MODE in default mode
 * @psoc: objmgr psoc
 * @vdev_id: vdev_id
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_clear_ml_links_settings_in_fw(struct wlan_objmgr_psoc *psoc,
					 uint8_t vdev_id);

/**
 * policy_mgr_activate_mlo_links_nlink() - Force active ML links based on user
 * requested link mac address with link bitmap
 * @psoc: objmgr psoc
 * @session_id: session id
 * @num_links: number of links to be forced active
 * @active_link_addr: link mac address of links to be forced active
 *
 * Return: void
 */
void policy_mgr_activate_mlo_links_nlink(struct wlan_objmgr_psoc *psoc,
					 uint8_t session_id, uint8_t num_links,
					 struct qdf_mac_addr *active_link_addr);

/**
 * policy_mgr_activate_mlo_links() - Force active ML links based on user
 * requested link mac address with vdev bitmap
 * @psoc: objmgr psoc
 * @session_id: session id
 * @num_links: number of links to be forced active
 * @active_link_addr: link mac address of links to be forced active
 *
 * Return: void
 */
void policy_mgr_activate_mlo_links(struct wlan_objmgr_psoc *psoc,
				   uint8_t session_id, uint8_t num_links,
				   struct qdf_mac_addr *active_link_addr);

/**
 * policy_mgr_update_mlo_links_based_on_linkid() - Force active ML links based
 * on user requested coming via QCA_NL80211_VENDOR_SUBCMD_MLO_LINK_STATE
 * @psoc: objmgr psoc
 * @vdev_id: vdev id
 * @num_links: number of links to be forced active
 * @link_id_list: link id(s) list coming from user space
 * @config_state_list: config state list coming from user space
 *
 * Return: success if the command gets processed successfully
 */
QDF_STATUS
policy_mgr_update_mlo_links_based_on_linkid(struct wlan_objmgr_psoc *psoc,
					    uint8_t vdev_id,
					    uint8_t num_links,
					    uint8_t *link_id_list,
					    uint32_t *config_state_list);

/**
 * policy_mgr_update_active_mlo_num_links() - Force active ML links based
 * on user requested coming via LINK_STATE_MIXED_MODE_ACTIVE_NUM_LINKS
 * @psoc: objmgr psoc
 * @vdev_id: vdev id
 * @num_links: number of links to be forced active
 *
 * Return: success if the command gets processed successfully
 */
QDF_STATUS policy_mgr_update_active_mlo_num_links(struct wlan_objmgr_psoc *psoc,
						  uint8_t vdev_id,
						  uint8_t num_links);
#else
static inline bool
policy_mgr_vdev_is_force_inactive(struct wlan_objmgr_psoc *psoc,
				  uint8_t vdev_id)
{
	return false;
}

static inline bool policy_mgr_is_mlo_sap_concurrency_allowed(
			struct wlan_objmgr_psoc *psoc,
			bool is_new_vdev_mlo,
			uint8_t new_vdev_id)
{
	return true;
}

static inline uint32_t
policy_mgr_get_conc_ext_flags(struct wlan_objmgr_vdev *vdev, bool force_mlo)
{
	return 0;
}

static inline bool
policy_mgr_is_non_ml_sta_present(struct wlan_objmgr_psoc *psoc)
{
	return true;
}

static inline bool policy_mgr_is_mlo_sta_present(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline
bool policy_mgr_is_mlo_in_mode_sbs(struct wlan_objmgr_psoc *psoc,
				   enum policy_mgr_con_mode mode,
				   uint8_t *mlo_vdev_lst, uint8_t *num_mlo)
{
	return false;
}

static inline
bool policy_mgr_is_mlo_in_mode_dbs(struct wlan_objmgr_psoc *psoc,
				   enum policy_mgr_con_mode mode,
				   uint8_t *mlo_vdev_lst, uint8_t *num_mlo)
{
	return false;
}

static inline void
policy_mgr_handle_ml_sta_links_on_vdev_up_csa(struct wlan_objmgr_psoc *psoc,
					      enum QDF_OPMODE mode,
					      uint8_t vdev_id)
{
}

static inline void
policy_mgr_handle_ml_sta_link_on_traffic_type_change(
						struct wlan_objmgr_psoc *psoc,
						struct wlan_objmgr_vdev *vdev)
{
}

static inline
bool policy_mgr_is_mlo_in_mode_emlsr(struct wlan_objmgr_psoc *psoc,
				     uint8_t *mlo_vdev_lst, uint8_t *num_mlo)
{
	return false;
}

static inline
void policy_mgr_handle_ml_sta_links_on_vdev_down(struct wlan_objmgr_psoc *psoc,
						 enum QDF_OPMODE mode,
						 uint8_t vdev_id)
{
}
#endif

/**
 * policy_mgr_is_hwmode_offload_enabled() - Check for HW mode selection offload
 *  support
 * @psoc: PSOC object information
 *
 * Check if target supports HW mode selection offload or not
 *
 * Return: True if the target supports HW mode selection offload,
 *         False otherwise.
 */
bool policy_mgr_is_hwmode_offload_enabled(struct wlan_objmgr_psoc *psoc);
/**
 * policy_mgr_is_3rd_conn_on_same_band_allowed() - Check the third connection
 * on same band allowed or not
 * list for third connection
 * @psoc: PSOC object information
 * @mode: Device mode
 * @ch_freq: 3rd channel frequency
 *
 * This function checks whether to allow third connection on same band or not
 * based on pcl table
 *
 * Return: TRUE/FALSE
 */
bool policy_mgr_is_3rd_conn_on_same_band_allowed(struct wlan_objmgr_psoc *psoc,
						 enum policy_mgr_con_mode mode,
						 qdf_freq_t ch_freq);

/**
 * policy_mgr_get_connected_roaming_vdev_band_mask() - get connected vdev
 * band mask
 * @psoc: PSOC object
 * @vdev_id: Vdev id
 *
 * Return: reg wifi band mask
 */
uint32_t
policy_mgr_get_connected_roaming_vdev_band_mask(struct wlan_objmgr_psoc *psoc,
						uint8_t vdev_id);

/**
 * policy_mgr_is_sta_chan_valid_for_connect_and_roam  - Check if given
 * channel is valid for STA connection/roam pcl channels
 * @pdev: pdev
 * @freq: frequency
 *
 * Return: true if channel is valid else false
 */
bool policy_mgr_is_sta_chan_valid_for_connect_and_roam(
					struct wlan_objmgr_pdev *pdev,
					qdf_freq_t freq);

/**
 * policy_mgr_is_ap_ap_mcc_allow() - Check AP AP MCC allow or not
 * @psoc: psoc object
 * @pdev: pdev object
 * @vdev: vdev object of new SAP or P2P GO
 * @ch_freq: channel frequency of up coming vdev
 * @ch_width: channel width
 * @con_vdev_id: concurrent SAP/GO vdev id
 * @con_freq: concurrent SAP/GO home channel.
 *
 * Check if AP AP MCC allow or not when new SAP or P2P GO creating.
 * If not allowed, the concurrency SAP/GO vdev and channel will
 * be returned.
 *
 * Return: True if the target allow AP AP MCC,
 *         False otherwise.
 */
bool policy_mgr_is_ap_ap_mcc_allow(struct wlan_objmgr_psoc *psoc,
				   struct wlan_objmgr_pdev *pdev,
				   struct wlan_objmgr_vdev *vdev,
				   uint32_t ch_freq,
				   enum phy_ch_width ch_width,
				   uint8_t *con_vdev_id,
				   uint32_t *con_freq);

/**
 * policy_mgr_any_other_vdev_on_same_mac_as_freq() - Function to check
 * whether more than one vdev are present on same mac or not
 * @psoc: PSOC object
 * @freq: Channel frequency
 * @vdev_id: Vdev id
 *
 * Return: True if more than one vdev are present on same mac
 *
 */
bool policy_mgr_any_other_vdev_on_same_mac_as_freq(
				struct wlan_objmgr_psoc *psoc,
				uint32_t freq, uint8_t vdev_id);

/**
 * policy_mgr_get_sbs_cfg() - Get SBS INI value
 * @psoc: PSOC object
 * @sbs: output sbs cfg value
 *
 */
QDF_STATUS policy_mgr_get_sbs_cfg(struct wlan_objmgr_psoc *psoc, bool *sbs);

/**
 * policy_mgr_get_ll_sap_freq()- Function to get ll sap freq if it's present
 * @psoc: PSOC object
 *
 * Based on vdev id ap profile set via vendor command is get and compared with
 * ll_type_any AP type and return freq for that SAP if profile set is latency
 * sensitive or throghput sensitive.
 *
 * Return: freq if it's LL SAP otherwise 0
 *
 */
qdf_freq_t policy_mgr_get_ll_sap_freq(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_ll_lt_sap_freq()- Function to get LT LL sap freq if it's
 * present
 * @psoc: PSOC object
 *
 * Based on vdev id ap profile set via vendor command is get and compared with
 * lt_ll_type AP type and return freq for that SAP if profile set is latency
 * sensitive example gaming or losless audio.
 *
 * Return: freq if it's LT LL SAP otherwise 0
 *
 */
qdf_freq_t policy_mgr_get_ll_lt_sap_freq(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_ll_ht_sap_freq()- Function to get LL HT sap freq if it's
 * present
 * @psoc: PSOC object
 *
 * Based on vdev id ap profile set via vendor command is get and compared with
 * ll_ht_type AP type and return freq for that SAP if profile set is throghput
 * sensitive.
 *
 * Return: freq if it's HT LL SAP otherwise 0
 *
 */
qdf_freq_t policy_mgr_get_ll_ht_sap_freq(struct wlan_objmgr_psoc *psoc);

#ifndef WLAN_FEATURE_LL_LT_SAP
/**
 * policy_mgr_is_ll_sap_concurrency_valid() - Function to check whether
 * low latency SAP + STA/SAP/GC/GO concurrency allowed or not
 * @psoc: PSOC object
 * @freq: Channel frequency
 * @mode: Device mode
 *
 * Return: True if concurrency are allowed otherwise false
 *
 */
bool policy_mgr_is_ll_sap_concurrency_valid(struct wlan_objmgr_psoc *psoc,
					    qdf_freq_t freq,
					    enum policy_mgr_con_mode mode);
#else
static inline
bool policy_mgr_is_ll_sap_concurrency_valid(struct wlan_objmgr_psoc *psoc,
					    qdf_freq_t freq,
					    enum policy_mgr_con_mode mode)
{
	return true;
}
#endif
/**
 * policy_mgr_update_indoor_concurrency() - Function to update the indoor
 * concurrency related regulatory changes
 *
 * @psoc: pointer to psoc
 * @vdev_id: vdev id
 * @discon_freq: disconnect frequency
 * @type: enum indoor_conc_update_type
 *
 * Return: True if need to compute pdev current channel list
 */
bool
policy_mgr_update_indoor_concurrency(struct wlan_objmgr_psoc *psoc,
				     uint8_t vdev_id,
				     uint32_t discon_freq,
				     enum indoor_conc_update_type type);
/**
 * policy_mgr_is_conc_sap_present_on_sta_freq() - Function to check if
 * SAP or GO exists on the STA frequency
 *
 * @psoc: pointer to psoc
 * @mode: interface mode
 * @ch_freq: channel frequency
 *
 * Return: AP mode exists
 */
bool
policy_mgr_is_conc_sap_present_on_sta_freq(struct wlan_objmgr_psoc *psoc,
					   enum policy_mgr_con_mode mode,
					   uint32_t ch_freq);

/**
 * policy_mgr_get_connection_count_with_ch_freq() - Get number of active
 * connections on the channel frequecy
 * @ch_freq: channel frequency
 *
 * Return: number of active connection on the specific frequency
 */
uint32_t policy_mgr_get_connection_count_with_ch_freq(uint32_t ch_freq);

/**
 * policy_mgr_is_sap_go_interface_allowed_on_indoor() - Check if SAP or GO
 * interface is allowed on the indoor channel
 *
 * @pdev: pointer to pdev
 * @vdev_id: vdev id
 * @ch_freq: SAP frequency
 *
 * Return: is SAP allowed
 */
bool
policy_mgr_is_sap_go_interface_allowed_on_indoor(struct wlan_objmgr_pdev *pdev,
						 uint8_t vdev_id,
						 qdf_freq_t ch_freq);

#ifdef WLAN_FEATURE_TDLS_CONCURRENCIES
/**
 * policy_mgr_get_allowed_tdls_offchannel_freq() - Check if TDLS off-channel is
 * allowed during concurrency. When off-channel is allowed, update the provided
 * input channel frequency with concurrent vdev frequency in DBS case.
 * Fill the provided channel frequency as 0 if all 5GHz/6GHz channels are
 * allowed for off-channel operation in SCC case.
 * Don't allow off channel operation in any MCC case.
 * @psoc: psoc pointer
 * @vdev: vdev pointer
 * @ch_freq: Frequency pointer
 *
 * Return: true or false based on current concurrency combination
 */
bool
policy_mgr_get_allowed_tdls_offchannel_freq(struct wlan_objmgr_psoc *psoc,
					    struct wlan_objmgr_vdev *vdev,
					    qdf_freq_t *ch_freq);
#endif /* WLAN_FEATURE_TDLS_CONCURRENCIES */

/**
 * policy_mgr_is_sap_mode() - Check if mode is SAP mode
 * @mode: Policy manager concurrency mode
 *
 * Return: true if mode is SAP mode else false
 */
bool policy_mgr_is_sap_mode(enum policy_mgr_con_mode mode);

/**
 * policy_mgr_is_beaconing_mode() - Check if mode represents beaconing entity
 * @mode: Policy manager concurrency mode
 *
 * Return: true if mode represents beaconing entity else false
 */
bool policy_mgr_is_beaconing_mode(enum policy_mgr_con_mode mode);

/**
 * policy_mgr_get_nan_sap_scc_on_lte_coex_chnl() -Get if NAN + SAP SCC on
 * lte coex channel is allowed on lte coex channel or not
 * @psoc: psoc pointer
 *
 * Return: cfg value of nan sap scc is allowed or not on lte coex channel
 */

bool policy_mgr_get_nan_sap_scc_on_lte_coex_chnl(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_reset_sap_mandatory_channels() - Reset the SAP mandatory channels
 * @psoc: psoc object
 *
 * Resets the SAP mandatory channel list and the length of the list
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_reset_sap_mandatory_channels(struct wlan_objmgr_psoc *psoc);

/**
 * policy_mgr_get_sap_mode_count() - Get SAP interface counts
 * @psoc: psoc object
 * @list: To provide the indices on pm_conc_connection_list
 *	(optional)
 *
 * Return: No of SAP interface counts
 */
uint32_t policy_mgr_get_sap_mode_count(struct wlan_objmgr_psoc *psoc,
				       uint32_t *list);

/**
 * policy_mgr_get_beaconing_mode_count() - Get Beaconing interface counts
 * @psoc: psoc object
 * @list: To provide the indices on pm_conc_connection_list
 *	(optional)
 *
 * Return: No of Beaconing interface counts
 */
uint32_t policy_mgr_get_beaconing_mode_count(struct wlan_objmgr_psoc *psoc,
					     uint32_t *list);

/**
 * policy_mgr_get_sap_mode_info() - Get active SAP channels and vdev ids
 * @psoc: PSOC object information
 * @ch_freq_list: Mode specific channel freq list
 * @vdev_id: Mode specific vdev id (list)
 *
 * Get active SAP channel and vdev id
 *
 * Return: number of SAP connections found
 */
uint32_t policy_mgr_get_sap_mode_info(struct wlan_objmgr_psoc *psoc,
				      uint32_t *ch_freq_list, uint8_t *vdev_id);

/**
 * policy_mgr_get_beaconing_mode_info() - Get active beaconing entity
 * channels and vdev ids
 * @psoc: PSOC object information
 * @ch_freq_list: Mode specific channel freq list
 * @vdev_id: Mode specific vdev id (list)
 *
 * Get active beaconing entity channels and vdev ids
 *
 * Return: number of beaconing entities found
 */
uint32_t policy_mgr_get_beaconing_mode_info(struct wlan_objmgr_psoc *psoc,
					    uint32_t *ch_freq_list,
					    uint8_t *vdev_id);

/**
 * policy_mgr_is_freq_on_mac_id() - Check if given freq belongs to given mac id
 * @freq_range: Frequency range pointer
 * @freq: Frequency which needs to be checked
 * @mac_id: MAC id on which this frequency needs to be checked
 *
 * Return: True if given frequency belongs to the given MAC id
 */
bool policy_mgr_is_freq_on_mac_id(struct policy_mgr_freq_range *freq_range,
				  qdf_freq_t freq, uint8_t mac_id);

/**
 * policy_mgr_is_conn_lead_to_dbs_sbs() - New freq leads to DBS/SBS
 * @psoc: PSOC object information
 * @vdev_id: vdev id of the caller
 * @freq: New connection frequency
 *
 * This API loops through existing connections from policy_mgr connection table
 *
 * Return: True if new frequency causes DBS/SBS with existing connections
 */
bool
policy_mgr_is_conn_lead_to_dbs_sbs(struct wlan_objmgr_psoc *psoc,
				   uint8_t vdev_id, qdf_freq_t freq);

/**
 * policy_mgr_sap_ch_width_update() - Update SAP ch_width
 * @psoc: PSOC object information
 * @next_action: next action to happen in order to update bandwidth
 * @reason: reason for ch_width update
 * @conc_vdev_id: Concurrent connection vdev_id that is causing ch_width update
 * @request_id: request id for connection manager
 *
 * Update ch_width as per next_action
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
policy_mgr_sap_ch_width_update(struct wlan_objmgr_psoc *psoc,
			       enum policy_mgr_conc_next_action next_action,
			       enum policy_mgr_conn_update_reason reason,
			       uint8_t conc_vdev_id, uint32_t request_id);

/*
 * policy_mgr_get_vdev_same_freq_new_conn() - Get vdev_id of the first
 *					      connection that has same
 *					      channel frequency as new_freq
 * @psoc: psoc object pointer
 * @new_freq: channel frequency for the new connection
 * @vdev_id: Output parameter to return vdev id of the first existing connection
 *	     that has same channel frequency as @new_freq
 *
 * This function is to return the first connection that has same
 * channel frequency as @new_freq.
 *
 * Return: true if connection that has same channel frequency as
 *	   @new_freq exists. Otherwise false.
 */
bool policy_mgr_get_vdev_same_freq_new_conn(struct wlan_objmgr_psoc *psoc,
					    uint32_t new_freq,
					    uint8_t *vdev_id);

/*
 * policy_mgr_get_vdev_diff_freq_new_conn() - Get vdev id of the first
 *					      connection that has different
 *					      channel freq from new_freq
 * @psoc: psoc object pointer
 * @new_freq: channel frequency for the new connection
 * @vdev_id: Output parameter to return vdev id of the first existing connection
 *	     that has different channel frequency from @new_freq
 *
 * This function is to return the first connection that has different
 * channel frequency from @new_freq.
 *
 * Return: true if connection that has different channel frequency from
 *	   @new_freq exists. Otherwise false.
 */
bool policy_mgr_get_vdev_diff_freq_new_conn(struct wlan_objmgr_psoc *psoc,
					    uint32_t new_freq,
					    uint8_t *vdev_id);

/**
 * policy_mgr_sap_on_non_psc_channel() - Check if STA operates in PSC or Non-PSC
 *					 channel to restart SAP on Non-PSC
 *					 channel
 * @psoc: PSOC object information
 * @intf_ch_freq: input/out interference channel frequency to sap
 * @sap_vdev_id: SAP vdev id
 *
 * This function is to check if STA operates in PSC or Non-PSC channel
 * to restart SAP on Non-PSC channel.
 *
 * Return: None
 */
void
policy_mgr_sap_on_non_psc_channel(struct wlan_objmgr_psoc *psoc,
				  qdf_freq_t *intf_ch_freq,
				  uint8_t sap_vdev_id);

#ifdef WLAN_FEATURE_LL_LT_SAP
/**
 * policy_mgr_get_pcl_ch_list_for_ll_sap() - Get PCL channel list for LL_LT_SAP
 * @psoc: psoc object
 * @pcl: pcl list
 * @vdev_id: vdev id
 * @info: pointer to connection_info structure
 * @connection_count: total number of existing connection present
 *
 * Return: QDF_STATUS
 */
QDF_STATUS policy_mgr_get_pcl_ch_list_for_ll_sap(
					struct wlan_objmgr_psoc *psoc,
					struct policy_mgr_pcl_list *pcl,
					uint8_t vdev_id,
					struct connection_info *info,
					uint8_t *connection_count);
#endif

/**
 * policy_mgr_is_given_freq_5g_low() - API to check whether given freq
 * is 5GHz low or not
 * @psoc: psoc object
 * @given_freq: given freq
 *
 * Return: True if it 5GHz low otherwise false
 */
bool policy_mgr_is_given_freq_5g_low(struct wlan_objmgr_psoc *psoc,
				     qdf_freq_t given_freq);
#endif /* __WLAN_POLICY_MGR_API_H */
