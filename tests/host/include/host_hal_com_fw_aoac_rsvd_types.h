/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for W4-145 WOW AOAC reserved-page H2C L2 harness. */

#ifndef HOST_HAL_COM_FW_AOAC_RSVD_TYPES_H
#define HOST_HAL_COM_FW_AOAC_RSVD_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct mlme_priv {
	s32 fw_state;
};

struct pwrctrl_priv {
	u8 wowlan_in_resume;
	u8 wowlan_aoac_rpt_loc;
	u8 _pad[2];
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct pwrctrl_priv pwrctl;
};

#define WIFI_ASOC_STATE 0x00000001
#define H2C_AOAC_RSVD_PAGE 0x83
#define H2C_AOAC_RSVDPAGE3 0x88
#define H2C_AOAC_RSVDPAGE_LOC_LEN 7

typedef struct rsvdpage_loc_host {
	u8 LocRemoteCtrlInfo;
	u8 LocArpRsp;
	u8 LocNbrAdv;
	u8 LocGTKRsp;
	u8 LocGTKInfo;
	u8 LocGTKEXTMEM;
	u8 LocNDPInfo;
	u8 LocAOACReport;
	u8 LocPNOInfo;
} RSVDPAGE_LOC, *PRSVDPAGE_LOC;

void rtw_hal_set_FwAoacRsvdPage_cmd(struct _adapter *padapter, PRSVDPAGE_LOC rsvdpageloc);

#endif