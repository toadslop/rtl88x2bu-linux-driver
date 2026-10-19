// SPDX-License-Identifier: GPL-2.0
/* Kernel layout accessors for rust/wapi_sms4_frame.rs (W3-142 PR4). */
#include <drv_types.h>
#include <rtw_wapi.h>

#if defined(CONFIG_WAPI_SUPPORT) && defined(CONFIG_RUST)

const size_t rtw_rust_wapi_frame_off_WapiSupport = offsetof(_adapter, WapiSupport);
const size_t rtw_rust_wapi_frame_off_wapiInfo = offsetof(_adapter, wapiInfo);
const size_t rtw_rust_wapi_frame_off_wapi_bWapiEnable = offsetof(RT_WAPI_T, bWapiEnable);

#endif /* CONFIG_WAPI_SUPPORT && CONFIG_RUST */
