// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-230 proc IQK/LCK cal test handlers (host L2). */

#include <stdint.h>

#include "host_rtw_debug_proc_cal_types.h"

int proc_get_iqk_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;
	(void)padapter;
	return 0;
}

ssize_t proc_set_iqk(void *file, const void *buffer, size_t count, long *pos,
		     void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	char tmp[32];
	u32 recovery, clear, segment;

	(void)file;
	(void)pos;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		/* NUL-terminate for ordered L2 vectors; keep index in-bounds at count == sizeof(tmp). */
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d %d %d", &recovery, &clear, &segment);

		if (num != 3) {
			RTW_INFO("Invalid format\n");
			return (ssize_t)count;
		}

		rtw_hal_iqk_test(padapter, (bool)recovery, (bool)clear,
				 (bool)segment);
	}

	return (ssize_t)count;
}

int proc_get_lck_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;
	(void)padapter;
	return 0;
}

ssize_t proc_set_lck(void *file, const void *buffer, size_t count, long *pos,
		     void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	char tmp[32];
	u32 trigger;

	(void)file;
	(void)pos;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d", &trigger);

		if (num != 1) {
			RTW_INFO("Invalid format\n");
			return (ssize_t)count;
		}

		(void)trigger;
		rtw_hal_lck_test(padapter);
	}

	return (ssize_t)count;
}
