// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include "ipa_rtp_genl.h"
#include "ipa_i.h"
#include <net/sock.h>
#include <linux/skbuff.h>
#include <uapi/linux/in.h>
#include <synx_api.h>

#define MAX_OPEN_FRAMES 3
/* Single-NAL:0, FU-A Type: 1 */
#define MAX_STREAM_TYPES 2
#define MAX_IP_TYPES 2

#define IPA_RTP_RT_TBL_NAME "ipa_rtp_rt"

#define IPA_RTP_GENL_OP(_cmd, _func)			\
	{						\
		.cmd	= _cmd,				\
		.doit	= _func,			\
		.dumpit	= NULL,				\
		.flags	= 0,				\
	}

static u8 ipa_rtp_active_streams[MAX_STREAMS];

static struct nla_policy ipa_rtp_genl_attr_policy[IPA_RTP_GENL_ATTR_MAX + 1] = {
	[IPA_RTP_GENL_ATTR_STR]  = { .type = NLA_NUL_STRING, .len = IPA_RTP_GENL_MAX_STR_LEN },
	[IPA_RTP_GENL_ATTR_INT]  = { .type = NLA_S32 },
	[IPA_RTP_GENL_ATTR_TUPLE_INFO] = NLA_POLICY_EXACT_LEN(sizeof(struct traffic_tuple_info)),
	[IPA_RTP_GENL_ATTR_ASSIGN_STREAM_ID] =
				NLA_POLICY_EXACT_LEN(sizeof(struct assign_stream_id)),
	[IPA_RTP_GENL_ATTR_ADD_BITSTREAM_BUFF] =
				 NLA_POLICY_EXACT_LEN(sizeof(struct bitstream_buffers)),
	[IPA_RTP_GENL_ATTR_SMMU_MAP_BUFF] = NLA_POLICY_EXACT_LEN(sizeof(struct map_buffer)),
	[IPA_RTP_GENL_ATTR_SMMU_UNMAP_BUFF] = NLA_POLICY_EXACT_LEN(sizeof(struct unmap_buffer)),
	[IPA_RTP_GENL_ATTR_REMOVE_STREAM_ID] =
				 NLA_POLICY_EXACT_LEN(sizeof(struct remove_bitstream_buffers)),
};

static const struct genl_ops ipa_rtp_genl_ops[] = {
	IPA_RTP_GENL_OP(IPA_RTP_GENL_CMD_TUPLE_INFO,
			ipa_rtp_tuple_info_req_hdlr),
	IPA_RTP_GENL_OP(IPA_RTP_GENL_CMD_ADD_BITSTREAM_BUFF,
			ipa_rtp_add_bitstream_buff_req_hdlr),
	IPA_RTP_GENL_OP(IPA_RTP_GENL_CMD_SMMU_MAP_BUFF,
			ipa_rtp_smmu_map_buff_req_hdlr),
	IPA_RTP_GENL_OP(IPA_RTP_GENL_CMD_SMMU_UNMAP_BUFF,
			ipa_rtp_smmu_unmap_buff_req_hdlr),
	IPA_RTP_GENL_OP(IPA_RTP_GENL_CMD_REMOVE_STREAM_ID,
			ipa_rtp_rmv_stream_id_req_hdlr),
};

struct genl_family ipa_rtp_genl_family = {
	.id = 0,
	.hdrsize = 0,
	.name    = IPA_RTP_GENL_FAMILY_NAME,
	.version = IPA_RTP_GENL_VERSION,
	.maxattr = IPA_RTP_GENL_ATTR_MAX,
	.policy  = ipa_rtp_genl_attr_policy,
	.ops     = ipa_rtp_genl_ops,
	.n_ops   = ARRAY_SIZE(ipa_rtp_genl_ops),
};

static enum ipa_hdr_proc_type ipa3_get_rtp_hdr_proc_type(u32 stream_id)
{
	enum ipa_hdr_proc_type rtp_hdr_proc_type = IPA_HDR_PROC_MAX;

	switch (stream_id) {
	case 0:
		rtp_hdr_proc_type = IPA_HDR_PROC_RTP_METADATA_STREAM0;
		break;
	case 1:
		rtp_hdr_proc_type = IPA_HDR_PROC_RTP_METADATA_STREAM1;
		break;
	case 2:
		rtp_hdr_proc_type = IPA_HDR_PROC_RTP_METADATA_STREAM2;
		break;
	case 3:
		rtp_hdr_proc_type = IPA_HDR_PROC_RTP_METADATA_STREAM3;
		break;
	default:
		IPAERR("invalid stream_id %u params\n", stream_id);
		break;
	}
	return rtp_hdr_proc_type;
}

static enum ipa_client_type ipa3_get_rtp_dst_pipe(u32 stream_id)
{
	enum ipa_client_type dst_pipe_num = IPA_CLIENT_MAX;

	switch (stream_id) {
	case 0:
		dst_pipe_num = IPA_CLIENT_UC_RTP1_CONS;
		break;
	case 1:
		dst_pipe_num = IPA_CLIENT_UC_RTP2_CONS;
		break;
	case 2:
		dst_pipe_num = IPA_CLIENT_UC_RTP3_CONS;
		break;
	case 3:
		dst_pipe_num = IPA_CLIENT_UC_RTP4_CONS;
		break;
	default:
		IPAERR("invalid stream_id %u params\n", stream_id);
		break;
	}
	return dst_pipe_num;
}

