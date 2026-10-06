// SPDX-License-Identifier: GPL-2.0
/* Host pktfile + tos_to_up for W3-86 PR21 L2 harness. */

#include "host_xmit_update_attrib_main_types.h"

u8 tos_to_up(u8 tos)
{
	return tos >> 5;
}

void _rtw_open_pktfile(_pkt *pkt, struct pkt_file *pf)
{
	pf->cur = pkt->data;
	pf->remain = pkt->len;
	pf->pkt_len = pkt->len;
}

s32 _rtw_pktfile_read(struct pkt_file *pf, u8 *buf, u32 len)
{
	if (len > pf->remain)
		len = pf->remain;
	if (buf && len)
		memcpy(buf, pf->cur, len);
	pf->cur += len;
	pf->remain -= len;
	return (s32)len;
}
