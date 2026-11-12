// SPDX-License-Identifier: GPL-2.0
/* Rust build: compile hal_halmac.c with mapper symbols provided by rust/hal_halmac.o */
#define RTW_HALMAC_RUST_MAPPERS 1
#include "hal_halmac.c"

void rtw_rust_hal_halmac_log_rsvd_page_num(u16 num)
{
	RTW_WARN("%s: Fail to allocate RSVD page(%d)!!"
		 " The MAX RSVD page number is 256...\n",
		 __func__, num);
}

void rtw_rust_hal_halmac_log_invalid_rf_drv(u32 rf_drv)
{
	RTW_ERR("%s: Invalid RF type(0x%x)!\n", __func__, rf_drv);
}

void rtw_rust_hal_halmac_log_invalid_rf_halmac(u32 rf_mac)
{
	RTW_ERR("%s: Invalid RF type(0x%x)!\n", __func__, rf_mac);
}

#ifdef CONFIG_SUPPORT_TRX_SHARED
void rtw_rust_hal_halmac_print_sel(void *sel, const char *line)
{
	RTW_PRINT_SEL(sel, "%s", line);
}

void rtw_rust_hal_halmac_trx_share_mode(struct _ADAPTER *adapter, u8 *out)
{
	if (adapter && out)
		*out = adapter->registrypriv.trx_share_mode;
}
#endif /* CONFIG_SUPPORT_TRX_SHARED */
