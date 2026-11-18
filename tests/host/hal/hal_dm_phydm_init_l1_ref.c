// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: PHYDM init orchestration (W4-113). Name-only stub for L1;
 * behaviour lives in rust/hal_dm_phydm_init.inc.rs when CONFIG_RUST is set.
 */

struct _adapter;

void rtw_phydm_init(struct _adapter *adapter);