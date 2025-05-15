// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2024, Qualcomm Innovation Center, Inc. All rights reserved.
 */

#define pr_fmt(fmt)	"BATTERY_CHG: %s: " fmt, __func__

#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/rpmsg.h>
#include <linux/mutex.h>
#include <linux/pm_wakeup.h>
#include <linux/power_supply.h>
#include <linux/reboot.h>
#include <linux/thermal.h>
#include <linux/soc/qcom/pmic_glink.h>
#include <linux/soc/qcom/battery_charger.h>
#include <linux/soc/qcom/panel_event_notifier.h>
#ifndef NT_EDIT
#define NT_EDIT
#endif
#ifdef NT_EDIT
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/ctype.h>

#define BC_SET_DEBUG_PARAM_REQ		                0x46
#define OEM_GET_LOG_BUFFER                          0x0050
#define OEM_GET_REGISTER_BUFFER                     0x0051
#define OEM_CHARGE_ABNORMAL                         0x0052
#define READ_LOG_BUFFER_SIZE_MAX                    128
#define READ_REGISTER_BUFFER_SIZE_MAX               612
#define BATTERY_MAX_CURRENT                         9840000
#define ADSP_INIT_TRIES                             20
#define CHG_DATA_ID_DEF_VAL			    6
#endif
#define MSG_OWNER_BC			32778
#define MSG_TYPE_REQ_RESP		1
#define MSG_TYPE_NOTIFY			2

/* opcode for battery charger */
#define BC_SET_NOTIFY_REQ		0x04
#define BC_DISABLE_NOTIFY_REQ		0x05
#define BC_NOTIFY_IND			0x07
#define BC_BATTERY_STATUS_GET		0x30
#define BC_BATTERY_STATUS_SET		0x31
#define BC_USB_STATUS_GET(port_id)	(0x32 | (port_id) << 16)
#define BC_USB_STATUS_SET(port_id)	(0x33 | (port_id) << 16)
#define BC_WLS_STATUS_GET		0x34
#define BC_WLS_STATUS_SET		0x35
#define BC_SHIP_MODE_REQ_SET		0x36
#define BC_WLS_FW_CHECK_UPDATE		0x40
#define BC_WLS_FW_PUSH_BUF_REQ		0x41
#define BC_WLS_FW_UPDATE_STATUS_RESP	0x42
#define BC_WLS_FW_PUSH_BUF_RESP		0x43
#define BC_WLS_FW_GET_VERSION		0x44
#define BC_SHUTDOWN_NOTIFY		0x47
#define BC_CHG_CTRL_LIMIT_EN		0x48
#define BC_HBOOST_VMAX_CLAMP_NOTIFY	0x79
#define BC_GENERIC_NOTIFY		0x80

/* Generic definitions */
#define MAX_STR_LEN			128
#define BC_WAIT_TIME_MS			1000
#define WLS_FW_PREPARE_TIME_MS		1000
#define WLS_FW_WAIT_TIME_MS		500
#define WLS_FW_UPDATE_TIME_MS		1000
#define WLS_FW_BUF_SIZE			128
#define DEFAULT_RESTRICT_FCC_UA		1000000
#ifdef NT_EDIT
#define NT_NOTIFY_NORMAL            0
#define NT_GET_CHARGE_KEY_INFO      1
#define KEY_INFO_COUNT			5
enum nt_notify_finish_type {
        NT_NOTIFY_NOT_FINISH = 0,
        NT_NOTIFY_FINISH = 1,
};
enum nt_notify_count_times {
        NT_NOTIFY_COUNT_START = 0,
        NT_NOTIFY_COUNT_END = 2,
};
enum nt_chg_data_type {
        NT_CHG_USB_TEMP = 1,
};

 /**
    @}
  */

 /**  for userspace to get/set debug params */
 /**
    @{
  */
enum battman_debug_param_type_e{
  BC_DEBUG_PARAM_SET_ICL = 0,
  BC_DEBUG_PARAM_SUSPEND_USB,
  BC_DEBUG_PARAM_SMB_SETTING,
  BC_DEBUG_PARAM_CHG_TEMP_OVR,
  BC_DEBUG_PARAM_SET_MAIN_TEMP,
  BC_DEBUG_PARAM_SET_SMB_TEMP,
  BC_DEBUG_PARAM_WLS_TX_SOURCE,
  BC_DEBUG_PARAM_EN_OPT_MTC_CHG,
  BC_DEBUG_PARAM_USB_OTG_SOURCE,
  BC_DEBUG_PARAM_AGING_TEST,
  BC_DEBUG_PARAM_MAX
};

enum nt_charge_abnormal_type {
		NT_NOTIFY_USB_TEMP_ABNORMAL =            1 << 0,
		NT_NOTIFY_CHARGER_OVER_VOL =            1 << 1,
		NT_NOTIFY_CHARGER_LOW_VOL    =      1 << 2,
		NT_NOTIFY_BAT_OVER_TEMP  =          1 << 3,
		NT_NOTIFY_BAT_LOW_TEMP   =          1 << 4,
		NT_NOTIFY_BAT_NOT_CONNECT    =      1 << 5,
		NT_NOTIFY_BAT_OVER_VOL   =          1 << 6,
		NT_NOTIFY_BAT_FULL       =          1 << 7,
		NT_NOTIFY_CHGING_CURRENT     =      1 << 8,
		NT_NOTIFY_CHGING_OVERTIME    =      1 << 9,
		NT_NOTIFY_BAT_FULL_PRE_HIGH_TEMP =      1 << 10,
		NT_NOTIFY_BAT_FULL_PRE_LOW_TEMP  =      1 << 11,
		NT_NOTIFY_BAT_FULL_THIRD_BATTERY     =  1 << 12,
		NT_NOTIFY_CHARGE_PUMP_ERR            =  1 << 13,
		NT_NOTIFY_WLS_TX_FOD_ERR_CODE2 =  1 << 14,
		NT_NOTIFY_BAT_SOC_ZERO_ERR =  1 << 15,
		NT_NOTIFY_SHORT_C_BAT_DYNAMIC_ERR_CODE4 =   1 << 16,
		NT_NOTIFY_SHORT_C_BAT_DYNAMIC_ERR_CODE5 =   1 << 17,
		NT_NOTIFY_CHARGER_TERMINAL   =          1 << 18,
		NT_NOTIFY_GAUGE_I2C_ERR  =              1 << 19,
};
#endif
enum usb_port_id {
	USB_1_PORT_ID,
	USB_2_PORT_ID,
	NUM_USB_PORTS,
};

enum psy_type {
	PSY_TYPE_BATTERY,
	PSY_TYPE_USB,
	PSY_TYPE_USB_2,
	PSY_TYPE_WLS,
	PSY_TYPE_MAX,
};

enum ship_mode_type {
	SHIP_MODE_PMIC,
	SHIP_MODE_PACK_SIDE,
};

/* property ids */
enum battery_property_id {
	BATT_STATUS,
	BATT_HEALTH,
	BATT_PRESENT,
	BATT_CHG_TYPE,
	BATT_CAPACITY,
	BATT_SOH,
	BATT_VOLT_OCV,
	BATT_VOLT_NOW,
	BATT_VOLT_MAX,
	BATT_CURR_NOW,
	BATT_CHG_CTRL_LIM,
	BATT_CHG_CTRL_LIM_MAX,
	BATT_TEMP,
	BATT_TECHNOLOGY,
	BATT_CHG_COUNTER,
	BATT_CYCLE_COUNT,
	BATT_CHG_FULL_DESIGN,
	BATT_CHG_FULL,
	BATT_MODEL_NAME,
	BATT_TTF_AVG,
	BATT_TTE_AVG,
	BATT_RESISTANCE,
	BATT_POWER_NOW,
	BATT_POWER_AVG,
	BATT_CHG_CTRL_EN,
	BATT_CHG_CTRL_START_THR,
	BATT_CHG_CTRL_END_THR,
	BATT_CURR_AVG,
#ifdef NT_EDIT
	BATT_FAKE_IBAT,
	BATT_FAKE_VBAT,
	BATT_FAKE_TBAT,
	BATT_FAKE_TUSB,
	BATT_FAKE_SOC,
	BATT_SHUT_DOWN,
	BATT_ID,
#endif
	BATT_PROP_MAX,
};

enum usb_property_id {
	USB_ONLINE,
	USB_VOLT_NOW,
	USB_VOLT_MAX,
	USB_CURR_NOW,
	USB_CURR_MAX,
	USB_INPUT_CURR_LIMIT,
	USB_TYPE,
	USB_ADAP_TYPE,
	USB_MOISTURE_DET_EN,
	USB_MOISTURE_DET_STS,
	USB_TEMP,
	USB_REAL_TYPE,
	USB_TYPEC_COMPLIANT,
#ifdef NT_EDIT
	/* create new usb property ids for get charge adsp info*/
	NT_CHARGE_PUMP_ENABLE,
	NT_CC_ORIENTATION,
	NT_CHARGE_ENABLE,
	NT_SCENARIO_FCC,
	NT_EXIST_CHARGE_PUMP,
	NT_EXIST_CHARGE_BUCK,
	NT_OTG_ENABLE,
	NT_CHARGE_BUCK_ENABLE,
	NT_GET_CHIP_REG,
	NT_CHG_PARAM,
	NT_CHG_KEY_INFO,
	NT_ADP_POWER,
#endif
	USB_SCOPE,
	USB_CONNECTOR_TYPE,
	F_ACTIVE,
	USB_NUM_PORTS,
	USB_PROP_MAX,
};

enum wireless_property_id {
	WLS_ONLINE,
	WLS_VOLT_NOW,
	WLS_VOLT_MAX,
	WLS_CURR_NOW,
	WLS_CURR_MAX,
	WLS_TYPE,
	WLS_BOOST_EN,
	WLS_HBOOST_VMAX,
	WLS_INPUT_CURR_LIMIT,
	WLS_ADAP_TYPE,
	WLS_CONN_TEMP,
	WLS_PROP_MAX,
};

enum {
	QTI_POWER_SUPPLY_USB_TYPE_HVDCP = 0x80,
	QTI_POWER_SUPPLY_USB_TYPE_HVDCP_3,
	QTI_POWER_SUPPLY_USB_TYPE_HVDCP_3P5,
};

#ifdef NT_EDIT
static int key_info_num = 0;
static int usb_temp_type[7] = {1,2,3,4,5,6,7};
static int param[10] = {0,0,0,0,0,0,0,0,0,0};
struct battman_get_logs_resp {
	struct pmic_glink_hdr	hdr;
	char read_buffer[READ_LOG_BUFFER_SIZE_MAX];
};
struct battman_get_registers_resp {
	struct pmic_glink_hdr	hdr;
	char read_buffer[READ_REGISTER_BUFFER_SIZE_MAX];
};
struct battman_abnormal_resp {
	struct pmic_glink_hdr	hdr;
	int value;
};
struct nt_proc {
	char *name;
	struct proc_ops fops;
};
/** Resquest Message; for set debug buffer */
struct battman_set_debug_param_req_msg {
	struct pmic_glink_hdr	hdr;
	u32 debug_param_property_id;
	u32 data;
};  /* Message */
#endif
struct battery_charger_set_notify_msg {
	struct pmic_glink_hdr	hdr;
	u32			battery_id;
	u32			power_state;
	u32			low_capacity;
	u32			high_capacity;
};

struct battery_charger_notify_msg {
	struct pmic_glink_hdr	hdr;
	u32			notification;
};

struct battery_charger_req_msg {
	struct pmic_glink_hdr	hdr;
	u32			battery_id;
	u32			property_id;
	u32			value;
};

struct battery_charger_resp_msg {
	struct pmic_glink_hdr	hdr;
	u32			property_id;
	u32			value;
	u32			ret_code;
};

struct battery_model_resp_msg {
	struct pmic_glink_hdr	hdr;
	u32			property_id;
	char			model[MAX_STR_LEN];
};

struct wireless_fw_check_req {
	struct pmic_glink_hdr	hdr;
	u32			fw_version;
	u32			fw_size;
	u32			fw_crc;
};

struct wireless_fw_check_resp {
	struct pmic_glink_hdr	hdr;
	u32			ret_code;
};

struct wireless_fw_push_buf_req {
	struct pmic_glink_hdr	hdr;
	u8			buf[WLS_FW_BUF_SIZE];
	u32			fw_chunk_id;
};

struct wireless_fw_push_buf_resp {
	struct pmic_glink_hdr	hdr;
	u32			fw_update_status;
};

struct wireless_fw_update_status {
	struct pmic_glink_hdr	hdr;
	u32			fw_update_done;
};

struct wireless_fw_get_version_req {
	struct pmic_glink_hdr	hdr;
};

struct wireless_fw_get_version_resp {
	struct pmic_glink_hdr	hdr;
	u32			fw_version;
};

struct battery_charger_ship_mode_req_msg {
	struct pmic_glink_hdr	hdr;
	u32			ship_mode_type;
};

struct battery_charger_chg_ctrl_msg {
	struct pmic_glink_hdr	hdr;
	u32			enable;
	u32			target_soc;
	u32			delta_soc;
};

struct psy_state {
	struct power_supply	*psy;
	char			*model;
	const int		*map;
	u32			*prop;
	u32			prop_count;
	u32			opcode_get;
	u32			opcode_set;
};

struct battery_chg_dev {
	struct device			*dev;
	struct class			battery_class;
	struct pmic_glink_client	*client;
	struct mutex			rw_lock;
	struct rw_semaphore		state_sem;
	struct completion		ack;
	struct completion		fw_buf_ack;
	struct completion		fw_update_ack;
	struct psy_state		psy_list[PSY_TYPE_MAX];
	struct dentry			*debugfs_dir;
	void				*notifier_cookie;
	u32				*thermal_levels;
	const char			*wls_fw_name;
	int				curr_thermal_level;
	int				num_thermal_levels;
	int				shutdown_volt_mv;
	atomic_t			state;
	struct work_struct		subsys_up_work;
	struct work_struct		usb_type_work;
	struct work_struct		battery_check_work;
#ifdef NT_EDIT
	struct work_struct		nt_update_event;
	struct delayed_work 	nt_update_status_work;
	struct wakeup_source *chg_wake;
	int nt_abnormal_status_val;
	int notify_usbinovp_flag;
	int notify_usbinovp_count;
	bool				nt_need_update;
	bool				nt_usb_temp_abnormal;
	bool				nt_charge_pump_abnormal;
	bool				nt_charge_full_temp_abnormal;
	u32				is_aging_test;
#endif
	int				fake_soc;
	bool				block_tx;
	bool				ship_mode_en;
	bool				debug_battery_detected;
	bool				wls_not_supported;
	bool				wls_fw_update_reqd;
	u32				wls_fw_version;
	u16				wls_fw_crc;
	u32				wls_fw_update_time_ms;
	struct notifier_block		reboot_notifier;
	u32				thermal_fcc_ua;
	u32				restrict_fcc_ua;
	u32				last_fcc_ua;
	u32				usb_icl_ua[NUM_USB_PORTS];
	u32				thermal_fcc_step;
	bool				restrict_chg_en;
	u8				chg_ctrl_start_thr;
	u8				chg_ctrl_end_thr;
	bool				chg_ctrl_en;
	bool				usb_active[NUM_USB_PORTS];
	/* To track the driver initialization status */
	bool				initialized;
	bool				notify_en;
	bool				error_prop;
	bool				has_usb_2;
#ifdef NT_EDIT
	u32				chg_data_id;
#endif

	unsigned int			num_usb_ports;
};

