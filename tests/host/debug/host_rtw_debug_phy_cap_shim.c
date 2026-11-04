// SPDX-License-Identifier: GPL-2.0
/* Host L2 + kernel FFI for rust/rtw_debug_phy_cap.rs (W3-210). */

#include <stddef.h>
#include <stdio.h>

#include "host_rtw_debug_phy_cap_types.h"

#ifdef HOST_RTW_DEBUG_PHY_CAP_TEST
void rtw_ht_use_default_setting(_adapter *padapter)
{
	(void)padapter;
}

void rtw_vht_use_default_setting(_adapter *padapter)
{
	(void)padapter;
}
#endif

struct rtw_rust_phy_cap_view {
	u8 ht_stbc;
	u8 ht_ldpc;
	u8 ht_bf;
	u8 vht_stbc;
	u8 vht_ldpc;
	u16 vht_bf;
	u8 reg_stbc;
	u8 reg_ldpc;
	u8 reg_bf;
	u8 reg_bfer_rf;
	u8 reg_bfee_rf;
};

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	if (!sel || !line)
		return;
	fputs(line, (FILE *)sel);
}

void rtw_rust_debug_ht_use_default_setting(void *adapter)
{
	if (adapter)
		rtw_ht_use_default_setting((_adapter *)adapter);
}

void rtw_rust_debug_vht_use_default_setting(void *adapter)
{
	if (adapter)
		rtw_vht_use_default_setting((_adapter *)adapter);
}

void rtw_rust_debug_phy_cap_fill(void *adapter, struct rtw_rust_phy_cap_view *out)
{
	_adapter *a;

	if (!adapter || !out)
		return;
	a = (_adapter *)adapter;
	out->ht_stbc = a->mlmepriv.htpriv.stbc_cap;
	out->ht_ldpc = a->mlmepriv.htpriv.ldpc_cap;
	out->ht_bf = a->mlmepriv.htpriv.beamform_cap;
	out->vht_stbc = a->mlmepriv.vhtpriv.stbc_cap;
	out->vht_ldpc = a->mlmepriv.vhtpriv.ldpc_cap;
	out->vht_bf = a->mlmepriv.vhtpriv.beamform_cap;
	out->reg_stbc = a->registrypriv.stbc_cap;
	out->reg_ldpc = a->registrypriv.ldpc_cap;
	out->reg_bf = a->registrypriv.beamform_cap;
	out->reg_bfer_rf = a->registrypriv.beamformer_rf_num;
	out->reg_bfee_rf = a->registrypriv.beamformee_rf_num;
}
