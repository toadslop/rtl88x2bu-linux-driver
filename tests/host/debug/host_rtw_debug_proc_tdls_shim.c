// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-212 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_tdls_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

u8 rtw_is_tdls_enabled(struct _adapter *padapter)
{
	if (!padapter)
		return _FALSE;
	return padapter->registrypriv.en_tdls ? _TRUE : _FALSE;
}

void rtw_enable_tdls_func(struct _adapter *padapter)
{
	if (!padapter || rtw_is_tdls_enabled(padapter) == _TRUE)
		return;
	padapter->registrypriv.en_tdls = _TRUE;
}

void rtw_disable_tdls_func(struct _adapter *padapter, u8 enqueue_cmd)
{
	(void)enqueue_cmd;

	if (!padapter || rtw_is_tdls_enabled(padapter) == _FALSE)
		return;
	padapter->registrypriv.en_tdls = _FALSE;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_proc_emit_tdls_enable(void *sel, void *adapter)
{
	struct registry_priv *pregpriv =
		&((struct _adapter *)adapter)->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "TDLS is %s !\n",
			      (rtw_is_tdls_enabled(adapter) == _TRUE) ? "enabled"
								       : "disabled");
}

long rtw_rust_debug_proc_set_tdls_enable(void *adapter, const void *buffer,
					 size_t count)
{
	struct registry_priv *pregpriv =
		&((struct _adapter *)adapter)->registrypriv;
	char tmp[32];
	u32 en_tdls = 0;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &en_tdls) == 1 && pregpriv) {
			if (en_tdls > 0)
				rtw_enable_tdls_func(adapter);
			else
				rtw_disable_tdls_func(adapter, _TRUE);
		}
	}

	return (long)count;
}

int rtw_rust_debug_proc_tdls_display_function_info(void *m)
{
	struct seq_file *seq = m;
	struct net_device *dev = seq->private;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;

	RTW_PRINT_SEL(m, "============[TDLS Function Info]============\n");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Enable",
		      (rtw_is_tdls_enabled(padapter) == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Driver Setup",
		      (ptdlsinfo->driver_setup == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Prohibited",
		      (ptdlsinfo->ap_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue,
		      "TDLS Channel Switch Prohibited",
		      (ptdlsinfo->ch_switch_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Link Established",
		      (ptdlsinfo->link_established == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %d/%d\n", SpaceBtwnItemAndValue,
		      "TDLS STA Num (Linked/Allowed)", ptdlsinfo->sta_cnt,
		      MAX_ALLOWED_TDLS_STA_NUM);
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue,
		      "TDLS Allowed STA Num Reached",
		      (ptdlsinfo->sta_maximum == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Device Discovered",
		      (ptdlsinfo->dev_discovered == _TRUE) ? "_TRUE" : "_FALSE");

	return 0;
}

int rtw_rust_debug_proc_tdls_display_network_info(void *m)
{
	(void)m;
	return 0;
}

int rtw_rust_debug_proc_tdls_display_tdls_sta_info(void *m)
{
	(void)m;
	return 0;
}
