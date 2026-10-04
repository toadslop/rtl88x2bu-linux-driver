// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include "host_rm_parse_types.h"

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *p, size_t sz)
{
	(void)sz;
	free(p);
}

u8 host_rm_test_rcpi;
u8 host_rm_test_rsni;

int rm_en_cap_chk_and_set(struct rm_obj *prm, enum rm_cap_en en)
{
	(void)prm;
	(void)en;
	return _SUCCESS;
}

u8 rm_get_bcn_rcpi(struct rm_obj *prm, struct wlan_network *pnetwork)
{
	(void)prm;
	(void)pnetwork;
	return host_rm_test_rcpi;
}

u8 rm_get_bcn_rsni(struct rm_obj *prm, struct wlan_network *pnetwork)
{
	(void)prm;
	(void)pnetwork;
	return host_rm_test_rsni;
}
