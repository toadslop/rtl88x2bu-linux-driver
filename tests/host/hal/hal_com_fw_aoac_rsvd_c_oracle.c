// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-145 rtw_hal_set_FwAoacRsvdPage_cmd (hal_com_rest.c). */

#include "host_hal_com_fw_aoac_rsvd_types.h"

extern struct pwrctrl_priv *adapter_to_pwrctl(struct _adapter *adapter);
extern s32 check_fwstate(struct mlme_priv *pmlmepriv, s32 state);
extern void _rtw_memset(void *p, int v, size_t n);
extern s32 rtw_hal_fill_h2c_cmd(void *adapter, u8 id, u32 len, u8 *buf);

static void set_bits_to_le_1byte(u8 *p, u8 bit_offset, u8 bit_len, u8 value)
{
	u8 mask = bit_len >= 8 ? 0xff : (u8)((1u << bit_len) - 1u);
	u8 cleared = *p & (u8)~(mask << bit_offset);
	*p = cleared | (u8)((value & mask) << bit_offset);
}

#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_REMOTE_WAKE_CTRL_INFO(p, v) \
	set_bits_to_le_1byte((p), 0, 8, (v))
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_ARP_RSP(p, v) \
	set_bits_to_le_1byte((p) + 1, 0, 8, (v))
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_NEIGHBOR_ADV(p, v) \
	set_bits_to_le_1byte((p) + 2, 0, 8, (v))
#ifdef CONFIG_GTK_OL
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_RSP(p, v) \
	set_bits_to_le_1byte((p) + 3, 0, 8, (v))
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_INFO(p, v) \
	set_bits_to_le_1byte((p) + 4, 0, 8, (v))
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_EXT_MEM(p, v) \
	set_bits_to_le_1byte((p) + 5, 0, 8, (v))
#endif
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_NDP_INFO(p, v) \
	set_bits_to_le_1byte((p) + 6, 0, 8, (v))
#ifdef CONFIG_PNO_SUPPORT
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_NLO_INFO(p, v) \
	set_bits_to_le_1byte((p), 0, 8, (v))
#endif
#define SET_H2CCMD_AOAC_RSVDPAGE_LOC_AOAC_REPORT(p, v) \
	set_bits_to_le_1byte((p) + 1, 0, 8, (v))

void rtw_hal_set_FwAoacRsvdPage_cmd_c_oracle(struct _adapter *padapter,
					      PRSVDPAGE_LOC rsvdpageloc)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	u8 ret = 0;
#ifdef CONFIG_WOWLAN
	u8 u1H2CAoacRsvdPageParm[H2C_AOAC_RSVDPAGE_LOC_LEN] = {0};

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_REMOTE_WAKE_CTRL_INFO(
		    u1H2CAoacRsvdPageParm, rsvdpageloc->LocRemoteCtrlInfo);
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_ARP_RSP(u1H2CAoacRsvdPageParm,
						     rsvdpageloc->LocArpRsp);
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_NEIGHBOR_ADV(
		    u1H2CAoacRsvdPageParm, rsvdpageloc->LocNbrAdv);
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_NDP_INFO(u1H2CAoacRsvdPageParm,
						      rsvdpageloc->LocNDPInfo);
#ifdef CONFIG_GTK_OL
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_RSP(u1H2CAoacRsvdPageParm,
						     rsvdpageloc->LocGTKRsp);
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_INFO(u1H2CAoacRsvdPageParm,
						      rsvdpageloc->LocGTKInfo);
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_GTK_EXT_MEM(
		    u1H2CAoacRsvdPageParm, rsvdpageloc->LocGTKEXTMEM);
#endif
		ret = rtw_hal_fill_h2c_cmd(padapter, H2C_AOAC_RSVD_PAGE,
					   H2C_AOAC_RSVDPAGE_LOC_LEN,
					   u1H2CAoacRsvdPageParm);
		(void)ret;

		_rtw_memset(u1H2CAoacRsvdPageParm, 0, sizeof(u1H2CAoacRsvdPageParm));
		SET_H2CCMD_AOAC_RSVDPAGE_LOC_AOAC_REPORT(u1H2CAoacRsvdPageParm,
							 rsvdpageloc->LocAOACReport);
		ret = rtw_hal_fill_h2c_cmd(padapter, H2C_AOAC_RSVDPAGE3,
					   H2C_AOAC_RSVDPAGE_LOC_LEN,
					   u1H2CAoacRsvdPageParm);
		(void)ret;
		pwrpriv->wowlan_aoac_rpt_loc = rsvdpageloc->LocAOACReport;
	}
#ifdef CONFIG_PNO_SUPPORT
	else {
		if (!pwrpriv->wowlan_in_resume) {
			_rtw_memset(u1H2CAoacRsvdPageParm, 0,
				    sizeof(u1H2CAoacRsvdPageParm));
			SET_H2CCMD_AOAC_RSVDPAGE_LOC_NLO_INFO(
			    u1H2CAoacRsvdPageParm, rsvdpageloc->LocPNOInfo);
			ret = rtw_hal_fill_h2c_cmd(padapter, H2C_AOAC_RSVDPAGE3,
						   H2C_AOAC_RSVDPAGE_LOC_LEN,
						   u1H2CAoacRsvdPageParm);
			(void)ret;
		}
	}
#endif
#endif /* CONFIG_WOWLAN */
}