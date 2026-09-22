// SPDX-License-Identifier: GPL-2.0
#include "host_ft_types.h"

u8 host_ft_reassoc_called;
u8 host_ft_last_reassoc_mac[ETH_ALEN];
static u8 host_auth_rsp_storage[512];

void host_ft_test_adapter_init(_adapter *a)
{
	_rtw_memset(a, 0, sizeof(*a));
	a->mlmepriv.auth_rsp = host_auth_rsp_storage;
	a->mlmepriv.auth_rsp_len = 0;
	host_ft_reassoc_called = 0;
	_rtw_memset(host_ft_last_reassoc_mac, 0, sizeof(host_ft_last_reassoc_mac));
}

void rtw_buf_update(u8 **pbuf, u32 *size, u8 *src, u32 len)
{
	if (!pbuf || !*pbuf || !size || !src || len > 512)
		return;
	_rtw_memcpy(*pbuf, src, len);
	*size = len;
}

void rtw_ft_report_reassoc_evt(_adapter *padapter, u8 *pMacAddr)
{
	(void)padapter;
	host_ft_reassoc_called = 1;
	if (pMacAddr)
		_rtw_memcpy(host_ft_last_reassoc_mac, pMacAddr, ETH_ALEN);
}
