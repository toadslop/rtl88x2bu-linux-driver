// SPDX-License-Identifier: GPL-2.0
/* C oracle: dump_mi_status leaf from core/rtw_mi.c (W3-198 L2). */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "host_mi_dump_types.h"

struct host_sel_capture {
	char buf[4096];
	size_t len;
};

static struct host_sel_capture *sel_cap(void *sel)
{
	return (struct host_sel_capture *)sel;
}

static void host_print_sel(void *sel, const char *fmt, ...)
{
	struct host_sel_capture *cap = sel_cap(sel);
	va_list ap;
	size_t remain;
	int n;

	if (!cap)
		return;
	remain = sizeof(cap->buf) - cap->len - 1;
	if (remain == 0)
		return;
	va_start(ap, fmt);
	n = vsnprintf(cap->buf + cap->len, remain, fmt, ap);
	va_end(ap);
	if (n > 0)
		cap->len += (size_t)n;
}

#define RTW_PRINT_SEL(sel, fmt, ...) host_print_sel(sel, fmt, ##__VA_ARGS__)
#define RTW_INFO(fmt, ...) ((void)0)

void dump_mi_status(void *sel, struct mi_dump_dvobj *dvobj)
{
	RTW_PRINT_SEL(sel, "== dvobj-iface_state ==\n");
	RTW_PRINT_SEL(sel, "sta_num:%d\n", DEV_STA_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linking_sta_num:%d\n", DEV_STA_LG_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_sta_num:%d\n", DEV_STA_LD_NUM(dvobj));
#ifdef CONFIG_TDLS
	RTW_PRINT_SEL(sel, "linked_tdls_num:%d\n", DEV_TDLS_LD_NUM(dvobj));
#endif
#ifdef CONFIG_AP_MODE
	RTW_PRINT_SEL(sel, "ap_num:%d\n", DEV_AP_NUM(dvobj));
	RTW_PRINT_SEL(sel, "starting_ap_num:%d\n", DEV_AP_STARTING_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_ap_num:%d\n", DEV_AP_LD_NUM(dvobj));
#endif
	RTW_PRINT_SEL(sel, "adhoc_num:%d\n", DEV_ADHOC_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_adhoc_num:%d\n", DEV_ADHOC_LD_NUM(dvobj));
#ifdef CONFIG_RTW_MESH
	RTW_PRINT_SEL(sel, "mesh_num:%d\n", DEV_MESH_NUM(dvobj));
	RTW_PRINT_SEL(sel, "linked_mesh_num:%d\n", DEV_MESH_LD_NUM(dvobj));
#endif
#ifdef CONFIG_P2P
	RTW_PRINT_SEL(sel, "p2p_device_num:%d\n", DEV_P2P_DV_NUM(dvobj));
	RTW_PRINT_SEL(sel, "p2p_gc_num:%d\n", DEV_P2P_GC_NUM(dvobj));
	RTW_PRINT_SEL(sel, "p2p_go_num:%d\n", DEV_P2P_GO_NUM(dvobj));
#endif
	RTW_PRINT_SEL(sel, "scan_num:%d\n", DEV_SCAN_NUM(dvobj));
	RTW_PRINT_SEL(sel, "under_wps_num:%d\n", DEV_WPS_NUM(dvobj));
#if defined(CONFIG_IOCTL_CFG80211)
	#if defined(CONFIG_P2P)
	RTW_PRINT_SEL(sel, "roch_num:%d\n", DEV_ROCH_NUM(dvobj));
	#endif
	RTW_PRINT_SEL(sel, "mgmt_tx_num:%d\n", DEV_MGMT_TX_NUM(dvobj));
#endif
	RTW_PRINT_SEL(sel, "union_ch:%d\n", DEV_U_CH(dvobj));
	RTW_PRINT_SEL(sel, "union_bw:%d\n", DEV_U_BW(dvobj));
	RTW_PRINT_SEL(sel, "union_offset:%d\n", DEV_U_OFFSET(dvobj));
	RTW_PRINT_SEL(sel, "================\n\n");
}

void dump_dvobj_mi_status(void *sel, const char *fun_name, _adapter *adapter)
{
	if (0)
		RTW_INFO("\n[ %s ] call %s\n", fun_name, __func__);
	dump_mi_status(sel, (struct mi_dump_dvobj *)adapter_to_dvobj(adapter));
}
