// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

struct host_hal_dm_init_odm_tail_trace host_hal_dm_init_odm_tail_trace;

void host_hal_dm_init_odm_tail_reset(void)
{
	memset(&host_hal_dm_init_odm_tail_trace, 0, sizeof(host_hal_dm_init_odm_tail_trace));
}

void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr)
{
	(void)dm;
	switch (cmn_info) {
	case 58: host_hal_dm_init_odm_tail_trace.chnl = ptr; break;
	case 64: host_hal_dm_init_odm_tail_trace.scan = ptr; break;
	case 65: host_hal_dm_init_odm_tail_trace.ps = ptr; break;
	case 31: host_hal_dm_init_odm_tail_trace.usb_speed = ptr; break;
	default: break;
	}
}

void halrf_cmn_info_hook(struct dm_struct *dm, u8 id, void *ptr)
{
	(void)dm;
	(void)id;
	(void)ptr;
}

struct fixture {
	struct _adapter adapter;
	u8 channel;
	u8 scan;
	u8 usb;
};

static int run_case(void)
{
	struct fixture fx;

	memset(&fx, 0, sizeof(fx));
	fx.channel = 36;
	fx.scan = 1;
	fx.usb = 3;
	fx.adapter.hal.current_channel = fx.channel;
	fx.adapter.hal.b_scan_in_process = fx.scan;
	fx.adapter.dvobj.usb_speed = fx.usb;
	host_hal_dm_init_odm_tail_reset();
	rtw_rust_hal_dm_init_odm_cominfo_tail(&fx.adapter);

	if (host_hal_dm_init_odm_tail_trace.chnl != &fx.adapter.hal.current_channel ||
	    host_hal_dm_init_odm_tail_trace.scan != &fx.adapter.hal.b_scan_in_process ||
	    host_hal_dm_init_odm_tail_trace.usb_speed != &fx.adapter.dvobj.usb_speed ||
	    !host_hal_dm_init_odm_tail_trace.sta_hooks ||
	    !host_hal_dm_init_odm_tail_trace.finish) {
		return 1;
	}
	return 0;
}

int main(void)
{
	if (run_case())
		return fprintf(stderr, "FAIL hal_dm_init_odm_cominfo_tail L2\n"), 1;
	printf("OK hal_dm_init_odm_cominfo_tail L2\n");
	return 0;
}