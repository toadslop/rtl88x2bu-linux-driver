// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-79 sta priv / mfree / bcmc L2 oracle. */

#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"

void *rtw_zvmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_vmfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

struct macid_ctl_t *adapter_to_macidctl(_adapter *adapter)
{
	return &adapter->macid_ctl;
}

void rtw_macaddr_acl_init(_adapter *adapter, int index)
{
	(void)adapter;
	(void)index;
}

void rtw_macaddr_acl_deinit(_adapter *adapter, int index)
{
	(void)adapter;
	(void)index;
}

void rtw_pre_link_sta_ctl_init(struct sta_priv *stapriv)
{
	(void)stapriv;
}

void rtw_pre_link_sta_ctl_deinit(struct sta_priv *stapriv)
{
	(void)stapriv;
}

void rtw_set_rx_chk_limit(_adapter *adapter, int limit)
{
	(void)adapter;
	(void)limit;
}

void _cancel_timer_ex(void *timer)
{
	(void)timer;
}

void host_sta_mgt_free_reset(_adapter *adapter)
{
	memset(&adapter->stapriv, 0, sizeof(adapter->stapriv));
	memset(&adapter->macid_ctl, 0, sizeof(adapter->macid_ctl));
	adapter->macid_ctl.num = NUM_STA;
	adapter->stapriv.padapter = adapter;
}

int host_sta_mgt_free_setup(_adapter *adapter)
{
	host_sta_mgt_free_reset(adapter);
	return 0;
}
