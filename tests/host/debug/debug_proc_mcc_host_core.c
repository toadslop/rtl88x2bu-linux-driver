// SPDX-License-Identifier: GPL-2.0
/* Shared setter core for W3-222 MCC proc L2 (C oracle + kernel helpers). */

#include "host_rtw_debug_proc_mcc_types.h"

struct host_mcc_cmd_log g_mcc_cmd_log;

void host_debug_proc_mcc_reset_log(void)
{
	memset(&g_mcc_cmd_log, 0, sizeof(g_mcc_cmd_log));
}

const struct host_mcc_cmd_log *host_debug_proc_mcc_get_log(void)
{
	return &g_mcc_cmd_log;
}

u8 host_rtw_set_mcc_duration_cmd(struct _adapter *adapter, u8 type, u8 val)
{
	(void)adapter;
	g_mcc_cmd_log.duration_cmd++;
	g_mcc_cmd_log.last_type = type;
	g_mcc_cmd_log.last_val = val;
	return 0;
}

u8 host_rtw_set_mcc_phydm_offload_enable_cmd(struct _adapter *adapter, u8 enable,
					     u8 enqueue)
{
	(void)adapter;
	(void)enqueue;
	g_mcc_cmd_log.phydm_cmd++;
	g_mcc_cmd_log.last_phydm_enable = enable;
	return 0;
}

ssize_t host_proc_set_mcc_enable(struct _adapter *padapter, const void *buffer,
				 size_t count)
{
	char tmp[255];
	u32 en_mcc = 0;

	if (!buffer) {
		RTW_INFO(": input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
		struct _adapter *iface = NULL;
		u8 i = 0;
		int num = sscanf(tmp, "%u", &en_mcc);

		if (num < 1)
			return -EINVAL;

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if (!iface)
				continue;
			iface->registrypriv.en_mcc = (u8)en_mcc;
		}
	}

	return (ssize_t)count;
}

ssize_t host_proc_set_mcc_duration(struct _adapter *padapter, const void *buffer,
				   size_t count)
{
	char tmp[255];
	u32 enable_runtime_duration = 0, mcc_duration = 0, type = 0;

	if (!buffer)
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u %u", &enable_runtime_duration, &type,
				 &mcc_duration);

		if (num < 1)
			return -EINVAL;

		if (num > 3)
			return -EINVAL;

		if (num == 2)
			return -EINVAL;

		if (num >= 1)
			SET_MCC_RUNTIME_DURATION(padapter,
						 (u8)enable_runtime_duration);

		if (num == 3)
			host_rtw_set_mcc_duration_cmd(padapter, (u8)type,
						      (u8)mcc_duration);
	}

	return (ssize_t)count;
}

#ifdef CONFIG_MCC_PHYDM_OFFLOAD
ssize_t host_proc_set_mcc_phydm_offload_enable(struct _adapter *padapter,
					       const void *buffer, size_t count)
{
	char tmp[255];
	u32 mcc_phydm_enable = 0;

	if (!buffer)
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u", &mcc_phydm_enable);

		if (num < 1)
			return -EINVAL;

		host_rtw_set_mcc_phydm_offload_enable_cmd(
			padapter, (u8)mcc_phydm_enable, _TRUE);
	}

	return (ssize_t)count;
}
#endif /* CONFIG_MCC_PHYDM_OFFLOAD */

ssize_t host_proc_set_mcc_single_tx_criteria(struct _adapter *padapter,
					     const void *buffer, size_t count)
{
	char tmp[255];
	u32 mcc_single_tx_criteria = 0;

	if (!buffer)
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
		struct _adapter *iface = NULL;
		u8 i = 0;
		int num = sscanf(tmp, "%u", &mcc_single_tx_criteria);

		if (num < 1)
			return -EINVAL;

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if (!iface)
				continue;
			iface->registrypriv.rtw_mcc_single_tx_cri =
				mcc_single_tx_criteria;
		}
	}

	return (ssize_t)count;
}
