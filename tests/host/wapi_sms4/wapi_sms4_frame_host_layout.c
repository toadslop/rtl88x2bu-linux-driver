// SPDX-License-Identifier: GPL-2.0
/* Host layout symbols for rust/wapi_sms4_frame.rs (T24 PR10). */
#include <stddef.h>

#include "host_wapi_sms4_frame_oracle.h"

const size_t rtw_rust_wapi_frame_off_WapiSupport =
	offsetof(struct host_wapi_adapter, wapi_support);
const size_t rtw_rust_wapi_frame_off_wapiInfo =
	offsetof(struct host_wapi_adapter, wapi_info);
const size_t rtw_rust_wapi_frame_off_wapi_bWapiEnable =
	offsetof(struct host_wapi_info, b_wapi_enable);
const size_t rtw_rust_wep_off_xmit_frame_buf_addr =
	offsetof(struct host_wapi_xmit_frame, buf_addr);
