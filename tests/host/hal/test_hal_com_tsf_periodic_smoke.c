// SPDX-License-Identifier: GPL-2.0
/* Smoke L2 for W4-130 periodic TSF — full vectors in PR2. */

#include "host_hal_com_tsf_periodic_types.h"
#include <stdio.h>

int dev_is_surprise_removed(struct dvobj_priv *dvobj);
int dev_is_drv_stopped(struct dvobj_priv *dvobj);
struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj);
u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter);

int main(void)
{
	host_hal_com_tsf_periodic_reset();
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	if (host_hal_com_tsf_periodic_trace.periodic_end_cmd_count != 1) {
		fprintf(stderr, "FAIL hdl_cmd\n");
		return 1;
	}
	puts("OK");
	return 0;
}