static const int battery_prop_map[BATT_PROP_MAX] = {
	[BATT_STATUS]		= POWER_SUPPLY_PROP_STATUS,
	[BATT_HEALTH]		= POWER_SUPPLY_PROP_HEALTH,
	[BATT_PRESENT]		= POWER_SUPPLY_PROP_PRESENT,
	[BATT_CHG_TYPE]		= POWER_SUPPLY_PROP_CHARGE_TYPE,
	[BATT_CAPACITY]		= POWER_SUPPLY_PROP_CAPACITY,
	[BATT_VOLT_OCV]		= POWER_SUPPLY_PROP_VOLTAGE_OCV,
	[BATT_VOLT_NOW]		= POWER_SUPPLY_PROP_VOLTAGE_NOW,
	[BATT_VOLT_MAX]		= POWER_SUPPLY_PROP_VOLTAGE_MAX,
	[BATT_CURR_NOW]		= POWER_SUPPLY_PROP_CURRENT_NOW,
	[BATT_CHG_CTRL_LIM]	= POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT,
	[BATT_CHG_CTRL_LIM_MAX]	= POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX,
	[BATT_TEMP]		= POWER_SUPPLY_PROP_TEMP,
	[BATT_TECHNOLOGY]	= POWER_SUPPLY_PROP_TECHNOLOGY,
	[BATT_CHG_COUNTER]	= POWER_SUPPLY_PROP_CHARGE_COUNTER,
	[BATT_CYCLE_COUNT]	= POWER_SUPPLY_PROP_CYCLE_COUNT,
	[BATT_CHG_FULL_DESIGN]	= POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	[BATT_CHG_FULL]		= POWER_SUPPLY_PROP_CHARGE_FULL,
	[BATT_MODEL_NAME]	= POWER_SUPPLY_PROP_MODEL_NAME,
	[BATT_TTF_AVG]		= POWER_SUPPLY_PROP_TIME_TO_FULL_AVG,
	[BATT_TTE_AVG]		= POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG,
	[BATT_POWER_NOW]	= POWER_SUPPLY_PROP_POWER_NOW,
	[BATT_POWER_AVG]	= POWER_SUPPLY_PROP_POWER_AVG,
	[BATT_CHG_CTRL_START_THR] = POWER_SUPPLY_PROP_CHARGE_CONTROL_START_THRESHOLD,
	[BATT_CHG_CTRL_END_THR]   = POWER_SUPPLY_PROP_CHARGE_CONTROL_END_THRESHOLD,
	[BATT_CURR_AVG]		= POWER_SUPPLY_PROP_CURRENT_AVG,
};

static const int usb_prop_map[USB_PROP_MAX] = {
	[USB_ONLINE]		= POWER_SUPPLY_PROP_ONLINE,
	[USB_VOLT_NOW]		= POWER_SUPPLY_PROP_VOLTAGE_NOW,
	[USB_VOLT_MAX]		= POWER_SUPPLY_PROP_VOLTAGE_MAX,
	[USB_CURR_NOW]		= POWER_SUPPLY_PROP_CURRENT_NOW,
	[USB_CURR_MAX]		= POWER_SUPPLY_PROP_CURRENT_MAX,
	[USB_INPUT_CURR_LIMIT]	= POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	[USB_ADAP_TYPE]		= POWER_SUPPLY_PROP_USB_TYPE,
	[USB_TEMP]		= POWER_SUPPLY_PROP_TEMP,
};

static const int wls_prop_map[WLS_PROP_MAX] = {
	[WLS_ONLINE]		= POWER_SUPPLY_PROP_ONLINE,
	[WLS_VOLT_NOW]		= POWER_SUPPLY_PROP_VOLTAGE_NOW,
	[WLS_VOLT_MAX]		= POWER_SUPPLY_PROP_VOLTAGE_MAX,
	[WLS_CURR_NOW]		= POWER_SUPPLY_PROP_CURRENT_NOW,
	[WLS_CURR_MAX]		= POWER_SUPPLY_PROP_CURRENT_MAX,
	[WLS_INPUT_CURR_LIMIT]	= POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	[WLS_CONN_TEMP]		= POWER_SUPPLY_PROP_TEMP,
};

/* Standard usb_type definitions similar to power_supply_sysfs.c */
static const char * const power_supply_usb_type_text[] = {
	"Unknown", "SDP", "DCP", "CDP", "ACA", "C",
#ifdef NT_EDIT
	"PD", "PD_DRP", "PD_PPS", "BrickID", "UFCS"
#else
	"PD", "PD_DRP", "PD_PPS", "BrickID"
#endif
};

/* Custom usb_type definitions */
static const char * const qc_power_supply_usb_type_text[] = {
	"HVDCP", "HVDCP_3", "HVDCP_3P5"
};

/* wireless_type definitions */
static const char * const qc_power_supply_wls_type_text[] = {
	"Unknown", "BPP", "EPP", "HPP"
};

static struct battman_get_registers_resp g_get_registers_resp_msg = {0};

static RAW_NOTIFIER_HEAD(hboost_notifier);

int register_hboost_event_notifier(struct notifier_block *nb)
{
	return raw_notifier_chain_register(&hboost_notifier, nb);
}
EXPORT_SYMBOL(register_hboost_event_notifier);

int unregister_hboost_event_notifier(struct notifier_block *nb)
{
	return raw_notifier_chain_unregister(&hboost_notifier, nb);
}
EXPORT_SYMBOL(unregister_hboost_event_notifier);

static int battery_chg_fw_write(struct battery_chg_dev *bcdev, void *data,
				int len)
{
	int rc;

	down_read(&bcdev->state_sem);
	if (atomic_read(&bcdev->state) == PMIC_GLINK_STATE_DOWN) {
		pr_debug("glink state is down\n");
		up_read(&bcdev->state_sem);
		return -ENOTCONN;
	}
	up_read(&bcdev->state_sem);

	reinit_completion(&bcdev->fw_buf_ack);
	rc = pmic_glink_write(bcdev->client, data, len);
	if (!rc) {
		rc = wait_for_completion_timeout(&bcdev->fw_buf_ack,
					msecs_to_jiffies(WLS_FW_WAIT_TIME_MS));
		if (!rc) {
			pr_err("Error, timed out sending message\n");
			return -ETIMEDOUT;
		}

		rc = 0;
	}

	return rc;
}

static int battery_chg_write(struct battery_chg_dev *bcdev, void *data,
				int len)
{
	int rc;

	/*
	 * When the subsystem goes down, it's better to return the last
	 * known values until it comes back up. Hence, return 0 so that
	 * pmic_glink_write() is not attempted until pmic glink is up.
	 */
	down_read(&bcdev->state_sem);
	if (atomic_read(&bcdev->state) == PMIC_GLINK_STATE_DOWN) {
		pr_debug("glink state is down\n");
		up_read(&bcdev->state_sem);
		return 0;
	}
	up_read(&bcdev->state_sem);

	if (bcdev->debug_battery_detected && bcdev->block_tx) {
		return 0;
	}

	mutex_lock(&bcdev->rw_lock);
	reinit_completion(&bcdev->ack);
	bcdev->error_prop = false;
	rc = pmic_glink_write(bcdev->client, data, len);
	if (!rc) {
		rc = wait_for_completion_timeout(&bcdev->ack,
					msecs_to_jiffies(BC_WAIT_TIME_MS));
		if (!rc) {
			pr_err("Error, timed out sending message\n");
			mutex_unlock(&bcdev->rw_lock);
			return -ETIMEDOUT;
		}
		rc = 0;

		/*
		 * In case the opcode used is not supported, the remote
		 * processor might ack it immediately with a return code indicating
		 * an error. This additional check is to check if such an error has
		 * happened and return immediately with error in that case. This
		 * avoids wasting time waiting in the above timeout condition for this
		 * type of error.
		 */
		if (bcdev->error_prop) {
			bcdev->error_prop = false;
			rc = -ENODATA;
		}
	}
	mutex_unlock(&bcdev->rw_lock);

	return rc;
}

static int write_property_id(struct battery_chg_dev *bcdev,
			struct psy_state *pst, u32 prop_id, u32 val)
{
	struct battery_charger_req_msg req_msg = { { 0 } };

	req_msg.property_id = prop_id;
	req_msg.battery_id = 0;
	req_msg.value = val;
	req_msg.hdr.owner = MSG_OWNER_BC;
	req_msg.hdr.type = MSG_TYPE_REQ_RESP;
	req_msg.hdr.opcode = pst->opcode_set;

	if (pst->psy)
		pr_debug("psy: %s prop_id: %u val: %u\n", pst->psy->desc->name,
			req_msg.property_id, val);

	return battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
}

static int read_property_id(struct battery_chg_dev *bcdev,
			struct psy_state *pst, u32 prop_id)
{
	struct battery_charger_req_msg req_msg = { { 0 } };

	req_msg.property_id = prop_id;
	req_msg.battery_id = 0;
	req_msg.value = 0;
	req_msg.hdr.owner = MSG_OWNER_BC;
	req_msg.hdr.type = MSG_TYPE_REQ_RESP;
	req_msg.hdr.opcode = pst->opcode_get;

	if (pst->psy)
		pr_debug("psy: %s prop_id: %u\n", pst->psy->desc->name,
			req_msg.property_id);

	return battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
}

static int get_property_id(struct psy_state *pst,
			enum power_supply_property prop)
{
	u32 i;

	for (i = 0; i < pst->prop_count; i++)
		if (pst->map[i] == prop)
			return i;

	if (pst->psy)
		pr_err("No property id for property %d in psy %s\n", prop,
			pst->psy->desc->name);

	return -ENOENT;
}

static void battery_chg_notify_disable(struct battery_chg_dev *bcdev)
{
	struct battery_charger_set_notify_msg req_msg = { { 0 } };
	int rc;

	if (bcdev->notify_en) {
		/* Send request to disable notification */
		req_msg.hdr.owner = MSG_OWNER_BC;
		req_msg.hdr.type = MSG_TYPE_NOTIFY;
		req_msg.hdr.opcode = BC_DISABLE_NOTIFY_REQ;

		rc = battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
		if (rc < 0)
			pr_err("Failed to disable notification rc=%d\n", rc);
		else
			bcdev->notify_en = false;
	}
}

static void battery_chg_notify_enable(struct battery_chg_dev *bcdev)
{
	struct battery_charger_set_notify_msg req_msg = { { 0 } };
	int rc;

	if (!bcdev->notify_en) {
		/* Send request to enable notification */
		req_msg.hdr.owner = MSG_OWNER_BC;
		req_msg.hdr.type = MSG_TYPE_NOTIFY;
		req_msg.hdr.opcode = BC_SET_NOTIFY_REQ;

		rc = battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
		if (rc < 0)
			pr_err("Failed to enable notification rc=%d\n", rc);
		else
			bcdev->notify_en = true;
	}
}

static void battery_chg_state_cb(void *priv, enum pmic_glink_state state)
{
	struct battery_chg_dev *bcdev = priv;

	pr_debug("state: %d\n", state);

	down_write(&bcdev->state_sem);
	if (!bcdev->initialized) {
		pr_warn("Driver not initialized, pmic_glink state %d\n", state);
		up_write(&bcdev->state_sem);
		return;
	}
	atomic_set(&bcdev->state, state);
	up_write(&bcdev->state_sem);

	if (state == PMIC_GLINK_STATE_UP)
		schedule_work(&bcdev->subsys_up_work);
	else if (state == PMIC_GLINK_STATE_DOWN)
		bcdev->notify_en = false;
}

/**
 * qti_battery_charger_get_prop() - Gets the property being requested
 *
 * @name: Power supply name
 * @prop_id: Property id to be read
 * @val: Pointer to value that needs to be updated
 *
 * Return: 0 if success, negative on error.
 */
int qti_battery_charger_get_prop(const char *name,
				enum battery_charger_prop prop_id, int *val)
{
	struct power_supply *psy;
	struct battery_chg_dev *bcdev;
	struct psy_state *pst;
	int rc = 0;

	if (prop_id >= BATTERY_CHARGER_PROP_MAX)
		return -EINVAL;

	if (strcmp(name, "battery") && strcmp(name, "usb") &&
	    strcmp(name, "usb-2") && strcmp(name, "wireless"))
		return -EINVAL;

	psy = power_supply_get_by_name(name);
	if (!psy)
		return -ENODEV;

	bcdev = power_supply_get_drvdata(psy);
	if (!bcdev)
		return -ENODEV;

	power_supply_put(psy);

	switch (prop_id) {
	case BATTERY_RESISTANCE:
		pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
		rc = read_property_id(bcdev, pst, BATT_RESISTANCE);
		if (!rc)
			*val = pst->prop[BATT_RESISTANCE];
		break;
	default:
		break;
	}

	return rc;
}
EXPORT_SYMBOL(qti_battery_charger_get_prop);

static bool validate_message(struct battery_chg_dev *bcdev,
			struct battery_charger_resp_msg *resp_msg, size_t len)
{
	if (len != sizeof(*resp_msg)) {
		pr_err("Incorrect response length %zu for opcode %#x\n", len,
			resp_msg->hdr.opcode);
		return false;
	}

	if (resp_msg->ret_code) {
		pr_err_ratelimited("Error in response for opcode %#x prop_id %u, rc=%d\n",
			resp_msg->hdr.opcode, resp_msg->property_id,
			(int)resp_msg->ret_code);
		bcdev->error_prop = true;
		return false;
	}

	return true;
}

#define MODEL_DEBUG_BOARD	"Debug_Board"
static void handle_message(struct battery_chg_dev *bcdev, void *data,
				size_t len)
{
	struct battery_charger_resp_msg *resp_msg = data;
#ifdef NT_EDIT
	struct battman_get_logs_resp *get_logs_resp_msg = data;
	struct battman_get_registers_resp *get_registers_resp_msg = data;
	struct battman_abnormal_resp *abnormal_resp_msg = data;
#endif
	struct battery_model_resp_msg *model_resp_msg = data;
	struct wireless_fw_check_resp *fw_check_msg;
	struct wireless_fw_push_buf_resp *fw_resp_msg;
	struct wireless_fw_update_status *fw_update_msg;
	struct wireless_fw_get_version_resp *fw_ver_msg;
	struct psy_state *pst;
	bool ack_set = false;

	switch (resp_msg->hdr.opcode) {
	case BC_BATTERY_STATUS_GET:
		pst = &bcdev->psy_list[PSY_TYPE_BATTERY];

		/* Handle model response uniquely as it's a string */
		if (pst->model && len == sizeof(*model_resp_msg)) {
			memcpy(pst->model, model_resp_msg->model, MAX_STR_LEN);
			ack_set = true;
			bcdev->debug_battery_detected = !strcmp(pst->model,
					MODEL_DEBUG_BOARD);
			break;
		}

		/* Other response should be of same type as they've u32 value */
		if (validate_message(bcdev, resp_msg, len) &&
		    resp_msg->property_id < pst->prop_count) {
			pst->prop[resp_msg->property_id] = resp_msg->value;
			ack_set = true;
		}

		break;
	case BC_USB_STATUS_GET(USB_1_PORT_ID):
		pst = &bcdev->psy_list[PSY_TYPE_USB];
		if (validate_message(bcdev, resp_msg, len) &&
		    resp_msg->property_id < pst->prop_count) {
			pst->prop[resp_msg->property_id] = resp_msg->value;
			ack_set = true;
		}

		break;
	case BC_USB_STATUS_GET(USB_2_PORT_ID):
		if (bcdev->num_usb_ports < 2) {
			pr_debug("opcode: %u for missing USB_2 port\n",
					resp_msg->hdr.opcode);
			break;
		}
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
		if (validate_message(bcdev, resp_msg, len) &&
		    resp_msg->property_id < pst->prop_count) {
			pst->prop[resp_msg->property_id] = resp_msg->value;
			ack_set = true;
		}

		break;
	case BC_WLS_STATUS_GET:
		pst = &bcdev->psy_list[PSY_TYPE_WLS];
		if (validate_message(bcdev, resp_msg, len) &&
		    resp_msg->property_id < pst->prop_count) {
			pst->prop[resp_msg->property_id] = resp_msg->value;
			ack_set = true;
		}

		break;
	case BC_USB_STATUS_SET(USB_2_PORT_ID):
		if (bcdev->num_usb_ports < 2) {
			pr_debug("opcode: %u for missing USB_2 port\n",
					resp_msg->hdr.opcode);
			break;
		}
		fallthrough;
	case BC_BATTERY_STATUS_SET:
	case BC_USB_STATUS_SET(USB_1_PORT_ID):
	case BC_WLS_STATUS_SET:
		if (validate_message(bcdev, data, len))
			ack_set = true;

		break;
	case BC_SET_NOTIFY_REQ:
	case BC_DISABLE_NOTIFY_REQ:
	case BC_SHUTDOWN_NOTIFY:
	case BC_SHIP_MODE_REQ_SET:
	case BC_CHG_CTRL_LIMIT_EN:
		/* Always ACK response for notify or ship_mode request */
		ack_set = true;
		break;
	case BC_WLS_FW_CHECK_UPDATE:
		if (len == sizeof(*fw_check_msg)) {
			fw_check_msg = data;
			if (fw_check_msg->ret_code == 1)
				bcdev->wls_fw_update_reqd = true;
			ack_set = true;
		} else {
			pr_err("Incorrect response length %zu for wls_fw_check_update\n",
				len);
		}
		break;
	case BC_WLS_FW_PUSH_BUF_RESP:
		if (len == sizeof(*fw_resp_msg)) {
			fw_resp_msg = data;
			if (fw_resp_msg->fw_update_status == 1)
				complete(&bcdev->fw_buf_ack);
		} else {
			pr_err("Incorrect response length %zu for wls_fw_push_buf_resp\n",
				len);
		}
		break;
	case BC_WLS_FW_UPDATE_STATUS_RESP:
		if (len == sizeof(*fw_update_msg)) {
			fw_update_msg = data;
			if (fw_update_msg->fw_update_done == 1)
				complete(&bcdev->fw_update_ack);
			else
				pr_err("Wireless FW update not done %d\n",
					(int)fw_update_msg->fw_update_done);
		} else {
			pr_err("Incorrect response length %zu for wls_fw_update_status_resp\n",
				len);
		}
		break;
	case BC_WLS_FW_GET_VERSION:
		if (len == sizeof(*fw_ver_msg)) {
			fw_ver_msg = data;
			bcdev->wls_fw_version = fw_ver_msg->fw_version;
			ack_set = true;
		} else {
			pr_err("Incorrect response length %zu for wls_fw_get_version\n",
				len);
		}
		break;
#ifdef NT_EDIT
	case OEM_GET_LOG_BUFFER:
		pr_err("adsp_to_kernel_log:%s",get_logs_resp_msg->read_buffer);
		ack_set = true;
		break;
	case OEM_GET_REGISTER_BUFFER:
		memcpy(&g_get_registers_resp_msg, get_registers_resp_msg, sizeof(g_get_registers_resp_msg));
		pr_err("adsp_to_kernel_log:%s",get_registers_resp_msg->read_buffer);
		ack_set = true;
		break;
	case OEM_CHARGE_ABNORMAL:
		bcdev->nt_abnormal_status_val = abnormal_resp_msg->value;
		schedule_work(&bcdev->nt_update_event);
		pr_err("nt_abnormal_status_val:%d",bcdev->nt_abnormal_status_val);
		ack_set = true;
		break;
	case BC_SET_DEBUG_PARAM_REQ:
		pr_err("set debug param req\n");
		ack_set = true;
		break;
#endif
	default:
		pr_err("Unknown opcode: %u\n", resp_msg->hdr.opcode);
		break;
	}

	if (ack_set || bcdev->error_prop)
		complete(&bcdev->ack);
}

