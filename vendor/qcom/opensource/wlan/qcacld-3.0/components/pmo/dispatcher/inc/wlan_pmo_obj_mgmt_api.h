/*
 * Copyright (c) 2017-2019, 2021 The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2024 Qualcomm Innovation Center, Inc. All rights reserved.
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
/**
 * DOC: declare utility API related to the pmo component
 * called by other components
 */

#ifndef _WLAN_PMO_OBJ_MGMT_API_H_
#define _WLAN_PMO_OBJ_MGMT_API_H_

#include "wlan_pmo_common_public_struct.h"
#include "wlan_pmo_obj_mgmt_public_struct.h"

#ifdef WLAN_POWER_MANAGEMENT_OFFLOAD
/**
 * pmo_init() - initialize pmo_ctx context.
 *
 * This function initializes the power manager offloads (a.k.a pmo) context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success else return error
 */
QDF_STATUS pmo_init(void);

/**
 * pmo_deinit() - De initialize pmo_ctx context.
 *
 * This function De initializes power manager offloads (a.k.a pmo) context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success else return error
 */
QDF_STATUS pmo_deinit(void);

/**
 * pmo_psoc_object_created_notification(): pmo psoc create handler
 * @psoc: psoc which is going to created by objmgr
 * @arg: argument for vdev create handler
 *
 * PMO, register this api with objmgr to detect psoc is created in fwr
 *
 * Return QDF_STATUS status in case of success else return error
 */
QDF_STATUS pmo_psoc_object_created_notification(struct wlan_objmgr_psoc *psoc,
						void *arg);

/**
 * pmo_psoc_object_destroyed_notification(): pmo psoc delete handler
 * @psoc: psoc which is going to delete by objmgr
 * @arg: argument for vdev delete handler
 *
 * PMO, register this api with objmgr to detect psoc is deleted in fwr
 *
 * Return QDF_STATUS status in case of success else return error
 */
QDF_STATUS pmo_psoc_object_destroyed_notification(struct wlan_objmgr_psoc *psoc,
						  void *arg);

/**
 * pmo_vdev_object_created_notification(): pmo vdev create handler
 * @vdev: vdev which is going to created by objmgr
 * @arg: argument for vdev create handler
 *
 * PMO, register this api with objmgr to detect vdev is created in fwr
 *
 * Return QDF_STATUS status in case of success else return error
 */
QDF_STATUS pmo_vdev_object_created_notification(struct wlan_objmgr_vdev *vdev,
						void *arg);

/**
 * pmo_vdev_ready() - handles vdev ready in firmware event
 * @vdev: vdev which is ready in firmware
 * @bridgeaddr: Bridge MAC address
 *
 * Objmgr vdev_create event does not guarantee vdev creation in firmware.
 * Any logic that would normally go in the vdev_create event, but needs to
 * communicate with firmware, needs to go here instead.
 *
 * Return QDF_STATUS
 */
QDF_STATUS pmo_vdev_ready(struct wlan_objmgr_vdev *vdev,
			  struct qdf_mac_addr *bridgeaddr);

/**
 * pmo_vdev_object_destroyed_notification(): pmo vdev delete handler
 * @vdev: vdev which is going to delete by objmgr
 * @arg: argument for vdev delete handler
 *
 * PMO, register this api with objmgr to detect vdev is deleted in fwr
 *
 * Return QDF_STATUS status in case of success else return error
 */
QDF_STATUS pmo_vdev_object_destroyed_notification(struct wlan_objmgr_vdev *vdev,
						  void *arg);

/**
 * pmo_register_suspend_handler(): register suspend handler for components
 * @id: component id
 * @handler: resume handler for the mention component
 * @arg: argument to pass while calling resume handler
 *
 * Return QDF_STATUS status -in case of success else return error
 */
QDF_STATUS pmo_register_suspend_handler(enum wlan_umac_comp_id id,
					pmo_psoc_suspend_handler handler,
					void *arg);

/**
 * pmo_unregister_suspend_handler():unregister suspend handler for components
 * @id: component id
 * @handler: resume handler for the mention component
 *
 * Return QDF_STATUS status -in case of success else return error
 */
QDF_STATUS pmo_unregister_suspend_handler(enum wlan_umac_comp_id id,
					  pmo_psoc_suspend_handler handler);

