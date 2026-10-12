/* SPDX-License-Identifier: GPL-2.0 */
/* Kernel-matching `struct rtw_halmac_bcn_ctrl` for host HALMAC tests (W4-10). */

#ifndef HOST_HAL_HALMAC_DRV_BCN_CTRL_H
#define HOST_HAL_HALMAC_DRV_BCN_CTRL_H

#include <stdint.h>

struct rtw_halmac_bcn_ctrl {
	uint8_t rx_bssid_fit:1;
	uint8_t txbcn_rpt:1;
	uint8_t tsf_update:1;
	uint8_t enable_bcn:1;
	uint8_t rxbcn_rpt:1;
	uint8_t p2p_ctwin:1;
	uint8_t p2p_bcn_area:1;
};

#endif
