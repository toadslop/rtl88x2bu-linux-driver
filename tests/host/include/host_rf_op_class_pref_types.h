/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for op_class_pref lifecycle tests (W3-56).
 */
#ifndef HOST_RF_OP_CLASS_PREF_TYPES_H
#define HOST_RF_OP_CLASS_PREF_TYPES_H

#include "host_types.h"
#include "host_rf_types.h"
#include "host_autoconf.h"

#include <stdbool.h>
#include <stdio.h>

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define WIRELESS_11B BIT0
#define WIRELESS_11G BIT1
#define WIRELESS_11A BIT2
#define WIRELESS_11_24N BIT3
#define WIRELESS_11_5N BIT4
#define WIRELESS_11AC BIT6
#define WIRELESS_MODE_24G (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_MODE_5G (WIRELESS_11A | WIRELESS_11_5N | WIRELESS_11AC)
#define SUPPORTED_24G_NETTYPE_MSK WIRELESS_MODE_24G
#define SUPPORTED_5G_NETTYPE_MSK WIRELESS_MODE_5G
#define IsSupported24G(NetType) ((NetType) & SUPPORTED_24G_NETTYPE_MSK ? _TRUE : _FALSE)
#define is_supported_5g(NetType) ((NetType) & SUPPORTED_5G_NETTYPE_MSK ? _TRUE : _FALSE)
#define is_supported_vht(NetType) ((NetType) & WIRELESS_11AC ? _TRUE : _FALSE)

#define BW_MODE_2G(bw_mode) ((bw_mode) & 0x0F)
#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define REGSTY_BW_2G(regsty) BW_MODE_2G((regsty)->bw_mode)
#define REGSTY_BW_5G(regsty) BW_MODE_5G((regsty)->bw_mode)
#define REGSTY_IS_11AC_ENABLE(regsty) ((regsty)->vht_enable != 0)

#define RTW_CHF_NO_IR (1 << 0)
#define RTW_CHF_DFS (1 << 1)
#define RTW_CHF_NO_HT40U (1 << 4)
#define RTW_CHF_NO_HT40L (1 << 5)
#define RTW_CHF_NO_80MHZ (1 << 6)
#define RTW_CHF_NO_160MHZ (1 << 7)

#define MAX_CHANNEL_NUM 59
#define MAX_CHANNEL_NUM_OF_BAND 28

#define RTW_ERR(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)
#define RTW_PRINT(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

typedef struct _RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
} RT_CHANNEL_INFO;

struct country_chplan {
	u8 alpha2[2];
	u8 chplan;
	u8 en_11ac;
};

#define COUNTRY_CHPLAN_EN_11AC(_ent) ((_ent)->en_11ac)

struct op_ch_t {
	u8 ch;
	u8 static_non_op : 1;
	u8 no_ir : 1;
	s16 max_txpwr;
};

struct op_class_pref_t {
	u8 class_id;
	BAND_TYPE band;
	enum opc_bw bw;
	u8 ch_num;
	u8 op_ch_num;
	u8 ir_ch_num;
	struct op_ch_t chs[MAX_CHANNEL_NUM_OF_BAND];
};

struct registry_priv {
	u8 wireless_mode;
	u8 bw_mode;
	u8 vht_enable;
};

struct rf_ctl_t {
	const struct country_chplan *country_ent;
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
	struct op_class_pref_t **spt_op_class_ch;
	u8 cap_spt_op_class_num;
	u8 reg_spt_op_class_num;
	u8 cur_spt_op_class_num;
};

typedef struct {
	struct registry_priv registrypriv;
	struct rf_ctl_t rf_ctl;
} _adapter;

#define adapter_to_regsty(adapter) (&(adapter)->registrypriv)
#define adapter_to_rfctl(adapter) (&(adapter)->rf_ctl)

struct hal_spec_t {
	u8 bw_cap;
};

struct hal_spec_t *host_rf_op_class_pref_hal_spec(void);
#define GET_HAL_SPEC(adapter) host_rf_op_class_pref_hal_spec()

void *rtw_zmalloc(u32 sz);
void rtw_mfree(void *p, u32 sz);

bool hal_chk_band_cap(_adapter *adapter, u8 cap);
s16 rtw_rfctl_get_reg_max_txpwr_mbm(struct rf_ctl_t *rfctl, u8 ch, u8 bw,
				    u8 offset, bool eirp);
u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl);
int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch);

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);
u8 rtw_get_op_chs_by_cch_bw(u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num);

int op_class_pref_init(_adapter *adapter);
void op_class_pref_deinit(_adapter *adapter);
void op_class_pref_apply_regulatory(_adapter *adapter, u8 reason);

#define REG_BEACON_HINT 0
#define REG_TXPWR_CHANGE 1
#define REG_CHANGE 2

void host_rf_op_class_pref_reset(_adapter *adapter);
void host_rf_op_class_pref_set_hal(u8 band_cap, u8 bw_cap);
void host_rf_op_class_pref_set_txpwr(s16 mbm);
void host_rf_op_class_pref_set_dfs_unknown(u8 unknown);
struct op_class_pref_t *host_rf_op_class_pref_by_class_id(_adapter *adapter,
							   u8 class_id);

#endif /* HOST_RF_OP_CLASS_PREF_TYPES_H */
