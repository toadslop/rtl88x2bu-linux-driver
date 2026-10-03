// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_wapi.rs (W3-109 PR4). */
#include <drv_types.h>
#include <stddef.h>

#if defined(CONFIG_WAPI_SUPPORT) && defined(CONFIG_RUST_WAPI_PN_CAM)

RT_WAPI_T *rtw_rust_wapi_info(_adapter *padapter)
{
	return &padapter->wapiInfo;
}

const size_t rtw_rust_wapi_off_wapiIE = offsetof(RT_WAPI_T, wapiIE);
const size_t rtw_rust_wapi_off_wapiIELength = offsetof(RT_WAPI_T, wapiIELength);
const size_t rtw_rust_wapi_off_bWapiPSK = offsetof(RT_WAPI_T, bWapiPSK);
const size_t rtw_rust_wapi_off_wapiCamEntry = offsetof(RT_WAPI_T, wapiCamEntry);
const size_t rtw_rust_wapi_size_cam_entry = sizeof(RT_WAPI_CAM_ENTRY);

#endif /* CONFIG_WAPI_SUPPORT && CONFIG_RUST_WAPI_PN_CAM */
