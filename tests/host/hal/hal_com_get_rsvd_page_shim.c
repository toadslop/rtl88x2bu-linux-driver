// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
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
	memset(buffer, (u8)(addr & 0xff), size);
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