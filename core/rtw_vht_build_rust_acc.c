// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_vht_build_kern.rs (W3-84 PR8). */
#include <drv_types.h>

#if defined(CONFIG_RUST_VHT_BUILD) && !defined(HOST_VHT_BUILD_TEST)

u8 rtw_rust_vht_build_regsty_bw5g(_adapter *adapter)
{
	return REGSTY_BW_5G(&adapter->registrypriv);
}

u8 rtw_rust_vht_build_regsty_is_bw5g_support(_adapter *adapter, u8 bw)
{
	return REGSTY_IS_BW_5G_SUPPORT(&adapter->registrypriv, bw) ? 1 : 0;
}

u8 rtw_rust_vht_build_regsty_ampdu_factor(_adapter *adapter)
{
	return adapter->registrypriv.ampdu_factor;
}

u8 *rtw_rust_vht_build_vht_cap(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.vht_cap;
}

u8 *rtw_rust_vht_build_vht_mcs_map(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.vht_mcs_map;
}

u8 rtw_rust_vht_build_ldpc_cap(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.ldpc_cap;
}

u8 rtw_rust_vht_build_stbc_cap(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.stbc_cap;
}

u8 rtw_rust_vht_build_sgi_80m(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.sgi_80m;
}

u8 rtw_rust_vht_build_vht_highest_rate(_adapter *adapter)
{
	return adapter->mlmepriv.vhtpriv.vht_highest_rate;
}

#endif /* CONFIG_RUST_VHT_BUILD && !HOST_VHT_BUILD_TEST */
