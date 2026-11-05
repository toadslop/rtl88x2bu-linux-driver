// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-211 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_phy_cap_registry_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return -1;
	if (count > tmp_size)
		return -1;
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

void rtw_rust_debug_proc_emit_stbc_cap(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "0x%02x\n", pregpriv->stbc_cap);
}

long rtw_rust_debug_proc_set_stbc_cap(void *adapter, const void *buffer,
				      size_t count)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &mode) == 1 && pregpriv)
			pregpriv->stbc_cap = (u8)mode;
	}
	return (long)count;
}

void rtw_rust_debug_proc_emit_rx_stbc(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "%d\n", pregpriv->rx_stbc);
}

long rtw_rust_debug_proc_set_rx_stbc(void *adapter, const void *buffer,
				     size_t count)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &mode) == 1 && pregpriv &&
		    (mode == 0 || mode == 1 || mode == 2 || mode == 3))
			pregpriv->rx_stbc = (u8)mode;
	}
	return (long)count;
}

void rtw_rust_debug_proc_emit_ldpc_cap(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "0x%02x\n", pregpriv->ldpc_cap);
}

long rtw_rust_debug_proc_set_ldpc_cap(void *adapter, const void *buffer,
				      size_t count)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &mode) == 1 && pregpriv)
			pregpriv->ldpc_cap = (u8)mode;
	}
	return (long)count;
}

#ifdef CONFIG_BEAMFORMING
void rtw_rust_debug_proc_emit_txbf_cap(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "0x%02x\n", pregpriv->beamform_cap);
}

long rtw_rust_debug_proc_set_txbf_cap(void *adapter, const void *buffer,
				      size_t count)
{
	struct _adapter *padapter = adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &mode) == 1 && pregpriv)
			pregpriv->beamform_cap = (u8)mode;
	}
	return (long)count;
}
#endif
