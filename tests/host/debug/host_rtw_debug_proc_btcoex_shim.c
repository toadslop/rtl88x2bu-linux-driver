// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-215 proc btcoex (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_btcoex_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

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

void rtw_rust_debug_proc_emit_btcoex_dbg(void *sel, void *adapter)
{
	char buf[512] = {0};

	rtw_btcoex_GetDBG(adapter, (u8 *)buf, 512);
	_RTW_PRINT_SEL(sel, "%s", buf);
}

long rtw_rust_debug_proc_set_btcoex_dbg(void *adapter, const void *buffer,
				      size_t count)
{
	u8 tmp[80] = {0};
	u32 module[2] = {0};
	u32 num;

	if (!buffer)
		return -1;
	if (count < 1)
		return -1;

	num = count;
	if (num > (sizeof(tmp) - 1))
		num = (sizeof(tmp) - 1);

	if (copy_from_user(tmp, buffer, num))
		return -1;

	tmp[num] = '\0';
	num = sscanf((char *)tmp, "%x %x", &module[0], &module[1]);
	if (num == 1) {
		if (module[0] == 0)
			_rtw_memset(module, 0, sizeof(module));
		else
			_rtw_memset(module, 0xFF, sizeof(module));
	} else if (num != 2) {
		if (num == 0)
			return -1;
	}

	rtw_btcoex_SetDBG(adapter, module);
	return (long)count;
}

int rtw_rust_debug_proc_emit_btcoex_info(void *sel, void *adapter)
{
	const u32 bufsize = 40 * 100;
	u8 *pbuf = rtw_zmalloc(bufsize);

	if (!pbuf)
		return -12;

	rtw_btcoex_DisplayBtCoexInfo(adapter, pbuf, bufsize);
	_RTW_PRINT_SEL(sel, "%s\n", pbuf);
	rtw_mfree(pbuf, bufsize);
	return 0;
}
