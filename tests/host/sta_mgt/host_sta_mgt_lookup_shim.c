// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-77 stainfo init + hash lookup L2 oracle. */

#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"

static struct sta_info host_lookup_sta_buf[NUM_STA];

void rtw_mfree(u8 *pbuf, u32 sz)
{
	(void)sz;
	free(pbuf);
}

bool test_st_match_rule(_adapter *adapter, u8 *local_naddr, u8 *local_port,
			u8 *remote_naddr, u8 *remote_port)
{
	(void)adapter;
	(void)local_naddr;
	(void)remote_naddr;
	if (ntohs(*((u16 *)local_port)) == 5001 ||
	    ntohs(*((u16 *)remote_port)) == 5001)
		return _TRUE;
	return _FALSE;
}

void _rtw_init_sta_xmit_priv(struct sta_xmit_priv *psta_xmitpriv)
{
	_rtw_spinlock_init(&psta_xmitpriv->lock);
}

void _rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv)
{
	_rtw_spinlock_init(&psta_recvpriv->lock);
}

void host_sta_mgt_lookup_reset(_adapter *adapter)
{
	int i;

	memset(host_lookup_sta_buf, 0, sizeof(host_lookup_sta_buf));
	memset(&adapter->stapriv, 0, sizeof(adapter->stapriv));
	adapter->stapriv.padapter = adapter;
	_rtw_spinlock_init(&adapter->stapriv.sta_hash_lock);
	for (i = 0; i < NUM_STA; i++)
		_rtw_init_listhead(&adapter->stapriv.sta_hash[i]);
}

int host_sta_mgt_lookup_buf_setup(_adapter *adapter)
{
	adapter->stapriv.pstainfo_buf = (u8 *)host_lookup_sta_buf;
	return adapter->stapriv.pstainfo_buf ? 0 : -1;
}

void host_sta_mgt_lookup_hash_insert(_adapter *adapter, u8 sta_index,
				     const u8 *mac)
{
	struct sta_info *sta;
	u32 index;

	if (sta_index >= NUM_STA || !mac)
		return;

	sta = &host_lookup_sta_buf[sta_index];
	_rtw_memcpy(sta->cmn.mac_addr, mac, ETH_ALEN);
	sta->padapter = adapter;
	index = wifi_mac_hash(mac);
	rtw_list_insert_tail(&sta->hash_list,
			       &adapter->stapriv.sta_hash[index]);
}
