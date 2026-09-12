// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_rf18_restore.rs (W3-82 PR15). */
#include <drv_types.h>
#include <hal_data.h>

#if defined(CONFIG_RUST_AP_RF18_RESTORE) && !defined(HOST_AP_RF18_RESTORE_TEST)

u8 rtw_rust_rf18_mlme_cur_channel(_adapter *padapter)
{
	return padapter->mlmeextpriv.cur_channel;
}

u8 rtw_rust_rf18_mlme_cur_ch_offset(_adapter *padapter)
{
	return padapter->mlmeextpriv.cur_ch_offset;
}

u8 rtw_rust_rf18_mlme_cur_bwmode(_adapter *padapter)
{
	return padapter->mlmeextpriv.cur_bwmode;
}

void rtw_rust_rf18_clear_current_channel(_adapter *padapter)
{
	GET_HAL_DATA(padapter)->current_channel = 0;
}

#endif /* CONFIG_RUST_AP_RF18_RESTORE && !HOST_AP_RF18_RESTORE_TEST */
