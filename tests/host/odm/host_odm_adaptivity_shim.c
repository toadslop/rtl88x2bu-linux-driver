// SPDX-License-Identifier: GPL-2.0
#include "host_odm_adaptivity_types.h"

struct host_sel_capture host_sel_out;

void host_odm_adaptivity_reset(_adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	host_sel_reset();
}