static struct power_supply_desc usb_psy_desc[NUM_USB_PORTS];

static void battery_chg_update_usb_type_work(struct work_struct *work)
{
	struct battery_chg_dev *bcdev = container_of(work,
					struct battery_chg_dev, usb_type_work);
	struct power_supply_desc *desc;
	struct psy_state *pst;
	int rc, i;

	for (i = 0; i < NUM_USB_PORTS; i++) {
		if (!bcdev->usb_active[i])
			continue;

		bcdev->usb_active[i] = false;

		switch (i) {
		case USB_1_PORT_ID:
			pst = &bcdev->psy_list[PSY_TYPE_USB];
			break;
		case USB_2_PORT_ID:
			pst = &bcdev->psy_list[PSY_TYPE_USB_2];
			break;
		default:
			pr_err_ratelimited("USB port %d not supported\n", i);
			continue;
		}

		desc = &usb_psy_desc[i];

		rc = read_property_id(bcdev, pst, USB_ADAP_TYPE);
		if (rc < 0) {
			pr_err("Failed to read USB_ADAP_TYPE rc=%d\n", rc);
			continue;
		}

		/* Reset usb_icl_ua whenever USB adapter type changes */
		if (pst->prop[USB_ADAP_TYPE] != POWER_SUPPLY_USB_TYPE_SDP &&
		    pst->prop[USB_ADAP_TYPE] != POWER_SUPPLY_USB_TYPE_PD)
			bcdev->usb_icl_ua[i] = 0;

		pr_debug("usb_adap_type: %u\n", pst->prop[USB_ADAP_TYPE]);

		switch (pst->prop[USB_ADAP_TYPE]) {
		case POWER_SUPPLY_USB_TYPE_SDP:
			desc->type = POWER_SUPPLY_TYPE_USB;
			break;
		case POWER_SUPPLY_USB_TYPE_DCP:
		case POWER_SUPPLY_USB_TYPE_APPLE_BRICK_ID:
		case QTI_POWER_SUPPLY_USB_TYPE_HVDCP:
		case QTI_POWER_SUPPLY_USB_TYPE_HVDCP_3:
		case QTI_POWER_SUPPLY_USB_TYPE_HVDCP_3P5:
			desc->type = POWER_SUPPLY_TYPE_USB_DCP;
			break;
		case POWER_SUPPLY_USB_TYPE_CDP:
			desc->type = POWER_SUPPLY_TYPE_USB_CDP;
			break;
		case POWER_SUPPLY_USB_TYPE_ACA:
			desc->type = POWER_SUPPLY_TYPE_USB_ACA;
			break;
		case POWER_SUPPLY_USB_TYPE_C:
			desc->type = POWER_SUPPLY_TYPE_USB_TYPE_C;
			break;
		case POWER_SUPPLY_USB_TYPE_PD:
		case POWER_SUPPLY_USB_TYPE_PD_DRP:
		case POWER_SUPPLY_USB_TYPE_PD_PPS:
			desc->type = POWER_SUPPLY_TYPE_USB_PD;
			break;
		default:
			desc->type = POWER_SUPPLY_TYPE_USB;
			break;
		}
	}
}

#ifdef NT_EDIT
#define LOW_BAT_THR 3400 * 1000
#define PLUGIN_VOLTAGE 2500 * 1000

static void nt_update_status_function_work(struct work_struct *work)
{
	struct battery_chg_dev *bcdev = container_of(work,
					struct battery_chg_dev, nt_update_status_work.work);
	int rc;
	int capacity, vbat, vusbin = 0, vwls = 0,Vinput_present = 0;
	static int	pre_capacity,Vinput_present_pre;
	struct psy_state *pst = NULL;

	if (!bcdev) {
		pr_info("bcdev is null \n");
		goto out;
	}
	pst = &bcdev->psy_list[PSY_TYPE_USB];
	if (pst && pst->psy) {
		rc = read_property_id(bcdev, pst, USB_VOLT_NOW);
		vusbin = pst->prop[USB_VOLT_NOW];
	}
	pst = &bcdev->psy_list[PSY_TYPE_WLS];
	if (pst && pst->psy) {
		rc = read_property_id(bcdev, pst, WLS_VOLT_NOW);
		vwls = pst->prop[WLS_VOLT_NOW];
	}
	Vinput_present = (vwls > PLUGIN_VOLTAGE ||vusbin > PLUGIN_VOLTAGE);
	if (Vinput_present_pre != Vinput_present) {
		pr_info("vwls:%d,vusbin:%d\n", vwls, vusbin);
		Vinput_present_pre = Vinput_present;
		if (Vinput_present) {
			if (bcdev->chg_wake) {
				pr_info("chg_wake stay_awake\n");
				__pm_stay_awake(bcdev->chg_wake);
			}
		} else {
			if (bcdev->chg_wake) {
				pr_err("chg_wake relax\n");
				__pm_relax(bcdev->chg_wake);
			}
		}
	}

	pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	if (pst && pst->psy) {
		rc = read_property_id(bcdev, pst, BATT_VOLT_NOW);
		vbat = pst->prop[BATT_VOLT_NOW];
		rc = read_property_id(bcdev, pst, BATT_CAPACITY);
		capacity = DIV_ROUND_CLOSEST(pst->prop[BATT_CAPACITY], 100);
		if (rc < 0)
			pr_info("rc:%d\n", rc);
		if ((pre_capacity != capacity) || (vbat < LOW_BAT_THR)) {
			pr_err("capacity:%d,Vbat:%d\n", capacity, vbat);
			if (pst && pst->psy) {
				power_supply_changed(pst->psy);
			}
			pm_wakeup_dev_event(bcdev->dev, 500, true);
		}
		pre_capacity = capacity;
	}
	if(bcdev->nt_abnormal_status_val == NT_NOTIFY_NORMAL)
	{
		goto out;
	}
	read_property_id(bcdev, pst, BATT_STATUS);
	if((pst->prop[BATT_STATUS] == POWER_SUPPLY_STATUS_CHARGING) || (pst->prop[BATT_STATUS] == POWER_SUPPLY_STATUS_FULL))
	{
		if(bcdev->nt_abnormal_status_val & NT_NOTIFY_CHARGER_OVER_VOL)
		{
			bcdev->notify_usbinovp_count++;
			if(bcdev->notify_usbinovp_count == NT_NOTIFY_COUNT_END)
			{
				bcdev->notify_usbinovp_count = NT_NOTIFY_COUNT_START;
				bcdev->nt_abnormal_status_val &= ~NT_NOTIFY_CHARGER_OVER_VOL;
				if (pst && pst->psy) {
					power_supply_changed(pst->psy);
				}
				if(bcdev->notify_usbinovp_flag == NT_NOTIFY_FINISH)
				{
					bcdev->notify_usbinovp_flag = NT_NOTIFY_NOT_FINISH;
				}

			}
		}
	} else {
		if(bcdev->notify_usbinovp_count != NT_NOTIFY_COUNT_START)
		{
			bcdev->notify_usbinovp_count = NT_NOTIFY_COUNT_START;
		}
	}
	if(bcdev->nt_abnormal_status_val & NT_NOTIFY_CHARGER_OVER_VOL)
	{
		if(bcdev->notify_usbinovp_flag == NT_NOTIFY_NOT_FINISH)
		{
			if (pst && pst->psy) {
				power_supply_changed(pst->psy);
			}
			bcdev->notify_usbinovp_flag = NT_NOTIFY_FINISH;
		}
	}
out:
	if(key_info_num > KEY_INFO_COUNT)
	{
		write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],NT_CHG_KEY_INFO,NT_GET_CHARGE_KEY_INFO);
		key_info_num = 0;
	} else {
		key_info_num++;
	}
	queue_delayed_work(system_wq, &bcdev->nt_update_status_work,
								round_jiffies(10 * HZ));
}

static void nt_update_event_work(struct work_struct *work)
{
	struct psy_state *pst = NULL;
	struct battery_chg_dev *bcdev = container_of(work,
					struct battery_chg_dev, nt_update_event);
	pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	if((pst->prop[BATT_STATUS] == POWER_SUPPLY_STATUS_DISCHARGING) || (pst->prop[BATT_STATUS] == POWER_SUPPLY_STATUS_NOT_CHARGING))
	{
		if(bcdev->nt_abnormal_status_val & NT_NOTIFY_CHARGER_OVER_VOL)
		{
			if(bcdev->notify_usbinovp_flag == NT_NOTIFY_NOT_FINISH)
			{
				bcdev->nt_need_update = true;
				bcdev->notify_usbinovp_flag = NT_NOTIFY_FINISH;
			}
		}
		if((bcdev->nt_abnormal_status_val & NT_NOTIFY_USB_TEMP_ABNORMAL) && (!(bcdev->nt_usb_temp_abnormal)))
		{
				bcdev->nt_usb_temp_abnormal = true;
				bcdev->nt_need_update = true;
		} else if((!(bcdev->nt_abnormal_status_val & NT_NOTIFY_USB_TEMP_ABNORMAL)) && (bcdev->nt_usb_temp_abnormal)){
				bcdev->nt_usb_temp_abnormal = false;
				bcdev->nt_need_update = true;
		}
	}
	if((bcdev->nt_abnormal_status_val & NT_NOTIFY_CHARGE_PUMP_ERR) && (!(bcdev->nt_charge_pump_abnormal)))
	{
		bcdev->nt_charge_pump_abnormal = true;
		bcdev->nt_need_update = true;
	} else if((!(bcdev->nt_abnormal_status_val & NT_NOTIFY_CHARGE_PUMP_ERR)) && (bcdev->nt_charge_pump_abnormal)){
		bcdev->nt_charge_pump_abnormal = false;
		bcdev->nt_need_update = true;
	}
	if(bcdev->nt_abnormal_status_val & (NT_NOTIFY_BAT_OVER_TEMP | NT_NOTIFY_BAT_LOW_TEMP |
		NT_NOTIFY_WLS_TX_FOD_ERR_CODE2 | NT_NOTIFY_BAT_SOC_ZERO_ERR))
	{
		bcdev->nt_need_update = true;
	}
	if((bcdev->nt_abnormal_status_val & (NT_NOTIFY_BAT_FULL_PRE_HIGH_TEMP | NT_NOTIFY_BAT_FULL_PRE_LOW_TEMP))
						&& (!(bcdev->nt_charge_full_temp_abnormal)))
	{
		bcdev->nt_charge_full_temp_abnormal = true;
		bcdev->nt_need_update = true;
	} else if((!(bcdev->nt_abnormal_status_val & (NT_NOTIFY_BAT_FULL_PRE_HIGH_TEMP | NT_NOTIFY_BAT_FULL_PRE_LOW_TEMP)))
						&& (bcdev->nt_charge_full_temp_abnormal)){
		bcdev->nt_charge_full_temp_abnormal = false;
		bcdev->nt_need_update = true;
	}
	if(bcdev->nt_need_update)
	{
		pr_err("nt_need_update:%d",bcdev->nt_need_update);
		if (pst && pst->psy) {
			power_supply_changed(pst->psy);
		}
		bcdev->nt_need_update = false;
	}
	return;
}
#endif

static void battery_chg_check_status_work(struct work_struct *work)
{
	struct battery_chg_dev *bcdev = container_of(work,
					struct battery_chg_dev,
					battery_check_work);
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;

	rc = read_property_id(bcdev, pst, BATT_STATUS);
	if (rc < 0) {
		pr_err("Failed to read BATT_STATUS, rc=%d\n", rc);
		return;
	}

	if (pst->prop[BATT_STATUS] == POWER_SUPPLY_STATUS_CHARGING) {
		pr_debug("Battery is charging\n");
		return;
	}

	rc = read_property_id(bcdev, pst, BATT_CAPACITY);
	if (rc < 0) {
		pr_err("Failed to read BATT_CAPACITY, rc=%d\n", rc);
		return;
	}

	if (DIV_ROUND_CLOSEST(pst->prop[BATT_CAPACITY], 100) > 0) {
		pr_debug("Battery SOC is > 0\n");
		return;
	}

	/*
	 * If we are here, then battery is not charging and SOC is 0.
	 * Check the battery voltage and if it's lower than shutdown voltage,
	 * then initiate an emergency shutdown.
	 */

	rc = read_property_id(bcdev, pst, BATT_VOLT_NOW);
	if (rc < 0) {
		pr_err("Failed to read BATT_VOLT_NOW, rc=%d\n", rc);
		return;
	}

	if (pst->prop[BATT_VOLT_NOW] / 1000 > bcdev->shutdown_volt_mv) {
		pr_debug("Battery voltage is > %d mV\n",
			bcdev->shutdown_volt_mv);
		return;
	}

	pr_emerg("Initiating a shutdown in 100 ms\n");
	msleep(100);
	pr_emerg("Attempting kernel_power_off: Battery voltage low\n");
	kernel_power_off();
}
#ifdef NT_EDIT
int force_set_usb_icl(struct battery_chg_dev *bcdev, int val)
{
	struct battman_set_debug_param_req_msg req_msg = { { 0 } };
	int rc = 0;

	req_msg.hdr.owner = MSG_OWNER_BC;
	req_msg.hdr.type = MSG_TYPE_REQ_RESP;
	req_msg.hdr.opcode = BC_SET_DEBUG_PARAM_REQ;
	req_msg.debug_param_property_id = BC_DEBUG_PARAM_AGING_TEST;
	if (val < 0)
		req_msg.data = UINT_MAX;
	else
		req_msg.data = (u32)val/1000;

	rc = battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
	if (rc < 0)
		pr_err("Failed to set icl, rc=%d\n", rc);
	else
		pr_err("aging_test: set icl %d mA\n", req_msg.data);

	return rc;
}

static int is_aging_test_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;

	seq_printf(m, "is_aging_test:%d\n", bcdev->is_aging_test);

	return 0;
}

static int is_aging_test_open(struct inode *inode, struct file *file)
{
	return single_open(file, is_aging_test_show, pde_data(inode));
}