static int ipa3_rtp_del_flt_rule(u32 stream_id)
{
	int rc = 0;
	int ipa_ep_idx;
	struct ipa3_ep_context *ep;
	struct ipa_ioc_del_flt_rule *rtp_del_flt_rule = NULL;

	rtp_del_flt_rule = kzalloc(sizeof(*rtp_del_flt_rule) +
		1 * sizeof(struct ipa_flt_rule_del), GFP_KERNEL);
	if (!rtp_del_flt_rule) {
		IPAERR("failed at kzalloc of rtp_del_flt_rule\n");
		rc = -ENOMEM;
		return rc;
	}

	ipa_ep_idx = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
	ep = &ipa3_ctx->ep[ipa_ep_idx];

	/* check whether filter rule hdl is deleted or not */
	if (ep->rtp_flt4_rule_hdls[stream_id] != -1) {
		IPADBG("Deleting rtp filter rules of stream_id: %u\n", stream_id);
		rtp_del_flt_rule->commit = 1;
		rtp_del_flt_rule->ip = 0;
		rtp_del_flt_rule->num_hdls = 1;
		rtp_del_flt_rule->hdl[0].hdl = ep->rtp_flt4_rule_hdls[stream_id];
		if (ipa3_del_flt_rule(rtp_del_flt_rule) || rtp_del_flt_rule->hdl[0].status) {
			IPAERR("failed to del rtp_flt_rule\n");
			kfree(rtp_del_flt_rule);
			rc = -EPERM;
			return rc;
		}
		ep->rtp_flt4_rule_hdls[stream_id] = -1;
	}

	kfree(rtp_del_flt_rule);
	return rc;
}

static int ipa3_rtp_del_rt_rule(u32 stream_id)
{
	int rc = 0;
	struct ipa_ioc_del_rt_rule *rtp_del_rt_rule = NULL;

	rtp_del_rt_rule = kzalloc(sizeof(*rtp_del_rt_rule) +
		1 * sizeof(struct ipa_rt_rule_del), GFP_KERNEL);
	if (!rtp_del_rt_rule) {
		IPAERR("failed at kzalloc of rtp_del_rt_rule\n");
		rc = -ENOMEM;
		return rc;
	}

	/* check whether route rule hdl is deleted or not */
	if (ipa3_ctx->rtp_rt4_rule_hdls[stream_id] != -1) {
		IPADBG("Deleting rtp route rules of stream_id: %u\n", stream_id);
		rtp_del_rt_rule->commit = 1;
		rtp_del_rt_rule->ip = 0;
		rtp_del_rt_rule->num_hdls = 1;
		rtp_del_rt_rule->hdl[0].hdl = ipa3_ctx->rtp_rt4_rule_hdls[stream_id];
		if (ipa3_del_rt_rule(rtp_del_rt_rule) || rtp_del_rt_rule->hdl[0].status) {
			IPAERR("failed to del rtp_rt_rule\n");
			kfree(rtp_del_rt_rule);
			rc = -EPERM;
			return rc;
		}
		ipa3_ctx->rtp_rt4_rule_hdls[stream_id] = -1;
	}

	kfree(rtp_del_rt_rule);
	return rc;
}

static int ipa3_rtp_del_hdr_proc_ctx(u32 stream_id)
{
	int buf_size, rc = 0;

	struct ipa_ioc_del_hdr_proc_ctx *rtp_del_proc_ctx = NULL;
	struct ipa_hdr_proc_ctx_del *rtp_del_proc_ctx_entry = NULL;

	buf_size = (sizeof(struct ipa_ioc_del_hdr_proc_ctx) +
		(sizeof(struct ipa_hdr_proc_ctx_del)));
	rtp_del_proc_ctx = kzalloc(buf_size, GFP_KERNEL);
	if (!rtp_del_proc_ctx) {
		IPAERR("failed at kzalloc of rtp_del_proc_ctx\n");
		rc = -ENOMEM;
		return rc;
	}

	/* check whether hdr proc ctx hdl is deleted or not */
	if (ipa3_ctx->rtp_proc_hdls[stream_id] != -1) {
		IPADBG("Deleting rtp hdr proc ctx of stream_id: %u\n", stream_id);
		rtp_del_proc_ctx_entry = &(rtp_del_proc_ctx->hdl[0]);
		rtp_del_proc_ctx->commit = 1;
		rtp_del_proc_ctx->num_hdls = 1;
		rtp_del_proc_ctx->hdl[0].hdl = ipa3_ctx->rtp_proc_hdls[stream_id];
		if (ipa3_del_hdr_proc_ctx(rtp_del_proc_ctx) || rtp_del_proc_ctx->hdl[0].status) {
			IPAERR("failed to del rtp proc ctx hdl\n");
			kfree(rtp_del_proc_ctx);
			rc = -EPERM;
			return rc;
		}
		ipa3_ctx->rtp_proc_hdls[stream_id] = -1;
	}

	kfree(rtp_del_proc_ctx);
	return rc;
}

