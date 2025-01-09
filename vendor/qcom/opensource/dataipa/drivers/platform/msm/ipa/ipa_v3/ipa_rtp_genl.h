/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _IPA_RTP_GENL_H_
#define _IPA_RTP_GENL_H_

#include <net/genetlink.h>

/* Generic Netlink Definitions */
#define IPA_RTP_GENL_VERSION 1
#define IPA_RTP_GENL_FAMILY_NAME "ipa_rtp"

#define IPA_RTP_GENL_MAX_STR_LEN 255
#define MAX_BUFF 10
#define MAX_FDS 10
#define IPA_BS_BUFF_COOKIE 0x45670198

/* XR IPAC2 <-> IPA Commands */

/**
 * struct buffer_info - buffer information of map and unmap buffers.
 * @bitstream_buffer_fd: bit stream buffer file descriptor.
 * @meta_buff_fd: meta buffer file descriptor.
 * @bitstream_buffer_size: bit stream buffer fd size.
 * @meta_buff_size: meta buffer fd size.
 */

struct buffer_info {
	uint64_t bitstream_buffer_fd;
	uint64_t meta_buff_fd;
	uint64_t bitstream_buffer_size;
	uint64_t meta_buff_size;
};

/**
 * struct map_buffer - SMMU map buffers.
 * @nfd: number of fd's.
 * @stream_id: reciving stream ID.
 * @buff_info: buffer information to map buffers.
 */

struct map_buffer {
	uint32_t nfd;
	uint32_t stream_id;
	struct buffer_info buff_info[MAX_BUFF];
};

/**
 * struct unmap_buffer - SMMU unmap buffers.
 * @nfd: number of fd's.
 * @stream_id: reciving stream ID.
 * @buff_info: buffer information to unmap buffers.
 */

struct unmap_buffer {
	uint32_t nfd;
	uint32_t stream_id;
	struct buffer_info buff_info[MAX_BUFF];
};

/**
 * struct remove_bitstream_buffers - remove bitstream buffers.
 * @stream_id: stream ID to stop using bitstream buffres of the specific stream.
 */

struct remove_bitstream_buffers {
	uint32_t stream_id;
};

struct bitstream_buffer_info_to_uc {
	uint8_t stream_id;
	uint16_t fence_id;
	uint8_t reserved;
	u64 buff_addr;
	u32 buff_fd;
	u32 buff_size;
	u64 meta_buff_addr;
	u32 meta_buff_fd;
	u32 meta_buff_size;
} __packed;

struct bitstream_buffers_to_uc {
	uint16_t buff_cnt;
	uint16_t cookie;
	struct bitstream_buffer_info_to_uc bs_info[MAX_BUFF];
} __packed;

/**
 * struct traffic_selector_info - traffic selector information.
 * @no_of_openframe: no. of openframes in a stream.
 * @max_pkt_frame: maximum packets per frame.
 * @stream_type: type of stream.
 * @reorder_timeout: RTP packets reordering timeout.
 * @num_slices_per_frame: no. of slices per frame.
 */

struct traffic_selector_info {
	uint32_t no_of_openframe;
	uint32_t max_pkt_frame;
	uint32_t stream_type;
	uint64_t reorder_timeout;
	uint32_t num_slices_per_frame;
};

/**
 * struct ipv6_tuple_info - ipv6 tuple information.
 * @src_port_number: source port number.
 * @dst_port_number: dst port number.
 * @src_ip: source IP.
 * @dst_ip: dst IP.
 * @protocol: protocol type.
 */

struct ipv6_tuple_info {
	uint32_t src_port_number;
	uint32_t dst_port_number;
	uint8_t src_ip[16];
	uint8_t dst_ip[16];
	uint32_t protocol;
};

/**
 * struct ipv4_tuple_info - ipv4 tuple information.
 * @src_port_number: source port number.
 * @dst_port_number: dst port number.
 * @src_ip: source IP.
 * @dst_ip: dst IP.
 * @protocol: protocol type.
 */

struct ipv4_tuple_info {
	uint32_t src_port_number;
	uint32_t dst_port_number;
	uint32_t src_ip;
	uint32_t dst_ip;
	uint32_t protocol;
};

/**
 * struct ip_tuple_info - ip tuple information.
 * @ipv4_tuple_info: ipv4 tuple information.
 * @ipv6_tuple_info: ipv6 tuple information.
 */

union ip_tuple_info {
	struct ipv4_tuple_info ipv4;
	struct ipv6_tuple_info ipv6;
};

/**
 * struct traffic_tuple_info - traffic tuple information.
 * @ip_type: ip type (ipv4 or ipv6).
 * @ip_tuple_info: ip tuple information.
 */

struct traffic_tuple_info {
	struct traffic_selector_info ts_info;
	uint8_t ip_type;
	union ip_tuple_info ip_info;
};

/**
 * struct assign_stream_id - assign stream id for a stream.
 * @stream_id: assigned stream id.
 */

struct assign_stream_id {
	uint32_t stream_id;
};

/**
 * struct bitstream_buffer_info_to_ipa - bitstream buffer info to ipa.
 * @stream_id: stream Identifier.
 * @fence_id: fence Identifier.
 * @buff_offset: bit stream buffer offset.
 * @buff_fd: bit stream file descriptor.
 * @buff_size: bit stream suffer size.
 * @meta_buff_offset: bit stream metadata buffer offset.
 * @meta_buff_fd: bit stream metadata buffer file descriptor.
 * @meta_buff_size: bit stream metadata buffer size.
 */

struct bitstream_buffer_info_to_ipa {
	uint32_t stream_id;
	uint32_t fence_id;
	uint32_t buff_offset;
	uint32_t buff_fd;
	uint32_t buff_size;
	uint32_t meta_buff_offset;
	uint32_t meta_buff_fd;
	uint32_t meta_buff_size;
};

