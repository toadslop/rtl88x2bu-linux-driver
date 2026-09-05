// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_scan_types.h"

static systime host_now;
static u32 host_pass_ms;
static u8 host_busy, host_mirc;
static struct mi_state host_mi;

void host_scan_set_current_time(systime t) { host_now = t; }
void host_scan_set_passing_time_ms(u32 ms) { host_pass_ms = ms; }
void host_scan_set_busy_traffic(u8 v) { host_busy = v; }
void host_scan_set_miracast(u8 v) { host_mirc = v; }
void host_scan_set_mi_state(const struct mi_state *m) { host_mi = *m; }
systime rtw_get_current_time(void) { return host_now; }
u32 rtw_get_passing_time_ms(systime s) { (void)s; return host_pass_ms; }
bool rtw_mi_busy_traffic_check(_adapter *a) { (void)a; return host_busy; }
bool rtw_mi_check_miracast_enabled(_adapter *a) { (void)a; return host_mirc; }
void rtw_mi_status(_adapter *a, struct mi_state *m) { (void)a; *m = host_mi; }
