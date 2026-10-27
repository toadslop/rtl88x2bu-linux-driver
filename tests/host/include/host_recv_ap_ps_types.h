/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RECV_AP_PS_TYPES_H
#define HOST_RECV_AP_PS_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define ETH_ALEN 6
#define WIFI_SLEEP_STATE 0x00000004u
#define BIT(x) (1U << (x))

struct cmn_sta_info_host {
	u16 dm_ctrl;
	u8 bw_mode;
	u8 mac_id;
	u8 mac_addr[ETH_ALEN];
};

struct sta_info_host {
	u32 state;
	u32 qos_option;
	u8 qos_info;
	u8 uapsd_bk;
	u8 uapsd_be;
	u8 uapsd_vi;
	u8 uapsd_vo;
	unsigned int sleepq_ac_len;
	struct cmn_sta_info_host cmn;
};

struct rx_pkt_attrib_host {
	u8 priority;
};

struct recv_frame_hdr_host {
	struct rx_pkt_attrib_host attrib;
	u8 *rx_data;
};

union recv_frame_host {
	struct recv_frame_hdr_host hdr;
};

struct adapter_host {
	u8 pad;
};

typedef struct adapter_host _adapter;

struct host_recv_ap_ps_trace {
	u32 stop_sta_xmit;
	u32 wakeup_sta_to_xmit;
	u32 xmit_delivery;
	u32 issue_qos_null;
	u16 last_qos_tid;
	u8 last_qos_ps;
};

void host_recv_ap_ps_reset_trace(void);
struct host_recv_ap_ps_trace *host_recv_ap_ps_get_trace(void);

void process_pwrbit_data(_adapter *padapter, union recv_frame_host *precv_frame,
			 struct sta_info_host *psta);
void process_wmmps_data(_adapter *padapter, union recv_frame_host *precv_frame,
			struct sta_info_host *psta);

void stop_sta_xmit(_adapter *padapter, struct sta_info_host *psta);
void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info_host *psta);
void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info_host *psta);
int issue_qos_nulldata(_adapter *padapter, unsigned char *da, u16 tid, u8 ps,
		       int try_cnt, int wait_ms);

#endif /* HOST_RECV_AP_PS_TYPES_H */
