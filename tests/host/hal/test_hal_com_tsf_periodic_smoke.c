// SPDX-License-Identifier: GPL-2.0
/* Smoke L2 for W4-130 periodic TSF — full vectors in PR2. */

#include "host_hal_com_tsf_periodic_types.h"
#include <stdio.h>
#include <string.h>

int dev_is_surprise_removed(struct dvobj_priv *dvobj);
int dev_is_drv_stopped(struct dvobj_priv *dvobj);
struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj);
u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter);

static int expect_cmd_count(u32 want, const char *tag)
{
	host_hal_com_tsf_periodic_reset();
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	if (host_hal_com_tsf_periodic_trace.periodic_end_cmd_count != want) {
		fprintf(stderr, "FAIL %s\n", tag);
		return 1;
	}
	return 0;
}

int main(void)
{
	if (expect_cmd_count(1, "hdl_cmd"))
		return 1;
	if (host_hal_com_tsf_periodic_trace.last_end_cmd_adapter !=
	    host_hal_com_tsf_periodic_dvobj()->padapters[0]) {
		fprintf(stderr, "FAIL hdl_primary\n");
		return 1;
	}

	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->surprise_removed = 1;
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	if (host_hal_com_tsf_periodic_trace.periodic_end_cmd_count != 0) {
		fprintf(stderr, "FAIL hdl_surprise\n");
		return 1;
	}

	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->drv_stopped = 1;
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	if (host_hal_com_tsf_periodic_trace.periodic_end_cmd_count != 0) {
		fprintf(stderr, "FAIL hdl_stopped\n");
		return 1;
	}

	puts("OK");
	return 0;
}