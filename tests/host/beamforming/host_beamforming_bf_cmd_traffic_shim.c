// SPDX-License-Identifier: GPL-2.0
/* Host shims for W3-191 cmd/traffic leaf L2 + L1 C reference. */

#include "host_beamforming_bf_init_cmd_oracle.h"
#include "host_beamforming_bf_cmd_traffic_oracle.h"

struct bf_host_cmd_enqueue_tr bf_host_cmd_enqueue_tr;
struct bf_host_c2h_tr bf_host_c2h_tr;
struct bf_host_traffic_tr bf_host_traffic_tr;
u8 bf_host_phydm_sounding_idx;

static u32 g_host_time_ms;

void bf_host_cmd_traffic_set_time(u32 t)
{
	g_host_time_ms = t;
}

void bf_host_cmd_traffic_set_phydm_idx(u8 idx)
{
	bf_host_phydm_sounding_idx = idx;
}

void bf_host_cancel_sounding_timeout_timer(bf_host_cmd_traffic_padpt a)
{
	(void)a;
}

void bf_host_set_sounding_timer(bf_host_cmd_traffic_padpt a, u32 ms)
{
	(void)ms;
	if (a)
		a->traffic.sounding_timer_armed = 1;
}

struct bf_host_sta *bf_host_get_stainfo(bf_host_cmd_traffic_padpt a, u8 *mac)
{
	unsigned i;

	if (!a || !mac)
		return NULL;
	for (i = 0; i < 4; i++) {
		if (!a->stapriv.stas[i].used)
			continue;
		if (!memcmp(a->stapriv.stas[i].mac_addr, mac, BF_HOST_ETH_ALEN))
			return &a->stapriv.stas[i];
	}
	return NULL;
}

u8 bf_host_phydm_get_beamforming_sounding_info(u16 *tp, u8 num, u8 *tx_rate)
{
	(void)tp;
	(void)num;
	(void)tx_rate;
	return bf_host_phydm_sounding_idx;
}

u32 bf_host_get_current_time(void)
{
	return g_host_time_ms;
}

u32 bf_host_get_time_interval_ms(u32 a, u32 b)
{
	if (b >= a)
		return (u32)(b - a);
	return 1;
}

u8 bf_host_get_current_tx_rate(bf_host_cmd_traffic_padpt a, struct bf_host_sta *sta)
{
	(void)a;
	(void)sta;
	return 0x8c;
}

void bf_host_beamforming_enter(bf_host_padpt a, u8 *p)
{
	(void)a;
	(void)p;
}

void bf_host_beamforming_leave(bf_host_padpt a, u8 *p)
{
	(void)a;
	(void)p;
}

void bf_host_beamforming_reset(bf_host_padpt a)
{
	(void)a;
}

void bf_host_sounding_handler(bf_host_padpt a)
{
	(void)a;
}

void bf_host_beamforming_sounding_down(bf_host_padpt a, u8 macid)
{
	(void)a;
	(void)macid;
}

void bf_host_hal_set_gid(bf_host_padpt a, u8 *p)
{
	(void)a;
	(void)p;
}

void bf_host_hal_set_csi(bf_host_padpt a, u8 *p)
{
	(void)a;
	(void)p;
}
