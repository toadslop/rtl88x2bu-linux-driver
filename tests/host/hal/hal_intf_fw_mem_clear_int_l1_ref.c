// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: fw_mem_dl + clear_interrupt (W4-76). */

typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;
typedef struct _adapter _adapter;

#ifdef RTW_HALMAC
s32 rtw_hal_fw_mem_dl(_adapter *padapter, int mem)
{
	(void)padapter;
	(void)mem;
	return 0;
}
#endif

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void rtw_hal_clear_interrupt(_adapter *padapter)
{
	(void)padapter;
}
#endif