int ipa3_install_rtp_hdr_proc_rt_flt_rules(struct traffic_tuple_info *tuple_info, u32 stream_id)
{
	int rc = 0;
	int buf_size;
	static const int num_of_proc_ctx = 1;
	struct ipa_ioc_add_hdr_proc_ctx *rtp_proc_ctx = NULL;
	struct ipa_hdr_proc_ctx_add *rtp_proc_ctx_entry = NULL;
	struct ipa_rtp_hdr_proc_ctx_params rtp_params;

	struct ipa_ioc_add_rt_rule *rtp_rt_rule = NULL;
	struct ipa_rt_rule_add *rtp_rt_rule_entry = NULL;
	struct ipa3_rt_tbl *entry = NULL;

	struct ipa3_ep_context *ep;
	struct ipa_ioc_add_flt_rule *rtp_flt_rule = NULL;
	struct ipa_flt_rule_add *rtp_flt_rule_entry = NULL;
	int ipa_ep_idx = 0;

	IPADBG("adding rtp proc ctx entry\n");
	buf_size = (sizeof(struct ipa_ioc_add_hdr_proc_ctx) +
		(num_of_proc_ctx * sizeof(struct ipa_hdr_proc_ctx_add)));

	rtp_proc_ctx = kzalloc(buf_size, GFP_KERNEL);
	if (!rtp_proc_ctx) {
		IPAERR("failed at kzalloc of rtp_proc_ctx\n");
		rc = -ENOMEM;
		return rc;
	}

	memset(rtp_proc_ctx, 0, sizeof(*rtp_proc_ctx));

	rtp_proc_ctx_entry = &(rtp_proc_ctx->proc_ctx[0]);
	rtp_proc_ctx->commit = true;
	rtp_proc_ctx->num_proc_ctxs = num_of_proc_ctx;
	rtp_proc_ctx_entry->proc_ctx_hdl = -1;
	rtp_proc_ctx_entry->status       = -1;
	if (ipa3_get_rtp_hdr_proc_type(stream_id) >= IPA_HDR_PROC_MAX) {
		IPAERR("invalid stream_id %u params\n", stream_id);
		rc = -EPERM;
		goto free_rtp_proc_ctx;
	}
	rtp_proc_ctx_entry->type  = ipa3_get_rtp_hdr_proc_type(stream_id);
	rtp_params.hdr_add_param.input_ip_version = tuple_info->ip_type;

	if (ipa3_add_rtp_hdr_proc_ctx(rtp_proc_ctx, rtp_params, false)
					|| rtp_proc_ctx_entry->status) {
		IPAERR("failed to add rtp hdr proc ctx hdl\n");
		rc = -EPERM;
		goto free_rtp_proc_ctx;
	}

	IPADBG("rtp proc ctx hdl = %u\n", rtp_proc_ctx_entry->proc_ctx_hdl);
	ipa3_ctx->rtp_proc_hdls[stream_id] = rtp_proc_ctx_entry->proc_ctx_hdl;

	IPADBG("adding rtp route rule entry\n");

	rtp_rt_rule = kzalloc(sizeof(struct ipa_ioc_add_rt_rule) + 1 *
			sizeof(struct ipa_rt_rule_add), GFP_KERNEL);
	if (!rtp_rt_rule) {
		IPAERR("failed at kzalloc of rtp_rt_rule\n");
		rc = -ENOMEM;
		goto free_rtp_proc_ctx;
	}

	memset(rtp_rt_rule, 0, sizeof(*rtp_rt_rule));
	rtp_rt_rule->num_rules = 1;
	rtp_rt_rule->commit = 1;
	rtp_rt_rule->ip = tuple_info->ip_type;
	strscpy(rtp_rt_rule->rt_tbl_name, IPA_RTP_RT_TBL_NAME,
		IPA_RESOURCE_NAME_MAX);

	rtp_rt_rule_entry = &rtp_rt_rule->rules[0];
	rtp_rt_rule_entry->at_rear = 1;
	if (ipa3_get_rtp_dst_pipe(stream_id) >= IPA_CLIENT_MAX) {
		IPAERR("invalid stream_id %u params\n", stream_id);
		rc = -EPERM;
		goto free_rtp_rt_rule;
	}
	rtp_rt_rule_entry->rule.dst = ipa3_get_rtp_dst_pipe(stream_id);
	rtp_rt_rule_entry->rule.hdr_hdl = 0;
	rtp_rt_rule_entry->rule.hdr_proc_ctx_hdl = ipa3_ctx->rtp_proc_hdls[stream_id];
	rtp_rt_rule_entry->rule.hashable = 1;
	rtp_rt_rule_entry->rule.retain_hdr = 1;
	rtp_rt_rule_entry->status = -1;

	if (ipa_add_rt_rule(rtp_rt_rule) || rtp_rt_rule_entry->status) {
		IPAERR("fail to add rtp_rt_rule\n");
		rc = -EPERM;
		goto free_rtp_rt_rule;
	}

	ipa3_ctx->rtp_rt4_rule_hdls[stream_id] = rtp_rt_rule_entry->rt_rule_hdl;
	rtp_rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	entry = __ipa3_find_rt_tbl(tuple_info->ip_type, rtp_rt_rule->rt_tbl_name);
	ipa3_ctx->rtp_rt4_tbl_idxs[stream_id] = entry->idx;
	ipa3_ctx->rtp_rt4_tbl_hdls[stream_id] = entry->id;

	IPADBG("rtp rt rule hdl %d\n", ipa3_ctx->rtp_rt4_rule_hdls[stream_id]);
	IPADBG("rtp rt tbl idx %d\n", ipa3_ctx->rtp_rt4_tbl_idxs[stream_id]);
	IPADBG("rtp rt tbl hdl %d\n", ipa3_ctx->rtp_rt4_tbl_hdls[stream_id]);

	IPADBG("adding rtp flt rules for %d\n", ipa_ep_idx);

	rtp_flt_rule = kzalloc(sizeof(*rtp_flt_rule) +
		1 * sizeof(struct ipa_flt_rule_add), GFP_KERNEL);
	if (!rtp_flt_rule) {
		IPAERR("failed at kzalloc of rtp_flt_rule\n");
		rc = -ENOMEM;
		goto free_rtp_rt_rule;
	}

	memset(rtp_flt_rule, 0, sizeof(*rtp_flt_rule));
	ipa_ep_idx = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
	ep = &ipa3_ctx->ep[ipa_ep_idx];

	rtp_flt_rule->commit = 1;
	rtp_flt_rule->ip = tuple_info->ip_type;
	rtp_flt_rule->ep = IPA_CLIENT_WLAN2_PROD;
	rtp_flt_rule->num_rules = 1;
	rtp_flt_rule->rules[0].at_rear = 1;
	rtp_flt_rule_entry = &rtp_flt_rule->rules[0];

	rtp_flt_rule_entry->rule.hashable = 1;
	rtp_flt_rule_entry->status =  -1;
	rtp_flt_rule_entry->rule.action = IPA_PASS_TO_ROUTING;
	rtp_flt_rule_entry->rule.rt_tbl_hdl = ipa3_ctx->rtp_rt4_tbl_hdls[stream_id];
	rtp_flt_rule_entry->rule.rt_tbl_idx = ipa3_ctx->rtp_rt4_tbl_idxs[stream_id];

	rtp_flt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
	rtp_flt_rule_entry->rule.attrib.u.v4.dst_addr = tuple_info->ip_info.ipv4.dst_ip;
	rtp_flt_rule_entry->rule.attrib.u.v4.src_addr_mask = 0xFFFFFFFF;
	rtp_flt_rule_entry->rule.attrib.u.v4.src_addr = tuple_info->ip_info.ipv4.src_ip;
	rtp_flt_rule_entry->rule.attrib.u.v4.protocol = tuple_info->ip_info.ipv4.protocol;
	rtp_flt_rule_entry->rule.attrib.src_port = tuple_info->ip_info.ipv4.src_port_number;
	rtp_flt_rule_entry->rule.attrib.dst_port = tuple_info->ip_info.ipv4.dst_port_number;

	rtp_flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
	rtp_flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	rtp_flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
	rtp_flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_PORT;
	rtp_flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_PORT;

	if (ipa3_add_flt_rule(rtp_flt_rule) || rtp_flt_rule_entry->status) {
		IPAERR("fail to add rtp_flt_rule\n");
		rc = -EPERM;
		goto free_rtp_flt_rule;
	}

	ep->rtp_flt4_rule_hdls[stream_id] = rtp_flt_rule->rules[0].flt_rule_hdl;
	IPADBG("rtp flt rule hdl is %u\n", ep->rtp_flt4_rule_hdls[stream_id]);

free_rtp_flt_rule:
	kfree(rtp_flt_rule);
free_rtp_rt_rule:
	if (rc && !rtp_rt_rule_entry->status)
		ipa3_rtp_del_rt_rule(stream_id);
	kfree(rtp_rt_rule);
free_rtp_proc_ctx:
	if (rc && !rtp_proc_ctx_entry->status)
		ipa3_rtp_del_hdr_proc_ctx(stream_id);
	kfree(rtp_proc_ctx);
	return rc;
}

