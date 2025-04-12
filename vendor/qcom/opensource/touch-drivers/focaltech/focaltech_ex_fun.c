/*
 *
 * FocalTech TouchScreen driver.
 *
 * Copyright (c) 2012-2020, Focaltech Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*****************************************************************************
*
* File Name: Focaltech_ex_fun.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-08
*
* Abstract:
*
* Reference:
*
*****************************************************************************/

/*****************************************************************************
* 1.Included header files
*****************************************************************************/
#include "focaltech_core.h"

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
#define PROC_UPGRADE                            0
#define PROC_READ_REGISTER                      1
#define PROC_WRITE_REGISTER                     2
#define PROC_AUTOCLB                            4
#define PROC_UPGRADE_INFO                       5
#define PROC_WRITE_DATA                         6
#define PROC_READ_DATA                          7
#define PROC_SET_TEST_FLAG                      8
#define PROC_SET_SLAVE_ADDR                     10
#define PROC_HW_RESET                           11
#define PROC_READ_STATUS                        12
#define PROC_SET_BOOT_MODE                      13
#define PROC_ENTER_TEST_ENVIRONMENT             14
#define PROC_WRITE_DATA_DIRECT                  16
#define PROC_READ_DATA_DIRECT                   17
#define PROC_CONFIGURE                          18
#define PROC_CONFIGURE_INTR                     20
#define PROC_GET_DRIVER_INFO                    21
#define PROC_NAME                               "ftxxxx-debug"
#define PROC_BUF_SIZE                           256

/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
enum {
    RWREG_OP_READ = 0,
    RWREG_OP_WRITE = 1,
};

struct proc_dir_entry *proc_touchpanel;
extern struct fts_fwdbg *fts_fwdbg_data;
extern int fts_fwdbg_disable(struct fts_fwdbg *dbg);
extern int fts_fwdbg_enable(struct fts_fwdbg *dbg, int value);

/*****************************************************************************
* Static variables
*****************************************************************************/
static struct rwreg_operation_t {
    int type;           /*  0: read, 1: write */
    int reg;            /*  register */
    int len;            /*  read/write length */
    int val;            /*  length = 1; read: return value, write: op return */
    int res;            /*  0: success, otherwise: fail */
    char *opbuf;        /*  length >= 1, read return value, write: op return */
} rw_op;

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/

/*****************************************************************************
* Static function prototypes
*****************************************************************************/
static ssize_t fts_debug_write(
    struct file *filp, const char __user *buff, size_t count, loff_t *ppos)
{
    u8 *writebuf = NULL;
    u8 tmpbuf[PROC_BUF_SIZE] = { 0 };
    int buflen = count;
    int writelen = 0;
    int ret = 0;
    char tmp[PROC_BUF_SIZE];
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
    struct fts_ts_data *ts_data = pde_data(file_inode(filp));
#else
    struct fts_ts_data *ts_data = PDE_DATA(file_inode(filp));
#endif
    struct ftxxxx_proc *proc = &ts_data->proc;

    if (buflen < 1) {
        FTS_ERROR("apk proc count(%d) fail", buflen);
        return -EINVAL;
    }

    if (buflen > PROC_BUF_SIZE) {
        writebuf = (u8 *)kzalloc(buflen * sizeof(u8), GFP_KERNEL);
        if (NULL == writebuf) {
            FTS_ERROR("apk proc wirte buf zalloc fail");
            return -ENOMEM;
        }
    } else {
        writebuf = tmpbuf;
    }

    if (copy_from_user(writebuf, buff, buflen)) {
        FTS_ERROR("[APK]: copy from user error!!");
        ret = -EFAULT;
        goto proc_write_err;
    }

    proc->opmode = writebuf[0];
    if (buflen == 1) {
        ret = buflen;
        goto proc_write_err;
    }

    switch (proc->opmode) {
    case PROC_SET_TEST_FLAG:
        FTS_DEBUG("[APK]: PROC_SET_TEST_FLAG = %x", writebuf[1]);
        if (writebuf[1] == 0) {
            if (!ts_data->suspended) fts_esdcheck_switch(ts_data, ENABLE);
        } else {
            fts_esdcheck_switch(ts_data, DISABLE);
        }
        break;

    case PROC_READ_REGISTER:
        proc->cmd[0] = writebuf[1];
        break;

    case PROC_WRITE_REGISTER:
        ret = fts_write_reg(writebuf[1], writebuf[2]);
        if (ret < 0) {
            FTS_ERROR("PROC_WRITE_REGISTER write error");
            goto proc_write_err;
        }
        break;

    case PROC_READ_DATA:
        writelen = buflen - 1;
        if (writelen >= FTS_MAX_COMMMAND_LENGTH) {
            FTS_ERROR("cmd(PROC_READ_DATA) length(%d) fail", writelen);
            goto proc_write_err;
        }
        memcpy(proc->cmd, writebuf + 1, writelen);
        proc->cmd_len = writelen;
        if (ts_data->bus_type == BUS_TYPE_I2C) {
            ret = fts_write(writebuf + 1, writelen);
            if (ret < 0) {
                FTS_ERROR("PROC_READ_DATA write error");
                goto proc_write_err;
            }
        }
        break;

    case PROC_WRITE_DATA:
        writelen = buflen - 1;
        ret = fts_write(writebuf + 1, writelen);
        if (ret < 0) {
            FTS_ERROR("PROC_WRITE_DATA write error");
            goto proc_write_err;
        }
        break;

    case PROC_SET_SLAVE_ADDR:
        if (ts_data->bus_type == BUS_TYPE_I2C) {
            fts_bus_configure(ts_data, &writebuf[1], buflen - 1);
        }
        break;

    case PROC_HW_RESET:
        if (buflen < PROC_BUF_SIZE) {
            memcpy(tmp, writebuf + 1, buflen - 1);
            tmp[buflen - 1] = '\0';
            if (strncmp(tmp, "focal_driver", 12) == 0) {
                FTS_INFO("APK execute HW Reset");
                fts_reset_proc(ts_data, false, 0);
            }
        }
        break;

    case PROC_SET_BOOT_MODE:
        FTS_DEBUG("[APK]: PROC_SET_BOOT_MODE = %x", writebuf[1]);
        if (0 == writebuf[1]) {
            ts_data->fw_is_running = true;
        } else {
            ts_data->fw_is_running = false;
        }
        break;

    case PROC_ENTER_TEST_ENVIRONMENT:
        FTS_DEBUG("[APK]: PROC_ENTER_TEST_ENVIRONMENT = %x", writebuf[1]);
        if (0 == writebuf[1]) {
            fts_enter_test_environment(0);
        } else {
            fts_enter_test_environment(1);
        }
        break;

    case PROC_READ_DATA_DIRECT:
        writelen = buflen - 1;
        if (writelen >= FTS_MAX_COMMMAND_LENGTH) {
            FTS_ERROR("cmd(PROC_READ_DATA_DIRECT) length(%d) fail", writelen);
            goto proc_write_err;
        }
        memcpy(proc->cmd, writebuf + 1, writelen);
        proc->cmd_len = writelen;
        break;

    case PROC_WRITE_DATA_DIRECT:
        writelen = buflen - 1;
        ret = fts_bus_transfer_direct(writebuf + 1, writelen, NULL, 0);
        if (ret < 0) {
            FTS_ERROR("PROC_WRITE_DATA_DIRECT write error");
            goto proc_write_err;
        }
        break;

    case PROC_CONFIGURE:
        if (ts_data->bus_type == BUS_TYPE_SPI) {
            fts_bus_configure(ts_data, &writebuf[1], buflen - 1);
        }
        break;

    case PROC_CONFIGURE_INTR:
        if (writebuf[1] == 0)
            fts_irq_disable();
        else
            fts_irq_enable();
        break;

    default:
        break;
    }

