// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-208 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_vht_types.h"

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

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	struct seq_file *seq = sel;

	if (!line)
		return;
	if (seq && seq->out)
		fputs(line, seq->out);
}

void rtw_rust_debug_proc_emit_vht_24g_enable(void *sel, void *adapter)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "%d\n", pregpriv->vht_24g_enable);
}

long rtw_rust_debug_proc_set_vht_24g_enable(void *adapter, const void *buffer,
					    size_t count)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &mode) == 1 && pregpriv && mode < 2)
			pregpriv->vht_24g_enable = (u8)mode;
	}
	return (long)count;
}