int ipa3_delete_rtp_hdr_proc_rt_flt_rules(u32 stream_id)
{
	int rc = 0;

	if (ipa3_rtp_del_flt_rule(stream_id) ||
		ipa3_rtp_del_rt_rule(stream_id) ||
		ipa3_rtp_del_hdr_proc_ctx(stream_id)) {
		IPAERR("failed to delete rtp hdr proc rt flt rules\n");
		rc = -EPERM;
	}
	return rc;
}

int ipa_rtp_send_tuple_info_resp(struct genl_info *info,
			      struct assign_stream_id *tuple_info_resp)
{
	struct sk_buff *skb;
	void *msg_head;
	int rc = -1;

	IPADBG_LOW("Entry\n");

	if (!info || !tuple_info_resp) {
		IPAERR("Invalid params\n");
		return rc;
	}

	skb = genlmsg_new(sizeof(struct assign_stream_id), GFP_KERNEL);
	if (!skb) {
		IPAERR("failed to alloc genmsg_new\n");
		return rc;
	}

	msg_head = genlmsg_put(skb, 0, info->snd_seq,
			       &ipa_rtp_genl_family,
			       0, IPA_RTP_GENL_CMD_ASSIGN_STREAM_ID);
	if (!msg_head) {
		IPAERR("failed at genlmsg_put\n");
		goto free_skb;
	}

	rc = nla_put(skb, IPA_RTP_GENL_ATTR_ASSIGN_STREAM_ID,
		     sizeof(struct assign_stream_id),
		     tuple_info_resp);
	if (rc != 0) {
		IPAERR("failed at nla_put skb\n");
		goto free_skb;
	}

	genlmsg_end(skb, msg_head);

	rc = genlmsg_unicast(genl_info_net(info), skb, info->snd_portid);
	if (rc != 0) {
		IPAERR("failed in doing genlmsg_unicast\n");
		goto free_skb;
	}

