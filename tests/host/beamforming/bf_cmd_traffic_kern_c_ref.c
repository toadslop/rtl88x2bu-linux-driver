// SPDX-License-Identifier: GPL-2.0
/* L1 C reference for W3-191 kernel symbol exports (host oracle wrappers). */

#include "host_beamforming_bf_cmd_traffic_oracle.h"

u8 rtw_bf_cmd(bf_host_cmd_traffic_padpt adapter, s32 type, u8 *pbuf, s32 size, u8 enqueue)
{
	return o_bf_cmd(adapter, type, pbuf, size, enqueue);
}

void rtw_bf_update_attrib(bf_host_cmd_traffic_padpt adapter, struct bf_host_pkt_attrib *attrib,
			  struct bf_host_sta *sta)
{
	o_bf_update_attrib(adapter, attrib, sta);
}

void rtw_bf_c2h_handler(bf_host_cmd_traffic_padpt adapter, u8 id, u8 *buf, u8 buf_len)
{
	o_bf_c2h_handler(adapter, id, buf, buf_len);
}

void rtw_bf_update_traffic(bf_host_cmd_traffic_padpt adapter)
{
	o_bf_update_traffic(adapter);
}
