// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-211 proc PHY cap registry handlers (host L2). */

#include "host_rtw_debug_proc_phy_cap_registry_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int proc_get_stbc_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	(void)v;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%02x\n", pregpriv->stbc_cap);

	return 0;
}

ssize_t proc_set_stbc_cap(void *file, const void *buffer, size_t count,
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

		if (num == 1 && pregpriv) {
			pregpriv->stbc_cap = (u8)mode;
		}
	}

	return (ssize_t)count;
}

int proc_get_rx_stbc(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	(void)v;

	if (pregpriv)
		RTW_PRINT_SEL(m, "%d\n", pregpriv->rx_stbc);

	return 0;
}

ssize_t proc_set_rx_stbc(void *file, const void *buffer, size_t count,
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

		if (num == 1 && pregpriv &&
		    (mode == 0 || mode == 1 || mode == 2 || mode == 3))
			pregpriv->rx_stbc = (u8)mode;
	}

	return (ssize_t)count;
}

int proc_get_ldpc_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	(void)v;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%02x\n", pregpriv->ldpc_cap);

	return 0;
}

ssize_t proc_set_ldpc_cap(void *file, const void *buffer, size_t count,
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

		if (num == 1 && pregpriv)
			pregpriv->ldpc_cap = (u8)mode;
	}

	return (ssize_t)count;
}

#ifdef CONFIG_BEAMFORMING
int proc_get_txbf_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	(void)v;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%02x\n", pregpriv->beamform_cap);

	return 0;
}

ssize_t proc_set_txbf_cap(void *file, const void *buffer, size_t count,
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

		if (num == 1 && pregpriv)
			pregpriv->beamform_cap = (u8)mode;
	}

	return (ssize_t)count;
}
#endif
