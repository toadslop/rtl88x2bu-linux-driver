// SPDX-License-Identifier: GPL-2.0
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "host_hal_dm_set_odm_var_rx_types.h"

static u8 host_stub_is_linked;
static u8 host_stub_rx_rate;
static u8 host_stub_rssi_a;
static u8 host_stub_rssi_b;
static u8 host_stub_cur_igi;
static s8 host_stub_phydm_min_rssi;
static s8 host_stub_dm_min_rssi;
static u32 host_stub_fa_cck;
static u32 host_stub_fa_ofdm;
static u32 host_stub_fa_total;

const char *host_hdata_rate_str(u8 rate)
{
	if (rate == 31)
		return "OFDM54";
	return "UNKNOWN";
}

int host_is_hardware_type_8814a(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

void host_hal_dm_set_odm_var_rx_fixture(
	u8 is_linked, u8 rx_rate, u8 rssi_a, u8 rssi_b,
	u8 cur_igi, s8 phydm_min_rssi, s8 dm_min_rssi,
	u32 fa_cck, u32 fa_ofdm, u32 fa_total)
{
	host_stub_is_linked = is_linked;
	host_stub_rx_rate = rx_rate;
	host_stub_rssi_a = rssi_a;
	host_stub_rssi_b = rssi_b;
	host_stub_cur_igi = cur_igi;
	host_stub_phydm_min_rssi = phydm_min_rssi;
	host_stub_dm_min_rssi = dm_min_rssi;
	host_stub_fa_cck = fa_cck;
	host_stub_fa_ofdm = fa_ofdm;
	host_stub_fa_total = fa_total;
}

u8 rtw_phydm_get_cur_igi(_adapter *Adapter)
{
	(void)Adapter;
	return host_stub_cur_igi;
}

s8 rtw_phydm_get_min_rssi(_adapter *Adapter)
{
	(void)Adapter;
	return host_stub_phydm_min_rssi;
}

s8 rtw_dm_get_min_rssi(_adapter *Adapter)
{
	(void)Adapter;
	return host_stub_dm_min_rssi;
}

u32 rtw_phydm_get_phy_cnt(_adapter *Adapter, u32 cnt)
{
	(void)Adapter;
	if (cnt == FA_CCK)
		return host_stub_fa_cck;
	if (cnt == FA_OFDM)
		return host_stub_fa_ofdm;
	if (cnt == FA_TOTAL)
		return host_stub_fa_total;
	return 0;
}

void host_hal_dm_print_capture_reset(struct host_hal_dm_print_capture *cap)
{
	memset(cap, 0, sizeof(*cap));
}

const char *host_hal_dm_print_capture_text(const struct host_hal_dm_print_capture *cap)
{
	return cap->buf;
}

static void host_append_capture(struct host_hal_dm_print_capture *cap,
				const char *chunk, size_t n)
{
	size_t room;

	if (!cap || !chunk || n == 0)
		return;
	room = sizeof(cap->buf) - 1 - cap->len;
	if (n > room)
		n = room;
	memcpy(cap->buf + cap->len, chunk, n);
	cap->len += n;
	cap->buf[cap->len] = '\0';
}

void _RTW_PRINT_SEL(void *sel, const char *fmt, ...)
{
	va_list ap;
	char tmp[512];
	int n;

	if (!sel)
		return;
	va_start(ap, fmt);
	n = vsnprintf(tmp, sizeof(tmp), fmt, ap);
	va_end(ap);
	if (n < 0)
		return;
	if ((size_t)n >= sizeof(tmp))
		n = (int)sizeof(tmp) - 1;
	host_append_capture((struct host_hal_dm_print_capture *)sel, tmp, (size_t)n);
}

void host_hal_dm_apply_rx_fixture_to_dm(_adapter *a)
{
	struct dm_struct *dm = adapter_to_phydm(a);

	dm->is_linked = host_stub_is_linked;
	dm->rx_rate = host_stub_rx_rate;
	dm->rssi_a = host_stub_rssi_a;
	dm->rssi_b = host_stub_rssi_b;
}

struct rtw_hal_dm_dm_rx_fields {
	u8 is_linked;
	u8 rx_rate;
	s8 rssi_a;
	s8 rssi_b;
	s8 rssi_c;
	s8 rssi_d;
};

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

struct rtw_hal_dm_dm_rx_fields rtw_rust_hal_dm_dm_rx_fields(struct dm_struct *podm)
{
	struct rtw_hal_dm_dm_rx_fields out;

	out.is_linked = podm->is_linked;
	out.rx_rate = podm->rx_rate;
	out.rssi_a = podm->rssi_a;
	out.rssi_b = podm->rssi_b;
	out.rssi_c = podm->rssi_c;
	out.rssi_d = podm->rssi_d;
	return out;
}

void rtw_rust_hal_dm_print_sel(void *sel, const char *line)
{
	size_t n;

	if (!sel || !line)
		return;
	n = strlen(line);
	host_append_capture((struct host_hal_dm_print_capture *)sel, line, n);
}

const char *rtw_rust_hal_dm_hdata_rate(u8 rate)
{
	return host_hdata_rate_str(rate);
}

u8 rtw_rust_hal_dm_is_hw_8814a(_adapter *adapter)
{
	return host_is_hardware_type_8814a(adapter) ? 1 : 0;
}

void rtw_rust_hal_dm_dump_rx_dframe_info(_adapter *adapter, void *sel)
{
	(void)adapter;
	(void)sel;
}

void rtw_rust_hal_dm_dump_raw_rssi_info(_adapter *adapter, void *sel)
{
	(void)adapter;
	(void)sel;
}