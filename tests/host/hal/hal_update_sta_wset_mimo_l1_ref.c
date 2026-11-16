// SPDX-License-Identifier: GPL-2.0
/* L1 ref: rtw_hal_update_sta_wset / rtw_hal_update_sta_mimo_type (no host-only symbols). */

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;

struct _adapter;
struct sta_info;

#define WIRELESS_11B (1U << 0)
#define WIRELESS_11G (1U << 1)
#define WIRELESS_11A (1U << 2)
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define WIRELESS_11AC (1U << 6)
#define WIRELESS_CCK 0x01U
#define WIRELESS_OFDM 0x02U
#define WIRELESS_HT 0x04U
#define WIRELESS_VHT 0x08U

s8 rtw_get_sta_tx_nss(struct _adapter *a, struct sta_info *p);
s8 rtw_get_sta_rx_nss(struct _adapter *a, struct sta_info *p);
void rtw_rust_hal_com_warn_on(int c);
void rtw_rust_hal_com_sta_set_support_wireless_set(struct sta_info *p, u8 w);
void rtw_rust_hal_com_sta_set_mimo_type(struct sta_info *p, u8 m);
u8 rtw_rust_hal_com_sta_wireless_mode(struct sta_info *p);
void rtw_rust_hal_com_log_sta_mimo_ss(u8 mac_id, s8 tx, s8 rx);
u8 rtw_rust_hal_com_sta_mac_id(struct sta_info *p);

void rtw_hal_update_sta_wset(struct _adapter *adapter, struct sta_info *psta)
{
	u8 w_set = 0;
	u8 mode = rtw_rust_hal_com_sta_wireless_mode(psta);

	(void)adapter;
	if (mode & WIRELESS_11B)
		w_set |= WIRELESS_CCK;
	if ((mode & WIRELESS_11G) || (mode & WIRELESS_11A))
		w_set |= WIRELESS_OFDM;
	if ((mode & WIRELESS_11_24N) || (mode & WIRELESS_11_5N))
		w_set |= WIRELESS_HT;
	if (mode & WIRELESS_11AC)
		w_set |= WIRELESS_VHT;
	rtw_rust_hal_com_sta_set_support_wireless_set(psta, w_set);
}

void rtw_hal_update_sta_mimo_type(struct _adapter *adapter, struct sta_info *psta)
{
	s8 tx = rtw_get_sta_tx_nss(adapter, psta);
	s8 rx = rtw_get_sta_rx_nss(adapter, psta);
	u8 mimo = 0xff;

	(void)adapter;
	if (tx == 1 && rx == 1)
		mimo = 0;
	else if (tx == 1 && rx == 2)
		mimo = 1;
	else if (tx == 2 && rx == 2)
		mimo = 2;
	else
		rtw_rust_hal_com_warn_on(1);
	if (mimo != 0xff)
		rtw_rust_hal_com_sta_set_mimo_type(psta, mimo);
	rtw_rust_hal_com_log_sta_mimo_ss(rtw_rust_hal_com_sta_mac_id(psta), tx, rx);
}