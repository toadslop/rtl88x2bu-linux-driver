/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_EXT_PEER_ALIVE_TYPES_H
#define HOST_MLME_EXT_PEER_ALIVE_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 0
#define _FAIL (-1)
#define TID_NUM 16
#define HT_IOT_PEER_BROADCOM 3
#define RX_AMPDU_SIZE_INVALID 0xFF
#define MAX_CONTINUAL_NORXPACKET_COUNT 4

typedef int ATOMIC_T;
#define ATOMIC_INC_RETURN(v) __sync_add_and_fetch((v), 1)
#define ATOMIC_SET(v, x) (*(v) = (x))

struct cmn_sta_info { u8 mac_addr[6]; };
struct stainfo_stats {
	u64 rx_mgnt_pkts, last_rx_mgnt_pkts, rx_beacon_pkts, last_rx_beacon_pkts;
	u64 rx_probereq_pkts, last_rx_probereq_pkts, rx_probersp_pkts, last_rx_probersp_pkts;
	u64 rx_probersp_bm_pkts, last_rx_probersp_bm_pkts;
	u64 rx_probersp_uo_pkts, last_rx_probersp_uo_pkts;
	u64 rx_ctrl_pkts, last_rx_ctrl_pkts, rx_data_pkts, last_rx_data_pkts;
	u64 rx_data_bc_pkts, last_rx_data_bc_pkts, rx_data_mc_pkts, last_rx_data_mc_pkts;
	u64 rx_data_qos_pkts[TID_NUM], last_rx_data_qos_pkts[TID_NUM];
#ifdef CONFIG_TDLS
	u64 rx_tdls_disc_rsp_pkts, last_rx_tdls_disc_rsp_pkts;
#endif
};
struct recv_reorder_ctrl { u8 enable, ampdu_size; };
struct sta_info {
	struct cmn_sta_info cmn;
	struct stainfo_stats sta_stats;
	struct recv_reorder_ctrl recvreorder_ctrl[TID_NUM];
	ATOMIC_T continual_no_rx_packet[TID_NUM];
};
struct mlme_ext_info { u8 assoc_AP_vendor; };
struct mlme_ext_priv { struct mlme_ext_info mlmext_info; };
struct _adapter { struct mlme_ext_priv mlmeextpriv; };
typedef struct _adapter _adapter;

#define sta_rx_data_pkts(s) ((s)->sta_stats.rx_data_pkts)
#define sta_last_rx_data_pkts(s) ((s)->sta_stats.last_rx_data_pkts)
#define sta_rx_data_qos_pkts(s, i) ((s)->sta_stats.rx_data_qos_pkts[i])
#define sta_last_rx_data_qos_pkts(s, i) ((s)->sta_stats.last_rx_data_qos_pkts[i])
#define sta_rx_beacon_pkts(s) ((s)->sta_stats.rx_beacon_pkts)
#define sta_last_rx_beacon_pkts(s) ((s)->sta_stats.last_rx_beacon_pkts)
#define sta_rx_probersp_pkts(s) ((s)->sta_stats.rx_probersp_pkts)
#define sta_last_rx_probersp_pkts(s) ((s)->sta_stats.last_rx_probersp_pkts)
#define sta_update_last_rx_pkts(sta) do { \
	int _i; struct stainfo_stats *_s = &(sta)->sta_stats; \
	_s->last_rx_mgnt_pkts = _s->rx_mgnt_pkts; \
	_s->last_rx_beacon_pkts = _s->rx_beacon_pkts; \
	_s->last_rx_probereq_pkts = _s->rx_probereq_pkts; \
	_s->last_rx_probersp_pkts = _s->rx_probersp_pkts; \
	_s->last_rx_probersp_bm_pkts = _s->rx_probersp_bm_pkts; \
	_s->last_rx_probersp_uo_pkts = _s->rx_probersp_uo_pkts; \
	_s->last_rx_ctrl_pkts = _s->rx_ctrl_pkts; \
	_s->last_rx_data_pkts = _s->rx_data_pkts; \
	_s->last_rx_data_bc_pkts = _s->rx_data_bc_pkts; \
	_s->last_rx_data_mc_pkts = _s->rx_data_mc_pkts; \
	for (_i = 0; _i < TID_NUM; _i++) \
		_s->last_rx_data_qos_pkts[_i] = _s->rx_data_qos_pkts[_i]; \
} while (0)

struct host_delba_record { u8 called, tid; };
extern struct host_delba_record host_last_delba, host_last_delba_ex;

void rtw_delba_check(_adapter *padapter, struct sta_info *psta, u8 from_timer);
u8 chk_ap_is_alive(_adapter *padapter, struct sta_info *psta);
u8 chk_adhoc_peer_is_alive(struct sta_info *psta);
#ifdef CONFIG_TDLS
u8 chk_tdls_peer_sta_is_alive(_adapter *padapter, struct sta_info *psta);
#endif

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void issue_del_ba(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason,
		  u8 initiator);
int issue_del_ba_ex(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason,
		    u8 initiator, int try_cnt, int wait_ms);

#endif