/**
 * pmo_register_resume_handler(): API to register resume handler for components
 * @id: component id
 * @handler: resume handler for the mention component
 * @arg: argument to pass while calling resume handler
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_register_resume_handler(enum wlan_umac_comp_id id,
				       pmo_psoc_resume_handler handler,
				       void *arg);

/**
 * pmo_unregister_resume_handler(): unregister resume handler for components
 * @id: component id
 * @handler: resume handler for the mention component
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_unregister_resume_handler(enum wlan_umac_comp_id id,
					 pmo_psoc_resume_handler handler);

/**
 * pmo_suspend_all_components(): API to suspend all component
 * @psoc:objmgr psoc
 * @suspend_type: Tell suspend type (apps suspend / runtime suspend)
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_suspend_all_components(struct wlan_objmgr_psoc *psoc,
				      enum qdf_suspend_type suspend_type);

/**
 * pmo_resume_all_components(): API to resume all component
 * @psoc:objmgr psoc
 * @suspend_type: Tell suspend type from which resume is required
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_resume_all_components(struct wlan_objmgr_psoc *psoc,
				     enum qdf_suspend_type suspend_type);

/**
 * pmo_register_pause_bitmap_notifier(): API to register pause bitmap notifier
 * @psoc: objmgr psoc handle
 * @handler: pause bitmap updated notifier
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_register_pause_bitmap_notifier(struct wlan_objmgr_psoc *psoc,
					      pmo_notify_pause_bitmap handler);

/**
 * pmo_unregister_pause_bitmap_notifier(): API to unregister pause bitmap
 * notifier
 * @psoc: objmgr psoc handle
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_unregister_pause_bitmap_notifier(struct wlan_objmgr_psoc *psoc);

/**
 * pmo_register_get_pause_bitmap(): API to get register pause bitmap notifier
 * @psoc: objmgr psoc handle
 * @handler: pause bitmap updated notifier
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_register_get_pause_bitmap(struct wlan_objmgr_psoc *psoc,
					 pmo_get_pause_bitmap handler);

/**
 * pmo_unregister_get_pause_bitmap(): API to unregister get pause bitmap
 * callback
 * @psoc: objmgr psoc handle
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_unregister_get_pause_bitmap(struct wlan_objmgr_psoc *psoc);

/**
 * pmo_register_get_dtim_period_callback(): API to register callback that gets
 * dtim period from mlme
 * @psoc: objmgr psoc handle
 * @handler: pointer to the callback function
 *
 * Return: QDF_STATUS_SUCCESS in case of success else error
 */
QDF_STATUS pmo_register_get_dtim_period_callback(struct wlan_objmgr_psoc *psoc,
						 pmo_get_dtim_period handler);

/**
 * pmo_unregister_get_dtim_period_callback(): API to unregister callback that
 * gets dtim period from mlme
 * @psoc: objmgr psoc handle
 *
 * Return: QDF_STATUS_SUCCESS in case of success else error
 */
QDF_STATUS
pmo_unregister_get_dtim_period_callback(struct wlan_objmgr_psoc *psoc);

/**
 * pmo_register_get_beacon_interval_callback(): API to register callback that
 * gets beacon interval from mlme
 * @psoc: objmgr psoc handle
 * @handler: pointer to the callback function
 *
 * Return: QDF_STATUS_SUCCESS in case of success else error
 */
QDF_STATUS
pmo_register_get_beacon_interval_callback(struct wlan_objmgr_psoc *psoc,
					  pmo_get_beacon_interval handler);

/**
 * pmo_unregister_get_beacon_interval_callback(): API to unregister callback
 * that gets beacon interval from mlme
 * @psoc: objmgr psoc handle
 *
 * Return: QDF_STATUS_SUCCESS in case of success else error
 */
QDF_STATUS
pmo_unregister_get_beacon_interval_callback(struct wlan_objmgr_psoc *psoc);

/**
 * pmo_register_is_device_in_low_pwr_mode(): API to get register device  power
 * save check notifier.
 * @psoc: objmgr psoc handle
 * @handler: device power save check notifier
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS pmo_register_is_device_in_low_pwr_mode(struct wlan_objmgr_psoc *psoc,
					 pmo_is_device_in_low_pwr_mode handler);

/**
 * pmo_unregister_is_device_in_low_pwr_mode(): API to unregister device  power
 * save check notifier.
 * @psoc: objmgr psoc handle
 *
 * Return QDF_STATUS status - in case of success else return error
 */
QDF_STATUS
pmo_unregister_is_device_in_low_pwr_mode(struct wlan_objmgr_psoc *psoc);

/**
 * wlan_pmo_get_sap_mode_bus_suspend(): API to get SAP bus suspend config
 * @psoc: objmgr psoc handle
 *
 * Return true in case of peer connected SAP bus suspend is allowed
 * else return false
 */
bool
wlan_pmo_get_sap_mode_bus_suspend(struct wlan_objmgr_psoc *psoc);

/**
 * wlan_pmo_get_go_mode_bus_suspend(): API to get GO bus suspend config
 * @psoc: objmgr psoc handle
 *
 * Return true in case of peer connected GO bus suspend is allowed
 * else return false
 */
bool
wlan_pmo_get_go_mode_bus_suspend(struct wlan_objmgr_psoc *psoc);

/*
 * wlan_pmo_no_op_on_page_fault() - Whether to ignore page fault wakeups
 * @psoc: PSOC object manager
 *
 * Return: true if host has to ignore page fault wakeup events else false.
 */
bool wlan_pmo_no_op_on_page_fault(struct wlan_objmgr_psoc *psoc);

/*
 * wlan_pmo_enable_ssr_on_page_fault: Enable/disable ssr on pagefault
 * @psoc: objmgr psoc
 *
 * Return: True if SSR is enabled on pagefault
 */
