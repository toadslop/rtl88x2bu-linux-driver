// SPDX-License-Identifier: GPL-2.0
#include "host_ap_sta_ie_sec_types.h"

u8 rtw_check_amsdu_disable(u8 mode, u8 spp_opt)
{
	(void)mode;
	(void)spp_opt;
	return _FALSE;
}

u32 security_type_bip_to_gmcs(enum security_type type)
{
	(void)type;
	return 256;
}

u8 *rtw_get_wps_attr_content(u8 *wps_ie, unsigned int wps_ielen,
			     u16 target_attr_id, u8 *buf_content,
			     unsigned int *len_content)
{
	(void)wps_ie;
	(void)wps_ielen;
	(void)target_attr_id;
	(void)buf_content;
	(void)len_content;
	return NULL;
}
