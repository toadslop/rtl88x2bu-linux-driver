// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-110 FT IE leaf (provenance: core/rtw_ft.c). */
#include "host_ft_types.h"

void host_ft_info_init(struct ft_roam_info *pft)
{
	_rtw_memset(pft, 0, sizeof(struct ft_roam_info));
	pft->ft_flags = 0 | RTW_FT_EN
#ifdef CONFIG_RTW_BTM_ROAM
			| RTW_FT_BTM_ROAM
#endif
		;
	pft->ft_updated_bcn = _FALSE;
}

u8 host_ft_update_rsnie(_adapter *padapter, u8 bwrite, struct pkt_attrib *pattrib,
			  u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie;
	s32 len;

	pie = rtw_get_ie(pft_roam->updated_ft_ies, EID_WPA2, &len,
			 pft_roam->updated_ft_ies_len);

	if (!bwrite)
		return (pie) ? _SUCCESS : _FAIL;

	if (pie) {
		*pframe = rtw_set_ie(((u8 *)*pframe), EID_WPA2, (uint)len, pie + 2,
				     &(pattrib->pktlen));
	} else {
		return _FAIL;
	}

	return _SUCCESS;
}
