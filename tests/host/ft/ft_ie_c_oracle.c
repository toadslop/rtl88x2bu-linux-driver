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