static ssize_t is_aging_test_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;

	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc is_aging_test states fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &bcdev->is_aging_test))
		goto exit;
	pr_info("write is_aging_test %d success\n", bcdev->is_aging_test);

	if (!bcdev->is_aging_test)
		force_set_usb_icl(bcdev, -1);

exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static ssize_t nt_data_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	int rc;
	int i;
	int param_temp[10] = {0,0,0,0,0,0,0,0,0,0};
	u8 *buf_tmp = NULL;
	int buflen = count;
	if(bcdev == NULL)
	{
		pr_err("bcdev is NULL\n");
		return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_chg_data fail\n");
		goto exit;
	}
	rc = sscanf(buf_tmp,"%d %d %d %d %d %d %d %d %d %d",&(param[0]),&(param[1]),&(param[2]),&(param[3]),&(param[4]),&(param[5]),&(param[6]),&(param[7]),&(param[8]),&(param[9]));
	if(rc == 10)
	{
		if(param[0] == NT_CHG_USB_TEMP)
		{
			for (i = 1; i <= 7; i++)
			{
				param_temp[i] = param[i] + 10000*usb_temp_type[i-1] + 100000*NT_CHG_USB_TEMP;
			}
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[1]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[2]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[3]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[4]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[5]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[6]);
			rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHG_PARAM, param_temp[7]);
		}
	} else {
		pr_err("nt_chg_data_write failed\n");
	}
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int nt_data_show(struct seq_file *m, void *v)
{
    seq_printf(m,"%d %d %d %d %d %d %d %d %d %d\n",param[0],param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9]);
    return 0;
}

static int nt_data_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_data_show, pde_data(inode));
}


static int battery_health_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
		return -EINVAL;
	}

	rc = read_property_id(bcdev, pst, BATT_SOH);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_SOH]);
	return 0;
}

static int battery_health_open(struct inode *inode, struct file *file)
{
    return single_open(file, battery_health_show, pde_data(inode));
}

static int chg_data_id_show(struct seq_file *m, void *v)
{
    struct battery_chg_dev *bcdev = m->private;
    seq_printf(m, "%d\n", bcdev->chg_data_id);
    return 0;
}

static int chg_data_id_open(struct inode *inode, struct file *file)
{
    return single_open(file, chg_data_id_show, pde_data(inode));
}

static int nt_typec_cc_orientation_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_CC_ORIENTATION);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_CC_ORIENTATION]);
	return 0;
}
static int nt_typec_cc_orientation_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_typec_cc_orientation_show, pde_data(inode));
}

static int nt_charge_pump_enable_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_CHARGE_PUMP_ENABLE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_CHARGE_PUMP_ENABLE]);
	return 0;
}
static int nt_charge_pump_enable_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_charge_pump_enable_show, pde_data(inode));
}

static int nt_charge_exist_pump_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_EXIST_CHARGE_PUMP);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_EXIST_CHARGE_PUMP]);
	return 0;
}
static int nt_charge_exist_pump_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_charge_exist_pump_show, pde_data(inode));
}

static int nt_charge_exist_buck_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_EXIST_CHARGE_BUCK);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_EXIST_CHARGE_BUCK]);
	return 0;
}
static int nt_charge_exist_buck_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_charge_exist_buck_show, pde_data(inode));
}

static int usb_charger_en_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_CHARGE_ENABLE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_CHARGE_ENABLE]);
	return 0;
}
static int usb_charger_en_open(struct inode *inode, struct file *file)
{
	return single_open(file, usb_charger_en_show, pde_data(inode));
}
static ssize_t usb_charger_en_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
		pr_err("bcdev is NULL\n");
		return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc usb_charger_en fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
	{
		pr_err("kstrtou32 fail\n");
		goto exit;
	}
	pr_err("%s,val:%d", __func__, val);
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_CHARGE_ENABLE, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int scenario_fcc_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_SCENARIO_FCC);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_SCENARIO_FCC]);
	return 0;
}
static int scenario_fcc_open(struct inode *inode, struct file *file)
{
	return single_open(file, scenario_fcc_show, pde_data(inode));
}
static ssize_t scenario_fcc_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
		pr_err("bcdev is NULL\n");
		return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc scenario_fcc fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
	{
		pr_err("kstrtou32 fail\n");
		goto exit;
	}
	pr_err("%s,val:%d", __func__, val);
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_SCENARIO_FCC, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int nt_otg_enable_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_OTG_ENABLE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_OTG_ENABLE]);
	return 0;
}
static int nt_otg_enable_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_otg_enable_show, pde_data(inode));
}
static ssize_t nt_otg_enable_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
		pr_err("bcdev is NULL\n");
		return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
	{
		pr_err("kstrtou32 fail\n");
		goto exit;
	}
	pr_err("val:%d\n",val);
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
				NT_OTG_ENABLE, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int nt_charge_buck_enable_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_CHARGE_BUCK_ENABLE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_CHARGE_BUCK_ENABLE]);
	return 0;
}
static int nt_charge_buck_enable_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_charge_buck_enable_show, pde_data(inode));
}
static int charge_dump_reg_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc = 0;
	if((pst == NULL) || (bcdev == NULL))
		return -EINVAL;

	rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_USB],
		NT_GET_CHIP_REG, 0);
	if (rc < 0) {
		pr_err("write_property_id fail\n");
		return rc;
	}
	msleep(200);
	seq_printf(m, "%s\n", g_get_registers_resp_msg.read_buffer);
	memset(&g_get_registers_resp_msg, 0, sizeof(g_get_registers_resp_msg));
	return 0;
}

static int charge_dump_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, charge_dump_reg_show, pde_data(inode));
}

static int nt_abnormal_status_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	seq_printf(m, "%d\n",bcdev->nt_abnormal_status_val);
	return 0;
}

static int nt_abnormal_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_abnormal_status_show, pde_data(inode));
}
static int nt_fake_ibat_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_IBAT);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_FAKE_IBAT]);
	return 0;
}

static int nt_fake_ibat_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_fake_ibat_show, pde_data(inode));
}

static ssize_t nt_fake_ibat_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtoint(buf_tmp, 0, &val))
		goto exit;
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_FAKE_IBAT, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}


//fake vbat
static int nt_fake_vbat_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_VBAT);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_FAKE_VBAT]);
	return 0;
}

static int nt_fake_vbat_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_fake_vbat_show, pde_data(inode));
}

static ssize_t nt_fake_vbat_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
		goto exit;
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_FAKE_VBAT, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}


//fake tbat
static int nt_fake_tbat_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_TBAT);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_FAKE_TBAT]);
	return 0;
}

static int nt_fake_tbat_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_fake_tbat_show, pde_data(inode));
}

static ssize_t nt_fake_tbat_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtoint(buf_tmp, 0, &val))
		goto exit;
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_FAKE_TBAT, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

//fake tusb
static int nt_fake_tusb_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_TUSB);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_FAKE_TUSB]);
	return 0;
}

static int nt_fake_tusb_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_fake_tusb_show, pde_data(inode));
}

static ssize_t nt_fake_tusb_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtoint(buf_tmp, 0, &val))
		goto exit;
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_FAKE_TUSB, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

//fake soc
static int nt_fake_soc_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_SOC);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_FAKE_SOC]);
	return 0;
}

static int nt_fake_soc_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_fake_soc_show, pde_data(inode));
}

static ssize_t nt_fake_soc_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc nt_otg_enable fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
		goto exit;
	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_FAKE_SOC, val);
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int voltage_adc_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_VOLT_NOW);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_VOLT_NOW]);
	return 0;
}

static int voltage_adc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voltage_adc_show, pde_data(inode));
}
static const char *get_usb_type_name(u32 usb_type);
static int usb_real_type_proc_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, USB_REAL_TYPE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%s\n", get_usb_type_name(pst->prop[USB_REAL_TYPE]));
	return 0;
}

static int usb_real_type_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, usb_real_type_proc_show, pde_data(inode));
}

static int ship_mode_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_FAKE_SOC);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", bcdev->ship_mode_en);
	return 0;
}

static int ship_mode_open(struct inode *inode, struct file *file)
{
	return single_open(file, ship_mode_show, pde_data(inode));
}

static ssize_t ship_mode_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc ship_mode fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
		goto exit;

	if (val > 0)
		bcdev->ship_mode_en = true;
	else
		bcdev->ship_mode_en = false;
exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int usb_temp_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, USB_TEMP);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[USB_TEMP]);
	return 0;
}

static int usb_temp_open(struct inode *inode, struct file *file)
{
	return single_open(file, usb_temp_show, pde_data(inode));
}

static int g_shell_temp = 0;
static int shell_temp_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;

	if(bcdev == NULL)
        return -EINVAL;

	seq_printf(m, "%d\n", g_shell_temp);
	return 0;
}

static int shell_temp_open(struct inode *inode, struct file *file)
{
	return single_open(file, shell_temp_show, pde_data(inode));
}

static ssize_t shell_temp_write(struct file *file, const char __user *buff,
               size_t count, loff_t *ppos)
{
	struct battery_chg_dev *bcdev = pde_data(file_inode(file));
	u8 *buf_tmp = NULL;
	int buflen = count;
	u32 val;
	if(bcdev == NULL)
	{
        return -EINVAL;
	}
	if (buflen < 0) {
		pr_err("proc count fail:%d\n", buflen);
		return -EINVAL;
	} else {
		buf_tmp = (u8 *)kzalloc((buflen + 1) * sizeof(u8), GFP_KERNEL);
		if (buf_tmp == NULL) {
			pr_err("proc write buf zalloc fail\n");
			return -ENOMEM;
		}
	}

	if (copy_from_user(buf_tmp, buff, buflen)) {
		pr_err("proc ship_mode fail\n");
		goto exit;
	}

	if (kstrtou32(buf_tmp, 0, &val))
		goto exit;
	g_shell_temp = val;

exit:
	kfree(buf_tmp);
	buf_tmp = NULL;
	return buflen;
}

static int nt_qmax_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_CHG_FULL_DESIGN);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_CHG_FULL_DESIGN]);
	return 0;
}

static int nt_qmax_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_qmax_show, pde_data(inode));
}

static int nt_quse_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_CHG_FULL);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_CHG_FULL]);
	return 0;
}

static int nt_quse_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_quse_show, pde_data(inode));
}

static int nt_resistance_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_RESISTANCE);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_RESISTANCE]);
	return 0;
}

static int nt_resistance_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_resistance_show, pde_data(inode));
}
static int nt_adp_power_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, NT_ADP_POWER);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[NT_ADP_POWER]);
	return 0;
}

static int nt_adp_power_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_adp_power_show, pde_data(inode));
}

static int nt_maxchargercurrent_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, USB_CURR_MAX);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[USB_CURR_MAX]);
	return 0;
}

static int nt_maxchargervoltage_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, USB_VOLT_MAX);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[USB_VOLT_MAX]);
	return 0;
}

static int nt_maxchargervoltage_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_maxchargervoltage_show, pde_data(inode));
}

static int nt_maxchargercurrent_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_maxchargercurrent_show, pde_data(inode));
}

static int nt_ibus_now_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_USB];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, USB_CURR_NOW);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[USB_CURR_NOW]);
	return 0;
}

static int nt_ibus_now_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_ibus_now_show, pde_data(inode));
}

static int nt_real_soc_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	int capacity = 0;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}

	rc = read_property_id(bcdev, pst, BATT_CAPACITY);
	capacity = DIV_ROUND_CLOSEST(pst->prop[BATT_CAPACITY], 100);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", capacity);
	return 0;
}

static int nt_real_soc_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_real_soc_show, pde_data(inode));
}

static int nt_battid_show(struct seq_file *m, void *v)
{
	struct battery_chg_dev *bcdev = m->private;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int rc;
	if((pst == NULL) || (bcdev == NULL))
	{
        return -EINVAL;
	}
	rc = read_property_id(bcdev, pst, BATT_ID);
	if (rc < 0)
		return rc;
	seq_printf(m, "%d\n", pst->prop[BATT_ID]);
	return 0;
}

static int nt_battid_open(struct inode *inode, struct file *file)
{
	return single_open(file, nt_battid_show, pde_data(inode));
}

