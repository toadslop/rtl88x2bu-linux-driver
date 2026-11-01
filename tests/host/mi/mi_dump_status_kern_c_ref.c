// SPDX-License-Identifier: GPL-2.0
/* L1 reference: dump_mi_status leaf for W3-198 (matches core/rtw_mi.c). */
#include <stddef.h>

typedef unsigned char u8;

struct mi_state {
	u8 sta_num;
	u8 ld_sta_num;
	u8 lg_sta_num;
	u8 ld_tdls_num;
	u8 ap_num;
	u8 starting_ap_num;
	u8 ld_ap_num;
	u8 adhoc_num;
	u8 ld_adhoc_num;
	u8 mesh_num;
	u8 ld_mesh_num;
	u8 scan_num;
	u8 scan_enter_num;
	u8 uwps_num;
	u8 roch_num;
	u8 mgmt_tx_num;
	u8 p2p_device_num;
	u8 p2p_gc;
	u8 p2p_go;
};

struct dvobj_priv {
	u8 iface_nums;
	struct mi_state iface_state;
	void *padapters[4];
	u8 cfg80211_mgmt_tx[4];
	u8 cfg80211_roch[4];
	u8 union_ch;
	u8 union_bw;
	u8 union_offset;
};

struct _adapter {
	u8 iface_id;
	struct dvobj_priv *dvobj;
};

#define MSTATE_STA_NUM(m) ((m)->sta_num)
#define MSTATE_STA_LD_NUM(m) ((m)->ld_sta_num)
#define MSTATE_STA_LG_NUM(m) ((m)->lg_sta_num)
#define MSTATE_TDLS_LD_NUM(m) ((m)->ld_tdls_num)
#define MSTATE_AP_NUM(m) ((m)->ap_num)
#define MSTATE_AP_STARTING_NUM(m) ((m)->starting_ap_num)
#define MSTATE_AP_LD_NUM(m) ((m)->ld_ap_num)
#define MSTATE_ADHOC_NUM(m) ((m)->adhoc_num)
#define MSTATE_ADHOC_LD_NUM(m) ((m)->ld_adhoc_num)
#define MSTATE_MESH_NUM(m) ((m)->mesh_num)
#define MSTATE_MESH_LD_NUM(m) ((m)->ld_mesh_num)
#define MSTATE_P2P_DV_NUM(m) ((m)->p2p_device_num)
#define MSTATE_P2P_GC_NUM(m) ((m)->p2p_gc)
#define MSTATE_P2P_GO_NUM(m) ((m)->p2p_go)
#define MSTATE_SCAN_NUM(m) ((m)->scan_num)
#define MSTATE_WPS_NUM(m) ((m)->uwps_num)
#define MSTATE_ROCH_NUM(m) ((m)->roch_num)
#define MSTATE_MGMT_TX_NUM(m) ((m)->mgmt_tx_num)

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

#define adapter_to_dvobj(a) ((a)->dvobj)

static void host_print_sel(void *sel, const char *fmt, ...)
{
	(void)sel;
	(void)fmt;
}

#define RTW_PRINT_SEL(sel, fmt, ...) host_print_sel(sel, fmt, ##__VA_ARGS__)
#define RTW_INFO(fmt, ...) host_print_sel(NULL, fmt, ##__VA_ARGS__)

void dump_mi_status(void *sel, struct dvobj_priv *dvobj)
{
	RTW_PRINT_SEL(sel, "== dvobj-iface_state ==\n");
	RTW_PRINT_SEL(sel, "sta_num:%d\n", DEV_STA_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linking_sta_num:%d\n", DEV_STA_LG_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_sta_num:%d\n", DEV_STA_LD_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_tdls_num:%d\n", DEV_TDLS_LD_NUM(dvobj));
	RTW_PRINT_SEL(sel, "ap_num:%d\n", DEV_AP_NUM(dvobj));
	RTW_PRINT_SEL(sel, "starting_ap_num:%d\n", DEV_AP_STARTING_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_ap_num:%d\n", DEV_AP_LD_NUM(dvobj));
	RTW_PRINT_SEL(sel, "adhoc_num:%d\n", DEV_ADHOC_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_adhoc_num:%d\n", DEV_ADHOC_LD_NUM(dvobj));
	RTW_PRINT_SEL(sel, "mesh_num:%d\n", DEV_MESH_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_mesh_num:%d\n", DEV_MESH_LD_NUM(dvobj));
	RTW_PRINT_SEL(sel, "p2p_device_num:%d\n", DEV_P2P_DV_NUM(dvobj));
	RTW_PRINT_SEL(sel, "p2p_gc_num:%d\n", DEV_P2P_GC_NUM(dvobj));
	RTW_PRINT_SEL(sel, "p2p_go_num:%d\n", DEV_P2P_GO_NUM(dvobj));
	RTW_PRINT_SEL(sel, "scan_num:%d\n", DEV_SCAN_NUM(dvobj));
	RTW_PRINT_SEL(sel, "under_wps_num:%d\n", DEV_WPS_NUM(dvobj));
	RTW_PRINT_SEL(sel, "roch_num:%d\n", DEV_ROCH_NUM(dvobj));
	RTW_PRINT_SEL(sel, "mgmt_tx_num:%d\n", DEV_MGMT_TX_NUM(dvobj));
	RTW_PRINT_SEL(sel, "union_ch:%d\n", DEV_U_CH(dvobj));
	RTW_PRINT_SEL(sel, "union_bw:%d\n", DEV_U_BW(dvobj));
	RTW_PRINT_SEL(sel, "union_offset:%d\n", DEV_U_OFFSET(dvobj));
	RTW_PRINT_SEL(sel, "================\n\n");
}

void dump_dvobj_mi_status(void *sel, const char *fun_name, struct _adapter *adapter)
{
	RTW_INFO("\n[ %s ] call %s\n", fun_name, __func__);
	dump_mi_status(sel, adapter_to_dvobj(adapter));
}