/**
 * struct bitstream_buffers - bitstream buffers.
 * @buff_cnt: number of buffers per stream.
 * @cookie: pre-defined macro per stream.
 * @bitstream_buffer_info_to_ipa: bitstream buffer info to ipa.
 */

struct bitstream_buffers {
	uint32_t buff_cnt;
	uint32_t cookie;
	struct bitstream_buffer_info_to_ipa bs_info[MAX_BUFF];
};

/**
 * struct bitstream_buffer_info_to_uspace - bitstream buffer info to IPA C2.
 * @stream_id: stream Identifier.
 * @fence_id: fence Identifier.
 * @buff_offset: bit stream buffer offset.
 * @buff_fd: bit stream file descriptor.
 * @buff_size: bit stream suffer size.
 * @meta_buff_offset: bit stream metadata buffer offset.
 * @meta_buff_fd: bit stream metadata buffer file descriptor.
 * @meta_buff_size: bit stream metadata buffer size.
 * @reason_failure: reason for failure.
 * @qtime_first_pkt_processed: qtime of first packet processed.
 * @qtime_last_pkt_processed:  qtime of last packet processed.
 */

struct bitstream_buffer_info_to_uspace {
	uint32_t frame_id;
	uint32_t stream_id;
	uint32_t fence_id;
	uint64_t buff_offset;
	uint32_t buff_fd;
	uint32_t buff_size;
	uint64_t meta_buff_offset;
	uint32_t meta_buff_fd;
	uint32_t meta_buff_size;
	uint32_t reason_failure;
	uint64_t qtime_first_pkt_processed;
	uint64_t qtime_last_pkt_processed;
};

/**
 * struct statistics_info - statistics information.
 * @avg_reoder_latency: average reodering latency.
 * @num_frame_to_sw: no. frames to sw-path.
 * @last_frame_to_deco: last frame to decoder.
 */

struct statistics_info {
	uint32_t avg_reoder_latency;
	uint32_t num_frame_to_sw;
	uint32_t last_frame_to_deco;
};

enum {
	IPA_RTP_GENL_CMD_UNSPEC,
	IPA_RTP_GENL_CMD_STR,
	IPA_RTP_GENL_CMD_INT,
	IPA_RTP_GENL_CMD_TUPLE_INFO,
	IPA_RTP_GENL_CMD_ASSIGN_STREAM_ID,
	IPA_RTP_GENL_CMD_ADD_BITSTREAM_BUFF,
	IPA_RTP_GENL_CMD_SMMU_MAP_BUFF,
	IPA_RTP_GENL_CMD_SMMU_UNMAP_BUFF,
	IPA_RTP_GENL_CMD_REMOVE_STREAM_ID,
	IPA_RTP_GENL_CMD_MAX,
};

enum {
	IPA_RTP_GENL_ATTR_UNSPEC,
	IPA_RTP_GENL_ATTR_STR,
	IPA_RTP_GENL_ATTR_INT,
	IPA_RTP_GENL_ATTR_TUPLE_INFO,
	IPA_RTP_GENL_ATTR_ASSIGN_STREAM_ID,
	IPA_RTP_GENL_ATTR_ADD_BITSTREAM_BUFF,
	IPA_RTP_GENL_ATTR_SMMU_MAP_BUFF,
	IPA_RTP_GENL_ATTR_SMMU_UNMAP_BUFF,
	IPA_RTP_GENL_ATTR_REMOVE_STREAM_ID,
	IPA_RTP_GENL_ATTR_MAX,
};


/* Function Prototypes */
int ipa3_install_rtp_hdr_proc_rt_flt_rules(struct traffic_tuple_info *tuple_info, u32 stream_id);
int ipa3_delete_rtp_hdr_proc_rt_flt_rules(u32 stream_id);

/*
 * This handler will be invoked when IPA C2 sends TUPLE
 * info cmd to IPA Driver via generic netlink interface.
 */
int ipa_rtp_tuple_info_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info);

/*
 * This function will be invoked when IPA driver allocates stream
 * id and sends it to IPA C2 via generic netlink interface.
 */
int ipa_rtp_send_tuple_info_resp(struct genl_info *info,
					 struct assign_stream_id *sid);

/*
 * This handler will be invoked when IPA C2 sends SMMU MAP
 * info cmd to IPA Driver via generic netlink interface.
 */
int ipa_rtp_smmu_map_buff_req_hdlr(struct sk_buff *skb_2,
				       struct genl_info *info);

/*
 * This handler will be invoked when IPA C2 sends SMMU UNMAP
 * info cmd to IPA Driver via generic netlink interface.
 */
int ipa_rtp_smmu_unmap_buff_req_hdlr(struct sk_buff *skb_2,
					 struct genl_info *info);

/*
 * This handler will be invoked when IPA C2 sends BITSTREAM BUFF
 * info cmd to IPA Driver via generic netlink interface.
 */
int ipa_rtp_add_bitstream_buff_req_hdlr(struct sk_buff *skb_2,
					 struct genl_info *info);

/*
 * This handler will be invoked when IPA C2 sends REMOVE STREAM
 * info cmd to IPA Driver via generic netlink interface.
 */
int ipa_rtp_rmv_stream_id_req_hdlr(struct sk_buff *skb_2,
					 struct genl_info *info);

/*
 * This is a generic netlink family init from IPA driver
 * and when IPA C2 userspace comes, it will connect to this
 * family via pre-defined name.
 */
int ipa_rtp_genl_init(void);

int ipa_rtp_genl_deinit(void);

#endif /*_IPA_RTP_GENL_H_*/
