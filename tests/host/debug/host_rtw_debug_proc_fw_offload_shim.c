// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-228 (kernel + L2 Rust oracle). PR2 fills Rust-backed paths. */

#include <stdint.h>

#include "host_rtw_debug_proc_fw_offload_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return -EFAULT;
	if (count > tmp_size)
		return -EFAULT;
	if (buffer)
		memcpy(tmp, buffer, count);
	if (count < tmp_size)
		tmp[count] = '\0';
	return (ssize_t)count;
}

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

void rtw_rust_debug_proc_emit_fw_offload(void *sel, void *adapter)
{
	struct hal_data *hal = GET_HAL_DATA(adapter);

	RTW_PRINT_SEL(sel, "IQK FW offload:%s\n",
		      hal->RegIQKFWOffload ? "enable" : "disable");
	RTW_PRINT_SEL(sel, "Channel switch FW offload:%s\n",
		      hal->ch_switch_offload ? "enable" : "disable");
}

long rtw_rust_debug_proc_set_fw_offload(void *adapter, const void *buffer,
					size_t count)
{
	struct _adapter *pri_adapter = GET_PRIMARY_ADAPTER(adapter);
	struct hal_data *hal = GET_HAL_DATA(adapter);
	char tmp[32];
	u32 iqk_offload_enable = 0, ch_switch_offload_enable = 0;

	if (count < 1)
		return -EFAULT;
	if (count > sizeof(tmp))
		return -EFAULT;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d %d", &iqk_offload_enable,
			   &ch_switch_offload_enable) < 2)
			return -EINVAL;
		if (hal->RegIQKFWOffload != iqk_offload_enable) {
			hal->RegIQKFWOffload = (u8)iqk_offload_enable;
			rtw_run_in_thread_cmd(pri_adapter,
					      (void *)(uintptr_t)0x1,
					      pri_adapter);
		}
		if (hal->ch_switch_offload != ch_switch_offload_enable)
			hal->ch_switch_offload = (u8)ch_switch_offload_enable;
	}
	return (long)count;
}
