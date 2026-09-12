/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_RF18_RESTORE_TYPES_H
#define HOST_AP_RF18_RESTORE_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

struct hal_data {
	u8 current_channel;
};

struct mlme_ext_priv {
	u8 cur_channel;
	u8 cur_ch_offset;
	u8 cur_bwmode;
};

typedef struct {
	struct hal_data hal_data;
	struct mlme_ext_priv mlmeextpriv;
} _adapter;

u32 rtw_hal_read_rfreg(_adapter *padapter, u32 path, u32 addr, u32 mask);
u8 rtw_mi_get_ch_setting_union(_adapter *padapter, u8 *ch, u8 *bw, u8 *offset);
void set_channel_bwmode(_adapter *padapter, u8 ch, u8 offset, u8 bw);

void host_rf18_reset(void);
void host_rf18_set_reg(u32 path, u32 val);
void host_rf18_set_union_ok(u8 ok, u8 ch, u8 bw, u8 offset);
u8 host_rf18_set_channel_called(void);
void host_rf18_last_set_channel(u8 *ch, u8 *offset, u8 *bw);
u8 host_rf18_hal_current_channel(void);

void rtw_check_restore_rf18(_adapter *padapter);

#endif /* HOST_AP_RF18_RESTORE_TYPES_H */