    ret = buflen;
proc_write_err:
    if ((buflen > PROC_BUF_SIZE) && writebuf) {
        kfree(writebuf);
        writebuf = NULL;
    }
    return ret;
}

static ssize_t fts_debug_read(
    struct file *filp, char __user *buff, size_t count, loff_t *ppos)
{
    int ret = 0;
    int num_read_chars = 0;
    int buflen = count;
    u8 *readbuf = NULL;
    u8 tmpbuf[PROC_BUF_SIZE] = { 0 };
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
    struct fts_ts_data *ts_data = pde_data(file_inode(filp));
#else
    struct fts_ts_data *ts_data = PDE_DATA(file_inode(filp));
#endif
    struct ftxxxx_proc *proc = &ts_data->proc;

    if (buflen <= 0) {
        FTS_ERROR("apk proc read count(%d) fail", buflen);
        return -EINVAL;
    }

    if (buflen > PROC_BUF_SIZE) {
        readbuf = (u8 *)kzalloc(buflen * sizeof(u8), GFP_KERNEL);
        if (NULL == readbuf) {
            FTS_ERROR("apk proc buf zalloc fail");
            return -ENOMEM;
        }
    } else {
        readbuf = tmpbuf;
    }

    switch (proc->opmode) {
    case PROC_READ_REGISTER:
        num_read_chars = 1;
        ret = fts_read_reg(proc->cmd[0], &readbuf[0]);
        if (ret < 0) {
            FTS_ERROR("PROC_READ_REGISTER read error");
            goto proc_read_err;
        }
        break;

    case PROC_READ_DATA:
        num_read_chars = buflen;
        if (ts_data->bus_type == BUS_TYPE_SPI)
            ret = fts_read(proc->cmd, proc->cmd_len, readbuf, num_read_chars);
        else if (ts_data->bus_type == BUS_TYPE_I2C)
            ret = fts_read(NULL, 0, readbuf, num_read_chars);
        else FTS_ERROR("unknown bus type:%d", ts_data->bus_type);
        if (ret < 0) {
            FTS_ERROR("PROC_READ_DATA read error");
            goto proc_read_err;
        }
        break;

    case PROC_READ_DATA_DIRECT:
        num_read_chars = buflen;
        ret = fts_bus_transfer_direct(proc->cmd, proc->cmd_len, readbuf, num_read_chars);
        if (ret < 0) {
            FTS_ERROR("PROC_READ_DATA_DIRECT read error");
            goto proc_read_err;
        }
        break;

    case PROC_GET_DRIVER_INFO:
        if (buflen >= 64) {
            num_read_chars = buflen;
            readbuf[0] = ts_data->bus_type;
            snprintf(&readbuf[32], buflen - 32, "%s", FTS_DRIVER_VERSION);
        }
        break;

    default:
        break;
    }

    ret = num_read_chars;
proc_read_err:
    if ((num_read_chars > 0) && copy_to_user(buff, readbuf, num_read_chars)) {
        FTS_ERROR("copy to user error");
        ret = -EFAULT;
    }

    if ((buflen > PROC_BUF_SIZE) && readbuf) {
        kfree(readbuf);
        readbuf = NULL;
    }
    return ret;
}

/*/proc/fts_ta*/
static int fts_ta_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
    struct fts_ts_data *ts_data = pde_data(inode);
#else
    struct fts_ts_data *ts_data = PDE_DATA(inode);
#endif

    if (ts_data->touch_analysis_support) {
        FTS_INFO("fts_ta open");
        ts_data->ta_buf = kzalloc(FTS_MAX_TOUCH_BUF, GFP_KERNEL);
        if (!ts_data->ta_buf) {
            FTS_ERROR("kzalloc for ta_buf fails");
            return -ENOMEM;
        }
    }
    return 0;
}

static int fts_ta_release(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
    struct fts_ts_data *ts_data = pde_data(inode);
#else
    struct fts_ts_data *ts_data = PDE_DATA(inode);
#endif

    if (ts_data->touch_analysis_support) {
        FTS_INFO("fts_ta close");
        ts_data->ta_flag = 0;
        if (ts_data->ta_buf) {
            kfree(ts_data->ta_buf);
            ts_data->ta_buf = NULL;
        }
    }
    return 0;
}

static ssize_t fts_ta_read(
    struct file *filp, char __user *buff, size_t count, loff_t *ppos)
{
    int read_num = (int)count;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
    struct fts_ts_data *ts_data = pde_data(file_inode(filp));
#else
    struct fts_ts_data *ts_data = PDE_DATA(file_inode(filp));
#endif

    if (!ts_data->touch_analysis_support || !ts_data->ta_buf) {
        FTS_ERROR("touch_analysis is disabled, or ta_buf is NULL");
        return -EINVAL;
    }

    if (!(filp->f_flags & O_NONBLOCK)) {
        ts_data->ta_flag = 1;
        wait_event_interruptible(ts_data->ts_waitqueue, !ts_data->ta_flag);
    }

    read_num = (ts_data->ta_size < read_num) ? ts_data->ta_size : read_num;
    if ((read_num > 0) && (copy_to_user(buff, ts_data->ta_buf, read_num))) {
        FTS_ERROR("copy to user error");
        return -EFAULT;
    }

    return read_num;
}

static int TP_charger_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_CHARGER_MODE_EN, &val);

    seq_printf(s, "Charger Mode:%s\n", ts_data->charger_mode ? "On" : "Off");
    seq_printf(s, "Charger Reg(0x8B)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;
}

