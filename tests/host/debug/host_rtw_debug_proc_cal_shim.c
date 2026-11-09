// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-230 IQK/LCK cal proc L2 Rust oracle. */

#include <string.h>

#include "host_rtw_debug_proc_cal_types.h"

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

long rtw_rust_debug_proc_set_iqk(void *adapter, const void *buffer, size_t count)
{
	char tmp[32];
	u32 recovery, clear, segment;

	(void)adapter;
	if (count < 1)
		return -EFAULT;
	if (count > sizeof(tmp))
		return -EFAULT;
	memset(tmp, 0, sizeof(tmp));
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d %d %d", &recovery, &clear, &segment);

		if (num != 3)
			return (long)count;
		rtw_hal_iqk_test(adapter, (bool)recovery, (bool)clear,
				 (bool)segment);
	}
	return (long)count;
}

long rtw_rust_debug_proc_set_lck(void *adapter, const void *buffer, size_t count)
{
	char tmp[32];
	u32 trigger;

	(void)trigger;
	if (count < 1)
		return -EFAULT;
	if (count > sizeof(tmp))
		return -EFAULT;
	memset(tmp, 0, sizeof(tmp));
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &trigger);

		if (num != 1)
			return (long)count;
		rtw_hal_lck_test(adapter);
	}
	return (long)count;
}