const struct nt_proc entries[] = {
	{"battery_health",{.proc_open = battery_health_open,
                      .proc_read = seq_read,
                      .proc_lseek = seq_lseek,}
	},
	{"chg_data_id",{.proc_open = chg_data_id_open,
                      .proc_read = seq_read,
                      .proc_lseek = seq_lseek,}
	},
	{"typec_cc_orientation",{.proc_open = nt_typec_cc_orientation_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,}
	},
	{"charge_pump_enable",{.proc_open = nt_charge_pump_enable_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"charge_exist_pump",{.proc_open = nt_charge_exist_pump_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"charge_exist_buck",{.proc_open = nt_charge_exist_buck_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"usb_charger_en",{.proc_open = usb_charger_en_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = usb_charger_en_write,}
	},
	{"scenario_fcc",{.proc_open = scenario_fcc_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = scenario_fcc_write,}
	},
	{"nt_otg_enable",{.proc_open = nt_otg_enable_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_otg_enable_write,}
	},
	{"charge_buck_enable",{.proc_open = nt_charge_buck_enable_open,
		              .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"charge_dump_reg",{.proc_open = charge_dump_reg_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"is_aging_test",{.proc_open = is_aging_test_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
					  .proc_write = is_aging_test_write,}
	},
	{"nt_data",{.proc_open = nt_data_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
					  .proc_write = nt_data_write,}
	},
	{"nt_abnormal_status",{.proc_open = nt_abnormal_status_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"nt_fake_ibat",{.proc_open = nt_fake_ibat_open,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_fake_ibat_write,}
	},
	{"nt_fake_vbat",{.proc_open = nt_fake_vbat_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_fake_vbat_write,}
	},
	{"nt_fake_tbat",{.proc_open = nt_fake_tbat_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_fake_tbat_write,}
	},
	{"nt_fake_tusb",{.proc_open = nt_fake_tusb_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_fake_tusb_write,}
	},
	{"nt_fake_soc",{.proc_open = nt_fake_soc_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = nt_fake_soc_write,}
	},
	{"voltage_adc",{.proc_open = voltage_adc_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"usb_real_type",{.proc_open = usb_real_type_proc_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"ship_mode",{.proc_open = ship_mode_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = ship_mode_write,}
	},
	{"usb_temp",{.proc_open = usb_temp_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"temp",{.proc_open = shell_temp_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,
	                  .proc_write = shell_temp_write,}
	},
	{"nt_qmax",{.proc_open = nt_qmax_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"nt_quse",{.proc_open = nt_quse_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"nt_resistance",{.proc_open = nt_resistance_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"nt_adp_power",{.proc_open = nt_adp_power_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"maxchargercurrent",{.proc_open = nt_maxchargercurrent_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"maxchargervoltage",{.proc_open = nt_maxchargervoltage_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"ibus_now",{.proc_open = nt_ibus_now_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"real_soc",{.proc_open = nt_real_soc_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	},
	{"battid",{.proc_open = nt_battid_open,
	                  .proc_read = seq_read,
	                  .proc_lseek = seq_lseek,
	                  .proc_release = single_release,}
	}
};
#endif

static void handle_notification(struct battery_chg_dev *bcdev, void *data,
				size_t len)
{
	struct battery_charger_notify_msg *notify_msg = data;
	struct psy_state *pst = NULL;
	u32 hboost_vmax_mv, notification;

	if (len != sizeof(*notify_msg)) {
		pr_err("Incorrect response length %zu\n", len);
		return;
	}

	notification = notify_msg->notification;
	pr_debug("notification: %#x\n", notification);
	if ((notification & 0xffff) == BC_HBOOST_VMAX_CLAMP_NOTIFY) {
		hboost_vmax_mv = (notification >> 16) & 0xffff;
		raw_notifier_call_chain(&hboost_notifier, VMAX_CLAMP, &hboost_vmax_mv);
		pr_debug("hBoost is clamped at %u mV\n", hboost_vmax_mv);
		return;
	}

	switch (notification) {
	case BC_BATTERY_STATUS_GET:
	case BC_GENERIC_NOTIFY:
		pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
		if (bcdev->shutdown_volt_mv > 0)
			schedule_work(&bcdev->battery_check_work);
		break;
	case BC_USB_STATUS_GET(USB_1_PORT_ID):
		bcdev->usb_active[USB_1_PORT_ID] = true;
		pst = &bcdev->psy_list[PSY_TYPE_USB];
		schedule_work(&bcdev->usb_type_work);
		break;
	case BC_USB_STATUS_GET(USB_2_PORT_ID):
		if (bcdev->num_usb_ports < 2) {
			pr_debug("notification: %u for missing USB_2 port\n",
					notification);
			break;
		}
		bcdev->usb_active[USB_2_PORT_ID] = true;
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
		schedule_work(&bcdev->usb_type_work);
		break;
	case BC_WLS_STATUS_GET:
		pst = &bcdev->psy_list[PSY_TYPE_WLS];
		break;
	default:
		break;
	}

	if (pst && pst->psy) {
		/*
		 * For charger mode, keep the device awake at least for 50 ms
		 * so that device won't enter suspend when a non-SDP charger
		 * is removed. This would allow the userspace process like
		 * "charger" to be able to read power supply uevents to take
		 * appropriate actions (e.g. shutting down when the charger is
		 * unplugged).
		 */
		power_supply_changed(pst->psy);
		pm_wakeup_dev_event(bcdev->dev, 50, true);
	}
}

static int battery_chg_callback(void *priv, void *data, size_t len)
{
	struct pmic_glink_hdr *hdr = data;
	struct battery_chg_dev *bcdev = priv;

	pr_debug("owner: %u type: %u opcode: %#x len: %zu\n", hdr->owner,
		hdr->type, hdr->opcode, len);

	down_read(&bcdev->state_sem);
	if (!bcdev->initialized) {
		pr_debug("Driver initialization failed: Dropping glink callback message: state %d\n",
			 bcdev->state);
		up_read(&bcdev->state_sem);
		return 0;
	}
	up_read(&bcdev->state_sem);

	if (hdr->opcode == BC_NOTIFY_IND)
		handle_notification(bcdev, data, len);
	else
		handle_message(bcdev, data, len);

	return 0;
}

static int wls_psy_get_prop(struct power_supply *psy,
		enum power_supply_property prop,
		union power_supply_propval *pval)
{
	struct battery_chg_dev *bcdev = power_supply_get_drvdata(psy);
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_WLS];
	int prop_id, rc;

	pval->intval = -ENODATA;

	prop_id = get_property_id(pst, prop);
	if (prop_id < 0)
		return prop_id;

	rc = read_property_id(bcdev, pst, prop_id);
	if (rc < 0)
		return rc;

	pval->intval = pst->prop[prop_id];

	return 0;
}

static int wls_psy_set_prop(struct power_supply *psy,
		enum power_supply_property prop,
		const union power_supply_propval *pval)
{
	return 0;
}

static int wls_psy_prop_is_writeable(struct power_supply *psy,
		enum power_supply_property prop)
{
	return 0;
}

static enum power_supply_property wls_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	POWER_SUPPLY_PROP_TEMP,
};

static const struct power_supply_desc wls_psy_desc = {
	.name			= "wireless",
	.type			= POWER_SUPPLY_TYPE_WIRELESS,
	.properties		= wls_props,
	.num_properties		= ARRAY_SIZE(wls_props),
	.get_property		= wls_psy_get_prop,
	.set_property		= wls_psy_set_prop,
	.property_is_writeable	= wls_psy_prop_is_writeable,
};

static const char *get_wls_type_name(u32 wls_type)
{
	if (wls_type >= ARRAY_SIZE(qc_power_supply_wls_type_text))
		return "Unknown";

	return qc_power_supply_wls_type_text[wls_type];
}

static const char *get_usb_type_name(u32 usb_type)
{
	u32 i;

	if (usb_type >= QTI_POWER_SUPPLY_USB_TYPE_HVDCP &&
	    usb_type <= QTI_POWER_SUPPLY_USB_TYPE_HVDCP_3P5) {
		for (i = 0; i < ARRAY_SIZE(qc_power_supply_usb_type_text);
		     i++) {
			if (i == (usb_type - QTI_POWER_SUPPLY_USB_TYPE_HVDCP))
				return qc_power_supply_usb_type_text[i];
		}
		return "Unknown";
	}

	for (i = 0; i < ARRAY_SIZE(power_supply_usb_type_text); i++) {
		if (i == usb_type)
			return power_supply_usb_type_text[i];
	}

	return "Unknown";
}

static int usb_psy_set_icl(struct battery_chg_dev *bcdev,
		struct psy_state *pst, u32 prop_id, int val)
{
	u32 temp;
	int rc;

	rc = read_property_id(bcdev, pst, USB_ADAP_TYPE);
	if (rc < 0) {
		pr_err("Failed to read prop USB_ADAP_TYPE, rc=%d\n", rc);
		return rc;
	}

#ifndef NT_EDIT
	/* Allow this only for SDP, CDP or USB_PD and not for other charger types */
	switch (pst->prop[USB_ADAP_TYPE]) {
	case POWER_SUPPLY_USB_TYPE_SDP:
	case POWER_SUPPLY_USB_TYPE_PD:
	case POWER_SUPPLY_USB_TYPE_CDP:
		break;
	default:
		return -EINVAL;
	}
#else
	return 0;
#endif

	/*
	 * Input current limit (ICL) can be set by different clients. E.g. USB
	 * driver can request for a current of 500/900 mA depending on the
	 * port type. Also, clients like EUD driver can pass 0 or -22 to
	 * suspend or unsuspend the input for its use case.
	 */

	temp = val;
	if (val < 0)
		temp = UINT_MAX;

	rc = write_property_id(bcdev, pst, prop_id, temp);
	if (rc < 0) {
		pr_err("Failed to set ICL (%u uA) rc=%d\n", temp, rc);
	} else {
		pr_debug("Set ICL to %u\n", temp);

		if (!strcmp(pst->psy->desc->name, "usb-2"))
			bcdev->usb_icl_ua[USB_2_PORT_ID] = temp;
		else
			bcdev->usb_icl_ua[USB_1_PORT_ID] = temp;
	}

	return rc;
}

static int usb_psy_get_prop(struct power_supply *psy,
		enum power_supply_property prop,
		union power_supply_propval *pval)
{
	struct battery_chg_dev *bcdev = power_supply_get_drvdata(psy);
	struct psy_state *pst;
	int prop_id, rc;

	if (!strcmp(psy->desc->name, "usb-2"))
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
	else
		pst = &bcdev->psy_list[PSY_TYPE_USB];

	pval->intval = -ENODATA;
	prop_id = get_property_id(pst, prop);
	if (prop_id < 0)
		return prop_id;

	rc = read_property_id(bcdev, pst, prop_id);
	if (rc < 0)
		return rc;

	pval->intval = pst->prop[prop_id];
	if (prop == POWER_SUPPLY_PROP_TEMP)
		pval->intval = DIV_ROUND_CLOSEST((int)pval->intval, 10);

	return 0;
}

static int usb_psy_set_prop(struct power_supply *psy,
		enum power_supply_property prop,
		const union power_supply_propval *pval)
{
	struct battery_chg_dev *bcdev = power_supply_get_drvdata(psy);
	struct psy_state *pst;
	int prop_id, rc = 0;

	if (!strcmp(psy->desc->name, "usb-2"))
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
	else
		pst = &bcdev->psy_list[PSY_TYPE_USB];

	prop_id = get_property_id(pst, prop);
	if (prop_id < 0)
		return prop_id;

	switch (prop) {
	case POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT:
#ifdef NT_EDIT
		if(bcdev->is_aging_test)
			rc = force_set_usb_icl(bcdev, pval->intval);
		else
#endif
		rc = usb_psy_set_icl(bcdev, pst, prop_id, pval->intval);
		break;
	default:
		break;
	}

	return rc;
}

static int usb_psy_prop_is_writeable(struct power_supply *psy,
		enum power_supply_property prop)
{
	switch (prop) {
	case POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT:
		return 1;
	default:
		break;
	}

	return 0;
}

static enum power_supply_property usb_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	POWER_SUPPLY_PROP_USB_TYPE,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_usb_type usb_psy_supported_types[] = {
	POWER_SUPPLY_USB_TYPE_UNKNOWN,
	POWER_SUPPLY_USB_TYPE_SDP,
	POWER_SUPPLY_USB_TYPE_DCP,
	POWER_SUPPLY_USB_TYPE_CDP,
	POWER_SUPPLY_USB_TYPE_ACA,
	POWER_SUPPLY_USB_TYPE_C,
	POWER_SUPPLY_USB_TYPE_PD,
	POWER_SUPPLY_USB_TYPE_PD_DRP,
	POWER_SUPPLY_USB_TYPE_PD_PPS,
	POWER_SUPPLY_USB_TYPE_APPLE_BRICK_ID,
};

static struct power_supply_desc usb_psy_desc[NUM_USB_PORTS] = {
	[USB_1_PORT_ID] = {
		.name			= "usb",
		.type			= POWER_SUPPLY_TYPE_USB,
		.properties		= usb_props,
		.num_properties		= ARRAY_SIZE(usb_props),
		.get_property		= usb_psy_get_prop,
		.set_property		= usb_psy_set_prop,
		.usb_types		= usb_psy_supported_types,
		.num_usb_types		= ARRAY_SIZE(usb_psy_supported_types),
		.property_is_writeable	= usb_psy_prop_is_writeable,
	},

	[USB_2_PORT_ID] = {
		.name			= "usb-2",
		.type			= POWER_SUPPLY_TYPE_USB,
		.properties		= usb_props,
		.num_properties		= ARRAY_SIZE(usb_props),
		.get_property		= usb_psy_get_prop,
		.set_property		= usb_psy_set_prop,
		.usb_types		= usb_psy_supported_types,
		.num_usb_types		= ARRAY_SIZE(usb_psy_supported_types),
		.property_is_writeable	= usb_psy_prop_is_writeable,
	}
};

#define CHARGE_CTRL_START_THR_MIN	50
#define CHARGE_CTRL_START_THR_MAX	95
#define CHARGE_CTRL_END_THR_MIN		55
#define CHARGE_CTRL_END_THR_MAX		100
#define CHARGE_CTRL_DELTA_SOC		5

static int battery_psy_set_charge_threshold(struct battery_chg_dev *bcdev,
					u32 target_soc, u32 delta_soc)
{
	struct battery_charger_chg_ctrl_msg msg = { { 0 } };
	int rc;

	if (!bcdev->chg_ctrl_en)
		return 0;

	if (target_soc > CHARGE_CTRL_END_THR_MAX)
		target_soc = CHARGE_CTRL_END_THR_MAX;

	msg.hdr.owner = MSG_OWNER_BC;
	msg.hdr.type = MSG_TYPE_REQ_RESP;
	msg.hdr.opcode = BC_CHG_CTRL_LIMIT_EN;
	msg.enable = 1;
	msg.target_soc = target_soc;
	msg.delta_soc = delta_soc;

	rc = battery_chg_write(bcdev, &msg, sizeof(msg));
	if (rc < 0)
		pr_err("Failed to set charge_control thresholds, rc=%d\n", rc);
	else
		pr_debug("target_soc: %u delta_soc: %u\n", target_soc, delta_soc);

	return rc;
}

static int battery_psy_set_charge_end_threshold(struct battery_chg_dev *bcdev,
					int val)
{
	u32 delta_soc = CHARGE_CTRL_DELTA_SOC;
	int rc;

	if (val < CHARGE_CTRL_END_THR_MIN ||
	    val > CHARGE_CTRL_END_THR_MAX) {
		pr_err("Charge control end_threshold should be within [%u %u]\n",
			CHARGE_CTRL_END_THR_MIN, CHARGE_CTRL_END_THR_MAX);
		return -EINVAL;
	}

	if (bcdev->chg_ctrl_start_thr && val > bcdev->chg_ctrl_start_thr)
		delta_soc = val - bcdev->chg_ctrl_start_thr;

	rc = battery_psy_set_charge_threshold(bcdev, val, delta_soc);
	if (rc < 0)
		pr_err("Failed to set charge control end threshold %u, rc=%d\n",
			val, rc);
	else
		bcdev->chg_ctrl_end_thr = val;

	return rc;
}

static int battery_psy_set_charge_start_threshold(struct battery_chg_dev *bcdev,
					int val)
{
	u32 target_soc, delta_soc;
	int rc;

	if (val < CHARGE_CTRL_START_THR_MIN ||
	    val > CHARGE_CTRL_START_THR_MAX) {
		pr_err("Charge control start_threshold should be within [%u %u]\n",
			CHARGE_CTRL_START_THR_MIN, CHARGE_CTRL_START_THR_MAX);
		return -EINVAL;
	}

	if (val > bcdev->chg_ctrl_end_thr) {
		target_soc = val +  CHARGE_CTRL_DELTA_SOC;
		delta_soc = CHARGE_CTRL_DELTA_SOC;
	} else {
		target_soc = bcdev->chg_ctrl_end_thr;
		delta_soc = bcdev->chg_ctrl_end_thr - val;
	}

	rc = battery_psy_set_charge_threshold(bcdev, target_soc, delta_soc);
	if (rc < 0)
		pr_err("Failed to set charge control start threshold %u, rc=%d\n",
			val, rc);
	else
		bcdev->chg_ctrl_start_thr = val;

	return rc;
}

static int get_charge_control_en(struct battery_chg_dev *bcdev)
{
	int rc;

	rc = read_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_CHG_CTRL_EN);
	if (rc < 0)
		pr_err("Failed to read the CHG_CTRL_EN, rc = %d\n", rc);
	else
		bcdev->chg_ctrl_en =
			bcdev->psy_list[PSY_TYPE_BATTERY].prop[BATT_CHG_CTRL_EN];

	return rc;
}

static int __battery_psy_set_charge_current(struct battery_chg_dev *bcdev,
					u32 fcc_ua)
{
	int rc;

	if (bcdev->restrict_chg_en) {
		fcc_ua = min_t(u32, fcc_ua, bcdev->restrict_fcc_ua);
		fcc_ua = min_t(u32, fcc_ua, bcdev->thermal_fcc_ua);
	}

	rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_CHG_CTRL_LIM, fcc_ua);
	if (rc < 0) {
		pr_err("Failed to set FCC %u, rc=%d\n", fcc_ua, rc);
	} else {
		pr_debug("Set FCC to %u uA\n", fcc_ua);
		bcdev->last_fcc_ua = fcc_ua;
	}

	return rc;
}

static int battery_psy_set_charge_current(struct battery_chg_dev *bcdev,
					int val)
{
	int rc;
	u32 fcc_ua, prev_fcc_ua;

	if (!bcdev->num_thermal_levels)
		return 0;

	if (bcdev->num_thermal_levels < 0) {
		pr_err("Incorrect num_thermal_levels\n");
		return -EINVAL;
	}

	if (val < 0 || val > bcdev->num_thermal_levels)
		return -EINVAL;

	if (bcdev->thermal_fcc_step == 0)
		fcc_ua = bcdev->thermal_levels[val];
	else
		fcc_ua = bcdev->psy_list[PSY_TYPE_BATTERY].prop[BATT_CHG_CTRL_LIM_MAX]
				- (bcdev->thermal_fcc_step * val);

	prev_fcc_ua = bcdev->thermal_fcc_ua;
	bcdev->thermal_fcc_ua = fcc_ua;

	rc = __battery_psy_set_charge_current(bcdev, fcc_ua);
	if (!rc)
		bcdev->curr_thermal_level = val;
	else
		bcdev->thermal_fcc_ua = prev_fcc_ua;

	return rc;
}

static int battery_psy_get_prop(struct power_supply *psy,
		enum power_supply_property prop,
		union power_supply_propval *pval)
{
	struct battery_chg_dev *bcdev = power_supply_get_drvdata(psy);
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int prop_id, rc;

	pval->intval = -ENODATA;

	/*
	 * The prop id of TIME_TO_FULL_NOW and TIME_TO_FULL_AVG is same.
	 * So, map the prop id of TIME_TO_FULL_AVG for TIME_TO_FULL_NOW.
	 */
	if (prop == POWER_SUPPLY_PROP_TIME_TO_FULL_NOW) {
		prop = POWER_SUPPLY_PROP_TIME_TO_FULL_AVG;
#ifdef NT_EDIT
		rc = 0;
		pval->intval = -1;
		return rc;
#endif
	}
	prop_id = get_property_id(pst, prop);
	if (prop_id < 0)
		return prop_id;

	rc = read_property_id(bcdev, pst, prop_id);
	if (rc < 0)
		return rc;

	switch (prop) {
	case POWER_SUPPLY_PROP_MODEL_NAME:
		pval->strval = pst->model;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		pval->intval = DIV_ROUND_CLOSEST(pst->prop[prop_id], 100);
		if (IS_ENABLED(CONFIG_QTI_PMIC_GLINK_CLIENT_DEBUG) &&
		   (bcdev->fake_soc >= 0 && bcdev->fake_soc <= 100))
			pval->intval = bcdev->fake_soc;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		pval->intval = DIV_ROUND_CLOSEST((int)pst->prop[prop_id], 10);
		break;
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT:
		pval->intval = bcdev->curr_thermal_level;
		break;
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX:
		pval->intval = bcdev->num_thermal_levels;
		break;
	default:
		pval->intval = pst->prop[prop_id];
		break;
	}

	return rc;
}

static int battery_psy_set_prop(struct power_supply *psy,
		enum power_supply_property prop,
		const union power_supply_propval *pval)
{
	struct battery_chg_dev *bcdev = power_supply_get_drvdata(psy);

	switch (prop) {
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT:
		return battery_psy_set_charge_current(bcdev, pval->intval);
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_START_THRESHOLD:
		return battery_psy_set_charge_start_threshold(bcdev,
								pval->intval);
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_END_THRESHOLD:
		return battery_psy_set_charge_end_threshold(bcdev,
								pval->intval);
	default:
		return -EINVAL;
	}

	return 0;
}

static int battery_psy_prop_is_writeable(struct power_supply *psy,
		enum power_supply_property prop)
{
	switch (prop) {
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT:
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_START_THRESHOLD:
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_END_THRESHOLD:
		return 1;
	default:
		break;
	}

	return 0;
}

static enum power_supply_property battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_VOLTAGE_OCV,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_START_THRESHOLD,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_END_THRESHOLD,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CHARGE_COUNTER,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_TIME_TO_FULL_AVG,
	POWER_SUPPLY_PROP_TIME_TO_FULL_NOW,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG,
	POWER_SUPPLY_PROP_POWER_NOW,
	POWER_SUPPLY_PROP_POWER_AVG,
};

static const struct power_supply_desc batt_psy_desc = {
	.name			= "battery",
	.type			= POWER_SUPPLY_TYPE_BATTERY,
	.properties		= battery_props,
	.num_properties		= ARRAY_SIZE(battery_props),
	.get_property		= battery_psy_get_prop,
	.set_property		= battery_psy_set_prop,
	.property_is_writeable	= battery_psy_prop_is_writeable,
};

static int battery_chg_init_psy(struct battery_chg_dev *bcdev)
{
	struct power_supply_config psy_cfg = {};
	struct psy_state *pst;
	int rc;

	psy_cfg.drv_data = bcdev;
	psy_cfg.of_node = bcdev->dev->of_node;
	bcdev->psy_list[PSY_TYPE_USB].psy =
		devm_power_supply_register(bcdev->dev, &usb_psy_desc[USB_1_PORT_ID], &psy_cfg);
	if (IS_ERR(bcdev->psy_list[PSY_TYPE_USB].psy)) {
		rc = PTR_ERR(bcdev->psy_list[PSY_TYPE_USB].psy);
		bcdev->psy_list[PSY_TYPE_USB].psy = NULL;
		pr_err("Failed to register USB power supply, rc=%d\n", rc);
		return rc;
	}

	pst = &bcdev->psy_list[PSY_TYPE_USB];
	rc = read_property_id(bcdev, pst, USB_NUM_PORTS);
	if (rc < 0) {
		pr_debug("Failed to read prop USB_NUM_PORTS, rc=%d\n", rc);
		bcdev->num_usb_ports = 1;
	} else if (pst->prop[USB_NUM_PORTS] > NUM_USB_PORTS) {
		pr_err("Number of USB ports detected as supported:%d, greater than 2\n",
			pst->prop[USB_NUM_PORTS]);
		return -EINVAL;
	} else if (pst->prop[USB_NUM_PORTS] != bcdev->num_usb_ports) {
		pr_err("Number of USB ports detected as supported:%d, configured in apps:%d\n",
			pst->prop[USB_NUM_PORTS], bcdev->num_usb_ports);
		bcdev->num_usb_ports = 1;
	}

	if (bcdev->num_usb_ports == 2) {
		bcdev->usb_active[USB_2_PORT_ID] = true;
		bcdev->psy_list[PSY_TYPE_USB_2].psy =
			devm_power_supply_register(bcdev->dev,
				&usb_psy_desc[USB_2_PORT_ID], &psy_cfg);
		if (IS_ERR(bcdev->psy_list[PSY_TYPE_USB_2].psy)) {
			rc = PTR_ERR(bcdev->psy_list[PSY_TYPE_USB_2].psy);
			bcdev->psy_list[PSY_TYPE_USB_2].psy = NULL;
			pr_err("Failed to register USB_2 power supply, rc=%d\n", rc);
			return rc;
		}
	}


	if (bcdev->wls_not_supported) {
		pr_debug("Wireless charging is not supported\n");
	} else {
		bcdev->psy_list[PSY_TYPE_WLS].psy =
			devm_power_supply_register(bcdev->dev, &wls_psy_desc, &psy_cfg);

		if (IS_ERR(bcdev->psy_list[PSY_TYPE_WLS].psy)) {
			rc = PTR_ERR(bcdev->psy_list[PSY_TYPE_WLS].psy);
			bcdev->psy_list[PSY_TYPE_WLS].psy = NULL;
			pr_err("Failed to register wireless power supply, rc=%d\n", rc);
			return rc;
		}
	}

	bcdev->psy_list[PSY_TYPE_BATTERY].psy =
		devm_power_supply_register(bcdev->dev, &batt_psy_desc,
						&psy_cfg);
	if (IS_ERR(bcdev->psy_list[PSY_TYPE_BATTERY].psy)) {
		rc = PTR_ERR(bcdev->psy_list[PSY_TYPE_BATTERY].psy);
		bcdev->psy_list[PSY_TYPE_BATTERY].psy = NULL;
		pr_err("Failed to register battery power supply, rc=%d\n", rc);
		return rc;
	}

	return 0;
}

static void battery_chg_subsys_up_work(struct work_struct *work)
{
	struct battery_chg_dev *bcdev = container_of(work,
					struct battery_chg_dev, subsys_up_work);
	struct psy_state *pst;
	int rc;

	battery_chg_notify_enable(bcdev);

	/*
	 * Give some time after enabling notification so that USB adapter type
	 * information can be obtained properly which is essential for setting
	 * USB ICL.
	 */
	msleep(200);

	if (bcdev->last_fcc_ua) {
		rc = __battery_psy_set_charge_current(bcdev,
				bcdev->last_fcc_ua);
		if (rc < 0)
			pr_err("Failed to set FCC (%u uA), rc=%d\n",
				bcdev->last_fcc_ua, rc);
	}

	if (bcdev->usb_icl_ua[USB_1_PORT_ID]) {
		pst = &bcdev->psy_list[PSY_TYPE_USB];
		rc = usb_psy_set_icl(bcdev, pst, USB_INPUT_CURR_LIMIT,
				bcdev->usb_icl_ua[USB_1_PORT_ID]);
		if (rc < 0)
			pr_err("Failed to set ICL(%u uA), rc=%d\n",
				bcdev->usb_icl_ua[USB_1_PORT_ID], rc);
	}

	if (bcdev->num_usb_ports == 2 && bcdev->usb_icl_ua[USB_2_PORT_ID]) {
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
		rc = usb_psy_set_icl(bcdev, pst, USB_INPUT_CURR_LIMIT,
				bcdev->usb_icl_ua[USB_2_PORT_ID]);
		if (rc < 0)
			pr_err("Failed to set USB-2 ICL(%u uA), rc=%d\n",
				bcdev->usb_icl_ua[USB_2_PORT_ID], rc);
	}
}

static int wireless_fw_send_firmware(struct battery_chg_dev *bcdev,
					const struct firmware *fw)
{
	struct wireless_fw_push_buf_req msg = {};
	const u8 *ptr;
	u32 i, num_chunks, partial_chunk_size;
	int rc;

	num_chunks = fw->size / WLS_FW_BUF_SIZE;
	partial_chunk_size = fw->size % WLS_FW_BUF_SIZE;

	if (!num_chunks)
		return -EINVAL;

	pr_debug("Updating FW...\n");

	ptr = fw->data;
	msg.hdr.owner = MSG_OWNER_BC;
	msg.hdr.type = MSG_TYPE_REQ_RESP;
	msg.hdr.opcode = BC_WLS_FW_PUSH_BUF_REQ;

	for (i = 0; i < num_chunks; i++, ptr += WLS_FW_BUF_SIZE) {
		msg.fw_chunk_id = i + 1;
		memcpy(msg.buf, ptr, WLS_FW_BUF_SIZE);

		pr_debug("sending FW chunk %u\n", i + 1);
		rc = battery_chg_fw_write(bcdev, &msg, sizeof(msg));
		if (rc < 0)
			return rc;
	}

	if (partial_chunk_size) {
		msg.fw_chunk_id = i + 1;
		memset(msg.buf, 0, WLS_FW_BUF_SIZE);
		memcpy(msg.buf, ptr, partial_chunk_size);

		pr_debug("sending partial FW chunk %u\n", i + 1);
		rc = battery_chg_fw_write(bcdev, &msg, sizeof(msg));
		if (rc < 0)
			return rc;
	}

	return 0;
}

static int wireless_fw_check_for_update(struct battery_chg_dev *bcdev,
					u32 version, size_t size)
{
	struct wireless_fw_check_req req_msg = {};

	bcdev->wls_fw_update_reqd = false;

	req_msg.hdr.owner = MSG_OWNER_BC;
	req_msg.hdr.type = MSG_TYPE_REQ_RESP;
	req_msg.hdr.opcode = BC_WLS_FW_CHECK_UPDATE;
	req_msg.fw_version = version;
	req_msg.fw_size = size;
	req_msg.fw_crc = bcdev->wls_fw_crc;

	return battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
}

#define IDT9415_FW_MAJOR_VER_OFFSET		0x84
#define IDT9415_FW_MINOR_VER_OFFSET		0x86
#define IDT_FW_MAJOR_VER_OFFSET		0x94
#define IDT_FW_MINOR_VER_OFFSET		0x96
static int wireless_fw_update(struct battery_chg_dev *bcdev, bool force)
{
	const struct firmware *fw;
	struct psy_state *pst;
	u32 version;
	u16 maj_ver, min_ver;
	int rc;

	if (!bcdev->wls_fw_name) {
		pr_err("wireless FW name is not specified\n");
		return -EINVAL;
	}

	pm_stay_awake(bcdev->dev);

	/*
	 * Check for USB presence. If nothing is connected, check whether
	 * battery SOC is at least 50% before allowing FW update.
	 */
	pst = &bcdev->psy_list[PSY_TYPE_USB];
	rc = read_property_id(bcdev, pst, USB_ONLINE);
	if (rc < 0)
		goto out;

	if (bcdev->num_usb_ports == 2 && !pst->prop[USB_ONLINE]) {
		pst = &bcdev->psy_list[PSY_TYPE_USB_2];
		rc = read_property_id(bcdev, pst, USB_ONLINE);
		if (rc < 0)
			goto out;
	}

	if (!pst->prop[USB_ONLINE]) {
		pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
		rc = read_property_id(bcdev, pst, BATT_CAPACITY);
		if (rc < 0)
			goto out;

		if ((pst->prop[BATT_CAPACITY] / 100) < 50) {
			pr_err("Battery SOC should be at least 50%% or connect charger\n");
			rc = -EINVAL;
			goto out;
		}
	}

	rc = firmware_request_nowarn(&fw, bcdev->wls_fw_name, bcdev->dev);
	if (rc) {
		pr_err("Couldn't get firmware rc=%d\n", rc);
		goto out;
	}

	if (!fw || !fw->data || !fw->size) {
		pr_err("Invalid firmware\n");
		rc = -EINVAL;
		goto release_fw;
	}

	if (fw->size < SZ_16K) {
		pr_err("Invalid firmware size %zu\n", fw->size);
		rc = -EINVAL;
		goto release_fw;
	}

	if (strstr(bcdev->wls_fw_name, "9412")) {
		maj_ver = le16_to_cpu(*(__le16 *)(fw->data + IDT_FW_MAJOR_VER_OFFSET));
		min_ver = le16_to_cpu(*(__le16 *)(fw->data + IDT_FW_MINOR_VER_OFFSET));
	} else {
		maj_ver = le16_to_cpu(*(__le16 *)(fw->data + IDT9415_FW_MAJOR_VER_OFFSET));
		min_ver = le16_to_cpu(*(__le16 *)(fw->data + IDT9415_FW_MINOR_VER_OFFSET));
	}
	version = maj_ver << 16 | min_ver;

	if (force)
		version = UINT_MAX;

	pr_debug("FW size: %zu version: %#x\n", fw->size, version);

	rc = wireless_fw_check_for_update(bcdev, version, fw->size);
	if (rc < 0) {
		pr_err("Wireless FW update not needed, rc=%d\n", rc);
		goto release_fw;
	}

	if (!bcdev->wls_fw_update_reqd) {
		pr_warn("Wireless FW update not required\n");
		goto release_fw;
	}

	/* Wait for IDT to be setup by charger firmware */
	msleep(WLS_FW_PREPARE_TIME_MS);

	reinit_completion(&bcdev->fw_update_ack);
	rc = wireless_fw_send_firmware(bcdev, fw);
	if (rc < 0) {
		pr_err("Failed to send FW chunk, rc=%d\n", rc);
		goto release_fw;
	}

	pr_debug("Waiting for fw_update_ack\n");
	rc = wait_for_completion_timeout(&bcdev->fw_update_ack,
				msecs_to_jiffies(bcdev->wls_fw_update_time_ms));
	if (!rc) {
		pr_err("Error, timed out updating firmware\n");
		rc = -ETIMEDOUT;
		goto release_fw;
	} else {
		pr_debug("Waited for %d ms\n",
			bcdev->wls_fw_update_time_ms - jiffies_to_msecs(rc));
		rc = 0;
	}

	pr_info("Wireless FW update done\n");

release_fw:
	bcdev->wls_fw_crc = 0;
	release_firmware(fw);
out:
	pm_relax(bcdev->dev);

	return rc;
}

static ssize_t qti_charger_ro_show(struct class *c,
				struct class_attribute *attr, char *buf,
				int psy_type, int prop_id)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	struct psy_state *pst = &bcdev->psy_list[psy_type];
	int rc;

	rc = read_property_id(bcdev, pst, prop_id);
	if (rc < 0)
		return rc;

	return scnprintf(buf, PAGE_SIZE, "%d\n", (int)pst->prop[prop_id]);
}

#define QTI_CHARGER_RO_SHOW(name, psy_type, prop_id)			\
	static ssize_t name##_show(struct class *c,			\
					struct class_attribute *attr,	\
					char *buf)			\
	{								\
		return qti_charger_ro_show(c, attr, buf, psy_type, prop_id); \
	}								\
	static CLASS_ATTR_RO(name)					\

#define QTI_CHARGER_RW_SHOW(name, psy_type, prop_id)			\
	static ssize_t name##_show(struct class *c,			\
					struct class_attribute *attr,	\
					char *buf)			\
	{								\
		return qti_charger_ro_show(c, attr, buf, psy_type, prop_id); \
	}								\
	static CLASS_ATTR_RW(name)					\

#define QTI_CHARGER_RW_PROP_SHOW(name, field)				\
	static ssize_t name##_show(struct class *c,			\
					struct class_attribute *attr,	\
					char *buf)			\
	{								\
		struct battery_chg_dev *bcdev = container_of(c,		\
				struct battery_chg_dev, battery_class);	\
		return scnprintf(buf, PAGE_SIZE, "%d\n", bcdev->field);	\
	}								\
	static CLASS_ATTR_RW(name)					\

#define QTI_CHARGER_TYPE_RO_SHOW(name, psy, psy_type, prop_id)		\
	static ssize_t name##_show(struct class *c,			\
					struct class_attribute *attr,	\
					char *buf)			\
	{								\
		struct battery_chg_dev *bcdev = container_of(c,		\
				struct battery_chg_dev, battery_class);	\
		struct psy_state *pst = &bcdev->psy_list[psy_type];	\
		int rc;							\
									\
		rc = read_property_id(bcdev, pst, prop_id);		\
		if (rc < 0)						\
			return rc;					\
									\
		return scnprintf(buf, PAGE_SIZE, "%s\n",		\
				get_##psy##_type_name(pst->prop[prop_id]));	\
	}								\
	static CLASS_ATTR_RO(name)					\

static ssize_t wireless_fw_update_time_ms_store(struct class *c,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);

	if (kstrtou32(buf, 0, &bcdev->wls_fw_update_time_ms))
		return -EINVAL;

	return count;
}

QTI_CHARGER_RW_PROP_SHOW(wireless_fw_update_time_ms, wls_fw_update_time_ms);

static ssize_t wireless_fw_crc_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	u16 val;

	if (kstrtou16(buf, 0, &val) || !val)
		return -EINVAL;

	bcdev->wls_fw_crc = val;

	return count;
}
static CLASS_ATTR_WO(wireless_fw_crc);

static ssize_t wireless_fw_version_show(struct class *c,
					struct class_attribute *attr,
					char *buf)
{
#ifdef NT_EDIT
	return scnprintf(buf, PAGE_SIZE, "%d\n", -1);
#else
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	struct wireless_fw_get_version_req req_msg = {};
	int rc;

	req_msg.hdr.owner = MSG_OWNER_BC;
	req_msg.hdr.type = MSG_TYPE_REQ_RESP;
	req_msg.hdr.opcode = BC_WLS_FW_GET_VERSION;

	rc = battery_chg_write(bcdev, &req_msg, sizeof(req_msg));
	if (rc < 0) {
		pr_err("Failed to get FW version rc=%d\n", rc);
		return rc;
	}

	return scnprintf(buf, PAGE_SIZE, "%#x\n", bcdev->wls_fw_version);
#endif
}
static CLASS_ATTR_RO(wireless_fw_version);

static ssize_t wireless_fw_force_update_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	bool val;
	int rc;

	if (kstrtobool(buf, &val) || !val)
		return -EINVAL;

	rc = wireless_fw_update(bcdev, true);
	if (rc < 0)
		return rc;

	return count;
}
static CLASS_ATTR_WO(wireless_fw_force_update);

static ssize_t wireless_fw_update_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	bool val;
	int rc;

	if (kstrtobool(buf, &val) || !val)
		return -EINVAL;

	rc = wireless_fw_update(bcdev, false);
	if (rc < 0)
		return rc;

	return count;
}
static CLASS_ATTR_WO(wireless_fw_update);

QTI_CHARGER_TYPE_RO_SHOW(wireless_type, wls, PSY_TYPE_WLS, WLS_ADAP_TYPE);

static ssize_t charge_control_en_store(struct class *c,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;
	bool val;

	if (kstrtobool(buf, &val))
		return -EINVAL;

	if (val == bcdev->chg_ctrl_en)
		return count;

	rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY],
				BATT_CHG_CTRL_EN, val);
	if (rc < 0) {
		pr_err("Failed to set charge_control_en, rc=%d\n", rc);
		return rc;
	}

	bcdev->chg_ctrl_en = val;

	return count;
}

static ssize_t charge_control_en_show(struct class *c,
				struct class_attribute *attr, char *buf)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;

	rc = get_charge_control_en(bcdev);
	if (rc < 0)
		return rc;

	return scnprintf(buf, PAGE_SIZE, "%d\n", bcdev->chg_ctrl_en);
}
static CLASS_ATTR_RW(charge_control_en);

QTI_CHARGER_RO_SHOW(usb_typec_compliant, PSY_TYPE_USB, USB_TYPEC_COMPLIANT);

QTI_CHARGER_RO_SHOW(usb_num_ports, PSY_TYPE_USB, USB_NUM_PORTS);

QTI_CHARGER_TYPE_RO_SHOW(usb_real_type, usb, PSY_TYPE_USB, USB_REAL_TYPE);

QTI_CHARGER_RO_SHOW(usb_2_typec_compliant, PSY_TYPE_USB_2, USB_TYPEC_COMPLIANT);

QTI_CHARGER_TYPE_RO_SHOW(usb_2_real_type, usb, PSY_TYPE_USB_2, USB_REAL_TYPE);

static ssize_t restrict_cur_store(struct class *c, struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;
	u32 fcc_ua, prev_fcc_ua;

	if (kstrtou32(buf, 0, &fcc_ua) || fcc_ua > bcdev->thermal_fcc_ua)
		return -EINVAL;

	prev_fcc_ua = bcdev->restrict_fcc_ua;
	bcdev->restrict_fcc_ua = fcc_ua;
	if (bcdev->restrict_chg_en) {
		rc = __battery_psy_set_charge_current(bcdev, fcc_ua);
		if (rc < 0) {
			bcdev->restrict_fcc_ua = prev_fcc_ua;
			return rc;
		}
	}

	return count;
}
QTI_CHARGER_RW_PROP_SHOW(restrict_cur, restrict_fcc_ua);

static ssize_t restrict_chg_store(struct class *c, struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;
	bool val;

	if (kstrtobool(buf, &val))
		return -EINVAL;

	bcdev->restrict_chg_en = val;
	rc = __battery_psy_set_charge_current(bcdev, bcdev->restrict_chg_en ?
			bcdev->restrict_fcc_ua : bcdev->thermal_fcc_ua);
	if (rc < 0)
		return rc;

	return count;
}
QTI_CHARGER_RW_PROP_SHOW(restrict_chg, restrict_chg_en);

static ssize_t fake_soc_store(struct class *c, struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int val;

	if (kstrtoint(buf, 0, &val))
		return -EINVAL;

	bcdev->fake_soc = val;
	pr_debug("Set fake soc to %d\n", val);

	if (IS_ENABLED(CONFIG_QTI_PMIC_GLINK_CLIENT_DEBUG) && pst->psy)
		power_supply_changed(pst->psy);

	return count;
}
QTI_CHARGER_RW_PROP_SHOW(fake_soc, fake_soc);

static ssize_t wireless_boost_en_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;
	bool val;

	if (kstrtobool(buf, &val))
		return -EINVAL;

	rc = write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_WLS],
				WLS_BOOST_EN, val);
	if (rc < 0)
		return rc;

	return count;
}

