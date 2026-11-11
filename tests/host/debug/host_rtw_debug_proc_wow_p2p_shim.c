// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-236 L2 — emit logic mirrors debug_proc_wow_p2p_c_oracle.c */

#include "host_rtw_debug_proc_wow_p2p_types.h"

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void rtw_rust_debug_proc_emit_p2p_wowlan_info(void *sel, void *padapter)
{
	struct _adapter *adapter = padapter;
	struct wifidirect_info *pwdinfo;
	struct p2p_wowlan_info peerinfo;

	if (!adapter)
		return;

	pwdinfo = &adapter->wdinfo;
	peerinfo = pwdinfo->p2p_wow_info;

	if (_TRUE == peerinfo.is_trigger) {
		RTW_PRINT_SEL(sel, "is_trigger: TRUE\n");
		switch (peerinfo.wowlan_recv_frame_type) {
		case P2P_WOWLAN_RECV_NEGO_REQ:
			RTW_PRINT_SEL(sel, "Frame Type: Nego Request\n");
			break;
		case P2P_WOWLAN_RECV_INVITE_REQ:
			RTW_PRINT_SEL(sel, "Frame Type: Invitation Request\n");
			break;
		case P2P_WOWLAN_RECV_PROVISION_REQ:
			RTW_PRINT_SEL(sel, "Frame Type: Provision Request\n");
			break;
		default:
			break;
		}
		RTW_PRINT_SEL(sel, "Peer Addr: " MAC_FMT "\n",
			      MAC_ARG(peerinfo.wowlan_peer_addr));
		RTW_PRINT_SEL(sel, "Peer WPS Config: %x\n",
			      peerinfo.wowlan_peer_wpsconfig);
		RTW_PRINT_SEL(sel, "Persistent Group: %d\n",
			      peerinfo.wowlan_peer_is_persistent);
		RTW_PRINT_SEL(sel, "Intivation Type: %d\n",
			      peerinfo.wowlan_peer_invitation_type);
	} else {
		RTW_PRINT_SEL(sel, "is_trigger: False\n");
	}
}
