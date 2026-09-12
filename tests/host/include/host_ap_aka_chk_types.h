/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_AKA_CHK_TYPES_H
#define HOST_AP_AKA_CHK_TYPES_H

#include "host_types.h"

#define _SUCCESS 0
#define _FAIL (-1)
#define _FALSE 0
#define _TRUE 1

#define WIFI_SLEEP_STATE 0x00000004U
#define WIFI_AP_STATE 0x00000010U

struct mlme_priv {
	u32 fwstate;
};

struct cmn_sta_info {
	u8 mac_addr[6];
};

struct sta_info {
	struct cmn_sta_info cmn;
	u32 state;
};

typedef struct {
	struct mlme_priv mlmepriv;
} _adapter;

#define get_fwstate(p) ((p)->fwstate)
#define check_fwstate(p, s) (((p)->fwstate & (s)) != 0)
#define MLME_IS_AP(adapter) check_fwstate(&((adapter)->mlmepriv), WIFI_AP_STATE)

int issue_nulldata(_adapter *adapter, u8 *target_addr, int pwr, int ps, int retry);

int issue_aka_chk_frame(_adapter *adapter, struct sta_info *psta);

void host_aka_chk_reset(void);
void host_aka_chk_set_nulldata_ret(int ret);
int host_aka_chk_last_nulldata_ps(void);
u8 host_aka_chk_nulldata_called(void);

#endif /* HOST_AP_AKA_CHK_TYPES_H */