static ssize_t TP_charger_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
   char buf[20];
   u32 tmp;
   int ret = 0;
   int value = 0;
   struct fts_ts_data *ts_data = fts_data;

   memset(buf, 0x00, sizeof(buf));
   if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
   if (kstrtouint(buf, 0, &tmp))
        return -EINVAL;

    sscanf(buf, "%d", &value);

   if (value == 2) {
        if (!ts_data->charger_mode) {
            FTS_INFO("enter charger mode");
            ret = fts_write_reg(FTS_REG_CHARGER_MODE_EN, ENABLE);
            if (ret >= 0) {
                ts_data->charger_mode = ENABLE;
            }
        }
    } else if (value == 0) {
        if (ts_data->charger_mode) {
            FTS_INFO("exit charger mode");
            ret = fts_write_reg(FTS_REG_CHARGER_MODE_EN, DISABLE);
            if (ret >= 0) {
                ts_data->charger_mode = DISABLE;
            }
        }
    }

    return count;
}

static int TP_charger_open(struct inode* inode, struct file* file){
    return single_open(file, TP_charger_show, NULL);
}

static int fts_pocket_mode_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_POCKET_MODE, &val);

    seq_printf(s, "POCKET Mode:%s\n", ts_data->pocket_mode ? "On" : "Off");
    seq_printf(s, "POCKET Mode:Reg(0x9A)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;
}

static ssize_t fts_pocket_mode_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    u32 tmp;
    int ret = 0;
    u8 state = 0xFF;
    struct fts_ts_data *ts_data = fts_data;

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
    if (kstrtouint(buf, 0, &tmp))
        return -EINVAL;
    if ((!ts_data->gesture_support) && (ts_data->fod_mode == FTS_FOD_DISABLE)) {
        FTS_INFO("In sleep mode,not operation pocket mode!");
        return count;
    }
    mutex_lock(&ts_data->input_dev->mutex);

    FTS_ERROR("pocket mode info:pm_suspend = %d\n", ts_data->pm_suspend);
    if (ts_data->pm_suspend)
        __pm_stay_awake(ts_data->p_ws);

    if (FTS_SYSFS_ECHO_ON(buf)) {
        FTS_ERROR("Pre-pocket_mode = %d, Enter pocket mode", ts_data->pocket_mode);
        ret = fts_write_reg(FTS_REG_POCKET_MODE, ENABLE);
        ts_data->pocket_mode = ENABLE;
        fts_esdcheck_switch(ts_data, DISABLE);
        msleep(10); //Ensure that the firmware completes the pocket mode process.
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        FTS_ERROR("Pre-pocket_mode = %d, Exit pocket mode", ts_data->pocket_mode);
        ret = fts_write_reg(FTS_REG_POCKET_MODE, DISABLE);
        if (!ts_data->suspended) {
            fts_esdcheck_switch(ts_data, ENABLE);
        }
        msleep(10);
        fts_read_reg(FTS_REG_POWER_MODE, &state);
        if (state == 2) {
            ret = fts_write_reg(FTS_REG_POWER_MODE, 0);
            if (!ts_data->suspended) {
                fts_esdcheck_switch(ts_data, ENABLE);
            }
        }
        ts_data->pocket_mode = DISABLE;
    }
    __pm_relax(ts_data->p_ws);
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static int fts_pocket_mode_open(struct inode* inode, struct file* file){
    return single_open(file, fts_pocket_mode_show, NULL);
}

static int fts_edge_mode_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_EDGE_MODE_EN, &val);

    seq_printf(s, "EDGE Mode:Reg(0x8C)=%x\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;
}

static ssize_t fts_edge_mode_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    int ret = 0;
    int temp = 0;
    struct fts_ts_data *ts_data = fts_data;

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
    if (kstrtouint(buf, 0, &temp))
        return -EINVAL;

    mutex_lock(&ts_data->input_dev->mutex);
    ret = fts_write_reg(FTS_REG_EDGE_MODE_EN, temp);
    if (ret < 0) {
        FTS_ERROR("MODE_EDGE switch to %d fail\n", ret);
    }
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static int fts_edge_mode_open(struct inode* inode, struct file* file){
    return single_open(file, fts_edge_mode_show, NULL);
}

static int fts_game_mode_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_GAME_MODE_EN, &val);

    seq_printf(s, "Game Mode:%s\n", ts_data->game_mode ? "On" : "Off");
    seq_printf(s, "Reg(0x99)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;
}

static ssize_t fts_game_mode_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    int ret = 0;
    struct fts_ts_data *ts_data = fts_data;

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;

    mutex_lock(&ts_data->input_dev->mutex);
    if (FTS_SYSFS_ECHO_ON(buf)) {
        FTS_DEBUG("enable game_mode");
        ret = fts_write_reg(FTS_REG_GAME_MODE_EN, ENABLE);
        if (ret < 0) {
            FTS_ERROR("MODE_GAME switch to %d fail", ret);
        }
        ts_data->game_mode = ENABLE;
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        FTS_DEBUG("disable game_mode");
        ret = fts_write_reg(FTS_REG_GAME_MODE_EN, DISABLE);
        if (ret < 0) {
            FTS_ERROR("MODE_GAME switch to %d fail", ret);
        }
        ts_data->game_mode = DISABLE;
    }
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static int fts_game_mode_open(struct inode* inode, struct file* file){
    return single_open(file, fts_game_mode_show, NULL);
}

static int fts_gesture_point_open(struct inode* inode, struct file* file){
    return single_open(file, fts_gesture_point_show, NULL);
}

static int fts_gesture_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_GESTURE_EN, &val);

    seq_printf(s, "Gesture Mode:%s\n", ts_data->gesture_support ? "On" : "Off");
    seq_printf(s, "Reg(0xD0)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;

}

static ssize_t fts_gesture_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    u32 tmp;
    struct fts_ts_data *ts_data = fts_data;

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
    if (kstrtouint(buf, 0, &tmp))
        return -EINVAL;

    if (ts_data->suspended) {
        FTS_INFO("In suspend,not operation gesture mode!");
        return count;
    }
    mutex_lock(&ts_data->input_dev->mutex);
    if (FTS_SYSFS_ECHO_ON(buf)) {
        FTS_DEBUG("enable gesture");
        ts_data->gesture_support = ENABLE;
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        FTS_DEBUG("disable gesture");
        ts_data->gesture_support = DISABLE;
    }
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;


}

static int fts_gesture_open(struct inode* inode, struct file* file){
    return single_open(file, fts_gesture_show, NULL);
}

static int fts_fod_proc_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_FOD_MODE_EN, &val);

    seq_printf(s, "Fod Mode:%d\n", ts_data->fod_mode);
    seq_printf(s, "Reg(0xCF)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;

}

