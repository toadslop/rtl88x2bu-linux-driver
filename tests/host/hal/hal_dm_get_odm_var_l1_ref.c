// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: GetHalODMVar (W4-95). */

typedef unsigned char u8;
typedef struct _adapter _adapter;

u8 rtw_phydm_get_cur_igi(_adapter *adapter);
#ifdef CONFIG_ANTENNA_DIVERSITY
u8 rtw_rust_hal_dm_fat_rx_idle_ant(void *phydm);
#endif

void GetHalODMVar(_adapter *adapter, int e_variable, void *p_value1, void *p_value2)
{
	(void)adapter;
	(void)e_variable;
	(void)p_value1;
	(void)p_value2;
}