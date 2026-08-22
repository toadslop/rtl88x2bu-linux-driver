/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_REST_TYPES_H
#define HOST_AP_REST_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"
#include <stdbool.h>

#ifndef CONFIG_LIMITED_AP_NUM
#define CONFIG_LIMITED_AP_NUM 4
#endif
#ifndef CONFIG_FW_HANDLE_TXBCN
#define CONFIG_FW_HANDLE_TXBCN 1
#endif

#define _SUCCESS 1
#define _FAIL 0
#define BIT(x) (1U << (x))
#define BIT0 BIT(0)
#define WLAN_EID_TIM 5
#define RTW_ERR(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

typedef unsigned char u8;

struct dvobj_priv {
	u8 vap_map;
};

static inline bool rtw_bmp_is_set(const u8 *bmp, u8 bmp_len, u8 id)
{
	return (id / 8 < bmp_len) && (bmp[id / 8] & BIT(id % 8));
}

static inline bool rtw_bmp_not_empty(const u8 *bmp, u8 bmp_len)
{
	u8 i;

	for (i = 0; i < bmp_len; i++)
		if (bmp[i])
			return 1;
	return 0;
}

u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period, const u8 *tim_bmp, u8 tim_bmp_len,
		  u8 *tim_ie);
u8 rtw_ap_allocate_vapid(struct dvobj_priv *dvobj);
u8 rtw_ap_release_vapid(struct dvobj_priv *dvobj, u8 vap_id);

#endif /* HOST_AP_REST_TYPES_H */
