// SPDX-License-Identifier: GPL-2.0
/* Kernel helpers for rust/rtw_ap_expire_timeout.rs (W3-82 PR20). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_EXPIRE_TIMEOUT) && !defined(HOST_AP_EXPIRE_TIMEOUT_TEST)

void rtw_rust_expire_clients_update(_adapter *padapter, u8 updated)
{
	associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
}

#endif /* CONFIG_RUST_AP_EXPIRE_TIMEOUT && !HOST_AP_EXPIRE_TIMEOUT_TEST */
