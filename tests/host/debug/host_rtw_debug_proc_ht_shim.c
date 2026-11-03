// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-203 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_ht_types.h"

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

void rtw_rust_debug_proc_emit_ht_enable(void *sel, void *adapter)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "%d\n", pregpriv->ht_enable);
}

long rtw_rust_debug_proc_set_ht_enable(void *adapter, const void *buffer,
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
			pregpriv->ht_enable = (u8)mode;
	}
	return (long)count;
}

void rtw_rust_debug_proc_emit_bw_mode(void *sel, void *adapter)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "0x%02x\n", pregpriv->bw_mode);
}

long rtw_rust_debug_proc_set_bw_mode(void *adapter, const void *buffer,
				     size_t count)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;
	char tmp[32];
	u32 mode;
	u8 bw_2g;
	u8 bw_5g;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%x ", &mode) == 1) {
			bw_5g = (u8)(mode >> 4);
			bw_2g = (u8)(mode & 0x0f);
			if (pregpriv && bw_2g <= 4 && bw_5g <= 4)
				pregpriv->bw_mode = (u8)mode;
		}
	}
	return (long)count;
}

void rtw_rust_debug_proc_emit_ampdu_enable(void *sel, void *adapter)
{
	struct registry_priv *pregpriv = &((struct _adapter *)adapter)->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(sel, "%d\n", pregpriv->ampdu_enable);
}

long rtw_rust_debug_proc_set_ampdu_enable(void *adapter, const void *buffer,
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
			pregpriv->ampdu_enable = (u8)mode;
	}
	return (long)count;
}
