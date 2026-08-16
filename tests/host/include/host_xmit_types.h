/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_xmit_rest tests (W3-40).
 */
#ifndef HOST_XMIT_TYPES_H
#define HOST_XMIT_TYPES_H

#include <stdbool.h>

#include "host_rf_types.h"
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define MACID_NUM_SW_LIMIT 32
#define CONFIG_IFACE_NUMBER 2

#define WIFI_ASOC_STATE 0x00000001

#define MGN_MCS0 0x80
#define MGN_MCS31 0x9F
#define MGN_VHT1SS_MCS0 0xA0
#define MGN_VHT4SS_MCS9 0xD9

#define IS_HT_RATE(_rate) ((_rate) >= MGN_MCS0 && (_rate) <= MGN_MCS31)
#define IS_VHT_RATE(_rate) ((_rate) >= MGN_VHT1SS_MCS0 && (_rate) <= MGN_VHT4SS_MCS9)

#define BW_MODE_2G(bw_mode) ((bw_mode) & 0x0F)
#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define ADAPTER_TX_BW_2G(adapter) BW_MODE_2G((adapter)->driver_tx_bw_mode)
#define ADAPTER_TX_BW_5G(adapter) BW_MODE_5G((adapter)->driver_tx_bw_mode)
#define MLME_STATE(adapter) ((adapter)->mlmepriv.fw_state)
#define adapter_to_dvobj(adapter) ((adapter)->dvobj)
#define dvobj_to_macidctl(dvobj) (&(dvobj)->macid_ctl)
#define dvobj_to_rfctl(dvobj) (&(dvobj)->rf_ctl)
#define rtw_min(a, b) ((a) > (b) ? (b) : (a))
#ifndef BIT_ULL
#define BIT_ULL(x) (1ULL << (x))
#endif

#define RTW_WARN(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

struct macid_bmp {
	u32 m0;
};

struct macid_ctl_t {
	u8 num;
	struct macid_bmp used;
	struct macid_bmp bmc;
	struct macid_bmp if_g[CONFIG_IFACE_NUMBER];
	struct macid_bmp ch_g[2];
	u8 iface_bmc[CONFIG_IFACE_NUMBER];
	u8 h2c_msr[MACID_NUM_SW_LIMIT];
	u8 bw[MACID_NUM_SW_LIMIT];
	u8 vht_en[MACID_NUM_SW_LIMIT];
	u32 rate_bmp0[MACID_NUM_SW_LIMIT];
	u32 rate_bmp1[MACID_NUM_SW_LIMIT];
};

struct rf_ctl_t {
	u32 rate_bmp_ht_by_bw[2];
	u64 rate_bmp_vht_by_bw[4];
};

struct dvobj_priv {
	struct macid_ctl_t macid_ctl;
	struct rf_ctl_t rf_ctl;
};

struct mlme_priv {
	int fw_state;
};

struct mlme_ext_priv {
	unsigned char cur_channel;
};

struct ht_priv {
	u8 sgi_20m;
	u8 sgi_40m;
};

struct vht_priv {
	u8 vht_option;
	u8 sgi_80m;
};

struct sta_cmn_info {
	u8 bw_mode;
};

struct sta_info {
	struct sta_cmn_info cmn;
	struct ht_priv htpriv;
	struct vht_priv vhtpriv;
};

struct _adapter {
	struct dvobj_priv *dvobj;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	u8 driver_tx_bw_mode;
	u8 fix_rate;
	u8 fix_bw;
	u8 iface_id;
	u8 hal_bw_cap;
};

typedef struct _adapter _adapter;

bool rtw_macid_is_used(struct macid_ctl_t *macid_ctl, u8 id);
bool rtw_macid_is_iface_shared(struct macid_ctl_t *macid_ctl, u8 id);
bool rtw_macid_is_iface_specific(struct macid_ctl_t *macid_ctl, u8 id,
				 _adapter *adapter);
bool hal_is_bw_support(_adapter *adapter, u8 bw);

u8 rtw_get_tx_bw_mode(_adapter *adapter, struct sta_info *sta);
void rtw_get_adapter_tx_rate_bmp_by_bw(_adapter *adapter, u8 bw, u16 *r_bmp_cck_ofdm,
				     u32 *r_bmp_ht, u64 *r_bmp_vht);
void rtw_get_adapter_tx_rate_bmp(_adapter *adapter, u16 r_bmp_cck_ofdm[], u32 r_bmp_ht[],
				 u64 r_bmp_vht[]);
void rtw_get_shared_macid_tx_rate_bmp_by_bw(struct dvobj_priv *dvobj, u8 bw,
					    u16 *r_bmp_cck_ofdm, u32 *r_bmp_ht,
					    u64 *r_bmp_vht);
u8 query_ra_short_GI(struct sta_info *psta, u8 bw);
u8 rtw_get_tx_bw_bmp_of_ht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw);
u8 rtw_get_tx_bw_bmp_of_vht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw);

#endif /* HOST_XMIT_TYPES_H */
