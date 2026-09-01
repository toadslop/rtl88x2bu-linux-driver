// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ht_restructure_types.h"

_adapter *host_mlme_ht_restructure_adapter;

bool hal_chk_bw_cap(_adapter *adapter, u8 cap)
{
	(void)cap;
	return adapter->host_fixture.hal_bw_cap_40 != 0;
}

void rtw_hal_get_def_var(_adapter *padapter, hal_def_var_e def_var, void *val)
{
	switch (def_var) {
	case HAL_DEF_RX_PACKET_OFFSET:
		*(u32 *)val = padapter->host_fixture.rx_packet_offset;
		break;
	case HAL_DEF_MAX_RECVBUF_SZ:
		*(u32 *)val = padapter->host_fixture.max_recvbuf_sz;
		break;
	case HAL_DEF_RX_STBC:
		*(u8 *)val = padapter->host_fixture.rx_stbc_nss;
		break;
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*(u8 *)val = padapter->host_fixture.max_rx_ampdu_factor;
		break;
	case HW_VAR_BEST_AMPDU_DENSITY:
		*(u8 *)val = padapter->host_fixture.best_ampdu_density;
		break;
	default:
		break;
	}
}

void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask)
{
	mcs_set[0] &= (u8)(mask & 0xff);
	mcs_set[1] &= (u8)((mask >> 8) & 0xff);
	mcs_set[2] &= (u8)((mask >> 16) & 0xff);
	mcs_set[3] &= (u8)((mask >> 24) & 0xff);
}

u8 *rtw_get_ie(const u8 *pbuf, sint index, uint *len, sint limit)
{
	sint i = 0;
	const u8 *p = pbuf;

	if (limit < 1)
		return NULL;
	*len = 0;
	while (1) {
		sint tmp;

		if (*p == (u8)index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += tmp + 2;
		i += tmp + 2;
		if (i >= limit)
			break;
	}
	return NULL;
}

u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen)
{
	*pbuf = (u8)index;
	*(pbuf + 1) = (u8)len;
	if (len)
		_rtw_memcpy(pbuf + 2, source, len);
	if (frlen)
		*frlen += len + 2;
	return pbuf + len + 2;
}

bool rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			     u8 a, u8 b)
{
	(void)ch_set;
	(void)ch;
	(void)offset;
	(void)a;
	(void)b;
	if (!host_mlme_ht_restructure_adapter)
		return false;
	if (bw == CHANNEL_WIDTH_40)
		return host_mlme_ht_restructure_adapter->host_fixture.chset_allow_40 != 0;
	return true;
}

bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	(void)ch_set;
	(void)ch;
	(void)offset;
	if (!host_mlme_ht_restructure_adapter)
		return false;
	return host_mlme_ht_restructure_adapter->host_fixture.chbw_non_ocp_40 != 0 &&
	       bw == CHANNEL_WIDTH_40;
}

u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl)
{
	(void)rfctl;
	if (!host_mlme_ht_restructure_adapter)
		return _TRUE;
	return host_mlme_ht_restructure_adapter->host_fixture.dfs_domain_unknown;
}

/* Rust host oracle accessors */
RT_CHANNEL_INFO *rtw_rust_ht_channel_set(_adapter *padapter)
{
	return adapter_to_rfctl(padapter)->channel_set;
}

u8 *rtw_rust_ht_rfctl(_adapter *padapter)
{
	return (u8 *)adapter_to_rfctl(padapter);
}

u8 rtw_rust_ht_is_dfs_slave_with_rd(u8 *rfctl)
{
	return IS_DFS_SLAVE_WITH_RD((struct rf_ctl_t *)rfctl);
}

u8 rtw_rust_ht_rfctl_dfs_domain_unknown(u8 *rfctl)
{
	return rtw_rfctl_dfs_domain_unknown((struct rf_ctl_t *)rfctl);
}

u8 rtw_rust_ht_regsty_bw_2g(_adapter *padapter)
{
	return REGSTY_BW_2G(&padapter->registrypriv);
}

u8 rtw_rust_ht_regsty_bw_5g(_adapter *padapter)
{
	return REGSTY_BW_5G(&padapter->registrypriv);
}

u8 *rtw_rust_ht_ht_option(_adapter *padapter)
{
	return &padapter->mlmepriv.htpriv.ht_option;
}

u8 *rtw_rust_ht_get_ie(const u8 *pbuf, sint index, u32 *len, sint limit)
{
	return rtw_get_ie(pbuf, index, len, limit);
}

u8 rtw_rust_ht_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				   u8 offset, u8 a, u8 b)
{
	return rtw_chset_is_chbw_valid(ch_set, ch, bw, offset, a, b) ? 1 : 0;
}

u8 rtw_rust_ht_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				     u8 offset)
{
	return rtw_chset_is_chbw_non_ocp(ch_set, ch, bw, offset) ? 1 : 0;
}

void rtw_rust_ht_warn_on(int condition)
{
	rtw_warn_on(condition);
}

u8 rtw_rust_ht_hal_chk_bw_cap(_adapter *padapter, u8 cap)
{
	return hal_chk_bw_cap(padapter, cap) ? 1 : 0;
}

void rtw_rust_ht_hal_get_def_var(_adapter *padapter, u8 def_var, void *val)
{
	rtw_hal_get_def_var(padapter, (hal_def_var_e)def_var, val);
}

void rtw_rust_ht_set_mcs_rate_by_mask(u8 *mcs_set, u32 mask)
{
	set_mcs_rate_by_mask(mcs_set, mask);
}

u32 rtw_rust_ht_fw_state(_adapter *padapter)
{
	return padapter->mlmepriv.fw_state;
}

u8 rtw_rust_ht_cur_bwmode(_adapter *padapter)
{
	return padapter->mlmeextpriv.cur_bwmode;
}

u8 *rtw_rust_ht_default_mcs(_adapter *padapter)
{
	return padapter->mlmeextpriv.default_supported_mcs_set;
}

u8 rtw_rust_ht_sgi_20m(_adapter *padapter)
{
	return padapter->mlmepriv.htpriv.sgi_20m;
}

u8 rtw_rust_ht_sgi_40m(_adapter *padapter)
{
	return padapter->mlmepriv.htpriv.sgi_40m;
}

u8 rtw_rust_ht_ldpc_cap(_adapter *padapter)
{
	return padapter->mlmepriv.htpriv.ldpc_cap;
}

u8 rtw_rust_ht_stbc_cap(_adapter *padapter)
{
	return padapter->mlmepriv.htpriv.stbc_cap;
}

u8 rtw_rust_ht_rx_stbc(_adapter *padapter)
{
	return padapter->registrypriv.rx_stbc;
}

u8 rtw_rust_ht_wifi_spec(_adapter *padapter)
{
	return padapter->registrypriv.wifi_spec;
}

u8 rtw_rust_ht_rx_nss(_adapter *padapter)
{
	return padapter->host_fixture.rx_nss;
}

u8 rtw_rust_ht_driver_rx_ampdu_factor(_adapter *padapter)
{
	return padapter->driver_rx_ampdu_factor;
}

u8 rtw_rust_ht_driver_rx_ampdu_spacing(_adapter *padapter)
{
	return padapter->driver_rx_ampdu_spacing;
}

u32 rtw_rust_ht_dot11_privacy(_adapter *padapter)
{
	return padapter->securitypriv.dot11PrivacyAlgrthm;
}

u8 *rtw_rust_ht_set_ie(u8 *pbuf, sint index, u32 len, u8 *source, u32 *frlen)
{
	return rtw_set_ie(pbuf, index, len, source, frlen);
}