QTI_CHARGER_RW_SHOW(wireless_boost_en, PSY_TYPE_WLS, WLS_BOOST_EN);

static ssize_t _moisture_detection_en_store(struct class *c,
					struct class_attribute *attr, enum psy_type type,
					const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);
	int rc;
	bool val;

	if (kstrtobool(buf, &val))
		return -EINVAL;

	rc = write_property_id(bcdev, &bcdev->psy_list[type],
				USB_MOISTURE_DET_EN, val);
	if (rc < 0)
		return rc;

	return count;
}

static ssize_t moisture_detection_en_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	return _moisture_detection_en_store(c, attr, PSY_TYPE_USB, buf, count);
}

QTI_CHARGER_RW_SHOW(moisture_detection_en, PSY_TYPE_USB, USB_MOISTURE_DET_EN);

static ssize_t moisture_detection_usb_2_en_store(struct class *c,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	return _moisture_detection_en_store(c, attr, PSY_TYPE_USB_2, buf, count);
}

QTI_CHARGER_RW_SHOW(moisture_detection_usb_2_en, PSY_TYPE_USB_2, USB_MOISTURE_DET_EN);

QTI_CHARGER_RO_SHOW(moisture_detection_status, PSY_TYPE_USB, USB_MOISTURE_DET_STS);

