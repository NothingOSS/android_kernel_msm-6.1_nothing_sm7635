/*
 *
 * FocalTech fts TouchScreen driver.
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
* File Name: focaltech_upgrade_ft5662.c
*
* Author: Focaltech Driver Team
*
* Created: 2021-08-05
*
* Abstract:
*
* Reference:
*
*****************************************************************************/
/*****************************************************************************
* 1.Included header files
*****************************************************************************/
#include "../focaltech_flash.h"

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
u8 pb_file_ft5662[] = {
	0
};

/*****************************************************************************
* Static function prototypes
*****************************************************************************/

static int ft5662_fwupg_get_boot_state(enum FW_STATUS *fw_sts)
{
    int ret = 0;
    u8 cmd = 0;
    u8 val[2] = { 0 };

    FTS_INFO("**********read boot id**********");
    if (!fw_sts) {
        FTS_ERROR("fw_sts is null");
        return -EINVAL;
    }

    cmd = FTS_CMD_START1;
    ret = fts_write(&cmd, 1);
    if (ret < 0) {
        FTS_ERROR("write 55 cmd fail");
        return ret;
    }

    fts_msleep(FTS_CMD_START_DELAY);
    cmd = FTS_CMD_READ_ID;
    ret = fts_read(&cmd, 1, val, 2);
    if (ret < 0) {
        FTS_ERROR("write 90 cmd fail");
        return ret;
    }

    FTS_INFO("read boot id:0x%02x%02x", val[0], val[1]);
    if ((val[0] == 0x56) && (val[1] == 0x62)) {
        FTS_INFO("tp run in romboot");
        *fw_sts = FTS_RUN_IN_ROM;
    } else if ((val[0] == 0x56) && (val[1] == 0xE2)) {
        FTS_INFO("tp run in pramboot");
        *fw_sts = FTS_RUN_IN_PRAM;
    }

    return 0;
}

static bool ft5662_fwupg_check_state(enum FW_STATUS rstate)
{
    int ret = 0;
    int i = 0;
    enum FW_STATUS cstate = FTS_RUN_IN_ERROR;

    for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
        ret = ft5662_fwupg_get_boot_state(&cstate);
        /* FTS_DEBUG("fw state=%d, retries=%d", cstate, i); */
        if (cstate == rstate)
            return true;
        fts_msleep(FTS_DELAY_READ_ID);
    }

    return false;
}

static int ft5662_fwupg_reset_to_romboot(void)
{
    int ret = 0;
    int i = 0;
    u8 cmd = FTS_CMD_RESET;
    enum FW_STATUS state = FTS_RUN_IN_ERROR;

    ret = fts_write(&cmd, 1);
    if (ret < 0) {
        FTS_ERROR("pram/rom/bootloader reset cmd write fail");
        return ret;
    }
    mdelay(10);

    for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
        ret = ft5662_fwupg_get_boot_state(&state);
        if (FTS_RUN_IN_ROM == state)
            break;
        fts_msleep(5);
    }
    if (i >= FTS_UPGRADE_LOOP) {
        FTS_ERROR("reset to romboot fail");
        return -EIO;
    }

    return 0;
}

static void ft5662_crc16_calc_host(u8 *pbuf, u32 length, u16 *ecc)
{
    u32 i = 0;
    u32 j = 0;
    u16 tmp_ecc = 0;

    for ( i = 0; i < length; i += 2 ) {
        tmp_ecc ^= ((pbuf[i] << 8) | (pbuf[i + 1]));
        for (j = 0; j < 16; j ++) {
            if (tmp_ecc & 0x01)
                tmp_ecc = (u16)((tmp_ecc >> 1) ^ AL2_FCS_COEF);
            else
                tmp_ecc >>= 1;
        }
    }

    *ecc = tmp_ecc;
}

static int ft5662_pram_ecc_cal(u32 start_addr, u32 ecc_length, u16 *ecc)
{
    int ret = 0;
    u8 val[2] = { 0 };
    u8 cmd[FTS_ROMBOOT_CMD_ECC_NEW_LEN] = { 0 };

    FTS_INFO("read out pramboot checksum");
    cmd[0] = FTS_ROMBOOT_CMD_ECC;
    cmd[1] = BYTE_OFF_16(start_addr);
    cmd[2] = BYTE_OFF_8(start_addr);
    cmd[3] = BYTE_OFF_0(start_addr);
    cmd[4] = BYTE_OFF_16(ecc_length);
    cmd[5] = BYTE_OFF_8(ecc_length);
    cmd[6] = BYTE_OFF_0(ecc_length);
    ret = fts_write(cmd, FTS_ROMBOOT_CMD_ECC_NEW_LEN);
    if (ret < 0) {
        FTS_ERROR("write pramboot ecc cal cmd fail");
        return ret;
    }

    fts_msleep(10);
    cmd[0] = FTS_ROMBOOT_CMD_ECC_READ;
    ret = fts_read(cmd, 1, val, 2);
    if (ret < 0) {
        FTS_ERROR("read pramboot ecc fail");
        return ret;
    }

    *ecc = ((u16)(val[0] << 8) + val[1]);
    return 0;
}