static ssize_t fts_fod_proc_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    u32 tmp;
    struct fts_ts_data *ts_data = fts_data;

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
    if (kstrtouint(buf, 0, &tmp))
        return -EINVAL;

    mutex_lock(&ts_data->input_dev->mutex);
    if (tmp == 1) {
        FTS_INFO("enable fod\n");
        fts_fod_enable(FTS_FOD_ENABLE);
    } else if (tmp == 2) { //disable fod when input KEY_GESTURE_FOD_UP event
        FTS_INFO("unlock fod\n");
        fts_fod_enable(FTS_FOD_UNLOCK);
    } else if (tmp == 0) {
        FTS_INFO("disable fod\n");
        fts_fod_enable(FTS_FOD_DISABLE);
    } else if (tmp == 3) { //disable fod but not power off when suspend
        fts_fod_enable(3);
    } else {
        FTS_INFO("wrong value\n");
    }
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static int fts_fod_proc_open(struct inode* inode, struct file* file){
    return single_open(file, fts_fod_proc_show, NULL);
}

static int fts_diff_mode_show(struct seq_file *s, void *unused)
{
    u8 val = 0;
    struct fts_ts_data *ts_data = fts_data;
    fts_read_reg(0x9E, &val);
    mutex_lock(&ts_data->input_dev->mutex);

    seq_printf(s, "diff Mode:%d value:%d\n", ts_data->fwdbg_support, ts_data->fwdbg_value);
    seq_printf(s, "Reg(0x9E)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;

}

static ssize_t fts_diff_mode_store(
    struct file *filp, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[20];
    u32 tmp;
    struct fts_ts_data *ts_data = fts_data;
    struct fts_fwdbg *dbg = fts_fwdbg_data;
    if (!dbg || !ts_data) {
        FTS_ERROR("dbg/ts_data is null");
        return count;
    }

    memset(buf, 0x00, sizeof(buf));
    if (copy_from_user(&buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
        return -EFAULT;
    if (kstrtouint(buf, 0, &tmp))
        return -EINVAL;

    mutex_lock(&ts_data->input_dev->mutex);
    if (!!tmp ^ ts_data->fwdbg_support) {
        if (tmp) {
            if (0 == fts_fwdbg_enable(dbg, tmp)) {
                ts_data->fwdbg_value = (u8)tmp;
                ts_data->fwdbg_support = ENABLE;
                FTS_INFO("diff fwdbg enable\n!");
            }
        } else {
            ts_data->fwdbg_support = DISABLE;
            fts_msleep(30);
            fts_fwdbg_disable(dbg);
            FTS_INFO("diff fwdbg disable!");
        }
    } else FTS_INFO("value(%d)==fwdbg_support(%d)", !!tmp, ts_data->fwdbg_support);
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static int fts_diff_mode_open(struct inode* inode, struct file* file){
    return single_open(file, fts_diff_mode_show, NULL);
}

static int fts_fw_info_show(struct seq_file *s, void *unused)
{
    int ret = 0;
    int cnt = 0;
    u8 fwver = 0;
    struct fts_ts_data *ts_data = fts_data;
    u8 cmd = 0x90;
    u8 data[2] = {0};

    FTS_INFO("probe Test %lldms taken",ktime_to_ms(ktime_sub(ktime_get(), ts_data->probe_time)));
    FTS_INFO("resume Test %lldms taken",ktime_to_ms(ktime_sub(ktime_get(), ts_data->resume_time)));
    mutex_lock(&ts_data->input_dev->mutex);
    ret = fts_read_reg(FTS_REG_FW_VER, &fwver);
    if (ts_data->lcd_status) {
        if ((ret < 0) || (fwver == 0xFF) || (fwver == 0x00)) {
            seq_printf(s, "get tp fw version fail!fwver=%02x,ret=%d\n", fwver, ret);
            ret = fts_read_reg(0xA3, &fwver);
            fts_read(&cmd, 1, data, 2);
            FTS_INFO("A3 = %x, 90 = %x\n", fwver, ((data[0] << 8) + data[1]));
            } else {
                seq_printf(s,  "%02x\n", fwver);
                cnt = 0;
            }
    } else {
        FTS_INFO("TP suspend\n");
        seq_printf(s,  "%02x\n", ts_data->fwver);
    }

    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;

}

static int fts_driver_info_show(struct seq_file *s, void *unused)
{
    struct fts_ts_data *ts_data = fts_data;
    struct fts_ts_platform_data *pdata = ts_data->pdata;

    mutex_lock(&ts_data->input_dev->mutex);
    seq_printf(s, "Driver Ver:%s\n",
                      FTS_DRIVER_VERSION);

    seq_printf(s, "Resolution:(%d,%d)~(%d,%d)\n",
                      pdata->x_min, pdata->y_min, pdata->x_max, pdata->y_max);

    seq_printf(s, "Max Touchs:%d\n",
                      pdata->max_touch_number);

    seq_printf(s,
                      "reset gpio:%d,int gpio:%d,irq:%d\n",
                      pdata->reset_gpio, pdata->irq_gpio, ts_data->irq);

    seq_printf(s,"IC ID:0x%02x%02x\n",
                      ts_data->ic_info.ids.chip_idh,
                      ts_data->ic_info.ids.chip_idl);
    mutex_unlock(&ts_data->input_dev->mutex);

    return 0;

}

static int fts_fw_info_open(struct inode* inode, struct file* file){
    return single_open(file, fts_fw_info_show, NULL);
}

static int fts_driver_info_open(struct inode* inode, struct file* file){
    return single_open(file, fts_driver_info_show, NULL);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops fts_proc_fops = {
    .proc_read   = fts_debug_read,
    .proc_write  = fts_debug_write,
};

static const struct proc_ops fts_procta_fops = {
    .proc_open = fts_ta_open,
    .proc_release = fts_ta_release,
    .proc_read = fts_ta_read,
};
#else
static const struct file_operations fts_proc_fops = {
    .owner  = THIS_MODULE,
    .read   = fts_debug_read,
    .write  = fts_debug_write,
};

static const struct file_operations fts_procta_fops = {
    .open = fts_ta_open,
    .release = fts_ta_release,
    .read = fts_ta_read,
};
#endif

static const struct proc_ops TP_charger_fops = {
    .proc_open   = TP_charger_open,
    .proc_write  = TP_charger_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_pocket_mode_fops = {
    .proc_open   = fts_pocket_mode_open,
    .proc_write  = fts_pocket_mode_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_edge_mode_fops = {
    .proc_open   = fts_edge_mode_open,
    .proc_write  = fts_edge_mode_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_game_mode_fops = {
    .proc_open   = fts_game_mode_open,
    .proc_write  = fts_game_mode_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_gesture_point_fops = {
    .proc_open   = fts_gesture_point_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_gesture_fops = {
    .proc_open   = fts_gesture_open,
    .proc_write  = fts_gesture_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_fod_fops = {
    .proc_open   = fts_fod_proc_open,
    .proc_write  = fts_fod_proc_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_diff_mode_fops = {
    .proc_open   = fts_diff_mode_open,
    .proc_write  = fts_diff_mode_store,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_fw_info_fops = {
    .proc_open   = fts_fw_info_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static const struct proc_ops fts_driver_info_fops = {
    .proc_open   = fts_driver_info_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

int fts_create_apk_debug_channel(struct fts_ts_data *ts_data)
{
    struct ftxxxx_proc *proc = &ts_data->proc;
    proc->proc_entry = proc_create_data(PROC_NAME, 0777, NULL, &fts_proc_fops, ts_data);
    if (NULL == proc->proc_entry) {
        FTS_ERROR("create proc entry fail");
        return -ENOMEM;
    }

    ts_data->proc_ta.proc_entry = proc_create_data("fts_ta", 0777, NULL, \
                                  &fts_procta_fops, ts_data);
    if (!ts_data->proc_ta.proc_entry) {
        FTS_ERROR("create proc_ta entry fail");
        return -ENOMEM;
    }

    FTS_INFO("Create proc entry success!");
    return 0;
}

void fts_release_apk_debug_channel(struct fts_ts_data *ts_data)
{
    FTS_FUNC_ENTER();
    if (ts_data->proc.proc_entry)
        proc_remove(ts_data->proc.proc_entry);
    if (ts_data->proc_ta.proc_entry)
        proc_remove(ts_data->proc_ta.proc_entry);
    FTS_FUNC_EXIT();
}

/************************************************************************
 * sysfs interface
 ***********************************************************************/
/* fts_hw_reset interface */
static ssize_t fts_hw_reset_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;
    ssize_t count = 0;

    mutex_lock(&input_dev->mutex);
    fts_reset_proc(ts_data, false, 0);
    count = snprintf(buf, PAGE_SIZE, "hw reset executed\n");
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_hw_reset_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

/* fts_irq interface */
static ssize_t fts_irq_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct irq_desc *desc = irq_to_desc(ts_data->irq);

    count = snprintf(buf, PAGE_SIZE, "irq_depth:%d\n", desc->depth);

    return count;
}

static ssize_t fts_irq_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    if (FTS_SYSFS_ECHO_ON(buf)) {
        FTS_INFO("enable irq");
        fts_irq_enable();
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        FTS_INFO("disable irq");
        fts_irq_disable();
    }
    mutex_unlock(&input_dev->mutex);
    return count;
}

/* fts_boot_mode interface */
static ssize_t fts_bootmode_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    if (FTS_SYSFS_ECHO_ON(buf)) {
        FTS_INFO("[EX-FUN]set to boot mode");
        ts_data->fw_is_running = false;
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        FTS_INFO("[EX-FUN]set to fw mode");
        ts_data->fw_is_running = true;
    }
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

static ssize_t fts_bootmode_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    if (true == ts_data->fw_is_running) {
        count = snprintf(buf, PAGE_SIZE, "tp is in fw mode\n");
    } else {
        count = snprintf(buf, PAGE_SIZE, "tp is in boot mode\n");
    }
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

/* fts_tpfwver interface */
static ssize_t fts_tpfwver_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int ret = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;
    ssize_t num_read_chars = 0;
    u8 fwver = 0;
    u8 cmd = 0x90;
    u8 data[2] = {0};

    mutex_lock(&input_dev->mutex);
    //FTS_INFO("probe Test %lldms taken",ktime_ms_delta(ktime_get(), ts_data->probe_time));
    //FTS_INFO("resume Test %lldms taken",ktime_ms_delta(ktime_get(), ts_data->resume_time));
    FTS_INFO("probe Test %lldms taken",ktime_to_ms(ktime_sub(ktime_get(), ts_data->probe_time)));
    FTS_INFO("resume Test %lldms taken",ktime_to_ms(ktime_sub(ktime_get(), ts_data->resume_time)));

    ret = fts_read_reg(FTS_REG_FW_VER, &fwver);
    if ((ret < 0) || (fwver == 0xFF) || (fwver == 0x00)) {
        num_read_chars = snprintf(buf, PAGE_SIZE, "get tp fw version fail!ret=%d,fwver=%02x\n", ret, fwver);
        ret = fts_read_reg(0xA3, &fwver);
        FTS_INFO("A3 = %x\n", fwver);
        fts_read(&cmd, 1, data, 2);
        FTS_INFO("90 = %x\n", ((data[0] << 8) + data[1]));
    } else
        num_read_chars = snprintf(buf, PAGE_SIZE, "%02x\n", fwver);


    mutex_unlock(&input_dev->mutex);
    return num_read_chars;
}

static ssize_t fts_tpfwver_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

/* fts_rw_reg */
static ssize_t fts_tprwreg_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    int i;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);

    if (rw_op.len < 0) {
        count = snprintf(buf, PAGE_SIZE, "Invalid cmd line\n");
    } else if (rw_op.len == 1) {
        if (RWREG_OP_READ == rw_op.type) {
            if (rw_op.res == 0) {
                count = snprintf(buf, PAGE_SIZE, "Read %02X: %02X\n", rw_op.reg, rw_op.val);
            } else {
                count = snprintf(buf, PAGE_SIZE, "Read %02X failed, ret: %d\n", rw_op.reg,  rw_op.res);
            }
        } else {
            if (rw_op.res == 0) {
                count = snprintf(buf, PAGE_SIZE, "Write %02X, %02X success\n", rw_op.reg,  rw_op.val);
            } else {
                count = snprintf(buf, PAGE_SIZE, "Write %02X failed, ret: %d\n", rw_op.reg,  rw_op.res);
            }
        }
    } else {
        if (RWREG_OP_READ == rw_op.type) {
            count = snprintf(buf, PAGE_SIZE, "Read Reg: [%02X]-[%02X]\n", rw_op.reg, rw_op.reg + rw_op.len);
            count += snprintf(buf + count, PAGE_SIZE, "Result: ");
            if (rw_op.res) {
                count += snprintf(buf + count, PAGE_SIZE, "failed, ret: %d\n", rw_op.res);
            } else {
                if (rw_op.opbuf) {
                    for (i = 0; i < rw_op.len; i++) {
                        count += snprintf(buf + count, PAGE_SIZE, "%02X ", rw_op.opbuf[i]);
                    }
                    count += snprintf(buf + count, PAGE_SIZE, "\n");
                }
            }
        } else {
            ;
            count = snprintf(buf, PAGE_SIZE, "Write Reg: [%02X]-[%02X]\n", rw_op.reg, rw_op.reg + rw_op.len - 1);
            count += snprintf(buf + count, PAGE_SIZE, "Write Data: ");
            if (rw_op.opbuf) {
                for (i = 1; i < rw_op.len; i++) {
                    count += snprintf(buf + count, PAGE_SIZE, "%02X ", rw_op.opbuf[i]);
                }
                count += snprintf(buf + count, PAGE_SIZE, "\n");
            }
            if (rw_op.res) {
                count += snprintf(buf + count, PAGE_SIZE, "Result: failed, ret: %d\n", rw_op.res);
            } else {
                count += snprintf(buf + count, PAGE_SIZE, "Result: success\n");
            }
        }
        /*if (rw_op.opbuf) {
            kfree(rw_op.opbuf);
            rw_op.opbuf = NULL;
        }*/
    }
    mutex_unlock(&input_dev->mutex);

    return count;
}

static int shex_to_int(const char *hex_buf, int size)
{
    int i;
    int base = 1;
    int value = 0;
    char single;

    for (i = size - 1; i >= 0; i--) {
        single = hex_buf[i];

        if ((single >= '0') && (single <= '9')) {
            value += (single - '0') * base;
        } else if ((single >= 'a') && (single <= 'z')) {
            value += (single - 'a' + 10) * base;
        } else if ((single >= 'A') && (single <= 'Z')) {
            value += (single - 'A' + 10) * base;
        } else {
            return -EINVAL;
        }

        base *= 16;
    }

    return value;
}


static u8 shex_to_u8(const char *hex_buf, int size)
{
    return (u8)shex_to_int(hex_buf, size);
}
/*
 * Format buf:
 * [0]: '0' write, '1' read(reserved)
 * [1-2]: addr, hex
 * [3-4]: length, hex
 * [5-6]...[n-(n+1)]: data, hex
 */
static int fts_parse_buf(const char *buf, size_t cmd_len)
{
    int length;
    int i;
    char *tmpbuf;

    rw_op.reg = shex_to_u8(buf + 1, 2);
    length = shex_to_int(buf + 3, 2);

    if (buf[0] == '1') {
        rw_op.len = length;
        rw_op.type = RWREG_OP_READ;
        FTS_DEBUG("read %02X, %d bytes", rw_op.reg, rw_op.len);
    } else {
        if (cmd_len < (length * 2 + 5)) {
            pr_err("data invalided!\n");
            return -EINVAL;
        }
        FTS_DEBUG("write %02X, %d bytes", rw_op.reg, length);

        /* first byte is the register addr */
        rw_op.type = RWREG_OP_WRITE;
        rw_op.len = length + 1;
    }

    if (rw_op.len > 0) {
        tmpbuf = (char *)kzalloc(rw_op.len, GFP_KERNEL);
        if (!tmpbuf) {
            FTS_ERROR("allocate memory failed!\n");
            return -ENOMEM;
        }

        if (RWREG_OP_WRITE == rw_op.type) {
            tmpbuf[0] = rw_op.reg & 0xFF;
            FTS_DEBUG("write buffer: ");
            for (i = 1; i < rw_op.len; i++) {
                tmpbuf[i] = shex_to_u8(buf + 5 + i * 2 - 2, 2);
                FTS_DEBUG("buf[%d]: %02X", i, tmpbuf[i] & 0xFF);
            }
        }
        rw_op.opbuf = tmpbuf;
    }

    return rw_op.len;
}

static ssize_t fts_tprwreg_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;
    ssize_t cmd_length = 0;

    mutex_lock(&input_dev->mutex);
    cmd_length = count - 1; //remove "\n"

    if (rw_op.opbuf) {
        kfree(rw_op.opbuf);
        rw_op.opbuf = NULL;
    }

    FTS_DEBUG("cmd len: %d, buf: %s", (int)cmd_length, buf);
    /* compatible old ops */
    if (2 == cmd_length) {
        rw_op.type = RWREG_OP_READ;
        rw_op.len = 1;
        rw_op.reg = shex_to_int(buf, 2);
    } else if (4 == cmd_length) {
        rw_op.type = RWREG_OP_WRITE;
        rw_op.len = 1;
        rw_op.reg = shex_to_int(buf, 2);
        rw_op.val = shex_to_int(buf + 2, 2);
    } else if (cmd_length < 5) {
        FTS_ERROR("Invalid cmd buffer");
        mutex_unlock(&input_dev->mutex);
        return -EINVAL;
    } else {
        rw_op.len = fts_parse_buf(buf, cmd_length);
    }

    if (rw_op.len < 0) {
        FTS_ERROR("cmd buffer error!");

    } else {
        if (RWREG_OP_READ == rw_op.type) {
            if (rw_op.len == 1) {
                u8 reg, val;
                reg = rw_op.reg & 0xFF;
                rw_op.res = fts_read_reg(reg, &val);
                rw_op.val = val;
            } else {
                char reg;
                reg = rw_op.reg & 0xFF;

                rw_op.res = fts_read(&reg, 1, rw_op.opbuf, rw_op.len);
            }

            if (rw_op.res < 0) {
                FTS_ERROR("Could not read 0x%02x", rw_op.reg);
            } else {
                FTS_INFO("read 0x%02x, %d bytes successful", rw_op.reg, rw_op.len);
                rw_op.res = 0;
            }

        } else {
            if (rw_op.len == 1) {
                u8 reg, val;
                reg = rw_op.reg & 0xFF;
                val = rw_op.val & 0xFF;
                rw_op.res = fts_write_reg(reg, val);
            } else {
                rw_op.res = fts_write(rw_op.opbuf, rw_op.len);
            }
            if (rw_op.res < 0) {
                FTS_ERROR("Could not write 0x%02x", rw_op.reg);

            } else {
                FTS_INFO("Write 0x%02x, %d bytes successful", rw_op.val, rw_op.len);
                rw_op.res = 0;
            }
        }
    }

    mutex_unlock(&input_dev->mutex);
    return count;
}

/* fts_upgrade_bin interface */
static ssize_t fts_fwupgradebin_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    return -EPERM;
}

static ssize_t fts_fwupgradebin_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    char fwname[FILE_NAME_LENGTH] = { 0 };
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    if ((count <= 1) || (count >= FILE_NAME_LENGTH - 32)) {
        FTS_ERROR("fw bin name's length(%d) fail", (int)count);
        return -EINVAL;
    }
    memset(fwname, 0, sizeof(fwname));
    snprintf(fwname, FILE_NAME_LENGTH, "%s", buf);
    fwname[count - 1] = '\0';

    FTS_INFO("upgrade with bin file through sysfs node");
    mutex_lock(&input_dev->mutex);
    fts_upgrade_bin(fwname, 0);
    mutex_unlock(&input_dev->mutex);

    return count;
}

/* fts_force_upgrade interface */
static ssize_t fts_fwforceupg_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    return -EPERM;
}

static ssize_t fts_fwforceupg_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    char fwname[FILE_NAME_LENGTH];
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    if ((count <= 1) || (count >= FILE_NAME_LENGTH - 32)) {
        FTS_ERROR("fw bin name's length(%d) fail", (int)count);
        return -EINVAL;
    }
    memset(fwname, 0, sizeof(fwname));
    snprintf(fwname, FILE_NAME_LENGTH, "%s", buf);
    fwname[count - 1] = '\0';

    FTS_INFO("force upgrade through sysfs node");
    mutex_lock(&input_dev->mutex);
    fts_upgrade_bin(fwname, 1);
    mutex_unlock(&input_dev->mutex);

    return count;
}

/* fts_driver_info interface */
static ssize_t fts_driverinfo_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct fts_ts_platform_data *pdata = ts_data->pdata;
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "Driver Ver:%s\n",
                      FTS_DRIVER_VERSION);

    count += snprintf(buf + count, PAGE_SIZE, "Resolution:(%d,%d)~(%d,%d)\n",
                      pdata->x_min, pdata->y_min, pdata->x_max, pdata->y_max);

    count += snprintf(buf + count, PAGE_SIZE, "Max Touchs:%d\n",
                      pdata->max_touch_number);

    count += snprintf(buf + count, PAGE_SIZE,
                      "reset gpio:%d,int gpio:%d,irq:%d\n",
                      pdata->reset_gpio, pdata->irq_gpio, ts_data->irq);

    count += snprintf(buf + count, PAGE_SIZE, "IC ID:0x%02x%02x\n",
                      ts_data->ic_info.ids.chip_idh,
                      ts_data->ic_info.ids.chip_idl);
    mutex_unlock(&input_dev->mutex);
    return count;
}

static ssize_t fts_driverinfo_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

/* fts_dump_reg interface */
static ssize_t fts_dumpreg_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    u8 val = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);

    fts_read_reg(FTS_REG_POWER_MODE, &val);
    count += snprintf(buf + count, PAGE_SIZE, "Power Mode:0x%02x\n", val);

    fts_read_reg(FTS_REG_FW_VER, &val);
    count += snprintf(buf + count, PAGE_SIZE, "FW Ver:0x%02x\n", val);

    fts_read_reg(FTS_REG_LIC_VER, &val);
    count += snprintf(buf + count, PAGE_SIZE, "LCD Initcode Ver:0x%02x\n", val);

    fts_read_reg(FTS_REG_IDE_PARA_VER_ID, &val);
    count += snprintf(buf + count, PAGE_SIZE, "Param Ver:0x%02x\n", val);

    fts_read_reg(FTS_REG_IDE_PARA_STATUS, &val);
    count += snprintf(buf + count, PAGE_SIZE, "Param status:0x%02x\n", val);

    fts_read_reg(FTS_REG_VENDOR_ID, &val);
    count += snprintf(buf + count, PAGE_SIZE, "Vendor ID:0x%02x\n", val);

    fts_read_reg(FTS_REG_GESTURE_EN, &val);
    count += snprintf(buf + count, PAGE_SIZE, "Gesture Mode:0x%02x\n", val);

    fts_read_reg(FTS_REG_CHARGER_MODE_EN, &val);
    count += snprintf(buf + count, PAGE_SIZE, "charge stat:0x%02x\n", val);

    fts_read_reg(FTS_REG_INT_CNT, &val);
    count += snprintf(buf + count, PAGE_SIZE, "INT count:0x%02x\n", val);

    fts_read_reg(FTS_REG_FLOW_WORK_CNT, &val);
    count += snprintf(buf + count, PAGE_SIZE, "ESD count:0x%02x\n", val);

    mutex_unlock(&input_dev->mutex);
    return count;
}

static ssize_t fts_dumpreg_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

/* fts_dump_reg interface */
static ssize_t fts_tpbuf_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    int i = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "touch point buffer:\n");
    for (i = 0; i < FTS_TOUCH_DATA_LEN; i++) {
        count += snprintf(buf + count, PAGE_SIZE, "%02x ", ts_data->touch_buf[i]);
    }
    count += snprintf(buf + count, PAGE_SIZE, "\n");
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_tpbuf_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

