// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: PHYDM TRX cfg (W4-108). Name-only stub for L1; behaviour is
 * covered by hal_dm_phydm_trx_cfg_c_oracle.c.
 */

#ifdef CONFIG_CTRL_TXSS_BY_TP
void rtw_phydm_trx_cfg(void *adapter, int tx_1ss)
{
	(void)adapter;
	(void)tx_1ss;
}
#endif