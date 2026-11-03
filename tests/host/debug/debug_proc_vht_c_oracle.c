// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-208 proc vht_24g_enable handlers (host L2). */

#include "host_rtw_debug_proc_vht_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int proc_get_vht_24g_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "%d\n", pregpriv->vht_24g_enable);

	return 0;
}

ssize_t proc_set_vht_24g_enable(void *file, const void *buffer, size_t count,
				long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d ", &mode);

		if (num == 1 && pregpriv && mode < 2)
			pregpriv->vht_24g_enable = (u8)mode;
	}

	return (ssize_t)count;
}