/* fts_log_level node */
static ssize_t fts_log_level_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "log level:%d\n",
                      ts_data->log_level);
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_log_level_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    sscanf(buf, "%d", &value);
    FTS_DEBUG("log level:%d->%d", ts_data->log_level, value);
    ts_data->log_level = value;
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

/* fts_pen node */
static ssize_t fts_pen_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "pen event:%s\n",
                      ts_data->pen_etype ? "hover" : "default");
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_pen_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    sscanf(buf, "%d", &value);
    FTS_DEBUG("pen event:%d->%d", ts_data->pen_etype, value);
    ts_data->pen_etype = value;
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

/* fts_touch_size node */
static ssize_t fts_touchsize_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "touch size:%d\n", ts_data->touch_size);
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_touchsize_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    sscanf(buf, "%d", &value);
    if ((value > 2) && (value < FTS_MAX_TOUCH_BUF)) {
        FTS_DEBUG("touch size:%d->%d", ts_data->touch_size, value);
        ts_data->touch_size = value;
    } else
        FTS_DEBUG("touch size:%d invalid", value);
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

/* fts_ta_mode node */
static ssize_t fts_tamode_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count += snprintf(buf + count, PAGE_SIZE, "touch analysis:%s\n", \
                      ts_data->touch_analysis_support ? "Enable" : "Disable");
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_tamode_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
    mutex_lock(&input_dev->mutex);
    sscanf(buf, "%d", &value);
    ts_data->touch_analysis_support = !!value;
    FTS_DEBUG("set touch analysis:%d", ts_data->touch_analysis_support);
    mutex_unlock(&input_dev->mutex);
    FTS_FUNC_EXIT();

    return count;
}

