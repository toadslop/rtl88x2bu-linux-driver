// SPDX-License-Identifier: GPL-2.0
/* Kernel FFI for rust/rtw_recv_ap_ps.rs (W3-177). */

#include <drv_types.h>

#if defined(CONFIG_RUST)

u8 *rtw_rust_recv_ap_ps_rx_data(union recv_frame *frame)
{
#ifdef CONFIG_AP_MODE
	return frame->u.hdr.rx_data;
#else
	(void)frame;
	return NULL;
#endif
}

u8 rtw_rust_recv_ap_ps_priority(union recv_frame *frame)
{
#ifdef CONFIG_AP_MODE
	return frame->u.hdr.attrib.priority;
#else
	(void)frame;
	return 0;
#endif
}

u8 rtw_rust_recv_ap_ps_uapsd_bk(struct sta_info *sta)
{
	return sta->uapsd_bk;
}

u8 rtw_rust_recv_ap_ps_uapsd_be(struct sta_info *sta)
{
	return sta->uapsd_be;
}

u8 rtw_rust_recv_ap_ps_uapsd_vi(struct sta_info *sta)
{
	return sta->uapsd_vi;
}

u8 rtw_rust_recv_ap_ps_uapsd_vo(struct sta_info *sta)
{
	return sta->uapsd_vo;
}

u32 rtw_rust_recv_ap_ps_sleepq_ac_len(struct sta_info *sta)
{
	return sta->sleepq_ac_len;
}

u8 *rtw_rust_recv_ap_ps_mac_addr(struct sta_info *sta)
{
	return sta->cmn.mac_addr;
}

u8 rtw_rust_recv_ap_ps_wmmps_qos_ok(struct sta_info *sta)
{
#ifdef CONFIG_TDLS
	if (sta->tdls_sta_state & TDLS_LINKED_STATE)
		return 1;
#endif
	if (!sta->qos_option)
		return 0;
	if (!(sta->qos_info & 0xf))
		return 0;
	return 1;
}

#endif /* CONFIG_RUST */
