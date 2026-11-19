// SPDX-License-Identifier: GPL-2.0
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_get_rsvd_page_types.h"

#define HAL_FIFO_SEL_RSVD_PAGE 2

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;
static u32 g_page_size = 128;
static u32 g_last_fifo_sel;
static u32 g_last_addr;
static u32 g_last_size;

void host_get_rsvd_page_reset(void)
{
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.dvobj = &g_dvobj;
	g_page_size = 128;
	g_last_fifo_sel = 0;
	g_last_addr = 0;
	g_last_size = 0;
}

struct _adapter *host_get_rsvd_page_adapter(void)
{
	return &g_adapter;
}

void host_get_rsvd_page_set_page_size(u32 psz)
{
	g_page_size = psz;
}

u32 host_get_rsvd_page_fixture_page_size(void)
{
	return g_page_size;
}

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter)
{
	return adapter ? adapter->dvobj : 0;
}

void rtw_hal_get_def_var(struct _adapter *adapter, int def, u8 *val)
{
	u32 *p = (u32 *)val;

	(void)adapter;
	if (def == HAL_DEF_TX_PAGE_SIZE)
		*p = g_page_size;
}

int rtw_halmac_dump_fifo(struct dvobj_priv *d, u8 fifo_sel, u32 addr,
			 u32 size, u8 *buffer)
{
	(void)d;
	g_last_fifo_sel = fifo_sel;
	g_last_addr = addr;
	g_last_size = size;
	if (!buffer || size == 0)
		return -1;
	for (u32 i = 0; i < size; i++)
		buffer[i] = (u8)((addr + i) & 0xff);
	return 0;
}

u32 host_get_rsvd_last_fifo_sel(void)
{
	return g_last_fifo_sel;
}

u32 host_get_rsvd_last_addr(void)
{
	return g_last_addr;
}

u32 host_get_rsvd_last_size(void)
{
	return g_last_size;
}

int host_get_rsvd_last_fifo_sel_is_rsvd(void)
{
	return g_last_fifo_sel == HAL_FIFO_SEL_RSVD_PAGE;
}

#define RTW_ERR(fmt, ...) ((void)0)

void host_sel_reset(struct host_sel_capture *cap)
{
	cap->len = 0;
	cap->buf[0] = '\0';
}

int host_sel_string(struct host_sel_capture *cap, char *out, size_t outlen)
{
	if (cap->len >= outlen)
		return -1;
	memcpy(out, cap->buf, cap->len + 1);
	return (int)cap->len;
}

void *rtw_zvmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_vmfree(void *p, u32 sz)
{
	(void)sz;
	free(p);
}

int rtw_hal_get_rsvd_page_c_oracle(struct _adapter *adapter, u32 page_offset,
				   u32 page_num, u8 *buffer, u32 buffer_size);

int rtw_hal_get_rsvd_page(struct _adapter *adapter, u32 page_offset,
			  u32 page_num, u8 *buffer, u32 buffer_size)
{
	return rtw_hal_get_rsvd_page_c_oracle(adapter, page_offset, page_num,
					      buffer, buffer_size);
}