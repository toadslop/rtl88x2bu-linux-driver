// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-177 process_pwrbit_data / process_wmmps_data (core/rtw_recv.c). */

#include "host_recv_ap_ps_types.h"

#define _PWRMGT_ BIT(12)

static u16 le16_to_cpu_host(u16 v)
{
	return v;
}

#define GetPwrMgt(pbuf) \
	(((*(u16 *)(pbuf)) & le16_to_cpu_host(_PWRMGT_)) != 0)

void process_pwrbit_data(_adapter *padapter, union recv_frame_host *precv_frame,
			 struct sta_info_host *psta)
{
#ifdef CONFIG_AP_MODE
	unsigned char pwrbit;
	u8 *ptr = precv_frame->hdr.rx_data;

	pwrbit = GetPwrMgt(ptr);

	if (pwrbit) {
		if (!(psta->state & WIFI_SLEEP_STATE))
			stop_sta_xmit(padapter, psta);
	} else {
		if (psta->state & WIFI_SLEEP_STATE)
			wakeup_sta_to_xmit(padapter, psta);
	}
#endif
}

void process_wmmps_data(_adapter *padapter, union recv_frame_host *precv_frame,
			struct sta_info_host *psta)
{
#ifdef CONFIG_AP_MODE
	struct rx_pkt_attrib_host *pattrib = &precv_frame->hdr.attrib;

	if (!psta->qos_option)
		return;

	if (!(psta->qos_info & 0xf))
		return;

	if (psta->state & WIFI_SLEEP_STATE) {
		u8 wmmps_ac = 0;

		switch (pattrib->priority) {
		case 1:
		case 2:
			wmmps_ac = psta->uapsd_bk & BIT(1);
			break;
		case 4:
		case 5:
			wmmps_ac = psta->uapsd_vi & BIT(1);
			break;
		case 6:
		case 7:
			wmmps_ac = psta->uapsd_vo & BIT(1);
			break;
		case 0:
		case 3:
		default:
			wmmps_ac = psta->uapsd_be & BIT(1);
			break;
		}

		if (wmmps_ac) {
			if (psta->sleepq_ac_len > 0)
				xmit_delivery_enabled_frames(padapter, psta);
			else
				issue_qos_nulldata(padapter, psta->cmn.mac_addr,
						   (u16)pattrib->priority, 0, 0, 0);
		}
	}
#endif
}