#if FTS_FOD_EN
/* fts_fod_mode node */
static ssize_t fts_fod_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
    int count = 0;
    u8 val = 0;
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);

    mutex_lock(&ts_data->input_dev->mutex);
    fts_read_reg(FTS_REG_FOD_MODE_EN, &val);
    count = snprintf(buf, PAGE_SIZE, "FOD Mode:%s\n", ts_data->fod_mode ? "On" : "Off");
    count += snprintf(buf + count, PAGE_SIZE, "Reg(0xCF)=%d\n", val);
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}

static ssize_t fts_fod_store(
    struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    struct fts_ts_data *ts_data = dev_get_drvdata(dev);

    mutex_lock(&ts_data->input_dev->mutex);
    if (FTS_SYSFS_ECHO_ON(buf)) {
        fts_fod_enable(FTS_FOD_ENABLE);
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        fts_fod_enable(FTS_FOD_DISABLE);
    } else if (FTS_SYSFS_ECHO_UNLOCK(buf)) {
        fts_fod_enable(FTS_FOD_UNLOCK);
    } else {
        FTS_ERROR("wrong fod echo num\n");
    }
    mutex_unlock(&ts_data->input_dev->mutex);

    return count;
}
#endif

/* get the fw version  example:cat fw_version */
static DEVICE_ATTR(fts_fw_version, S_IRUGO | S_IWUSR, fts_tpfwver_show, fts_tpfwver_store);

