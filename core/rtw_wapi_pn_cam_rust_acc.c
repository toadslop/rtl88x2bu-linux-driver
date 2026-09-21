// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_wapi.rs (W3-109 PR4). */
#include <drv_types.h>

#if defined(CONFIG_WAPI_SUPPORT) && defined(CONFIG_RUST_WAPI_PN_CAM)

RT_WAPI_T *rtw_rust_wapi_info(_adapter *padapter)
{
	return &padapter->wapiInfo;
}

#endif /* CONFIG_WAPI_SUPPORT && CONFIG_RUST_WAPI_PN_CAM */