static int ft5662_pram_write_buf(u8 *buf, u32 len)
{
    int ret = 0;
    u32 i = 0;
    u32 j = 0;
    u32 offset = 0;
    u32 remainder = 0;
    u32 packet_number;
    u32 packet_len = 0;
    u8 packet_buf[FTS_FLASH_PACKET_LENGTH + FTS_CMD_WRITE_LEN] = { 0 };
    u32 cmdlen = 0;

    FTS_INFO("write pramboot to pram,pramboot len=%d", len);
    if (!buf || (len < PRAMBOOT_MIN_SIZE) || (len > PRAMBOOT_MAX_SIZE)) {
        FTS_ERROR("buf/pramboot length(%d) fail", len);
        return -EINVAL;
    }

    packet_number = len / FTS_FLASH_PACKET_LENGTH;
    remainder = len % FTS_FLASH_PACKET_LENGTH;
    if (remainder > 0)
        packet_number++;
    packet_len = FTS_FLASH_PACKET_LENGTH;

    for (i = 0; i < packet_number; i++) {
        offset = i * FTS_FLASH_PACKET_LENGTH;
        /* last packet */
        if ((i == (packet_number - 1)) && remainder)
            packet_len = remainder;

        if ((fts_data->bus_type == BUS_TYPE_SPI) && (fts_data->bus_ver == BUS_VER_V2)) {
            packet_buf[0] = FTS_ROMBOOT_CMD_SET_PRAM_ADDR;
            packet_buf[1] = BYTE_OFF_16(offset);
            packet_buf[2] = BYTE_OFF_8(offset);
            packet_buf[3] = BYTE_OFF_0(offset);

            ret = fts_write(packet_buf, FTS_ROMBOOT_CMD_SET_PRAM_ADDR_LEN);
            if (ret < 0) {
                FTS_ERROR("pramboot set write address(%d) fail", i);
                return ret;
            }

            packet_buf[0] = FTS_ROMBOOT_CMD_WRITE;
            cmdlen = 1;
        } else {
            packet_buf[0] = FTS_ROMBOOT_CMD_WRITE;
            packet_buf[1] = BYTE_OFF_16(offset);
            packet_buf[2] = BYTE_OFF_8(offset);
            packet_buf[3] = BYTE_OFF_0(offset);

            packet_buf[4] = BYTE_OFF_8(packet_len);
            packet_buf[5] = BYTE_OFF_0(packet_len);
            cmdlen = 6;
        }

        for (j = 0; j < packet_len; j++) {
            packet_buf[cmdlen + j] = buf[offset + j];
        }

        ret = fts_write(packet_buf, packet_len + cmdlen);
        if (ret < 0) {
            FTS_ERROR("pramboot write data(%d) fail", i);
            return ret;
        }
    }

    return 0;
}

static int ft5662_pram_start(void)
{
    u8 cmd = FTS_ROMBOOT_CMD_START_APP;
    int ret = 0;

    FTS_INFO("remap to start pramboot");

    ret = fts_write(&cmd, 1);
    if (ret < 0) {
        FTS_ERROR("write start pram cmd fail");
        return ret;
    }
    fts_msleep(FTS_DELAY_PRAMBOOT_START);

    return 0;
}

static int fts_ft5662_write_pramboot_private(void)
{
    int ret = 0;
    bool state = 0;
    enum FW_STATUS status = FTS_RUN_IN_ERROR;
    u16 ecc_in_host = 0;
    u16 ecc_in_tp = 0;
    u8 *pb_buf = pb_file_ft5662;
    u32 pb_len = sizeof(pb_file_ft5662);

    FTS_INFO("**********pram write and init**********");
    if (pb_len < FTS_MIN_LEN) {
        FTS_ERROR("pramboot length(%d) fail", pb_len);
        return -EINVAL;
    }

    FTS_DEBUG("check whether tp is in romboot or not ");
    /* need reset to romboot when non-romboot state */
    ret = ft5662_fwupg_get_boot_state(&status);
    if (status != FTS_RUN_IN_ROM) {
        FTS_INFO("tp isn't in romboot, need send reset to romboot");
        ret = ft5662_fwupg_reset_to_romboot();
        if (ret < 0) {
            FTS_ERROR("reset to romboot fail");
            return ret;
        }
    }

    /* write pramboot to pram */
    ret = ft5662_pram_write_buf(pb_buf, pb_len);
    if (ret < 0) {
        FTS_ERROR( "write pramboot buffer fail");
        return ret;
    }

    /* check CRC */
    ft5662_crc16_calc_host(pb_buf, pb_len, &ecc_in_host);
    ret = ft5662_pram_ecc_cal(0, pb_len, &ecc_in_tp);
    if (ret < 0) {
        FTS_ERROR( "read pramboot ecc fail");
        return ret;
    }

    FTS_INFO("pram ecc in tp:%x, host:%x", ecc_in_tp, ecc_in_host);
    /*  pramboot checksum != fw checksum, upgrade fail */
    if (ecc_in_host != ecc_in_tp) {
        FTS_ERROR("pramboot ecc check fail");
        return -EIO;
    }

    /*start pram*/
    ret = ft5662_pram_start();
    if (ret < 0) {
        FTS_ERROR("pram start fail");
        return ret;
    }

    FTS_DEBUG("after write pramboot, confirm run in pramboot");
    state = ft5662_fwupg_check_state(FTS_RUN_IN_PRAM);
    if (!state) {
        FTS_ERROR("not in pramboot");
        return -EIO;
    }

    return 0;
}