	ipa3_ctx->rtp_stream_id_cnt++;
	IPADBG("assigned stream-id is %u\n", tuple_info_resp->stream_id);
	IPADBG_LOW("Exit\n");
	return rc;

free_skb:
	kfree(skb);
	return rc;
}

int ipa_rtp_tuple_info_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info)
{
	struct nlattr *na;
	struct traffic_tuple_info tuple_info_req;
	struct assign_stream_id tuple_info_resp;
	struct remove_bitstream_buffers rmv_sid_req;
	int is_req_valid = 0, i = 0;
	int stream_id_available = 0, rc = -1;

	IPADBG("Entry\n");

	if (!info) {
		IPAERR("error genl info is null\n");
		return rc;
	}

	na = info->attrs[IPA_RTP_GENL_ATTR_TUPLE_INFO];
	if (na) {
		if (nla_memcpy(&tuple_info_req, na,
			       sizeof(tuple_info_req)) > 0) {
			is_req_valid = 1;
		} else {
			IPAERR("nla_memcpy failed %d\n",
			       IPA_RTP_GENL_ATTR_TUPLE_INFO);
			return rc;
		}
	} else {
		IPAERR("no info->attrs %d\n",
		       IPA_RTP_GENL_ATTR_TUPLE_INFO);
		return rc;
	}

	if (tuple_info_req.ts_info.no_of_openframe <= 0 ||
		tuple_info_req.ts_info.no_of_openframe > MAX_OPEN_FRAMES ||
		tuple_info_req.ts_info.stream_type >= MAX_STREAM_TYPES ||
		!tuple_info_req.ts_info.max_pkt_frame ||
		tuple_info_req.ip_type >= MAX_IP_TYPES) {
		IPAERR("invalid no-of-open-frames %u or stream_type %u\n",
				tuple_info_req.ts_info.no_of_openframe,
				tuple_info_req.ts_info.stream_type);
		IPAERR("or max_pkt_frames %u or ip_type %u params\n",
				tuple_info_req.ts_info.max_pkt_frame,
				tuple_info_req.ip_type);
		return rc;
	}

	/* IPv4 Type */
	if (!tuple_info_req.ip_type) {
		if (tuple_info_req.ip_info.ipv4.protocol != IPPROTO_UDP ||
			!tuple_info_req.ip_info.ipv4.src_ip ||
			!tuple_info_req.ip_info.ipv4.dst_ip) {
			IPAERR("invalid src_ip %u or dst_ip %u or protocol %u params\n",
			tuple_info_req.ip_info.ipv4.src_ip, tuple_info_req.ip_info.ipv4.dst_ip,
			tuple_info_req.ip_info.ipv4.protocol);
			return rc;
		}
	} else {
		if (tuple_info_req.ip_info.ipv6.protocol != IPPROTO_UDP) {
			IPAERR("invalid ipv6 protocol %u params\n",
				tuple_info_req.ip_info.ipv6.protocol);
			return rc;
		}
	}

	IPADBG_LOW("no_of_openframes are %u\n", tuple_info_req.ts_info.no_of_openframe);
	IPADBG_LOW("max_pkt_frame is %u\n", tuple_info_req.ts_info.max_pkt_frame);
	IPADBG_LOW("stream_type is %u\n", tuple_info_req.ts_info.stream_type);
	IPADBG_LOW("reorder_timeout is %u\n", tuple_info_req.ts_info.reorder_timeout);
	IPADBG_LOW("num_slices_per_frame are %u\n", tuple_info_req.ts_info.num_slices_per_frame);
	IPADBG_LOW("ip_type is %u\n", tuple_info_req.ip_type);
	IPADBG_LOW("src_port_number is %u\n", tuple_info_req.ip_info.ipv4.src_port_number);
	IPADBG_LOW("dst_port_number is %u\n", tuple_info_req.ip_info.ipv4.dst_port_number);
	IPADBG_LOW("src_ip is %u\n", tuple_info_req.ip_info.ipv4.src_ip);
	IPADBG_LOW("dst_ip is %u\n", tuple_info_req.ip_info.ipv4.dst_ip);
	IPADBG_LOW("protocol is %u\n", tuple_info_req.ip_info.ipv4.protocol);

	memset(&tuple_info_resp, 0, sizeof(tuple_info_resp));

	for (i = 0; i < MAX_STREAMS; i++) {
		if (ipa_rtp_active_streams[i] == 0) {
			tuple_info_resp.stream_id = i;
			stream_id_available = 1;
			break;
		}
	}

	if (!stream_id_available) {
		IPAERR("max stream-ids supported are %u only\n", MAX_STREAMS);
		return rc;
	}

	if ((!ipa3_ctx->rtp_stream_id_cnt) &&
		(synx_enable_resources(SYNX_CLIENT_HW_FENCE_IPA_CTX0,
		SYNX_RESOURCE_SOCCP, true) != SYNX_SUCCESS)) {
		IPAERR("failed to add power vote on SOCCP\n");
		return rc;
	}

	/* Call IPA driver/uC tuple info API's here */
	if (ipa3_install_rtp_hdr_proc_rt_flt_rules(&tuple_info_req, tuple_info_resp.stream_id) ||
		ipa3_tuple_info_cmd_to_wlan_uc(&tuple_info_req, tuple_info_resp.stream_id)) {
		IPAERR("failed to install hdr proc and flt rules or filters at WLAN\n");
		ipa3_delete_rtp_hdr_proc_rt_flt_rules(tuple_info_resp.stream_id);
		if ((!ipa3_ctx->rtp_stream_id_cnt) &&
			(synx_enable_resources(SYNX_CLIENT_HW_FENCE_IPA_CTX0,
			SYNX_RESOURCE_SOCCP, false) != SYNX_SUCCESS))
			IPAERR("failed to remove power vote on SOCCP\n");
		return rc;
	}

	ipa_rtp_active_streams[tuple_info_resp.stream_id] = 1;

	if (is_req_valid &&
			ipa_rtp_send_tuple_info_resp(info, &tuple_info_resp)) {
		IPAERR("failed in sending stream_id response\n");
		memset(&rmv_sid_req, 0, sizeof(rmv_sid_req));
		rmv_sid_req.stream_id = tuple_info_resp.stream_id;
		ipa3_uc_send_remove_stream_cmd(&rmv_sid_req);
		ipa3_delete_rtp_hdr_proc_rt_flt_rules(rmv_sid_req.stream_id);
		ipa_rtp_active_streams[tuple_info_resp.stream_id] = 0;
		if ((!ipa3_ctx->rtp_stream_id_cnt) &&
			(synx_enable_resources(SYNX_CLIENT_HW_FENCE_IPA_CTX0,
			SYNX_RESOURCE_SOCCP, false) != SYNX_SUCCESS))
			IPAERR("failed to remove power vote on SOCCP\n");
	} else
		rc = 0;

	IPADBG("Exit\n");
	return rc;
}