bool wlan_pmo_enable_ssr_on_page_fault(struct wlan_objmgr_psoc *psoc);

/*
 * wlan_pmo_get_min_pagefault_wakeups_for_action() - get min pagefault wakeups
 * for host to initiate action
 * @psoc: objmgr psoc
 *
 * Return: Min pagefault wakeups for action
 */
uint8_t
wlan_pmo_get_min_pagefault_wakeups_for_action(struct wlan_objmgr_psoc *psoc);

/*
 * wlan_pmo_get_interval_for_pagefault_wakeup_counts: get ssr interval for
 * pagefault
 * @psoc: objmgr psoc
 *
 * Return: SSR interval for pagefault
 */
uint32_t
wlan_pmo_get_interval_for_pagefault_wakeup_counts(
						struct wlan_objmgr_psoc *psoc);

QDF_STATUS wlan_pmo_get_listen_interval(struct wlan_objmgr_vdev *vdev,
					uint32_t *listen_interval);

/**
 * wlan_pmo_set_ps_params() - Set vdev OPM params
 * @vdev: pointer to vdev object
 * @ps_params: pointer to OPM params
 *
 * Return: None
 */
void wlan_pmo_set_ps_params(struct wlan_objmgr_vdev *vdev,
			    struct pmo_ps_params *ps_params);

/**
 * wlan_pmo_get_ps_params() - Get vdev OPM params
 * @vdev: pointer to vdev object
 * @ps_params: Pointer to get OPM params
 *
 * Return: QDF Status
 */
QDF_STATUS wlan_pmo_get_ps_params(struct wlan_objmgr_vdev *vdev,
				  struct pmo_ps_params *ps_params);

#else /* WLAN_POWER_MANAGEMENT_OFFLOAD */

static inline QDF_STATUS pmo_init(void)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS pmo_deinit(void)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_psoc_object_created_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_psoc_object_destroyed_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_vdev_object_created_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_vdev_ready(struct wlan_objmgr_vdev *vdev, struct qdf_mac_addr *bridgeaddr)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_vdev_object_destroyed_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_suspend_handler(enum wlan_umac_comp_id id,
			     pmo_psoc_suspend_handler handler,
			     void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_suspend_handler(enum wlan_umac_comp_id id,
			       pmo_psoc_suspend_handler handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_resume_handler(enum wlan_umac_comp_id id,
			    pmo_psoc_resume_handler handler,
			    void *arg)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_resume_handler(enum wlan_umac_comp_id id,
			      pmo_psoc_resume_handler handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_suspend_all_components(struct wlan_objmgr_psoc *psoc,
			   enum qdf_suspend_type suspend_type)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_resume_all_components(struct wlan_objmgr_psoc *psoc,
			  enum qdf_suspend_type suspend_type)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_pause_bitmap_notifier(struct wlan_objmgr_psoc *psoc,
				   pmo_notify_pause_bitmap handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_pause_bitmap_notifier(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_get_pause_bitmap(struct wlan_objmgr_psoc *psoc,
			      pmo_get_pause_bitmap handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_get_pause_bitmap(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_is_device_in_low_pwr_mode(struct wlan_objmgr_psoc *psoc,
				       pmo_is_device_in_low_pwr_mode handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_is_device_in_low_pwr_mode(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_get_dtim_period_callback(struct wlan_objmgr_psoc *psoc,
				      pmo_get_dtim_period handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_get_dtim_period_callback(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_register_get_beacon_interval_callback(struct wlan_objmgr_psoc *psoc,
					  pmo_get_beacon_interval handler)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
pmo_unregister_get_beacon_interval_callback(struct wlan_objmgr_psoc *psoc)
{
	return QDF_STATUS_SUCCESS;
}

static inline bool
wlan_pmo_get_sap_mode_bus_suspend(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline bool
wlan_pmo_get_go_mode_bus_suspend(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline bool wlan_pmo_no_op_on_page_fault(struct wlan_objmgr_psoc *psoc)
{
	return true;
}

static inline bool
wlan_pmo_enable_ssr_on_page_fault(struct wlan_objmgr_psoc *psoc)
{
	return false;
}

static inline uint8_t
wlan_pmo_get_min_pagefault_wakeups_for_action(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

static inline uint32_t
wlan_pmo_get_interval_for_pagefault_wakeup_counts(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

static QDF_STATUS wlan_pmo_get_listen_interval(struct wlan_objmgr_vdev *vdev,
					       uint32_t *listen_interval)
{
	return QDF_STATUS_SUCCESS;
}

static inline
void wlan_pmo_set_ps_params(struct wlan_objmgr_vdev *vdev,
			    struct pmo_ps_params *ps_params)
{
}

static inline QDF_STATUS
wlan_pmo_get_ps_params(struct wlan_objmgr_vdev *vdev,
		       struct pmo_ps_params *ps_params)
{
	return QDF_STATUS_SUCCESS;
}
#endif /* WLAN_POWER_MANAGEMENT_OFFLOAD */

#endif /* end  of _WLAN_PMO_OBJ_MGMT_API_H_ */
