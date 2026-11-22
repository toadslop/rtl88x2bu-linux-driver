// SPDX-License-Identifier: GPL-2.0
/* W4-130 PR1 — timer hdl slice from hal/hal_com_rest.c */

#include "host_hal_com_tsf_periodic_types.h"

struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj);
u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter);
int dev_is_surprise_removed(struct dvobj_priv *dvobj);
int dev_is_drv_stopped(struct dvobj_priv *dvobj);

void rtw_hal_periodic_tsf_update_end_timer_hdl(void *ctx)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)ctx;

	if (dev_is_surprise_removed(dvobj) || dev_is_drv_stopped(dvobj))
		return;

	rtw_periodic_tsf_update_end_cmd(dvobj_get_primary_adapter(dvobj));
}