int ipa_rtp_smmu_map_buff_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info)
{
	struct nlattr *na;
	struct map_buffer map_buffer_req;
	int i = 0, is_req_valid = 0;
	int rc = -1;

	IPADBG("Entry\n");

	if (!info) {
		IPAERR("error genl info is null\n");
		return rc;
	}

	na = info->attrs[IPA_RTP_GENL_ATTR_SMMU_MAP_BUFF];
	if (na) {
		if (nla_memcpy(&map_buffer_req, na,
			       sizeof(map_buffer_req)) > 0) {
			is_req_valid = 1;
		} else {
			IPAERR("nla_memcpy failed %d\n",
			       IPA_RTP_GENL_ATTR_SMMU_MAP_BUFF);
			return rc;
		}
	} else {
		IPAERR("no info->attrs %d\n",
		       IPA_RTP_GENL_ATTR_SMMU_MAP_BUFF);
		return rc;
	}

	if (map_buffer_req.nfd <= 0 || map_buffer_req.nfd > MAX_FDS
			 || map_buffer_req.stream_id > MAX_STREAMS) {
		IPAERR("invalid nfd %u or stream_id %u params\n",
					map_buffer_req.nfd, map_buffer_req.stream_id);
		return rc;
	}

	IPADBG_LOW("number of fd's are %u\n", map_buffer_req.nfd);
	IPADBG_LOW("stream_id is %u\n", map_buffer_req.stream_id);

	/* If IPA C2 component is providing two fd's for meta fd and bitstream buff fd then
	 * sizes need to be filled. If it is a single fd for both meta data and bitstream buff
	 * then meta_buff_fd and bitstream_buffer_fd will be the same. And they need to fill
	 * bitstream_buffer_size as actual size and meta_buff_size to zero.
	 */

	for (i = 0; i < map_buffer_req.nfd; i++) {
		if (map_buffer_req.buff_info[i].bitstream_buffer_fd ==
			map_buffer_req.buff_info[i].meta_buff_fd) {
			if (!map_buffer_req.buff_info[i].bitstream_buffer_size ||
				map_buffer_req.buff_info[i].meta_buff_size) {
				IPAERR("invalid bitstream_buff_size %u\n",
					map_buffer_req.buff_info[i].bitstream_buffer_size);
				IPAERR("or meta_buff_size %u params\n",
					map_buffer_req.buff_info[i].meta_buff_size);
				return rc;
			}
		} else {
			if (!map_buffer_req.buff_info[i].bitstream_buffer_size ||
				!map_buffer_req.buff_info[i].meta_buff_size) {
				IPAERR("invalid bitstream_buff_size %u\n",
					map_buffer_req.buff_info[i].bitstream_buffer_size);
				IPAERR("or meta_buff_size %u params\n",
					map_buffer_req.buff_info[i].meta_buff_size);
				return rc;
			}
		}

		IPADBG_LOW("bitstream_buffer_fd is %u\n",
			map_buffer_req.buff_info[i].bitstream_buffer_fd);
		IPADBG_LOW("meta_buff_fd is %u\n",
			map_buffer_req.buff_info[i].meta_buff_fd);
		IPADBG_LOW("bitstream_buffer_size is %u\n",
			map_buffer_req.buff_info[i].bitstream_buffer_size);
		IPADBG_LOW("meta_buff_size is %u\n",
			map_buffer_req.buff_info[i].meta_buff_size);
	}

	/* Call IPA driver/uC API's here */
	if (is_req_valid)
		rc = ipa3_map_buff_to_device_addr(&map_buffer_req);

	IPADBG("Exit\n");
	return rc;
}

