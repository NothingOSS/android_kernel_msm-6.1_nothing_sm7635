// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include "ipa_i.h"
#include <linux/delay.h>
#include <synx_api.h>
#include <linux/sync_file.h>

/* ER ==> (16B * 512 entries ) * 4 frames = 8k *4 = 32k */
#define IPA_UC_PROD_EVENT_RING_SIZE 512
/* TR ==> (16B *512 entries per frame * 6 frames) * 4 prodpipes=48k *4 = 192k */
#define IPA_UC_PROD_TRANSFER_RING_SIZE (512 * 3)
/* TR==> 1024B  * 8B TRE * 2 pipes */
#define IPA_UC_CON_TRANSFER_RING_SIZE  1024

#define MAX_NUMBER_OF_STREAMS 4
#define MAX_NUMBER_OF_PARTITIONS MAX_NUMBER_OF_STREAMS

#define MAX_UC_PROD_PIPES 4
#define MAX_UC_CONS_PIPES 2

#define MAX_UC_PROD_PIPES_TR_INDEX MAX_UC_PROD_PIPES
#define MAX_UC_PROD_PIPES_ER_INDEX (MAX_UC_PROD_PIPES_TR_INDEX + MAX_UC_PROD_PIPES)
#define MAX_UC_CONS_PIPES_TR_INDEX (MAX_UC_PROD_PIPES_ER_INDEX + MAX_UC_CONS_PIPES)

#define MAX_SYNX_FENCE_SESSION_NAME  64
#define DMA_DIR DMA_BIDIRECTIONAL

#define GSI_TRE_RE_XFER 2
#define TRE_SIZE 2048

MODULE_IMPORT_NS(DMA_BUF);

enum ipa3_cpu_2_hw_rtp_commands {
	IPA_CPU_2_HW_CMD_RTP_TUPLE_INFO             =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 0),
	IPA_CPU_2_HW_CMD_RTP_ADD_TEMP_BUFF_INFO     =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 1),
	IPA_CPU_2_HW_CMD_RTP_ADD_BIT_STREAM_BUFF    =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 2),
	IPA_CPU_2_HW_CMD_RTP_GET_HFI_STRUCT         =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 3),
	IPA_CPU_2_HW_CMD_RTP_START_STREAM           =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 4),
	IPA_CPU_2_HW_CMD_RTP_STOP_STREAM            =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 5),
	IPA_CPU_2_HW_CMD_RTP_TEAR_DOWN_STREAM       =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 6),
	IPA_CPU_2_HW_CMD_RTP_UPDATE_STREAM_INFO     =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 7),
	IPA_CPU_2_HW_CMD_RTP_SIGNAL_FENCE           =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 8),
	IPA_CPU_2_HW_CMD_RTP_PIPE_SETUP             =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 10),
	IPA_CPU_2_HW_CMD_RTP_REMOVE_STREAM          =
		FEATURE_ENUM_VAL(IPA_HW_FEATURE_RTP, 11),
};

struct dma_address_map_table {
	struct dma_buf *dma_buf_list[2];
	struct dma_buf_attachment *attachment[2];
	struct sg_table *sgt[2];
};

/* Bitstream and meta buffer dma addresses list */
struct list_node {
	struct list_head list_obj;
	struct dma_address_map_table *data;
};

struct prod_pipe_tre {
	uint64_t buffer_ptr;
	uint16_t buf_len;
	uint16_t resvd1;
	uint16_t chain:1;
	uint16_t resvd4:7;
	uint16_t ieob:1;
	uint16_t ieot:1;
	uint16_t bei:1;
	uint16_t resvd3:5;
	uint8_t re_type;
	uint8_t resvd2;
} __packed;

struct con_pipe_tre {
	uint16_t bufferIndex;
	uint16_t offset2Payload;
	uint16_t payloadSize:16;
	uint8_t  valid:1;
	uint8_t  ieot:1;
	uint8_t  tre_type:2;
	uint8_t  reserved0:4;
	uint8_t  last_tre:1;
	uint8_t  reserved1:7;
} __packed;

struct temp_buff_info {
	uint64_t temp_buff_pa;
	uint32_t temp_buff_size;
} __packed;

struct rtp_pipe_setup_cmd_data {
	struct temp_buff_info uc_prod_tr[MAX_UC_PROD_PIPES];
	struct temp_buff_info uc_prod_er[MAX_UC_PROD_PIPES];
	struct temp_buff_info uc_cons_tr[MAX_UC_CONS_PIPES];
} __packed;

struct hfi_queue_info {
	u64 hfi_queue_addr;
	u32 hfi_queue_payload_size;
	u64 queue_header_start_addr;
	u64 queue_payload_start_addr;
} __packed;

struct temp_buffer_info {
	uint64_t temp_buff_pa;
	uint32_t temp_buff_size;
} __packed;

struct uc_temp_buffer_info {
	uint16_t number_of_partitions;
	struct temp_buffer_info buffer_info[MAX_NUMBER_OF_PARTITIONS];
} __packed;

