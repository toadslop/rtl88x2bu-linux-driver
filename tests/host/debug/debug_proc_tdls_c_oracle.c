// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-212 proc TDLS enable get/set (host L2 PR1). */

#include "host_rtw_debug_proc_tdls_types.h"

u8 rtw_is_tdls_enabled(struct _adapter *padapter)
{
	if (!padapter)
		return _FALSE;
	return padapter->registrypriv.en_tdls ? _TRUE : _FALSE;
}

void rtw_enable_tdls_func(struct _adapter *padapter)
{
	if (!padapter || rtw_is_tdls_enabled(padapter) == _TRUE)
		return;
	padapter->registrypriv.en_tdls = _TRUE;
}

void rtw_disable_tdls_func(struct _adapter *padapter, u8 enqueue_cmd)
{
	(void)enqueue_cmd;

	if (!padapter || rtw_is_tdls_enabled(padapter) == _FALSE)
		return;
	padapter->registrypriv.en_tdls = _FALSE;
}

int proc_get_tdls_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	(void)v;

	if (pregpriv)
		RTW_PRINT_SEL(m, "TDLS is %s !\n",
			      (rtw_is_tdls_enabled(padapter) == _TRUE) ? "enabled"
								       : "disabled");

	return 0;
}

ssize_t proc_set_tdls_enable(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 en_tdls = 0;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -1;
	}

	if (buffer) {
		memcpy(tmp, buffer, count);
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &en_tdls) == 1 && pregpriv) {
			if (en_tdls > 0)
				rtw_enable_tdls_func(padapter);
			else
				rtw_disable_tdls_func(padapter, _TRUE);
		}
	}

	return (ssize_t)count;
}