int ipa_rtp_smmu_unmap_buff_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info)
{
	struct nlattr *na;
	struct unmap_buffer unmap_buffer_req;
	int i = 0, is_req_valid = 0, rc = -1;

	IPADBG("Entry\n");

	if (!info) {
		IPAERR("error genl info is null\n");
		return rc;
	}

	na = info->attrs[IPA_RTP_GENL_ATTR_SMMU_UNMAP_BUFF];
	if (na) {
		if (nla_memcpy(&unmap_buffer_req, na,
			       sizeof(unmap_buffer_req)) > 0) {
			is_req_valid = 1;
		} else {
			IPAERR("nla_memcpy failed %d\n",
			       IPA_RTP_GENL_ATTR_SMMU_UNMAP_BUFF);
			return rc;
		}
	} else {
		IPAERR("no info->attrs %d\n",
		       IPA_RTP_GENL_ATTR_SMMU_UNMAP_BUFF);
		return rc;
	}

	if (unmap_buffer_req.nfd <= 0 || unmap_buffer_req.nfd > MAX_FDS
				 || unmap_buffer_req.stream_id > MAX_STREAMS) {
		IPAERR("invalid nfd %u or stream_id %u params\n",
					unmap_buffer_req.nfd, unmap_buffer_req.stream_id);
		return rc;
	}

	IPADBG_LOW("number of fd's are %u\n", unmap_buffer_req.nfd);
	IPADBG_LOW("stream_id is %u\n", unmap_buffer_req.stream_id);

	/* If IPA C2 component is providing two fd's for meta fd and bitstream buff fd then
	 * sizes need to be filled. If it is a single fd for both meta data and bitstream buff
	 * then meta_buff_fd and bitstream_buffer_fd will be the same. And they need to fill
	 * bitstream_buffer_size as actual size and meta_buff_size to zero.
	 */

	for (i = 0; i < unmap_buffer_req.nfd; i++) {
		if (unmap_buffer_req.buff_info[i].bitstream_buffer_fd ==
			unmap_buffer_req.buff_info[i].meta_buff_fd) {
			if (!unmap_buffer_req.buff_info[i].bitstream_buffer_size ||
				unmap_buffer_req.buff_info[i].meta_buff_size) {
				IPAERR("invalid bitstream_buff_size %u\n",
					unmap_buffer_req.buff_info[i].bitstream_buffer_size);
				IPAERR("or meta_buff_size %u params\n",
					unmap_buffer_req.buff_info[i].meta_buff_size);
				return rc;
			}
		} else {
			if (!unmap_buffer_req.buff_info[i].bitstream_buffer_size ||
				!unmap_buffer_req.buff_info[i].meta_buff_size) {
				IPAERR("invalid bitstream_buff_size %u\n",
					unmap_buffer_req.buff_info[i].bitstream_buffer_size);
				IPAERR("or meta_buff_size %u params\n",
					unmap_buffer_req.buff_info[i].meta_buff_size);
				return rc;
			}
		}

		IPADBG_LOW("bitstream_buffer_fd is %u\n",
			unmap_buffer_req.buff_info[i].bitstream_buffer_fd);
		IPADBG_LOW("meta_buff_fd is %u\n",
			unmap_buffer_req.buff_info[i].meta_buff_fd);
		IPADBG_LOW("bitstream_buffer_size is %u\n",
			unmap_buffer_req.buff_info[i].bitstream_buffer_size);
		IPADBG_LOW("meta_buff_size is %u\n",
			unmap_buffer_req.buff_info[i].meta_buff_size);
	}

	/* Call IPA driver/uC API's here */
	if (is_req_valid)
		rc = ipa3_unmap_buff_from_device_addr(&unmap_buffer_req);

	IPADBG("Exit\n");
	return rc;
}

