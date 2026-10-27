/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_LPS_TP_H
#define HOST_PWRCTRL_LPS_TP_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define _TRUE 1
#define _FALSE 0

#define PS_MODE_ACTIVE 0
#define LPS_CTRL_TX_TRAFFIC_LEAVE 7
#define LPS_CTRL_RX_TRAFFIC_LEAVE 8

struct stainfo_stats {
	u64 tx_bytes;
	u64 acc_tx_bytes;
	u64 rx_bytes;
	u64 acc_rx_bytes;
};

struct sta_info {
	struct stainfo_stats sta_stats;
};

struct rt_link_detect {
	u32 NumTxOkInPeriod;
	u32 NumRxUnicastOkInPeriod;
};

struct pwrctrl_priv {
	u8 bLeisurePs;
	u8 pwr_mode;
	u16 lps_tx_tp_th;
	u16 lps_rx_tp_th;
	int lps_chk_cnt_th;
	int lps_chk_cnt;
	u32 lps_tx_pkts;
	u32 lps_rx_pkts;
};

struct mlme_priv {
	struct rt_link_detect LinkDetectInfo;
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
	struct mlme_priv mlmepriv;
};

typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

void traffic_check_for_leave_lps_by_tp(PADAPTER padapter, u8 tx, struct sta_info *sta);

struct host_pwrctrl_lps_tp_trace {
	u32 lps_ctrl_calls;
	u8 last_lps_ctrl_type;
	u8 bt_control_lps;
};

void host_pwrctrl_lps_tp_reset_trace(void);
struct host_pwrctrl_lps_tp_trace *host_pwrctrl_lps_tp_get_trace(void);
void host_pwrctrl_lps_tp_set_bt_control_lps(u8 v);

#endif /* HOST_PWRCTRL_LPS_TP_H */
