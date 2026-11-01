/* SPDX-License-Identifier: GPL-2.0 */
/* Host fixtures for W3-198 dump_mi_status L2. */
#ifndef HOST_MI_DUMP_TYPES_H
#define HOST_MI_DUMP_TYPES_H

#include "host_mi_status_types.h"

struct mi_dump_dvobj {
	u8 iface_nums;
	struct mi_state iface_state;
	struct _adapter *padapters[4];
	u8 cfg80211_mgmt_tx[4];
	u8 cfg80211_roch[4];
	u8 union_ch;
	u8 union_bw;
	u8 union_offset;
};

#define DEV_STA_NUM(d) MSTATE_STA_NUM(&(d)->iface_state)
#define DEV_STA_LG_NUM(d) MSTATE_STA_LG_NUM(&(d)->iface_state)
#define DEV_STA_LD_NUM(d) MSTATE_STA_LD_NUM(&(d)->iface_state)
#define DEV_TDLS_LD_NUM(d) MSTATE_TDLS_LD_NUM(&(d)->iface_state)
#define DEV_AP_NUM(d) MSTATE_AP_NUM(&(d)->iface_state)
#define DEV_AP_STARTING_NUM(d) MSTATE_AP_STARTING_NUM(&(d)->iface_state)
#define DEV_AP_LD_NUM(d) MSTATE_AP_LD_NUM(&(d)->iface_state)
#define DEV_ADHOC_NUM(d) MSTATE_ADHOC_NUM(&(d)->iface_state)
#define DEV_ADHOC_LD_NUM(d) MSTATE_ADHOC_LD_NUM(&(d)->iface_state)
#define DEV_MESH_NUM(d) MSTATE_MESH_NUM(&(d)->iface_state)
#define DEV_MESH_LD_NUM(d) MSTATE_MESH_LD_NUM(&(d)->iface_state)
#define DEV_P2P_DV_NUM(d) MSTATE_P2P_DV_NUM(&(d)->iface_state)
#define DEV_P2P_GC_NUM(d) MSTATE_P2P_GC_NUM(&(d)->iface_state)
#define DEV_P2P_GO_NUM(d) MSTATE_P2P_GO_NUM(&(d)->iface_state)
#define DEV_SCAN_NUM(d) MSTATE_SCAN_NUM(&(d)->iface_state)
#define DEV_WPS_NUM(d) MSTATE_WPS_NUM(&(d)->iface_state)
#define DEV_ROCH_NUM(d) MSTATE_ROCH_NUM(&(d)->iface_state)
#define DEV_MGMT_TX_NUM(d) MSTATE_MGMT_TX_NUM(&(d)->iface_state)
#define DEV_U_CH(d) ((d)->union_ch)
#define DEV_U_BW(d) ((d)->union_bw)
#define DEV_U_OFFSET(d) ((d)->union_offset)

#endif /* HOST_MI_DUMP_TYPES_H */