struct er_tr_to_free {
	void *cpu_address_prod_tr[MAX_UC_PROD_PIPES];
	void *cpu_address_prod_er[MAX_UC_PROD_PIPES];
	void *cpu_address_cons_tr[MAX_UC_CONS_PIPES];
	struct rtp_pipe_setup_cmd_data rtp_tr_er;
	uint8_t prod_tr_no_buffs;
	uint8_t prod_er_no_buffs;
	uint8_t cons_tr_no_buffs;
} __packed;

struct er_tr_to_free er_tr_cpu_addresses;
void *cpu_address[NO_OF_BUFFS];
struct uc_temp_buffer_info tb_info;
struct list_head mapped_bs_buff_lst[MAX_NUMBER_OF_STREAMS];
struct synx_session *glob_synx_session_ptr;

int ipa3_uc_send_tuple_info_cmd(struct traffic_tuple_info *data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct traffic_tuple_info *cmd_data;

	if (!data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct traffic_tuple_info *)cmd.base;
	cmd_data->ts_info.no_of_openframe = data->ts_info.no_of_openframe;
	cmd_data->ts_info.max_pkt_frame = data->ts_info.max_pkt_frame;
	cmd_data->ts_info.stream_type = data->ts_info.stream_type;
	cmd_data->ts_info.reorder_timeout = data->ts_info.reorder_timeout;
	cmd_data->ts_info.num_slices_per_frame = data->ts_info.num_slices_per_frame;
	cmd_data->ip_type = data->ip_type;
	if (cmd_data->ip_type) {
		cmd_data->ip_info.ipv6.src_port_number = data->ip_info.ipv6.src_port_number;
		cmd_data->ip_info.ipv6.dst_port_number = data->ip_info.ipv6.dst_port_number;
		memcpy(cmd_data->ip_info.ipv6.src_ip, data->ip_info.ipv6.src_ip, 16);
		memcpy(cmd_data->ip_info.ipv6.dst_ip, data->ip_info.ipv6.dst_ip, 16);
		cmd_data->ip_info.ipv6.protocol = data->ip_info.ipv6.protocol;
	} else {
		cmd_data->ip_info.ipv4.src_port_number = data->ip_info.ipv4.src_port_number;
		cmd_data->ip_info.ipv4.dst_port_number = data->ip_info.ipv4.dst_port_number;
		cmd_data->ip_info.ipv4.src_ip = data->ip_info.ipv4.src_ip;
		cmd_data->ip_info.ipv4.dst_ip = data->ip_info.ipv4.dst_ip;
		cmd_data->ip_info.ipv4.protocol = data->ip_info.ipv4.protocol;
	}

	IPADBG("Sending uc CMD RTP_TUPLE_INFO\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_TUPLE_INFO,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("uc send tuple info cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

int ipa3_tuple_info_cmd_to_wlan_uc(struct traffic_tuple_info *req, u32 stream_id)
{
	int result = 0;
	struct ipa_wdi_opt_dpath_flt_add_cb_params flt_add_req;

	if (!req) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	if (!atomic_read(&ipa3_ctx->ipa_xr_wdi_flt_rsv_status)) {
		result = ipa_xr_wdi_opt_dpath_rsrv_filter_req();
		if (result) {
			IPAERR("filter reservation failed at WLAN %d\n", result);
			return result;
		}
	}

	memset(&flt_add_req, 0, sizeof(struct ipa_wdi_opt_dpath_flt_add_cb_params));
	flt_add_req.num_tuples = 1;
	flt_add_req.flt_info[0].version = req->ip_type;
	if (!flt_add_req.flt_info[0].version) {
		flt_add_req.flt_info[0].ipv4_addr.ipv4_saddr = req->ip_info.ipv4.src_ip;
		flt_add_req.flt_info[0].ipv4_addr.ipv4_daddr = req->ip_info.ipv4.dst_ip;
		flt_add_req.flt_info[0].protocol = req->ip_info.ipv4.protocol;
		flt_add_req.flt_info[0].sport = req->ip_info.ipv4.src_port_number;
		flt_add_req.flt_info[0].dport = req->ip_info.ipv4.dst_port_number;
		IPADBG("IPv4 saddr:0x%x, daddr:0x%x\n",
			flt_add_req.flt_info[0].ipv4_addr.ipv4_saddr,
			flt_add_req.flt_info[0].ipv4_addr.ipv4_daddr);
	} else {
		memcpy(flt_add_req.flt_info[0].ipv6_addr.ipv6_saddr,
			req->ip_info.ipv6.src_ip,
			sizeof(req->ip_info.ipv6.src_ip));
		memcpy(flt_add_req.flt_info[0].ipv6_addr.ipv6_daddr,
			req->ip_info.ipv6.dst_ip,
			sizeof(req->ip_info.ipv6.dst_ip));
		flt_add_req.flt_info[0].protocol = req->ip_info.ipv6.protocol;
		flt_add_req.flt_info[0].sport = req->ip_info.ipv6.src_port_number;
		flt_add_req.flt_info[0].dport = req->ip_info.ipv6.dst_port_number;
		IPADBG("IPv6 saddr:0x%x:%x:%x:%x, daddr:0x%x:%x:%x:%x\n",
			flt_add_req.flt_info[0].ipv6_addr.ipv6_saddr[0],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_saddr[1],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_saddr[2],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_saddr[3],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_daddr[0],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_daddr[1],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_daddr[2],
			flt_add_req.flt_info[0].ipv6_addr.ipv6_daddr[3]);
	}

	result = ipa3_uc_send_tuple_info_cmd(req);
	if (result) {
		IPAERR("Fail to send tuple info cmd to uc\n");
		return -EPERM;
	}
	else
		IPADBG("send tuple info cmd to uc succeeded\n");

	result = ipa_xr_wdi_opt_dpath_add_filter_req(&flt_add_req, stream_id);
	if (result) {
		IPAERR("Fail to send tuple info cmd to wlan\n");
		return -EPERM;
	}

	return result;
}

int ipa3_uc_send_remove_stream_cmd(struct remove_bitstream_buffers *data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct remove_bitstream_buffers *cmd_data;

	if (!data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	result = ipa_xr_wdi_opt_dpath_remove_filter_req(data->stream_id);
	if (result) {
		IPAERR("Failed to remove wlan filter of stream ID %d\n", data->stream_id);
		return result;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct remove_bitstream_buffers *)cmd.base;
	cmd_data->stream_id = data->stream_id;
	IPADBG("Sending uc CMD RTP_REMOVE_STREAM\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_REMOVE_STREAM,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("uc send remove stream cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

int ipa3_uc_send_add_bitstream_buffers_cmd(struct bitstream_buffers_to_uc *data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct bitstream_buffers_to_uc *cmd_data = NULL;

	if (!data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct bitstream_buffers_to_uc *)cmd.base;
	cmd_data->buff_cnt = data->buff_cnt;
	cmd_data->cookie = data->cookie;
	memcpy(cmd_data->bs_info, data->bs_info, (cmd_data->buff_cnt *
		sizeof(struct bitstream_buffer_info_to_uc)));
	IPADBG("Sending uc CMD RTP_ADD_BIT_STREAM_BUFF\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_ADD_BIT_STREAM_BUFF,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("uc send bitstream buffers info cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

int ipa3_uc_send_temp_buffers_info_cmd(struct uc_temp_buffer_info *data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct uc_temp_buffer_info *cmd_data = NULL;

	if (!data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct uc_temp_buffer_info *)cmd.base;
	cmd_data->number_of_partitions = data->number_of_partitions;
	memcpy(cmd_data->buffer_info, data->buffer_info,
		(sizeof(struct temp_buffer_info)*cmd_data->number_of_partitions));
	IPADBG("Sending uc CMD RTP_ADD_TEMP_BUFF_INFO\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_ADD_TEMP_BUFF_INFO,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("uc send temp buffers info cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

void ipa3_free_uc_temp_buffs(unsigned int no_of_buffs)
{
	unsigned int indx = 0;

	for (indx = 0; indx < no_of_buffs; indx++) {
		dma_free_attrs(ipa3_ctx->uc_pdev,
		tb_info.buffer_info[indx].temp_buff_size, cpu_address[indx],
		tb_info.buffer_info[indx].temp_buff_pa,
		(DMA_ATTR_NO_KERNEL_MAPPING | DMA_ATTR_FORCE_CONTIGUOUS));
	}

	IPADBG("freed uc temp buffs\n");
}

int ipa3_alloc_temp_buffs_to_uc(unsigned int size, unsigned int no_of_buffs)
{
	void *cpu_addr = NULL;
	unsigned int indx = 0;
	dma_addr_t phys_base;

	if (size < 1 || no_of_buffs < 1) {
		IPAERR("Invallid params\n");
		return -EINVAL;
	}

	for (indx = 0; indx < no_of_buffs; indx++) {
		cpu_addr = dma_alloc_attrs(ipa3_ctx->uc_pdev, size, &phys_base,
		GFP_KERNEL, DMA_ATTR_NO_KERNEL_MAPPING | DMA_ATTR_FORCE_CONTIGUOUS);
		if (!cpu_addr) {
			IPAERR("No mem for tmp buffs\n");
			ipa3_free_uc_temp_buffs(indx);
			return -ENOMEM;
		}

		cpu_address[indx] = cpu_addr;
		tb_info.buffer_info[indx].temp_buff_pa = phys_base;
		tb_info.buffer_info[indx].temp_buff_size =  size;
		tb_info.number_of_partitions += 1;
	}

	IPADBG("allocated mem for temp buffs\n");
	return ipa3_uc_send_temp_buffers_info_cmd(&tb_info);
}

int ipa3_uc_send_RTPPipeSetup_cmd(struct rtp_pipe_setup_cmd_data *rtp_cmd_data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct rtp_pipe_setup_cmd_data *cmd_data = NULL;

	if (!rtp_cmd_data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct rtp_pipe_setup_cmd_data *)cmd.base;
	memcpy(cmd_data->uc_prod_tr, rtp_cmd_data->uc_prod_tr,
		(sizeof(struct temp_buff_info) * MAX_UC_PROD_PIPES));
	memcpy(cmd_data->uc_prod_er, rtp_cmd_data->uc_prod_er,
		(sizeof(struct temp_buff_info) * MAX_UC_PROD_PIPES));
	memcpy(cmd_data->uc_cons_tr, rtp_cmd_data->uc_cons_tr,
		(sizeof(struct temp_buff_info) * MAX_UC_CONS_PIPES));
	IPADBG("Sending uc CMD RTP_PIPE_SETUP\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_PIPE_SETUP,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("send RTP pipe setup cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

static int ipa3_uc_setup_prod_pipe_transfer_ring(
	struct rtp_pipe_setup_cmd_data *rtp_cmd_data, int idx)
{
	struct ipa_mem_buffer ring;
	struct prod_pipe_tre *tr = NULL;
	int val = 0;
	u64 next = 0;

	if (!rtp_cmd_data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	ring.size = sizeof(struct prod_pipe_tre) * IPA_UC_PROD_TRANSFER_RING_SIZE;
	ring.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, ring.size,
		&ring.phys_base, GFP_KERNEL);
	if (ring.base == NULL) {
		IPAERR("dma alloc coherent failed.\n");
		return -ENOMEM;
	}

	tr = (struct prod_pipe_tre *)ring.base;
	next = tb_info.buffer_info[idx].temp_buff_pa;

	for (val = 0; val < IPA_UC_PROD_TRANSFER_RING_SIZE; val++) {
		tr->buffer_ptr = next;
		tr->buf_len = TRE_SIZE;
		tr->re_type = GSI_TRE_RE_XFER;
		tr->bei = 0;
		tr->ieot = 1;
		next = tr->buffer_ptr + 2048;
		tr++;
	}

	rtp_cmd_data->uc_prod_tr[idx].temp_buff_pa = ring.phys_base;
	rtp_cmd_data->uc_prod_tr[idx].temp_buff_size = ring.size;
	er_tr_cpu_addresses.cpu_address_prod_tr[idx] = ring.base;
	er_tr_cpu_addresses.prod_tr_no_buffs += 1;
	IPADBG("prod pipe transfer ring setup done\n");
	return 0;
}

static int ipa3_uc_setup_prod_pipe_event_ring(
	struct rtp_pipe_setup_cmd_data *rtp_cmd_data, int index)
{
	struct ipa_mem_buffer ring;

	if (!rtp_cmd_data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	ring.size = sizeof(struct prod_pipe_tre) * IPA_UC_PROD_EVENT_RING_SIZE;
	ring.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, ring.size,
		&ring.phys_base, GFP_KERNEL);
	if (ring.base == NULL) {
		IPAERR("dma alloc coherent failed.\n");
		return -EFAULT;
	}

	rtp_cmd_data->uc_prod_er[index].temp_buff_pa = ring.phys_base;
	rtp_cmd_data->uc_prod_er[index].temp_buff_size = ring.size;
	er_tr_cpu_addresses.cpu_address_prod_er[index] = ring.base;
	er_tr_cpu_addresses.prod_er_no_buffs += 1;
	IPADBG("prod pipe event ring setup done\n");
	return 0;
}

static int ipa3_uc_setup_con_pipe_transfer_ring(
	struct rtp_pipe_setup_cmd_data *rtp_cmd_data, int index)
{
	struct ipa_mem_buffer ring;

	if (!rtp_cmd_data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	ring.size = sizeof(struct con_pipe_tre) * IPA_UC_CON_TRANSFER_RING_SIZE;
	ring.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, ring.size,
		&ring.phys_base, GFP_KERNEL);
	if (ring.base == NULL) {
		IPAERR("dma alloc coherent failed.\n");
		return -ENOMEM;
	}

	rtp_cmd_data->uc_cons_tr[index].temp_buff_pa = ring.phys_base;
	rtp_cmd_data->uc_cons_tr[index].temp_buff_size = ring.size;
	er_tr_cpu_addresses.cpu_address_cons_tr[index] = ring.base;
	er_tr_cpu_addresses.cons_tr_no_buffs += 1;
	IPADBG("con pipe transfer ring setup done\n");
	return 0;
}

void ipa3_free_uc_pipes_er_tr(void)
{
	uint8_t index = 0;

	for (index = 0; index < er_tr_cpu_addresses.prod_tr_no_buffs; index++) {
		dma_free_coherent(ipa3_ctx->uc_pdev,
		er_tr_cpu_addresses.rtp_tr_er.uc_prod_tr[index].temp_buff_size,
		er_tr_cpu_addresses.cpu_address_prod_tr[index],
		er_tr_cpu_addresses.rtp_tr_er.uc_prod_tr[index].temp_buff_pa);
	}

	for (index = 0; index < er_tr_cpu_addresses.prod_er_no_buffs; index++) {
		dma_free_coherent(ipa3_ctx->uc_pdev,
		er_tr_cpu_addresses.rtp_tr_er.uc_prod_er[index].temp_buff_size,
		er_tr_cpu_addresses.cpu_address_prod_er[index],
		er_tr_cpu_addresses.rtp_tr_er.uc_prod_er[index].temp_buff_pa);
	}

	for (index = 0; index < er_tr_cpu_addresses.cons_tr_no_buffs; index++) {
		dma_free_coherent(ipa3_ctx->uc_pdev,
		er_tr_cpu_addresses.rtp_tr_er.uc_cons_tr[index].temp_buff_size,
		er_tr_cpu_addresses.cpu_address_cons_tr[index],
		er_tr_cpu_addresses.rtp_tr_er.uc_cons_tr[index].temp_buff_pa);
	}

	IPADBG("freed uc pipes er and tr memory\n");
}

int ipa3_allocate_uc_pipes_er_tr_send_to_uc(void)
{
	int res = 0;
	struct rtp_pipe_setup_cmd_data rtp_cmd_data;
	int indx = 0;

	for (indx = 0; indx < MAX_UC_PROD_PIPES; indx++) {
		res = ipa3_uc_setup_prod_pipe_transfer_ring(&rtp_cmd_data, indx);
		if (res) {
			IPAERR("In RTP Pipe setup prod tr func failed\n");
			memcpy(&er_tr_cpu_addresses.rtp_tr_er, &rtp_cmd_data,
			sizeof(rtp_cmd_data));
			ipa3_free_uc_pipes_er_tr();
			return res;
		}
		res = ipa3_uc_setup_prod_pipe_event_ring(&rtp_cmd_data, indx);
		if (res) {
			IPAERR("In RTP Pipe setup pprod er func failed\n");
			memcpy(&er_tr_cpu_addresses.rtp_tr_er, &rtp_cmd_data,
			sizeof(rtp_cmd_data));
			ipa3_free_uc_pipes_er_tr();
			return res;
		}

		if (indx < MAX_UC_CONS_PIPES) {
			res = ipa3_uc_setup_con_pipe_transfer_ring(&rtp_cmd_data, indx);
			if (res) {
				memcpy(&er_tr_cpu_addresses.rtp_tr_er, &rtp_cmd_data,
				sizeof(rtp_cmd_data));
				ipa3_free_uc_pipes_er_tr();
				IPAERR("In RTP Pipe setup con tr func failed\n");
				return res;
			}
		}
	}

	memcpy(&er_tr_cpu_addresses.rtp_tr_er, &rtp_cmd_data, sizeof(rtp_cmd_data));
	res = ipa3_uc_send_RTPPipeSetup_cmd(&rtp_cmd_data);
	IPADBG("allocated uc pipes er, tr memory and send to uc\n");
	return res;
}

int ipa3_insert_dma_info(struct dma_address_map_table *map, uint32_t stream_id)
{
	struct list_node *new_node = kzalloc(sizeof(struct list_node), GFP_KERNEL);

	if (!new_node) {
		IPAERR("failed to alloc memory.\n");
		return -ENOMEM;
	}

	if (!map) {
		IPAERR("Invalid params.\n");
		kfree(new_node);
		return -EINVAL;
	}

	new_node->data = map;
	list_add(&new_node->list_obj, &mapped_bs_buff_lst[stream_id]);
	IPADBG("inserted dma buff info into list\n");
	return 0;
}

struct dma_address_map_table *ipa3_search_dma_info(struct dma_buf *dma_buf, uint32_t stream_id)
{
	struct list_head *ptr = NULL;
	struct list_node *entry = NULL;

	if (IS_ERR_OR_NULL(dma_buf)) {
		IPAERR("Invalid params.\n");
		return NULL;
	}
	list_for_each(ptr, &mapped_bs_buff_lst[stream_id]) {
		entry = list_entry(ptr, struct list_node, list_obj);
		if (!entry || !entry->data)
			continue;

		if (dma_buf == entry->data->dma_buf_list[0])
			return entry->data;
	}

	IPADBG("Not found dma buff info in list\n");
	return NULL;
}

struct dma_address_map_table *ipa3_delete_dma_info(struct dma_buf *dma_buf, int stream_id)
{
	struct list_head *ptr = NULL;
	struct list_node *entry = NULL;
	struct dma_address_map_table *table_entry = NULL;
	int found = 0;

	if (IS_ERR_OR_NULL(dma_buf)) {
		IPAERR("Invalid params.\n");
		return NULL;
	}
	list_for_each(ptr, &mapped_bs_buff_lst[stream_id]) {
		entry = list_entry(ptr, struct list_node, list_obj);
		if (!entry || !entry->data)
			continue;
		if (dma_buf == entry->data->dma_buf_list[0]) {
			found = 1;
			break;
		}
	}

	if (found && entry) {
		table_entry = entry->data;
		list_del(ptr);
		kfree(entry);
	}

	IPADBG("deleted dma buff info from list\n");
	return table_entry;
}

int ipa3_smmu_map_buff(uint64_t bitstream_buffer_fd,
		uint64_t meta_buff_fd, int stream_id)
{
	int err = 0;
	struct dma_buf *dbuff = NULL;
	struct dma_buf_attachment *attachment = NULL;
	struct dma_address_map_table *map_table = NULL;

	map_table = kzalloc(sizeof(struct dma_address_map_table), GFP_KERNEL);
	if (!map_table) {
		IPAERR("failed to alloc memory.\n");
		return -ENOMEM;
	}

	dbuff = dma_buf_get(bitstream_buffer_fd);
	if (IS_ERR_OR_NULL(dbuff)) {
		IPAERR("no dma handle for the fd.\n");
		err = -EFAULT;
		goto map_table_free;
	}

	attachment = dma_buf_attach(dbuff, ipa3_ctx->rtp_pdev);
	if (IS_ERR_OR_NULL(attachment)) {
		IPAERR("dma buf attachment failed\n");
		err = -EFAULT;
		goto dma_buff_put;
	}

	map_table->dma_buf_list[0] = dbuff;
	map_table->attachment[0] = attachment;
	map_table->sgt[0] = NULL;

	if (bitstream_buffer_fd == meta_buff_fd) {
		map_table->dma_buf_list[1] = NULL;
		map_table->attachment[1] = NULL;
		map_table->sgt[1] = NULL;
		err = ipa3_insert_dma_info(map_table, stream_id);
		if (err) {
			IPAERR("dma info insertion failed.\n");
			goto dma_buff_det;
		}
		return err;
	}

	dbuff = dma_buf_get(meta_buff_fd);
	if (IS_ERR_OR_NULL(dbuff)) {
		IPAERR("no dma handle for the fd.\n");
		err = -EFAULT;
		goto dma_buff_det;
	}

	attachment = dma_buf_attach(dbuff, ipa3_ctx->rtp_pdev);
	if (IS_ERR_OR_NULL(attachment)) {
		IPAERR("dma buf attachment failed.\n");
		err = -EFAULT;
		goto dma_buff_det;
	}

	map_table->dma_buf_list[1] = dbuff;
	map_table->attachment[1] = attachment;
	map_table->sgt[1] = NULL;
	err = ipa3_insert_dma_info(map_table, stream_id);
	if (err) {
		IPAERR("dma info insertion failed.\n");
		goto dma_buff_det;
	}

	IPADBG("smmu map buff addr done\n");
	return err;

dma_buff_det:
	if (map_table->dma_buf_list[0])
		dma_buf_detach(map_table->dma_buf_list[0], map_table->attachment[0]);
	if (map_table->dma_buf_list[1])
		dma_buf_detach(map_table->dma_buf_list[1], map_table->attachment[1]);

dma_buff_put:
	if (map_table->dma_buf_list[0])
		dma_buf_put(map_table->dma_buf_list[0]);
	if (map_table->dma_buf_list[1])
		dma_buf_put(map_table->dma_buf_list[1]);

map_table_free:
	kfree(map_table);

	return err;
}

int ipa3_smmu_unmap_buff(uint64_t bitstream_buffer_fd, uint64_t meta_buff_fd, int stream_id)
{
	struct dma_buf *dbuff = NULL;
	struct dma_address_map_table *map_table = NULL;

	dbuff = dma_buf_get(bitstream_buffer_fd);
	if (IS_ERR_OR_NULL(dbuff)) {
		IPAERR("no dma handle for the fd.\n");
		return -EFAULT;
	}

	map_table = ipa3_delete_dma_info(dbuff, stream_id);
	if (!map_table) {
		dma_buf_put(dbuff);
		IPAERR("Buffer is not mapped\n");
		return -EFAULT;
	}

	if (map_table->sgt[0] !=  NULL) {
		dma_buf_unmap_attachment(map_table->attachment[0],
			map_table->sgt[0], DMA_DIR);
	}

	dma_buf_detach(map_table->dma_buf_list[0], map_table->attachment[0]);
	dma_buf_put(map_table->dma_buf_list[0]);
	if (bitstream_buffer_fd != meta_buff_fd) {
		if (map_table->sgt[1] !=  NULL) {
			dma_buf_unmap_attachment(map_table->attachment[1],
				map_table->sgt[1], DMA_DIR);
		}
		dma_buf_detach(map_table->dma_buf_list[1], map_table->attachment[1]);
		dma_buf_put(map_table->dma_buf_list[1]);
	}

	IPADBG("smmu unmap done\n");
	kfree(map_table);
	return 0;
}

int ipa3_map_buff_to_device_addr(struct map_buffer *map_buffs)
{
	int index = 0;
	int err = 0;

	if (!map_buffs) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	INIT_LIST_HEAD(&mapped_bs_buff_lst[map_buffs->stream_id]);
	for (index = 0; index < map_buffs->nfd; index++) {
		err = ipa3_smmu_map_buff(map_buffs->buff_info[index].bitstream_buffer_fd,
			map_buffs->buff_info[index].meta_buff_fd, map_buffs->stream_id);
		if (err) {
			IPAERR("smmu map failed\n");
			return err;
		}
	}

	IPADBG("maped buff addr to device addr\n");
	return err;
}

int ipa3_unmap_buff_from_device_addr(struct unmap_buffer *unmap_buffs)
{
	unsigned char index = 0;
	int err = 0;

	if (!unmap_buffs) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	for (index = 0; index < unmap_buffs->nfd; index++) {
		err = ipa3_smmu_unmap_buff(unmap_buffs->buff_info[index].bitstream_buffer_fd,
			unmap_buffs->buff_info[index].meta_buff_fd, unmap_buffs->stream_id);
		if (err) {
			IPAERR("smmu unmap failed\n");
			return err;
		}
	}

	IPADBG("unmaped buff addr from device addr\n");
	return err;
}

int ipa3_send_bitstream_buff_info(struct bitstream_buffers *data)
{
	struct bitstream_buffers_to_uc tmp;
	int index = 0;
	int synx_result = 0;
	struct dma_buf *dmab = NULL;
	struct dma_address_map_table *map_table = NULL;
	struct sg_table *sgt = NULL;
	struct synx_import_params params = {0};
	struct dma_fence *fence =  NULL;
	u32 handle;

	if (!data || data->buff_cnt < 1) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	memset(&tmp, 0, sizeof(struct bitstream_buffers_to_uc));
	tmp.buff_cnt = data->buff_cnt;
	tmp.cookie = data->cookie;

	for (index = 0; index < data->buff_cnt; index++) {
		/*
		 * We need to get the underlying fence handle/hash on every
		 * fence fd received from IPA C2 and pass the handle to uC.
		 */
		params.type = SYNX_IMPORT_INDV_PARAMS;
		params.indv.flags = SYNX_IMPORT_DMA_FENCE | SYNX_IMPORT_GLOBAL_FENCE;
		fence = sync_file_get_fence(data->bs_info[index].fence_id);
		if (!fence) {
			IPAERR("sync_file_get_fence failure on %u fd\n",
						data->bs_info[index].fence_id);
			return -EFAULT;
		}
		params.indv.fence = fence;
		params.indv.new_h_synx = &handle;

		synx_result = synx_import(glob_synx_session_ptr, &params);
		if (synx_result) {
			IPAERR("synx_import is failed with %d\n", synx_result);
			dma_fence_put(fence);
			return -EFAULT;
		}

		tmp.bs_info[index].fence_id = handle;

		/*
		 * Irrespective of whether bitstream_buffer cmd is sent to uC,
		 * we can call synx_release, dma_fence_put to put one refcnt
		 * taken by synx_import & sync_file_get_fence() respectively.
		 */

		if (synx_release(glob_synx_session_ptr, handle))
			IPAERR("synx_release failed on this %u handle\n", handle);
		dma_fence_put(fence);

		tmp.bs_info[index].stream_id = data->bs_info[index].stream_id;
		tmp.bs_info[index].buff_fd = data->bs_info[index].buff_fd;
		tmp.bs_info[index].buff_size = data->bs_info[index].buff_size;
		tmp.bs_info[index].meta_buff_fd = data->bs_info[index].meta_buff_fd;
		tmp.bs_info[index].meta_buff_size = data->bs_info[index].meta_buff_size;

		dmab = dma_buf_get(tmp.bs_info[index].buff_fd);
		if (IS_ERR_OR_NULL(dmab)) {
			IPAERR("no dma handle for the fd.\n");
			return -EFAULT;
		}

		map_table = ipa3_search_dma_info(dmab, tmp.bs_info[index].stream_id);
		if (!map_table) {
			IPAERR("no map table from search dma info.\n");
			dma_buf_put(dmab);
			return -EFAULT;
		}

		if (!map_table->sgt[0]) {
			sgt = dma_buf_map_attachment(map_table->attachment[0], DMA_DIR);
			if (IS_ERR_OR_NULL(sgt)) {
				dma_buf_put(dmab);
				IPAERR("dma buf map attachment failed\n");
				return -EFAULT;
			}
			map_table->sgt[0] = sgt;
		}

		if (data->bs_info[index].meta_buff_fd != data->bs_info[index].buff_fd) {
			if (!map_table->sgt[1]) {
				sgt = dma_buf_map_attachment(map_table->attachment[1], DMA_DIR);
				if (IS_ERR_OR_NULL(sgt)) {
					dma_buf_detach(map_table->dma_buf_list[0],
						map_table->attachment[0]);
					dma_buf_put(dmab);
					IPAERR("dma buf map attachment failed\n");
					return -EFAULT;
				}
				map_table->sgt[1] = sgt;
			}

			tmp.bs_info[index].buff_addr = map_table->sgt[0]->sgl->dma_address;
			tmp.bs_info[index].meta_buff_addr  = map_table->sgt[1]->sgl->dma_address;
		} else {
			tmp.bs_info[index].buff_addr = map_table->sgt[0]->sgl->dma_address +
			data->bs_info[index].buff_offset;
			tmp.bs_info[index].meta_buff_addr  = map_table->sgt[1]->sgl->dma_address;
		}
	}

	return ipa3_uc_send_add_bitstream_buffers_cmd(&tmp);
}

int ipa3_uc_send_hfi_cmd(struct hfi_queue_info *data)
{
	int result = 0;
	struct ipa_mem_buffer cmd;
	struct hfi_queue_info *cmd_data;

	if (!data) {
		IPAERR("Invalid params.\n");
		return -EINVAL;
	}

	cmd.size = sizeof(*cmd_data);
	cmd.base = dma_alloc_coherent(ipa3_ctx->uc_pdev, cmd.size,
			&cmd.phys_base, GFP_KERNEL);
	if (cmd.base == NULL) {
		IPAERR("failed to alloc DMA memory.\n");
		return -ENOMEM;
	}

	cmd_data = (struct hfi_queue_info *)cmd.base;
	memcpy(cmd_data, data, sizeof(struct hfi_queue_info));
	IPADBG("Sending uc CMD RTP_GET_HFI_STRUCT\n");
	result = ipa3_uc_send_cmd((u32)(cmd.phys_base),
				IPA_CPU_2_HW_CMD_RTP_GET_HFI_STRUCT,
				0,
				false, 10*HZ);
	if (result) {
		IPAERR("uc send hfi queue info cmd failed\n");
		result = -EPERM;
	}

	dma_free_coherent(ipa3_ctx->uc_pdev, cmd.size, cmd.base, cmd.phys_base);
	return result;
}

int ipa3_create_hfi_send_uc(void)
{
	int res = 0;
	struct synx_initialization_params params;
	struct synx_queue_desc queue_desc;
	char synx_session_name[MAX_SYNX_FENCE_SESSION_NAME];
	struct hfi_queue_info data;
	dma_addr_t hfi_queue_addr = 0;
	struct ipa_smmu_cb_ctx *cb = NULL;
	struct synx_hw_fence_hfi_queue_header *hfi_queue_payload_vptr = NULL;

	snprintf(synx_session_name, MAX_SYNX_FENCE_SESSION_NAME, "ipa synx fence");
	queue_desc.vaddr = NULL;
	queue_desc.mem_data = NULL;
	queue_desc.size = 0;
	queue_desc.dev_addr = 0;

	params.name = (const char *)synx_session_name;
	params.ptr = &queue_desc;
	params.id = SYNX_CLIENT_HW_FENCE_IPA_CTX0;
	params.flags = SYNX_INIT_MAX;

	glob_synx_session_ptr = synx_initialize(&params);
	if (IS_ERR_OR_NULL(glob_synx_session_ptr)) {
		IPAERR("invalid synx fence session\n");
		return -EFAULT;
	}

	cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_UC);
	res = ipa3_iommu_map(cb->iommu_domain,
			queue_desc.dev_addr, queue_desc.dev_addr,
			queue_desc.size, IOMMU_READ | IOMMU_WRITE);
	if (res) {
		IPAERR("HFI - smmu map failed\n");
		synx_uninitialize(glob_synx_session_ptr);
		return -EFAULT;
	}

	IPADBG("hfi queue addr is 0x%x and size is 0x%x\n",
				queue_desc.dev_addr, queue_desc.size);

	hfi_queue_addr = queue_desc.dev_addr;
	data.hfi_queue_addr = hfi_queue_addr;
	data.queue_header_start_addr = hfi_queue_addr +
			sizeof(struct synx_hw_fence_hfi_queue_table_header);
	data.queue_payload_start_addr = data.queue_header_start_addr +
			sizeof(struct synx_hw_fence_hfi_queue_header);
	hfi_queue_payload_vptr = (struct synx_hw_fence_hfi_queue_header *)(queue_desc.vaddr +
				sizeof(struct synx_hw_fence_hfi_queue_table_header));
	data.hfi_queue_payload_size = hfi_queue_payload_vptr->queue_size;
	IPADBG("hfi queue payload vptr is 0x%x\n", hfi_queue_payload_vptr);
	IPADBG("hfi queue payload size is 0x%x\n", data.hfi_queue_payload_size);
	res = ipa3_uc_send_hfi_cmd(&data);
	return res;
}
