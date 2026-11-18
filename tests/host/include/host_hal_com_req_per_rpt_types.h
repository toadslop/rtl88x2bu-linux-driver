/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_REQ_PER_RPT_TYPES_H
#define HOST_HAL_COM_REQ_PER_RPT_TYPES_H

struct _adapter;

unsigned char rtw_hal_set_req_per_rpt_cmd(struct _adapter *adapter, unsigned char group_macid,
					unsigned char rpt_type, unsigned int macid_bitmap);

#endif