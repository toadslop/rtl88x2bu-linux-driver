// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-215 proc btcoex debug handlers (from core/rtw_debug_rest.c). */

#include <stdlib.h>

#include "host_rtw_debug_proc_btcoex_types.h"

static u32 host_btcoex_dbg[2];

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

void host_debug_proc_btcoex_reset_mocks(void)
{
	host_btcoex_dbg[0] = 0;
	host_btcoex_dbg[1] = 0;
}

void host_debug_proc_btcoex_set_dbg(u32 m0, u32 m1)
{
	host_btcoex_dbg[0] = m0;
	host_btcoex_dbg[1] = m1;
}

void host_debug_proc_btcoex_get_dbg(u32 *m0, u32 *m1)
{
	if (m0)
		*m0 = host_btcoex_dbg[0];
	if (m1)
		*m1 = host_btcoex_dbg[1];
}

u32 rtw_btcoex_GetDBG(struct _adapter *padapter, u8 *pStrBuf, u32 bufSize)
{
	(void)padapter;

	if (!pStrBuf || bufSize == 0)
		return 0;
	snprintf((char *)pStrBuf, bufSize, "0x%08x 0x%08x\n", host_btcoex_dbg[0],
		 host_btcoex_dbg[1]);
	return (u32)strlen((char *)pStrBuf);
}

void rtw_btcoex_SetDBG(struct _adapter *padapter, u32 *pDbgModule)
{
	(void)padapter;

	if (!pDbgModule)
		return;
	host_btcoex_dbg[0] = pDbgModule[0];
	host_btcoex_dbg[1] = pDbgModule[1];
}

void rtw_btcoex_DisplayBtCoexInfo(struct _adapter *padapter, u8 *pbuf,
				  u32 bufsize)
{
	(void)padapter;

	if (!pbuf || bufsize == 0)
		return;
	snprintf((char *)pbuf, bufsize, "BT Coex Info Header fixture");
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(void *p, u32 sz)
{
	(void)sz;
	free(p);
}

int proc_get_btcoex_dbg(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	char buf[512] = {0};

	(void)v;

	rtw_btcoex_GetDBG(padapter, (u8 *)buf, 512);
	_RTW_PRINT_SEL(m, "%s", buf);

	return 0;
}

ssize_t proc_set_btcoex_dbg(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	u8 tmp[80] = {0};
	u32 module[2] = {0};
	u32 num;

	(void)file;
	(void)pos;

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

	rtw_btcoex_SetDBG(padapter, module);

	return (ssize_t)count;
}

int proc_get_btcoex_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	const u32 bufsize = 40 * 100;
	u8 *pbuf = NULL;

	(void)v;

	pbuf = rtw_zmalloc(bufsize);
	if (!pbuf)
		return -12; /* -ENOMEM */

	rtw_btcoex_DisplayBtCoexInfo(padapter, pbuf, bufsize);
	_RTW_PRINT_SEL(m, "%s\n", pbuf);
	rtw_mfree(pbuf, bufsize);

	return 0;
}
