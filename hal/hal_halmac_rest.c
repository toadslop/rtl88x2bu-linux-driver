// SPDX-License-Identifier: GPL-2.0
/* Rust build: compile hal_halmac.c with mapper symbols provided by rust/hal_halmac.o */
#define RTW_HALMAC_RUST_MAPPERS 1
#include "hal_halmac.c"

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
