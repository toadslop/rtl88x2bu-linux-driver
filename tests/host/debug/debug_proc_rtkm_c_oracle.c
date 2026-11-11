// SPDX-License-Identifier: GPL-2.0
#include "host_rtw_debug_proc_rtkm_types.h"

int proc_get_rtkm_info(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	if (padapter)
		rtw_rust_debug_proc_emit_rtkm_info(m, padapter);
	return 0;
}