int ipa_rtp_add_bitstream_buff_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info)
{
	struct nlattr *na;
	struct bitstream_buffers bs_buffer_req;
	int i = 0, is_req_valid = 0, rc = -1;

	IPADBG("Entry\n");

	if (!info) {
		IPAERR("error genl info is null\n");
		return rc;
	}

	na = info->attrs[IPA_RTP_GENL_ATTR_ADD_BITSTREAM_BUFF];
	if (na) {
		if (nla_memcpy(&bs_buffer_req, na,
			       sizeof(bs_buffer_req)) > 0) {
			is_req_valid = 1;
		} else {
			IPAERR("nla_memcpy failed %d\n",
			       IPA_RTP_GENL_ATTR_ADD_BITSTREAM_BUFF);
			return rc;
		}
	} else {
		IPAERR("no info->attrs %d\n",
		       IPA_RTP_GENL_ATTR_ADD_BITSTREAM_BUFF);
		return rc;
	}

	if (bs_buffer_req.buff_cnt <= 0 || bs_buffer_req.buff_cnt > MAX_BUFF ||
				bs_buffer_req.cookie != IPA_BS_BUFF_COOKIE) {
		IPAERR("invalid buff_cnt %u or buff_cookie 0x%x params\n",
					bs_buffer_req.buff_cnt, bs_buffer_req.cookie);
		return rc;
	}

	IPADBG_LOW("buff_cnt is %u\n", bs_buffer_req.buff_cnt);
	IPADBG_LOW("cookie is 0x%x\n", bs_buffer_req.cookie);

	/* If IPA C2 component is providing two fd's for meta fd and bitstream buff fd then
	 * sizes need to be filled. If it is a single fd for both meta data and bitstream buff
	 * then meta_buff_fd and bitstream_buffer_fd will be the same. And they need to fill
	 * bitstream_buffer_size as actual size and meta_buff_size to zero.
	 */

	for (i = 0; i < bs_buffer_req.buff_cnt; i++) {
		if (bs_buffer_req.bs_info[i].stream_id >= MAX_STREAMS) {
			IPAERR("invalid stream_id in buffer %u params\n",
					bs_buffer_req.bs_info[i].stream_id);
			return rc;
		}

		if (bs_buffer_req.bs_info[i].meta_buff_fd == bs_buffer_req.bs_info[i].buff_fd) {
			if (bs_buffer_req.bs_info[i].meta_buff_size ||
				!bs_buffer_req.bs_info[i].buff_size) {
				IPAERR("or meta_buff_size %u or bs_buff_size %u params\n",
						bs_buffer_req.bs_info[i].meta_buff_size,
						bs_buffer_req.bs_info[i].buff_size);
				return rc;
			}
		} else {
			if (!bs_buffer_req.bs_info[i].meta_buff_size ||
				!bs_buffer_req.bs_info[i].buff_size) {
				IPAERR("or meta_buff_size %u or bs_buff_size %u params\n",
						bs_buffer_req.bs_info[i].meta_buff_size,
						bs_buffer_req.bs_info[i].buff_size);
				return rc;
			}
		}

		IPADBG_LOW("stream_id is %u\n", bs_buffer_req.bs_info[i].stream_id);
		IPADBG_LOW("fence_id is %u\n", bs_buffer_req.bs_info[i].fence_id);
		IPADBG_LOW("buff_offset is %u\n", bs_buffer_req.bs_info[i].buff_offset);
		IPADBG_LOW("buff_fd is %u\n", bs_buffer_req.bs_info[i].buff_fd);
		IPADBG_LOW("buff_size is %u\n", bs_buffer_req.bs_info[i].buff_size);
		IPADBG_LOW("meta_buff_offset is %u\n", bs_buffer_req.bs_info[i].meta_buff_offset);
		IPADBG_LOW("meta_buff_fd is %u\n", bs_buffer_req.bs_info[i].meta_buff_fd);
		IPADBG_LOW("meta_buff_size is %u\n", bs_buffer_req.bs_info[i].meta_buff_size);
	}

	/* Call IPA driver/uC API's here */
	if (is_req_valid)
		rc = ipa3_send_bitstream_buff_info(&bs_buffer_req);

	IPADBG("Exit\n");
	return rc;
}

int ipa_rtp_rmv_stream_id_req_hdlr(struct sk_buff *skb_2,
				     struct genl_info *info)
{
	struct nlattr *na;
	struct remove_bitstream_buffers rmv_sid_req;
	int is_req_valid = 0, rc = -1;

	IPADBG("Entry\n");

	if (!info) {
		IPAERR("error genl info is null\n");
		return rc;
	}

	na = info->attrs[IPA_RTP_GENL_CMD_REMOVE_STREAM_ID];
	if (na) {
		if (nla_memcpy(&rmv_sid_req, na,
			sizeof(rmv_sid_req)) > 0) {
			is_req_valid = 1;
		} else {
			IPAERR("nla_memcpy failed %d\n",
			       IPA_RTP_GENL_CMD_REMOVE_STREAM_ID);
			return rc;
		}
	} else {
		IPAERR("no info->attrs %d\n",
		       IPA_RTP_GENL_CMD_REMOVE_STREAM_ID);
		return rc;
	}

	if (rmv_sid_req.stream_id >= MAX_STREAMS) {
		IPAERR("invalid stream_id %u params\n", rmv_sid_req.stream_id);
		return rc;
	}

	/* Call IPA driver/uC API's here */
	if (is_req_valid && (ipa3_uc_send_remove_stream_cmd(&rmv_sid_req)
		|| ipa3_delete_rtp_hdr_proc_rt_flt_rules(rmv_sid_req.stream_id))) {
		IPAERR("failed in removing stream-id, deleting hdr proc and flt rules\n");
		return rc;
	}

	ipa_rtp_active_streams[rmv_sid_req.stream_id] = 0;
	ipa3_ctx->rtp_stream_id_cnt--;
	if ((!ipa3_ctx->rtp_stream_id_cnt) &&
		(synx_enable_resources(SYNX_CLIENT_HW_FENCE_IPA_CTX0,
		SYNX_RESOURCE_SOCCP, false) != SYNX_SUCCESS))
		IPAERR("failed to remove power vote on SOCCP\n");
	rc = 0;
	IPADBG("Exit\n");
	return rc;
}

/* register ipa rtp driver family with generic netlink */
int ipa_rtp_genl_init(void)
{
	int rc = 0;

	rc = genl_register_family(&ipa_rtp_genl_family);
	if (rc != 0) {
		IPAERR("ipa_rtp genl register family failed: %d", rc);
		genl_unregister_family(&ipa_rtp_genl_family);
		return rc;
	}

	IPAERR("successfully registered ipa_rtp genl family: %s",
	       IPA_RTP_GENL_FAMILY_NAME);
	return rc;
}

/* Unregister the generic netlink family */
int ipa_rtp_genl_deinit(void)
{
	int rc = 0;

	rc = genl_unregister_family(&ipa_rtp_genl_family);
	if (rc != 0)
		IPAERR("unregister ipa_rtp genl family failed: %d", rc);
	return rc;
}