/* read and write register(s)
*   All data type is **HEX**
*   Single Byte:
*       read:   echo 88 > rw_reg ---read register 0x88
*       write:  echo 8807 > rw_reg ---write 0x07 into register 0x88
*   Multi-bytes:
*       [0:rw-flag][1-2: reg addr, hex][3-4: length, hex][5-6...n-n+1: write data, hex]
*       rw-flag: 0, write; 1, read
*       read:  echo 10005           > rw_reg ---read reg 0x00-0x05
*       write: echo 000050102030405 > rw_reg ---write reg 0x00-0x05 as 01,02,03,04,05
*  Get result:
*       cat rw_reg
*/
static DEVICE_ATTR(fts_rw_reg, S_IRUGO | S_IWUSR, fts_tprwreg_show, fts_tprwreg_store);
/*  upgrade from fw bin file   example:echo "*.bin" > fts_upgrade_bin */
static DEVICE_ATTR(fts_upgrade_bin, S_IRUGO | S_IWUSR, fts_fwupgradebin_show, fts_fwupgradebin_store);
static DEVICE_ATTR(fts_force_upgrade, S_IRUGO | S_IWUSR, fts_fwforceupg_show, fts_fwforceupg_store);
static DEVICE_ATTR(fts_driver_info, S_IRUGO | S_IWUSR, fts_driverinfo_show, fts_driverinfo_store);
static DEVICE_ATTR(fts_dump_reg, S_IRUGO | S_IWUSR, fts_dumpreg_show, fts_dumpreg_store);
static DEVICE_ATTR(fts_hw_reset, S_IRUGO | S_IWUSR, fts_hw_reset_show, fts_hw_reset_store);
static DEVICE_ATTR(fts_irq, S_IRUGO | S_IWUSR, fts_irq_show, fts_irq_store);
static DEVICE_ATTR(fts_boot_mode, S_IRUGO | S_IWUSR, fts_bootmode_show, fts_bootmode_store);
static DEVICE_ATTR(fts_touch_point, S_IRUGO | S_IWUSR, fts_tpbuf_show, fts_tpbuf_store);
static DEVICE_ATTR(fts_log_level, S_IRUGO | S_IWUSR, fts_log_level_show, fts_log_level_store);
static DEVICE_ATTR(fts_pen, S_IRUGO | S_IWUSR, fts_pen_show, fts_pen_store);
static DEVICE_ATTR(fts_touch_size, S_IRUGO | S_IWUSR, fts_touchsize_show, fts_touchsize_store);
static DEVICE_ATTR(fts_ta_mode, S_IRUGO | S_IWUSR, fts_tamode_show, fts_tamode_store);
#if FTS_FOD_EN
static DEVICE_ATTR(fts_fod_mode, S_IRUGO | S_IWUSR, fts_fod_show, fts_fod_store);
#endif

