// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_sdio_cmd_types.h"

static struct host_adapter g_ad;
static struct host_dvobj g_d;
static struct host_sdio_if_ops g_ops;
static u32 g_last_addr;
static int g_io_count;

static int mock_read(struct host_dvobj *d, unsigned int addr, void *buf, size_t len, int fixed)
{
	(void)fixed;
	if (d->io_fail_remaining > 0) {
		d->io_fail_remaining--;
		return 1;
	}
	g_last_addr = addr;
	g_io_count++;
	memset(buf, d->read_fill, len);
	return 0;
}

static int mock_write(struct host_dvobj *d, unsigned int addr, void *buf, size_t len, int fixed)
{
	(void)buf;
	(void)len;
	(void)fixed;
	if (d->io_fail_remaining > 0) {
		d->io_fail_remaining--;
		return 1;
	}
	g_last_addr = addr;
	g_io_count++;
	return 0;
}

static u8 sdio_io(struct host_dvobj *d, u32 addr, void *buf, size_t len, u8 write, u8 cmd52)
{
	u32 addr_drv = cmd52 ? RTW_SDIO_ADDR_CMD52_GEN(addr) : addr;
	int err;
	u8 retry = 0;

	if (g_ad.surprise_removed)
		return _FAIL;

	do {
		err = write ? d->intf_ops->write(d, addr_drv, buf, len, 0)
			    : d->intf_ops->read(d, addr_drv, buf, len, 0);
		if (!err) {
			d->continual_io_error = 0;
			break;
		}
		retry++;
		if (++d->continual_io_error > SD_IO_TRY_CNT || retry > SD_IO_TRY_CNT)
			return _FAIL;
		if ((addr & 0x10000) || !(addr & 0xE000))
			continue;
		return _FAIL;
	} while (1);
	return _SUCCESS;
}

u8 rtw_sdio_read_cmd52(struct host_dvobj *d, u32 addr, void *buf, size_t len)
{ return sdio_io(d, addr, buf, len, 0, 1); }
u8 rtw_sdio_read_cmd53(struct host_dvobj *d, u32 addr, void *buf, size_t len)
{ return sdio_io(d, addr, buf, len, 0, 0); }
u8 rtw_sdio_write_cmd52(struct host_dvobj *d, u32 addr, void *buf, size_t len)
{ return sdio_io(d, addr, buf, len, 1, 1); }
u8 rtw_sdio_write_cmd53(struct host_dvobj *d, u32 addr, void *buf, size_t len)
{ return sdio_io(d, addr, buf, len, 1, 0); }

u8 rtw_sdio_f0_read(struct host_dvobj *d, u32 addr, void *buf, size_t len)
{
	addr = RTW_SDIO_ADDR_F0_GEN(addr);
	return d->intf_ops->read(d, addr, buf, len, 0) ? _FAIL : _SUCCESS;
}

void host_sdio_cmd_reset(void)
{
	memset(&g_ad, 0, sizeof(g_ad));
	memset(&g_d, 0, sizeof(g_d));
	g_ops.read = mock_read;
	g_ops.write = mock_write;
	g_d.primary_adapter = &g_ad;
	g_d.intf_ops = &g_ops;
	g_d.read_fill = 0xA5;
	g_last_addr = g_io_count = 0;
}

void host_sdio_cmd_set_surprise(u8 on) { g_ad.surprise_removed = on ? 1 : 0; }
u32 host_sdio_cmd_last_addr(void) { return g_last_addr; }
int host_sdio_cmd_io_count(void) { return g_io_count; }
struct host_dvobj *host_sdio_cmd_dvobj(void) { return &g_d; }
