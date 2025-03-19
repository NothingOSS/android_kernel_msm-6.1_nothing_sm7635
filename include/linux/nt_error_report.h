#ifndef _NT_ERROR_REPORT_H
#define _NT_ERROR_REPORT_H

#define NT_ER_INFO_PRINT(fmt, arg...) \
    pr_info("[nt_error_report] " fmt, ##arg)
#define NT_ER_ERR_PRINT(fmt, arg...) \
    pr_err("[nt_error_report] " fmt, ##arg)

enum {
	NT_DISP_ERR,
	NT_BATT_ERR,
	NT_UFS_ERR,
	NT_GLINK_ERR,
	NT_DSI_ERR,
};


extern void nt_er_in_schedule(int err_type);


#endif /* _NT_ERROR_REPORT_H */