/* add your attr in here*/
static struct attribute *fts_attributes[] = {
    &dev_attr_fts_fw_version.attr,
    &dev_attr_fts_rw_reg.attr,
    &dev_attr_fts_dump_reg.attr,
    &dev_attr_fts_upgrade_bin.attr,
    &dev_attr_fts_force_upgrade.attr,
    &dev_attr_fts_driver_info.attr,
    &dev_attr_fts_hw_reset.attr,
    &dev_attr_fts_irq.attr,
    &dev_attr_fts_boot_mode.attr,
    &dev_attr_fts_touch_point.attr,
    &dev_attr_fts_log_level.attr,
    &dev_attr_fts_pen.attr,
    &dev_attr_fts_touch_size.attr,
    &dev_attr_fts_ta_mode.attr,
#if FTS_FOD_EN
    &dev_attr_fts_fod_mode.attr,
#endif
    NULL
};

static struct attribute_group fts_attribute_group = {
    .attrs = fts_attributes
};

int fts_create_sysfs(struct fts_ts_data *ts_data)
{
    int ret = 0;

    ret = sysfs_create_group(&ts_data->dev->kobj, &fts_attribute_group);
    if (ret) {
        FTS_ERROR("[EX]: sysfs_create_group() failed!!");
        sysfs_remove_group(&ts_data->dev->kobj, &fts_attribute_group);
        return -ENOMEM;
    } else {
        FTS_INFO("[EX]: sysfs_create_group() succeeded!!");
    }

    return ret;
}

int fts_remove_sysfs(struct fts_ts_data *ts_data)
{
    sysfs_remove_group(&ts_data->dev->kobj, &fts_attribute_group);
    return 0;
}

int fts_procfs_init(void)
{
    proc_touchpanel = proc_mkdir("touchpanel", NULL);
    if (!proc_touchpanel) {
        FTS_ERROR("procfs(proc/touchpanel) create fail");
        remove_proc_subtree("touchpanel", NULL);
        return -ENOMEM;
    } else {
        proc_create_data("fod_mode", 0664, proc_touchpanel, &fts_fod_fops, NULL);
        proc_create_data("gesture_mode", 0664, proc_touchpanel, &fts_gesture_fops, NULL);
        proc_create_data("gesture_code", 0664, proc_touchpanel, &fts_gesture_point_fops, NULL);
        proc_create_data("game_mode", 0664, proc_touchpanel, &fts_game_mode_fops, NULL);
        proc_create_data("edge_mode", 0664, proc_touchpanel, &fts_edge_mode_fops, NULL);
        proc_create_data("pocket_mode", 0664, proc_touchpanel, &fts_pocket_mode_fops, NULL);
        proc_create_data("TP_charger_flags", 0664, proc_touchpanel, &TP_charger_fops, NULL);
        proc_create_data("diff_mode", 0664, proc_touchpanel, &fts_diff_mode_fops, NULL);
        proc_create_data("fw_version", 0664, proc_touchpanel, &fts_fw_info_fops, NULL);
        proc_create_data("driver_info", 0664, proc_touchpanel, &fts_driver_info_fops, NULL);
        FTS_DEBUG("procfs(test) create successfully");
#ifdef FTS_TP_DATA_DUMP_EN
        fts_tp_data_dump_proc_init();
#endif
    }

    return 0;
}
int fts_procfs_exit(void)
{
    remove_proc_subtree("touchpanel", NULL);
#ifdef FTS_TP_DATA_DUMP_EN
    fts_tp_data_dump_proc_exit();
#endif
    return 0;
}

