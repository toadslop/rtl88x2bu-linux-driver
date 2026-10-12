// SPDX-License-Identifier: GPL-2.0
/* ABI guard: Rust `RtwHalmacBcnCtrl` must match kernel bitfield layout (W4-10). */

#include <stdio.h>
#include <stdlib.h>

#include "hal_halmac_drv_bcn_ctrl.h"

#ifdef RUST_HAL_HALMAC_ORACLE
size_t rtw_halmac_bcn_ctrl_rust_size(void);
#endif

int main(void)
{
	if (sizeof(struct rtw_halmac_bcn_ctrl) != 1) {
		fprintf(stderr, "FAIL: C rtw_halmac_bcn_ctrl size %zu (expected 1)\n",
			sizeof(struct rtw_halmac_bcn_ctrl));
		return EXIT_FAILURE;
	}

#ifdef RUST_HAL_HALMAC_ORACLE
	if (rtw_halmac_bcn_ctrl_rust_size() != 1) {
		fprintf(stderr, "FAIL: Rust RtwHalmacBcnCtrl size %zu (expected 1)\n",
			rtw_halmac_bcn_ctrl_rust_size());
		return EXIT_FAILURE;
	}
#endif

	printf("PASS hal_halmac_bcn_ctrl ABI (1 byte)\n");
	return EXIT_SUCCESS;
}
