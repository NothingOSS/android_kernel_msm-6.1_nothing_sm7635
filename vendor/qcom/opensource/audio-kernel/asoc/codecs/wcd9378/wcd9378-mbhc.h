/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#ifndef __WCD9378_MBHC_H__
#define __WCD9378_MBHC_H__
#include <asoc/wcd-mbhc-v2.h>

struct wcd9378_mbhc {
	struct wcd_mbhc wcd_mbhc;
	struct blocking_notifier_head notifier;
	struct fw_info *fw_data;
};

#if IS_ENABLED(CONFIG_SND_SOC_WCD9378)
extern int wcd9378_mbhc_init(struct wcd9378_mbhc **mbhc,
			   struct snd_soc_component *component);
extern void wcd9378_mbhc_hs_detect_exit(struct snd_soc_component *component);
extern int wcd9378_mbhc_hs_detect(struct snd_soc_component *component,
				struct wcd_mbhc_config *mbhc_cfg);
extern void wcd9378_mbhc_deinit(struct snd_soc_component *component);
extern void wcd9378_mbhc_ssr_down(struct wcd9378_mbhc *mbhc,
				struct snd_soc_component *component);
extern int wcd9378_mbhc_post_ssr_init(struct wcd9378_mbhc *mbhc,
				    struct snd_soc_component *component);
extern int wcd9378_mbhc_get_impedance(struct wcd9378_mbhc *wcd9378_mbhc,
				    uint32_t *zl, uint32_t *zr);
#else
static inline int wcd9378_mbhc_init(struct wcd9378_mbhc **mbhc,
				  struct snd_soc_component *component)
{
	return 0;
}
static inline void wcd9378_mbhc_hs_detect_exit(
					struct snd_soc_component *component)
{
}
static inline int wcd9378_mbhc_hs_detect(struct snd_soc_component *component,
				       struct wcd_mbhc_config *mbhc_cfg)
{
		return 0;
}
static inline void wcd9378_mbhc_deinit(struct snd_soc_component *component)
{
}
static inline void wcd9378_mbhc_ssr_down(struct wcd9378_mbhc *mbhc,
					struct snd_soc_component *component)
{
}
static inline int wcd9378_mbhc_post_ssr_init(struct wcd9378_mbhc *mbhc,
					   struct snd_soc_component *component)
{
	return 0;
}

static inline int wcd9378_mbhc_get_impedance(struct wcd9378_mbhc *wcd9378_mbhc,
					   uint32_t *zl, uint32_t *zr)
{
	if (zl)
		*zl = 0;
	if (zr)
		*zr = 0;
	return -EINVAL;
}
#endif

#endif /* __WCD9378_MBHC_H__ */
