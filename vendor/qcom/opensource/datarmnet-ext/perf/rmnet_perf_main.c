// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2021-2023 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * RMNET PERF framework
 *
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include "rmnet_module.h"
#include <net/ipv6.h>
#include <net/ip.h>
#include "rmnet_perf_tcp.h"
#include "rmnet_perf_udp.h"
MODULE_LICENSE("\x47\x50\x4c\x20\x76\x32");static char*verinfo[]={
"\x38\x61\x62\x30\x61\x38\x65\x65","\x66\x32\x32\x62\x61\x63\x65\x30",
"\x63\x63\x39\x38\x66\x30\x38\x61","\x63\x65\x37\x39\x33\x32\x31\x63",
"\x35\x64\x63\x64\x64\x34\x63\x30","\x34\x63\x39\x62\x35\x33\x33\x37",
"\x61\x33\x62\x61\x62\x64\x34\x30","\x37\x66\x30\x37\x38\x66\x39\x36",
"\x65\x35\x36\x63\x62\x35\x35\x64","\x65\x32\x31\x38\x66\x34\x35\x31",
"\x33\x39\x63\x62\x64\x37\x64\x35","\x37\x34\x31\x35\x39\x32\x31\x63",
"\x34\x64\x65\x34\x39\x64\x62\x35","\x62\x31\x63\x34\x34\x62\x34\x63"};
module_param_array(verinfo,charp,NULL,(0xcb7+5769-0x221c));MODULE_PARM_DESC(
verinfo,
"\x56\x65\x72\x73\x69\x6f\x6e\x20\x6f\x66\x20\x74\x68\x65\x20\x64\x72\x69\x76\x65\x72"
);bool DATARMNETc03b668e4c=true;module_param_named(rmnet_perf_knob0,
DATARMNETc03b668e4c,bool,(0xdb7+6665-0x261c));static bool DATARMNET21c7a3c377=
true;module_param_named(rmnet_perf_knob1,DATARMNET21c7a3c377,bool,
(0xdb7+6665-0x261c));extern void(*rmnet_perf_egress_hook1)(struct sk_buff*
DATARMNET543491eb0f);
#define DATARMNETf241a4b20f (0xe07+6616-0x2624)
static inline bool DATARMNETe1ef3edea2(struct udphdr*DATARMNETa1abb4897c){return
 be16_to_cpu(DATARMNETa1abb4897c->source)==DATARMNETf241a4b20f||be16_to_cpu(
DATARMNETa1abb4897c->dest)==DATARMNETf241a4b20f;}static bool DATARMNET77ab42949f
(struct sk_buff*DATARMNET543491eb0f,int DATARMNET611d08d671){u8*
DATARMNET352416e828,DATARMNETd9bda61912;struct udphdr*DATARMNETa1abb4897c,
DATARMNET7b1af10647;DATARMNETa1abb4897c=skb_header_pointer(DATARMNET543491eb0f,
DATARMNET611d08d671,sizeof(*DATARMNETa1abb4897c),&DATARMNET7b1af10647);if(!
DATARMNETa1abb4897c||!DATARMNETe1ef3edea2(DATARMNETa1abb4897c))return false;if(
be16_to_cpu(DATARMNETa1abb4897c->len)<sizeof(struct udphdr)+(0xd26+209-0xdf6))
return false;DATARMNET352416e828=skb_header_pointer(DATARMNET543491eb0f,
DATARMNET611d08d671+sizeof(struct udphdr),(0xd26+209-0xdf6),&DATARMNETd9bda61912
);if(!DATARMNET352416e828)return false;return((*DATARMNET352416e828)&
(0xbf7+4869-0x1e3c))==(0xbf7+4869-0x1e3c);}static int DATARMNETee9214ce9b(struct
 sk_buff*DATARMNET543491eb0f,int DATARMNET611d08d671){if(DATARMNET77ab42949f(
DATARMNET543491eb0f,DATARMNET611d08d671)){DATARMNET543491eb0f->hash=
(0xd2d+202-0xdf7);DATARMNET543491eb0f->sw_hash=(0xd26+209-0xdf6);return
(0xd2d+202-0xdf7);}return-EINVAL;}int DATARMNETf2958615b1(struct sk_buff*
DATARMNET543491eb0f){if(DATARMNET543491eb0f->protocol==htons(ETH_P_IP)){struct 
iphdr*DATARMNET86f1f2cdc9,DATARMNETbf6548198e;DATARMNET86f1f2cdc9=
skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),sizeof(*
DATARMNET86f1f2cdc9),&DATARMNETbf6548198e);if(!DATARMNET86f1f2cdc9||
ip_is_fragment(DATARMNET86f1f2cdc9))return-EINVAL;if(DATARMNET86f1f2cdc9->
protocol==IPPROTO_UDP){if(DATARMNET21c7a3c377)DATARMNET2a6d02a6a5(
DATARMNET543491eb0f);return DATARMNETee9214ce9b(DATARMNET543491eb0f,
DATARMNET86f1f2cdc9->ihl*(0xd11+230-0xdf3));}if(DATARMNET86f1f2cdc9->protocol==
IPPROTO_TCP){if(DATARMNETc03b668e4c)DATARMNET9250714999(DATARMNET543491eb0f);
return-EINVAL;}}else if(DATARMNET543491eb0f->protocol==htons(ETH_P_IPV6)){struct
 ipv6hdr*DATARMNETbf55123e5b,DATARMNETcf1d9e2c1e;int DATARMNET611d08d671;__be16 
DATARMNET98fed39916;u8 DATARMNET65293f17c4;DATARMNETbf55123e5b=
skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),sizeof(*
DATARMNETbf55123e5b),&DATARMNETcf1d9e2c1e);if(!DATARMNETbf55123e5b)return-EINVAL
;DATARMNET65293f17c4=DATARMNETbf55123e5b->nexthdr;DATARMNET611d08d671=
ipv6_skip_exthdr(DATARMNET543491eb0f,sizeof(*DATARMNETbf55123e5b),&
DATARMNET65293f17c4,&DATARMNET98fed39916);if(DATARMNET611d08d671<
(0xd2d+202-0xdf7)||DATARMNET98fed39916)return-EINVAL;if(DATARMNET65293f17c4==
IPPROTO_UDP){if(DATARMNET21c7a3c377)DATARMNET2a6d02a6a5(DATARMNET543491eb0f);
return DATARMNETee9214ce9b(DATARMNET543491eb0f,DATARMNET611d08d671);}if(
DATARMNET65293f17c4==IPPROTO_TCP){if(DATARMNETc03b668e4c)DATARMNET9250714999(
DATARMNET543491eb0f);return-EINVAL;}}return-EINVAL;}void DATARMNET7b0253229f(
struct sk_buff*DATARMNET543491eb0f){if(DATARMNET543491eb0f->protocol==htons(
ETH_P_IP)){struct iphdr*DATARMNET86f1f2cdc9,DATARMNETbf6548198e;
DATARMNET86f1f2cdc9=skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),
sizeof(*DATARMNET86f1f2cdc9),&DATARMNETbf6548198e);if(!DATARMNET86f1f2cdc9||
ip_is_fragment(DATARMNET86f1f2cdc9))return;if(DATARMNET86f1f2cdc9->protocol==
IPPROTO_TCP){if(DATARMNETc03b668e4c)DATARMNETd8fdde8978(DATARMNET543491eb0f);}}
else if(DATARMNET543491eb0f->protocol==htons(ETH_P_IPV6)){struct ipv6hdr*
DATARMNETbf55123e5b,DATARMNETcf1d9e2c1e;int DATARMNET611d08d671;__be16 
DATARMNET98fed39916;u8 DATARMNET65293f17c4;DATARMNETbf55123e5b=
skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),sizeof(*
DATARMNETbf55123e5b),&DATARMNETcf1d9e2c1e);if(!DATARMNETbf55123e5b)return;
DATARMNET65293f17c4=DATARMNETbf55123e5b->nexthdr;DATARMNET611d08d671=
ipv6_skip_exthdr(DATARMNET543491eb0f,sizeof(*DATARMNETbf55123e5b),&
DATARMNET65293f17c4,&DATARMNET98fed39916);if(DATARMNET611d08d671<
(0xd2d+202-0xdf7)||DATARMNET98fed39916)return;if(DATARMNET65293f17c4==
IPPROTO_TCP){if(DATARMNETc03b668e4c)DATARMNETd8fdde8978(DATARMNET543491eb0f);}}}
static void DATARMNET05c4bf8af9(struct sk_buff*DATARMNET543491eb0f,int 
DATARMNET611d08d671){if(DATARMNET77ab42949f(DATARMNET543491eb0f,
DATARMNET611d08d671))DATARMNET543491eb0f->priority=14286874;}void 
DATARMNET324fe25dad(struct sk_buff*DATARMNET543491eb0f){if(DATARMNET543491eb0f->
protocol==htons(ETH_P_IP)){struct iphdr*DATARMNET86f1f2cdc9,DATARMNETbf6548198e;
DATARMNET86f1f2cdc9=skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),
sizeof(*DATARMNET86f1f2cdc9),&DATARMNETbf6548198e);if(!DATARMNET86f1f2cdc9||
ip_is_fragment(DATARMNET86f1f2cdc9))return;if(DATARMNET86f1f2cdc9->protocol==
IPPROTO_UDP){if(DATARMNET21c7a3c377)DATARMNET5ecc30669c(DATARMNET543491eb0f);
DATARMNET05c4bf8af9(DATARMNET543491eb0f,DATARMNET86f1f2cdc9->ihl*
(0xd11+230-0xdf3));return;}if(DATARMNET86f1f2cdc9->protocol==IPPROTO_TCP){if(
DATARMNETc03b668e4c)DATARMNET9d574e64b1(DATARMNET543491eb0f);return;}}else if(
DATARMNET543491eb0f->protocol==htons(ETH_P_IPV6)){struct ipv6hdr*
DATARMNETbf55123e5b,DATARMNETcf1d9e2c1e;int DATARMNET611d08d671;__be16 
DATARMNET98fed39916;u8 DATARMNET65293f17c4;DATARMNETbf55123e5b=
skb_header_pointer(DATARMNET543491eb0f,(0xd2d+202-0xdf7),sizeof(*
DATARMNETbf55123e5b),&DATARMNETcf1d9e2c1e);if(!DATARMNETbf55123e5b)return;
DATARMNET65293f17c4=DATARMNETbf55123e5b->nexthdr;DATARMNET611d08d671=
ipv6_skip_exthdr(DATARMNET543491eb0f,sizeof(*DATARMNETbf55123e5b),&
DATARMNET65293f17c4,&DATARMNET98fed39916);if(DATARMNET611d08d671<
(0xd2d+202-0xdf7)||DATARMNET98fed39916)return;if(DATARMNET65293f17c4==
IPPROTO_UDP){if(DATARMNET21c7a3c377)DATARMNET5ecc30669c(DATARMNET543491eb0f);
DATARMNET05c4bf8af9(DATARMNET543491eb0f,DATARMNET611d08d671);return;}if(
DATARMNET65293f17c4==IPPROTO_TCP){if(DATARMNETc03b668e4c)DATARMNET9d574e64b1(
DATARMNET543491eb0f);return;}}}static const struct 
rmnet_module_hook_register_info DATARMNET95a71e03c1[]={{.hooknum=
RMNET_MODULE_HOOK_PERF_INGRESS,.func=DATARMNETf2958615b1,},{.hooknum=
RMNET_MODULE_HOOK_PERF_EGRESS,.func=DATARMNET324fe25dad,},{.hooknum=
RMNET_MODULE_HOOK_PERF_SET_THRESH,.func=DATARMNET7fa1835595,},{.hooknum=
RMNET_MODULE_HOOK_PERF_INGRESS_RX_HANDLER,.func=DATARMNET7b0253229f,},};void 
DATARMNET49c17a32bc(void){rcu_assign_pointer(rmnet_perf_egress_hook1,
DATARMNET324fe25dad);rmnet_module_hook_register(DATARMNET95a71e03c1,ARRAY_SIZE(
DATARMNET95a71e03c1));}void DATARMNET41e8cc085c(void){rcu_assign_pointer(
rmnet_perf_egress_hook1,NULL);rmnet_module_hook_unregister(DATARMNET95a71e03c1,
ARRAY_SIZE(DATARMNET95a71e03c1));}static int __init DATARMNET63abbdc3d3(void){
int DATARMNETb14e52a504;pr_info("%s(): Loading\n",__func__);DATARMNETb14e52a504=
DATARMNET7e9995246e();if(DATARMNETb14e52a504)return DATARMNETb14e52a504;
DATARMNETb14e52a504=DATARMNETe80a33d544();if(DATARMNETb14e52a504){
DATARMNET38bb6f2b7a();return DATARMNETb14e52a504;}DATARMNET49c17a32bc();return
(0xd2d+202-0xdf7);}static void __exit DATARMNETa343229e33(void){
DATARMNET41e8cc085c();DATARMNET4b5170a1ef();DATARMNET38bb6f2b7a();pr_info(
"\x25\x73\x28\x29\x3a\x20\x65\x78\x69\x74\x69\x6e\x67" "\n",__func__);}
module_init(DATARMNET63abbdc3d3);module_exit(DATARMNETa343229e33);