/************************************************************************
* Name: fts_ft5662_upgrade
* Brief:
* Input:
* Output:
* Return: return 0 if success, otherwise return error code
***********************************************************************/
static int fts_ft5662_upgrade(u8 *buf, u32 len)
{
    int ret = 0;
    u32 start_addr = 0;
    u32 delay = 0;
    u8 cmd[4] = { 0 };
    int ecc_in_host = 0;
    int ecc_in_tp = 0;


    if ((NULL == buf) || (len < FTS_MIN_LEN)) {
        FTS_ERROR("buffer/len(%x) is invalid", len);
        return -EINVAL;
    }

    /* enter into upgrade environment */
    ret = fts_fwupg_enter_into_boot();
    if (ret < 0) {
        FTS_ERROR("enter into pramboot/bootloader fail,ret=%d", ret);
        goto fw_reset;
    }

    cmd[0] = FTS_CMD_FLASH_MODE;
    cmd[1] = FLASH_MODE_UPGRADE_VALUE;
    ret = fts_write(cmd, 2);
    if (ret < 0) {
        FTS_ERROR("upgrade mode(09) cmd write fail");
        goto fw_reset;
    }

    cmd[0] = FTS_CMD_APP_DATA_LEN_INCELL;
    cmd[1] = BYTE_OFF_16(len);
    cmd[2] = BYTE_OFF_8(len);
    cmd[3] = BYTE_OFF_0(len);
    ret = fts_write(cmd, FTS_CMD_DATA_LEN_LEN);
    if (ret < 0) {
        FTS_ERROR("data len cmd write fail");
        goto fw_reset;
    }

    delay = FTS_ERASE_SECTOR_DELAY * (len / FTS_MAX_LEN_SECTOR);
    ret = fts_fwupg_erase(delay);
    if (ret < 0) {
        FTS_ERROR("erase cmd write fail");
        goto fw_reset;
    }

    /* write app */
    ecc_in_host = fts_flash_write_buf(start_addr, buf, len, 1);
    if (ecc_in_host < 0 ) {
        FTS_ERROR("flash write fail");
        goto fw_reset;
    }

    FTS_INFO( "**********read out checksum**********");
    /* ecc */
    ecc_in_tp = fts_fwupg_ecc_cal(start_addr, len);
    if (ecc_in_tp < 0 ) {
        FTS_ERROR("ecc read fail");
        goto fw_reset;
    }

    FTS_INFO("ecc in tp:%x, host:%x", ecc_in_tp, ecc_in_host);
    if (ecc_in_tp != ecc_in_host) {
        FTS_ERROR("ecc check fail");
        goto fw_reset;
    }

    FTS_INFO("upgrade success, reset to normal boot");
    ret = fts_fwupg_reset_in_boot();
    if (ret < 0) {
        FTS_ERROR("reset to normal boot fail");
    }

    fts_msleep(200);
    return 0;

fw_reset:
    FTS_INFO("upgrade fail, reset to normal boot");
    ret = fts_fwupg_reset_in_boot();
    if (ret < 0) {
        FTS_ERROR("reset to normal boot fail");
    }
    return -EIO;
}

struct upgrade_func upgrade_func_ft5662 = {
    .ctype = {0x8A},
    .fwveroff = 0x010E,
    .fwcfgoff = 0x0F80,
    .appoff = 0x0000,
    .upgspec_version = UPGRADE_SPEC_V_1_1,
    .pramboot_supported = true,
    .pramboot = pb_file_ft5662,
    .pb_length = sizeof(pb_file_ft5662),
    .write_pramboot_private = fts_ft5662_write_pramboot_private,
    .hid_supported = false,
    .upgrade = fts_ft5662_upgrade,
};
