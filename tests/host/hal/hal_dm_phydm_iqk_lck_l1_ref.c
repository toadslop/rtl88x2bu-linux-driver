/* SPDX-License-Identifier: GPL-2.0 */
/* L1 reference leaf: IQK/LCK RFK triggers (W4-98). */

#if ((RTL8822B_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) || (RTL8814B_SUPPORT == 1) \
	|| (RTL8822C_SUPPORT == 1) || (RTL8723F_SUPPORT == 1))
void rtw_phydm_iqk_trigger(void *adapter)
{
	(void)adapter;
}
#endif

void rtw_phydm_iqk_trigger_all(void *adapter)
{
	(void)adapter;
}

void rtw_phydm_lck_trigger(void *adapter)
{
	(void)adapter;
}