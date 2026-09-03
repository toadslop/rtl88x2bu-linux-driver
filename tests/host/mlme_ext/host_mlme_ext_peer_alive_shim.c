// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_peer_alive_types.h"

struct host_delba_record host_last_delba, host_last_delba_ex;

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{
	return ATOMIC_INC_RETURN(&sta->continual_no_rx_packet[tid_index]) >=
		       MAX_CONTINUAL_NORXPACKET_COUNT ?
	       _TRUE :
	       _FALSE;
}

void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{
	ATOMIC_SET(&sta->continual_no_rx_packet[tid_index], 0);
}

void issue_del_ba(_adapter *a, unsigned char *ra, u8 tid, u16 reason, u8 ini)
{
	(void)a; (void)ra; (void)reason; (void)ini;
	memset(&host_last_delba, 0, sizeof(host_last_delba));
	host_last_delba.called = 1;
	host_last_delba.tid = tid;
}

int issue_del_ba_ex(_adapter *a, unsigned char *ra, u8 tid, u16 reason,
		    u8 ini, int tc, int wm)
{
	(void)a; (void)ra; (void)reason; (void)ini; (void)tc; (void)wm;
	memset(&host_last_delba_ex, 0, sizeof(host_last_delba_ex));
	host_last_delba_ex.called = 1;
	host_last_delba_ex.tid = tid;
	return _SUCCESS;
}