QTI_CHARGER_RO_SHOW(moisture_detection_usb_2_status, PSY_TYPE_USB_2, USB_MOISTURE_DET_STS);

QTI_CHARGER_RO_SHOW(resistance, PSY_TYPE_BATTERY, BATT_RESISTANCE);

QTI_CHARGER_RO_SHOW(soh, PSY_TYPE_BATTERY, BATT_SOH);

static ssize_t ship_mode_en_store(struct class *c, struct class_attribute *attr,
				const char *buf, size_t count)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);

	if (kstrtobool(buf, &bcdev->ship_mode_en))
		return -EINVAL;

	return count;
}

static ssize_t ship_mode_en_show(struct class *c, struct class_attribute *attr,
				char *buf)
{
	struct battery_chg_dev *bcdev = container_of(c, struct battery_chg_dev,
						battery_class);

	return scnprintf(buf, PAGE_SIZE, "%d\n", bcdev->ship_mode_en);
}
static CLASS_ATTR_RW(ship_mode_en);

static struct attribute *battery_class_attrs[] = {
	&class_attr_soh.attr,
	&class_attr_resistance.attr,
	&class_attr_moisture_detection_status.attr,
	&class_attr_moisture_detection_en.attr,
	&class_attr_wireless_boost_en.attr,
	&class_attr_fake_soc.attr,
	&class_attr_wireless_fw_update.attr,
	&class_attr_wireless_fw_force_update.attr,
	&class_attr_wireless_fw_version.attr,
	&class_attr_wireless_fw_crc.attr,
	&class_attr_wireless_fw_update_time_ms.attr,
	&class_attr_wireless_type.attr,
	&class_attr_ship_mode_en.attr,
	&class_attr_restrict_chg.attr,
	&class_attr_restrict_cur.attr,
	&class_attr_usb_num_ports.attr,
	&class_attr_usb_real_type.attr,
	&class_attr_usb_typec_compliant.attr,
	&class_attr_charge_control_en.attr,
	NULL,
};
ATTRIBUTE_GROUPS(battery_class);

static struct attribute *battery_class_usb_2_attrs[] = {
	&class_attr_soh.attr,
	&class_attr_resistance.attr,
	&class_attr_moisture_detection_status.attr,
	&class_attr_moisture_detection_usb_2_status.attr,
	&class_attr_moisture_detection_en.attr,
	&class_attr_moisture_detection_usb_2_en.attr,
	&class_attr_wireless_boost_en.attr,
	&class_attr_fake_soc.attr,
	&class_attr_wireless_fw_update.attr,
	&class_attr_wireless_fw_force_update.attr,
	&class_attr_wireless_fw_version.attr,
	&class_attr_wireless_fw_crc.attr,
	&class_attr_wireless_fw_update_time_ms.attr,
	&class_attr_wireless_type.attr,
	&class_attr_ship_mode_en.attr,
	&class_attr_restrict_chg.attr,
	&class_attr_restrict_cur.attr,
	&class_attr_usb_num_ports.attr,
	&class_attr_usb_real_type.attr,
	&class_attr_usb_2_real_type.attr,
	&class_attr_usb_typec_compliant.attr,
	&class_attr_usb_2_typec_compliant.attr,
	&class_attr_charge_control_en.attr,
	NULL,
};
ATTRIBUTE_GROUPS(battery_class_usb_2);

static struct attribute *battery_class_no_wls_attrs[] = {
	&class_attr_soh.attr,
	&class_attr_resistance.attr,
	&class_attr_moisture_detection_status.attr,
	&class_attr_moisture_detection_en.attr,
	&class_attr_fake_soc.attr,
	&class_attr_ship_mode_en.attr,
	&class_attr_restrict_chg.attr,
	&class_attr_restrict_cur.attr,
	&class_attr_usb_real_type.attr,
	&class_attr_usb_typec_compliant.attr,
	&class_attr_usb_num_ports.attr,
	&class_attr_charge_control_en.attr,
	NULL,
};
ATTRIBUTE_GROUPS(battery_class_no_wls);

static struct attribute *battery_class_usb_2_no_wls_attrs[] = {
	&class_attr_soh.attr,
	&class_attr_resistance.attr,
	&class_attr_moisture_detection_status.attr,
	&class_attr_moisture_detection_usb_2_status.attr,
	&class_attr_moisture_detection_en.attr,
	&class_attr_moisture_detection_usb_2_en.attr,
	&class_attr_fake_soc.attr,
	&class_attr_ship_mode_en.attr,
	&class_attr_restrict_chg.attr,
	&class_attr_restrict_cur.attr,
	&class_attr_usb_real_type.attr,
	&class_attr_usb_2_real_type.attr,
	&class_attr_usb_typec_compliant.attr,
	&class_attr_usb_num_ports.attr,
	&class_attr_usb_2_typec_compliant.attr,
	&class_attr_charge_control_en.attr,
	NULL,
};
ATTRIBUTE_GROUPS(battery_class_usb_2_no_wls);

#ifdef CONFIG_DEBUG_FS
static void battery_chg_add_debugfs(struct battery_chg_dev *bcdev)
{
	int rc;
	struct dentry *dir;

	dir = debugfs_create_dir("battery_charger", NULL);
	if (IS_ERR(dir)) {
		rc = PTR_ERR(dir);
		pr_err("Failed to create charger debugfs directory, rc=%d\n",
			rc);
		return;
	}

	bcdev->debugfs_dir = dir;
	debugfs_create_bool("block_tx", 0600, dir, &bcdev->block_tx);
}
#else
static void battery_chg_add_debugfs(struct battery_chg_dev *bcdev) { }
#endif

static int battery_chg_parse_dt(struct battery_chg_dev *bcdev)
{
	struct device_node *node = bcdev->dev->of_node;
	struct psy_state *pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	int i, rc, len;
	u32 prev, val;

	bcdev->wls_not_supported = of_property_read_bool(node,
			"qcom,wireless-charging-not-supported");

	of_property_read_string(node, "qcom,wireless-fw-name",
				&bcdev->wls_fw_name);

	of_property_read_u32(node, "qcom,shutdown-voltage",
				&bcdev->shutdown_volt_mv);

#ifdef NT_EDIT
	rc = of_property_read_u32(node, "chg_data_id",
				&bcdev->chg_data_id);

	if (rc < 0) {
		pr_err("Failed to read prop chg_data_id, use default value\n");
		bcdev->chg_data_id = CHG_DATA_ID_DEF_VAL;
	}
#endif
	if (of_property_read_bool(bcdev->dev->of_node, "qcom,multiport-usb"))
		bcdev->num_usb_ports = 2;
	else
		bcdev->num_usb_ports = 1;

	rc = read_property_id(bcdev, pst, BATT_CHG_CTRL_LIM_MAX);
	if (rc < 0) {
		pr_err("Failed to read prop BATT_CHG_CTRL_LIM_MAX, rc=%d\n",
			rc);
		return rc;
	}

	rc = of_property_count_elems_of_size(node, "qcom,thermal-mitigation",
						sizeof(u32));
	if (rc <= 0) {

		rc = of_property_read_u32(node, "qcom,thermal-mitigation-step",
						&val);

		if (rc < 0)
			return 0;

		if (val < 500000 || val >= pst->prop[BATT_CHG_CTRL_LIM_MAX]) {
			pr_err("thermal_fcc_step %d is invalid\n", val);
			return -EINVAL;
		}

		bcdev->thermal_fcc_step = val;
		len = pst->prop[BATT_CHG_CTRL_LIM_MAX] / bcdev->thermal_fcc_step;

		/*
		 * FCC values must be above 500mA.
		 * Since len is truncated when calculated, check and adjust len so
		 * that the above requirement is met.
		 */
		if (pst->prop[BATT_CHG_CTRL_LIM_MAX] - (bcdev->thermal_fcc_step * len) < 500000)
			len = len - 1;
	} else {
		bcdev->thermal_fcc_step = 0;
		len = rc;
		prev = pst->prop[BATT_CHG_CTRL_LIM_MAX];

		for (i = 0; i < len; i++) {
			rc = of_property_read_u32_index(node, "qcom,thermal-mitigation",
				i, &val);
			if (rc < 0)
				return rc;

			if (val > prev) {
				pr_err("Thermal levels should be in descending order\n");
				bcdev->num_thermal_levels = -EINVAL;
				return 0;
			}

			prev = val;
		}

		bcdev->thermal_levels = devm_kcalloc(bcdev->dev, len + 1,
						sizeof(*bcdev->thermal_levels),
						GFP_KERNEL);
		if (!bcdev->thermal_levels)
			return -ENOMEM;

		/*
		 * Element 0 is for normal charging current. Elements from index 1
		 * onwards is for thermal mitigation charging currents.
		 */

		bcdev->thermal_levels[0] = pst->prop[BATT_CHG_CTRL_LIM_MAX];

		rc = of_property_read_u32_array(node, "qcom,thermal-mitigation",
					&bcdev->thermal_levels[1], len);
		if (rc < 0) {
			pr_err("Error in reading qcom,thermal-mitigation, rc=%d\n", rc);
			return rc;
		}
	}

	bcdev->num_thermal_levels = len;
	bcdev->thermal_fcc_ua = pst->prop[BATT_CHG_CTRL_LIM_MAX];

	return 0;
}

