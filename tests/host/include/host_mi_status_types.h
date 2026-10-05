/* SPDX-License-Identifier: GPL-2.0 */
/* Host fixtures for W3-122 mi status / check_status L2 (core/rtw_mi_status.c). */
#ifndef HOST_MI_STATUS_TYPES_H
#define HOST_MI_STATUS_TYPES_H

#include <stddef.h>
#include "host_types.h"

typedef int sint;

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#endif

enum {
	MI_LINKED,
	MI_ASSOC,
	MI_UNDER_WPS,
	MI_AP_MODE,
	MI_AP_ASSOC,
	MI_ADHOC,
	MI_ADHOC_ASSOC,
	MI_MESH,
	MI_MESH_ASSOC,
	MI_STA_NOLINK,
	MI_STA_LINKED,
	MI_STA_LINKING,
};

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))

#define WIFI_ASOC_STATE 0x00000001
#define WIFI_STATION_STATE 0x00000008
#define WIFI_AP_STATE 0x00000010
#define WIFI_ADHOC_STATE 0x00000020
#define WIFI_ADHOC_MASTER_STATE 0x00000040
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_WPS 0x00000100
#define WIFI_MESH_STATE 0x00000200
#define WIFI_UNDER_SURVEY 0x00000800
#define WIFI_MONITOR_STATE 0x80000000

#define SCAN_DISABLE 0
#define SCAN_BACK_OP 6
#define SCAN_ENTER 3

#define MLME_IS_GC(a) 0
#define MLME_IS_GO(a) 0
#define MLME_IS_PD(a) 0

struct mlme_priv {
	s32 fw_state;
};

struct sitesurvey_res {
	u8 state;
};

struct mlme_ext_priv {
	struct sitesurvey_res sitesurvey_res;
};

struct tdls_info {
	u8 link_established;
};

struct sta_priv {
	int asoc_sta_count;
};

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
	struct _adapter *padapters[4];
	u8 cfg80211_mgmt_tx[4];
	u8 cfg80211_roch[4];
};

struct _adapter {
	u8 iface_id;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct sta_priv stapriv;
	struct tdls_info tdlsinfo;
	struct dvobj_priv *dvobj;
};

typedef struct _adapter _adapter;
typedef struct dvobj_priv dvobj_priv;
typedef struct mi_state mi_state;
typedef struct mlme_priv mlme_priv;

#define adapter_to_dvobj(a) ((a)->dvobj)

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
#define MSTATE_SCAN_NUM(m) ((m)->scan_num)
#define MSTATE_SCAN_ENTER_NUM(m) ((m)->scan_enter_num)
#define MSTATE_WPS_NUM(m) ((m)->uwps_num)
#define MSTATE_ROCH_NUM(m) ((m)->roch_num)
#define MSTATE_MGMT_TX_NUM(m) ((m)->mgmt_tx_num)
#define MSTATE_P2P_DV_NUM(m) ((m)->p2p_device_num)
#define MSTATE_P2P_GC_NUM(m) ((m)->p2p_gc)
#define MSTATE_P2P_GO_NUM(m) ((m)->p2p_go)

static inline u8 check_fwstate(struct mlme_priv *pmlmepriv, int state)
{
	if (!state)
		return pmlmepriv->fw_state ? _FALSE : _TRUE;
	return (pmlmepriv->fw_state & state) ? _TRUE : _FALSE;
}

#define mlmeext_scan_state(mx) ((mx)->sitesurvey_res.state)

static inline u8 rtw_cfg80211_get_is_mgmt_tx(struct _adapter *a)
{
	if (!a || !a->dvobj)
		return _FALSE;
	return a->dvobj->cfg80211_mgmt_tx[a->iface_id] ? _TRUE : _FALSE;
}

static inline u8 rtw_cfg80211_get_is_roch(struct _adapter *a)
{
	if (!a || !a->dvobj)
		return _FALSE;
	return a->dvobj->cfg80211_roch[a->iface_id] ? _TRUE : _FALSE;
}

void rtw_mi_status_by_ifbmp(struct dvobj_priv *dvobj, u8 ifbmp, struct mi_state *mstate);
void rtw_mi_status(_adapter *adapter, struct mi_state *mstate);
void rtw_mi_status_no_self(_adapter *adapter, struct mi_state *mstate);
void rtw_mi_status_no_others(_adapter *adapter, struct mi_state *mstate);
void rtw_mi_status_merge(struct mi_state *d, struct mi_state *a);
void rtw_mi_update_iface_status(struct mlme_priv *pmlmepriv, sint state);
u8 rtw_mi_check_status(_adapter *adapter, u8 type);

#endif /* HOST_MI_STATUS_TYPES_H */
