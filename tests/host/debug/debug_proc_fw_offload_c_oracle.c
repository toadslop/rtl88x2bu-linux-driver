// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-228 proc fw_offload handlers (host L2). */

#include <stdint.h>

#include "host_rtw_debug_proc_fw_offload_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int proc_get_fw_offload(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	struct hal_data *hal = GET_HAL_DATA(adapter);

	(void)v;

	RTW_PRINT_SEL(m, "IQK FW offload:%s\n",
		      hal->RegIQKFWOffload ? "enable" : "disable");
	RTW_PRINT_SEL(m, "Channel switch FW offload:%s\n",
		      hal->ch_switch_offload ? "enable" : "disable");
	return 0;
}

ssize_t proc_set_fw_offload(void *file, const void *buffer, size_t count,
			    long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	struct _adapter *pri_adapter = GET_PRIMARY_ADAPTER(adapter);
	struct hal_data *hal = GET_HAL_DATA(adapter);
	char tmp[32];
	u32 iqk_offload_enable = 0, ch_switch_offload_enable = 0;

	(void)file;
	(void)pos;

	if (buffer == NULL)
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d %d", &iqk_offload_enable,
				 &ch_switch_offload_enable);

		if (num < 2)
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

	return (ssize_t)count;
}

int proc_get_fw_tbtt_rpt(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	(void)v;

	RTW_PRINT_SEL(m, "FW BCN offload:%s\n",
		      dvobj->fw_bcn_offload ? "enable" : "disable");
	RTW_PRINT_SEL(m, "FW TBTT RPT:%x\n", dvobj->vap_tbtt_rpt_map);
	return 0;
}

ssize_t proc_set_fw_tbtt_rpt(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];
	u32 fw_tbtt_rpt = 0, fw_bcn_offload = 0;

	(void)file;
	(void)pos;

	if (buffer == NULL)
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num;

		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%d %x", &fw_bcn_offload, &fw_tbtt_rpt);

		if (num < 2)
			return -EINVAL;

		rtw_hal_set_fw_ap_bcn_offload_cmd(adapter, fw_bcn_offload,
						(u8)fw_tbtt_rpt);
	}

	return (ssize_t)count;
}