static int battery_chg_ship_mode(struct notifier_block *nb, unsigned long code,
		void *unused)
{
	struct battery_charger_notify_msg msg_notify = { { 0 } };
	struct battery_charger_ship_mode_req_msg msg = { { 0 } };
	struct battery_chg_dev *bcdev = container_of(nb, struct battery_chg_dev,
						     reboot_notifier);
	int rc;

	msg_notify.hdr.owner = MSG_OWNER_BC;
	msg_notify.hdr.type = MSG_TYPE_NOTIFY;
	msg_notify.hdr.opcode = BC_SHUTDOWN_NOTIFY;

	rc = battery_chg_write(bcdev, &msg_notify, sizeof(msg_notify));
	if (rc < 0)
		pr_err("Failed to send shutdown notification rc=%d\n", rc);

	if (!bcdev->ship_mode_en)
		return NOTIFY_DONE;

	msg.hdr.owner = MSG_OWNER_BC;
	msg.hdr.type = MSG_TYPE_REQ_RESP;
	msg.hdr.opcode = BC_SHIP_MODE_REQ_SET;
	msg.ship_mode_type = SHIP_MODE_PMIC;

	if (code == SYS_POWER_OFF) {
		rc = battery_chg_write(bcdev, &msg, sizeof(msg));
		if (rc < 0)
			pr_emerg("Failed to write ship mode: %d\n", rc);
	}

	return NOTIFY_DONE;
}

static void panel_event_notifier_callback(enum panel_event_notifier_tag tag,
			struct panel_event_notification *notification, void *data)
{
	struct battery_chg_dev *bcdev = data;

	if (!notification) {
		pr_debug("Invalid panel notification\n");
		return;
	}

	pr_debug("panel event received, type: %d\n", notification->notif_type);
	switch (notification->notif_type) {
	case DRM_PANEL_EVENT_BLANK:
		battery_chg_notify_disable(bcdev);
		break;
	case DRM_PANEL_EVENT_UNBLANK:
		battery_chg_notify_enable(bcdev);
		break;
	default:
		pr_debug("Ignore panel event: %d\n", notification->notif_type);
		break;
	}
}

static int battery_chg_register_panel_notifier(struct battery_chg_dev *bcdev)
{
	struct device_node *np = bcdev->dev->of_node;
	struct device_node *pnode;
	struct drm_panel *panel, *active_panel = NULL;
	void *cookie = NULL;
	int i, count, rc;

	count = of_count_phandle_with_args(np, "qcom,display-panels", NULL);
	if (count <= 0)
		return 0;

	for (i = 0; i < count; i++) {
		pnode = of_parse_phandle(np, "qcom,display-panels", i);
		if (!pnode)
			return -ENODEV;

		panel = of_drm_find_panel(pnode);
		of_node_put(pnode);
		if (!IS_ERR(panel)) {
			active_panel = panel;
			break;
		}
	}

	if (!active_panel) {
		rc = PTR_ERR(panel);
		if (rc != -EPROBE_DEFER)
			dev_err(bcdev->dev, "Failed to find active panel, rc=%d\n", rc);
		return rc;
	}

	cookie = panel_event_notifier_register(
			PANEL_EVENT_NOTIFICATION_PRIMARY,
			PANEL_EVENT_NOTIFIER_CLIENT_BATTERY_CHARGER,
			active_panel,
			panel_event_notifier_callback,
			(void *)bcdev);
	if (IS_ERR(cookie)) {
		rc = PTR_ERR(cookie);
		dev_err(bcdev->dev, "Failed to register panel event notifier, rc=%d\n", rc);
		return rc;
	}

	pr_debug("register panel notifier successful\n");
	bcdev->notifier_cookie = cookie;
	return 0;
}

static int
battery_chg_get_max_charge_cntl_limit(struct thermal_cooling_device *tcd,
					unsigned long *state)
{
	struct battery_chg_dev *bcdev = tcd->devdata;

	*state = bcdev->num_thermal_levels;

	return 0;
}

static int
battery_chg_get_cur_charge_cntl_limit(struct thermal_cooling_device *tcd,
					unsigned long *state)
{
	struct battery_chg_dev *bcdev = tcd->devdata;

	*state = bcdev->curr_thermal_level;

	return 0;
}

static int
battery_chg_set_cur_charge_cntl_limit(struct thermal_cooling_device *tcd,
					unsigned long state)
{
	struct battery_chg_dev *bcdev = tcd->devdata;

	return battery_psy_set_charge_current(bcdev, (int)state);
}

static const struct thermal_cooling_device_ops battery_tcd_ops = {
	.get_max_state = battery_chg_get_max_charge_cntl_limit,
	.get_cur_state = battery_chg_get_cur_charge_cntl_limit,
	.set_cur_state = battery_chg_set_cur_charge_cntl_limit,
};

static int battery_chg_probe(struct platform_device *pdev)
{
	struct battery_chg_dev *bcdev;
	struct device *dev = &pdev->dev;
	struct pmic_glink_client_data client_data = { };
	struct thermal_cooling_device *tcd;
	struct psy_state *pst;
	int rc, i;
#ifdef NT_EDIT
	int adsp_init_try = 0;
	struct proc_dir_entry *nt_chg_proc_dir = NULL;
#endif

	bcdev = devm_kzalloc(&pdev->dev, sizeof(*bcdev), GFP_KERNEL);
	if (!bcdev)
		return -ENOMEM;
	bcdev->psy_list[PSY_TYPE_BATTERY].map = battery_prop_map;
	bcdev->psy_list[PSY_TYPE_BATTERY].prop_count = BATT_PROP_MAX;
	bcdev->psy_list[PSY_TYPE_BATTERY].opcode_get = BC_BATTERY_STATUS_GET;
	bcdev->psy_list[PSY_TYPE_BATTERY].opcode_set = BC_BATTERY_STATUS_SET;
	bcdev->psy_list[PSY_TYPE_USB].map = usb_prop_map;
	bcdev->psy_list[PSY_TYPE_USB].prop_count = USB_PROP_MAX;
	bcdev->psy_list[PSY_TYPE_USB].opcode_get = BC_USB_STATUS_GET(USB_1_PORT_ID);
	bcdev->psy_list[PSY_TYPE_USB].opcode_set = BC_USB_STATUS_SET(USB_1_PORT_ID);
	bcdev->psy_list[PSY_TYPE_WLS].map = wls_prop_map;
	bcdev->psy_list[PSY_TYPE_WLS].prop_count = WLS_PROP_MAX;
	bcdev->psy_list[PSY_TYPE_WLS].opcode_get = BC_WLS_STATUS_GET;
	bcdev->psy_list[PSY_TYPE_WLS].opcode_set = BC_WLS_STATUS_SET;
	bcdev->usb_active[USB_1_PORT_ID] = true;

	bcdev->psy_list[PSY_TYPE_USB_2].map = usb_prop_map;
	bcdev->psy_list[PSY_TYPE_USB_2].prop_count = USB_PROP_MAX;
	bcdev->psy_list[PSY_TYPE_USB_2].opcode_get = BC_USB_STATUS_GET(USB_2_PORT_ID);
	bcdev->psy_list[PSY_TYPE_USB_2].opcode_set = BC_USB_STATUS_SET(USB_2_PORT_ID);

	for (i = 0; i < PSY_TYPE_MAX; i++) {
		bcdev->psy_list[i].prop =
			devm_kcalloc(&pdev->dev, bcdev->psy_list[i].prop_count,
					sizeof(u32), GFP_KERNEL);
		if (!bcdev->psy_list[i].prop)
			return -ENOMEM;
	}

	bcdev->psy_list[PSY_TYPE_BATTERY].model =
		devm_kzalloc(&pdev->dev, MAX_STR_LEN, GFP_KERNEL);
	if (!bcdev->psy_list[PSY_TYPE_BATTERY].model)
		return -ENOMEM;

	mutex_init(&bcdev->rw_lock);
	init_rwsem(&bcdev->state_sem);
	init_completion(&bcdev->ack);
	init_completion(&bcdev->fw_buf_ack);
	init_completion(&bcdev->fw_update_ack);
	INIT_WORK(&bcdev->subsys_up_work, battery_chg_subsys_up_work);
	INIT_WORK(&bcdev->usb_type_work, battery_chg_update_usb_type_work);
	INIT_WORK(&bcdev->battery_check_work, battery_chg_check_status_work);
#ifdef NT_EDIT
	INIT_WORK(&bcdev->nt_update_event, nt_update_event_work);
#endif
	bcdev->dev = dev;

	rc = battery_chg_register_panel_notifier(bcdev);
	if (rc < 0)
		return rc;

	client_data.id = MSG_OWNER_BC;
	client_data.name = "battery_charger";
	client_data.msg_cb = battery_chg_callback;
	client_data.priv = bcdev;
	client_data.state_cb = battery_chg_state_cb;

	bcdev->client = pmic_glink_register_client(dev, &client_data);
	if (IS_ERR(bcdev->client)) {
		rc = PTR_ERR(bcdev->client);
		if (rc != -EPROBE_DEFER)
			dev_err(dev, "Error in registering with pmic_glink %d\n",
				rc);
		goto reg_error;
	}

	down_write(&bcdev->state_sem);
	atomic_set(&bcdev->state, PMIC_GLINK_STATE_UP);
	/*
	 * This should be initialized here so that battery_chg_callback
	 * can run successfully when battery_chg_parse_dt() starts
	 * reading BATT_CHG_CTRL_LIM_MAX parameter and waits for a response.
	 */
	bcdev->initialized = true;
	up_write(&bcdev->state_sem);

	bcdev->reboot_notifier.notifier_call = battery_chg_ship_mode;
	bcdev->reboot_notifier.priority = 255;
	register_reboot_notifier(&bcdev->reboot_notifier);
#ifdef NT_EDIT
	//wait adsp charge init ok,we can read adsp charge prop
	pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	for(adsp_init_try=1;adsp_init_try <= ADSP_INIT_TRIES;adsp_init_try++)
	{
		read_property_id(bcdev, pst, BATT_CHG_CTRL_LIM_MAX);
		if(pst->prop[BATT_CHG_CTRL_LIM_MAX] == BATTERY_MAX_CURRENT)
		{
			break;
		}
		msleep(50);
	}
	if(adsp_init_try == ADSP_INIT_TRIES)
		pr_err("charge_ap_cannot_get_adspinit_data!!!\n");
#endif
	rc = battery_chg_parse_dt(bcdev);
	if (rc < 0) {
		dev_err(dev, "Failed to parse dt rc=%d\n", rc);
		goto error;
	}

	bcdev->restrict_fcc_ua = DEFAULT_RESTRICT_FCC_UA;
	platform_set_drvdata(pdev, bcdev);
	bcdev->fake_soc = -EINVAL;
	rc = battery_chg_init_psy(bcdev);
	if (rc < 0)
		goto error;

	bcdev->battery_class.name = "qcom-battery";

	if (bcdev->num_usb_ports == 2 && bcdev->wls_not_supported)
		bcdev->battery_class.class_groups = battery_class_usb_2_no_wls_groups;
	else if (bcdev->num_usb_ports == 2)
		bcdev->battery_class.class_groups = battery_class_usb_2_groups;
	else if (bcdev->wls_not_supported)
		bcdev->battery_class.class_groups = battery_class_no_wls_groups;
	else
		bcdev->battery_class.class_groups = battery_class_groups;

	rc = class_register(&bcdev->battery_class);
	if (rc < 0) {
		dev_err(dev, "Failed to create battery_class rc=%d\n", rc);
		goto error;
	}

	pst = &bcdev->psy_list[PSY_TYPE_BATTERY];
	tcd = devm_thermal_of_cooling_device_register(dev, dev->of_node,
			(char *)pst->psy->desc->name, bcdev, &battery_tcd_ops);
	if (IS_ERR_OR_NULL(tcd)) {
		rc = PTR_ERR_OR_ZERO(tcd);
		dev_err(dev, "Failed to register thermal cooling device rc=%d\n",
			rc);
		class_unregister(&bcdev->battery_class);
		goto error;
	}
#ifdef NT_EDIT
	nt_chg_proc_dir =  proc_mkdir("charger", NULL);
	if(!nt_chg_proc_dir){
		pr_err("proc dir creates failed !!\n");
		return -ENOMEM;
	}
	for (i = 0; i < ARRAY_SIZE(entries); i++) {
		if (!proc_create_data(entries[i].name, 0666, nt_chg_proc_dir, &(entries[i].fops), bcdev)){
			pr_info("%s: create /proc/charger/%s failed\\n",__func__, entries[i].name);
		}
	}
#endif
	bcdev->wls_fw_update_time_ms = WLS_FW_UPDATE_TIME_MS;
	battery_chg_add_debugfs(bcdev);
	bcdev->notify_en = false;
	battery_chg_notify_enable(bcdev);
	device_init_wakeup(bcdev->dev, true);
#ifdef NT_EDIT
	bcdev->chg_wake = wakeup_source_register(bcdev->dev, "chg_wakelock");
	INIT_DELAYED_WORK(&bcdev->nt_update_status_work,
						nt_update_status_function_work);
	queue_delayed_work(system_wq, &bcdev->nt_update_status_work,
								round_jiffies(10 * HZ));
#endif
	schedule_work(&bcdev->usb_type_work);

	rc = get_charge_control_en(bcdev);
	if (rc < 0)
		pr_debug("Failed to read charge_control_en, rc = %d\n", rc);

	return 0;
error:
	down_write(&bcdev->state_sem);
	atomic_set(&bcdev->state, PMIC_GLINK_STATE_DOWN);
	bcdev->initialized = false;
	up_write(&bcdev->state_sem);

	pmic_glink_unregister_client(bcdev->client);
	cancel_work_sync(&bcdev->usb_type_work);
	cancel_work_sync(&bcdev->subsys_up_work);
	cancel_work_sync(&bcdev->battery_check_work);
	complete(&bcdev->ack);
	unregister_reboot_notifier(&bcdev->reboot_notifier);
reg_error:
	if (bcdev->notifier_cookie)
		panel_event_notifier_unregister(bcdev->notifier_cookie);
	return rc;
}

static int battery_chg_remove(struct platform_device *pdev)
{
	struct battery_chg_dev *bcdev = platform_get_drvdata(pdev);

	down_write(&bcdev->state_sem);
	atomic_set(&bcdev->state, PMIC_GLINK_STATE_DOWN);
	bcdev->initialized = false;
	up_write(&bcdev->state_sem);

	if (bcdev->notifier_cookie)
		panel_event_notifier_unregister(bcdev->notifier_cookie);

	device_init_wakeup(bcdev->dev, false);
	debugfs_remove_recursive(bcdev->debugfs_dir);
	class_unregister(&bcdev->battery_class);
	pmic_glink_unregister_client(bcdev->client);
	cancel_work_sync(&bcdev->subsys_up_work);
	cancel_work_sync(&bcdev->usb_type_work);
	cancel_work_sync(&bcdev->battery_check_work);
	unregister_reboot_notifier(&bcdev->reboot_notifier);

	return 0;
}

#ifdef NT_EDIT
void battery_shut_down(struct platform_device *pdev) {
	struct battery_chg_dev *bcdev = platform_get_drvdata(pdev);

	write_property_id(bcdev, &bcdev->psy_list[PSY_TYPE_BATTERY], BATT_SHUT_DOWN, 1);
	pr_err("battery_shut_down !!!!!\n");
}

static int qti_charger_suspend(struct device *dev)
{
	return 0;
}

static int qti_charger_resume(struct device *dev)
{
	struct battery_chg_dev *bcdev = dev_get_drvdata(dev);
	if (!bcdev) {
		pr_err("bcdev is null \n");
		goto out;
	}
	cancel_delayed_work_sync(&bcdev->nt_update_status_work);
	queue_delayed_work(system_wq, &bcdev->nt_update_status_work,0);
out:
	return 0;
}

static const struct dev_pm_ops qti_charger_pm_ops = {
	.suspend	= qti_charger_suspend,
	.resume		= qti_charger_resume,
};
#endif

static const struct of_device_id battery_chg_match_table[] = {
	{ .compatible = "qcom,battery-charger" },
	{},
};

static struct platform_driver battery_chg_driver = {
	.driver = {
		.name = "qti_battery_charger",
		.of_match_table = battery_chg_match_table,
#ifdef NT_EDIT
		.pm		= &qti_charger_pm_ops,
#endif
	},
	.probe = battery_chg_probe,
	.remove = battery_chg_remove,
#ifdef NT_EDIT
	.shutdown = battery_shut_down,
#endif
};
module_platform_driver(battery_chg_driver);

MODULE_DESCRIPTION("QTI Glink battery charger driver");
MODULE_LICENSE("GPL v2");
