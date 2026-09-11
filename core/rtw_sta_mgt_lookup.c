/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_STA_MGT_LOOKUP_C_

#ifdef HOST_STA_MGT_TEST
#include "host_sta_mgt_types.h"
#else
#include <drv_types.h>
#endif

void rtw_st_ctl_rx(struct sta_info *sta, u8 *ehdr_pos)
{
	_adapter *adapter = sta->padapter;
	struct ethhdr *etherhdr = (struct ethhdr *)ehdr_pos;

	if (ntohs(etherhdr->h_proto) == ETH_P_IP) {
		u8 *ip = ehdr_pos + ETH_HLEN;

		if (GET_IPV4_PROTOCOL(ip) == 0x06  /* TCP */
			&& rtw_st_ctl_chk_reg_s_proto(&sta->st_ctl, 0x06) == _TRUE
		) {
			u8 *tcp = ip + GET_IPV4_IHL(ip) * 4;

			if (rtw_st_ctl_chk_reg_rule(&sta->st_ctl, adapter, IPV4_DST(ip), TCP_DST(tcp), IPV4_SRC(ip), TCP_SRC(tcp)) == _TRUE) {
				if (GET_TCP_SYN(tcp) && GET_TCP_ACK(tcp)) {
					session_tracker_add_cmd(adapter, sta
						, IPV4_DST(ip), TCP_DST(tcp)
						, IPV4_SRC(ip), TCP_SRC(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" SYN-ACK\n"
							, FUNC_ADPT_ARG(adapter)
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp))
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp)));
				}
				if (GET_TCP_FIN(tcp)) {
					session_tracker_del_cmd(adapter, sta
						, IPV4_DST(ip), TCP_DST(tcp)
						, IPV4_SRC(ip), TCP_SRC(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" FIN\n"
							, FUNC_ADPT_ARG(adapter)
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp))
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp)));
				}
			}

		}
	}
}

void _rtw_init_stainfo(struct sta_info *psta)
{
	_rtw_memset((u8 *)psta, 0, sizeof(struct sta_info));

	_rtw_spinlock_init(&psta->lock);
	_rtw_init_listhead(&psta->list);
	_rtw_init_listhead(&psta->hash_list);
	/* _rtw_init_listhead(&psta->asoc_list); */
	/* _rtw_init_listhead(&psta->sleep_list); */
	/* _rtw_init_listhead(&psta->wakeup_list);	 */

	_rtw_init_queue(&psta->sleep_q);
#ifdef CONFIG_RTW_MGMT_QUEUE
	_rtw_init_queue(&psta->mgmt_sleep_q);
#endif
	_rtw_init_sta_xmit_priv(&psta->sta_xmitpriv);
	_rtw_init_sta_recv_priv(&psta->sta_recvpriv);

#ifdef CONFIG_AP_MODE
	_rtw_init_listhead(&psta->asoc_list);
	_rtw_init_listhead(&psta->auth_list);
	psta->bpairwise_key_installed = _FALSE;

#ifdef CONFIG_RTW_80211R
	psta->ft_pairwise_key_installed = _FALSE;
#endif
#endif /* CONFIG_AP_MODE	 */

	rtw_st_ctl_init(&psta->st_ctl);
}

inline struct sta_info *rtw_get_stainfo_by_offset(struct sta_priv *stapriv, int offset)
{
	if (!stainfo_offset_valid(offset))
		RTW_INFO("%s invalid offset(%d), out of range!!!", __func__, offset);

	return (struct sta_info *)(stapriv->pstainfo_buf + offset * sizeof(struct sta_info));
}

struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, const u8 *hwaddr)
{

	_irqL	 irqL;

	_list	*plist, *phead;

	struct sta_info *psta = NULL;

	u32	index;

	const u8 *addr;

	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


	if (hwaddr == NULL)
		return NULL;

	if (IS_MCAST(hwaddr))
		addr = bc_addr;
	else
		addr = hwaddr;

	index = wifi_mac_hash(addr);

	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);

	phead = &(pstapriv->sta_hash[index]);
	plist = get_next(phead);


	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {

		psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

		if ((_rtw_memcmp(psta->cmn.mac_addr, addr, ETH_ALEN)) == _TRUE) {
			/* if found the matched address */
			break;
		}
		psta = NULL;
		plist = get_next(plist);
	}

	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	return psta;

}
