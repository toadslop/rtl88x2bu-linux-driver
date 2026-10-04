// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_sdio_cmd_types.h"

static struct host_adapter g_ad;
static struct host_dvobj g_d;
static struct host_sdio_if_ops g_ops;
static u32 g_last_addr;
static int g_io_count;

static int mock_io_err(struct host_dvobj *d)
{
	int err = d->io_mock_err ? d->io_mock_err : 1;

	g_io_count++;
	d->io_fail_remaining--;
	return err;
}

static int mock_read(struct host_dvobj *d, unsigned int addr, void *buf, size_t len, int fixed)
{
	(void)fixed;
	if (d->io_fail_remaining > 0)
		return mock_io_err(d);
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
	if (d->io_fail_remaining > 0)
		return mock_io_err(d);
	g_last_addr = addr;
	g_io_count++;
	return 0;
}

static u8 host_inc_and_chk_continual_io_error(struct host_dvobj *d)
{
	d->continual_io_error++;
	return d->continual_io_error > MAX_CONTINUAL_IO_ERR ? _TRUE : _FALSE;
}

static u8 sdio_io(struct host_dvobj *d, u32 addr, void *buf, size_t len, u8 write, u8 cmd52)
{
	u32 addr_drv = cmd52 ? RTW_SDIO_ADDR_CMD52_GEN(addr) : addr;
	int err;
	u8 retry = 0;
	u8 stop_retry = _FALSE;

	if (d->primary_adapter && d->primary_adapter->surprise_removed)
		return _FAIL;

	do {
		err = write ? d->intf_ops->write(d, addr_drv, buf, len, 0)
			    : d->intf_ops->read(d, addr_drv, buf, len, 0);
		if (!err) {
			d->continual_io_error = 0;
			break;
		}
		retry++;
		stop_retry = host_inc_and_chk_continual_io_error(d);
		if ((err == -1) || (stop_retry == _TRUE) || (retry > SD_IO_TRY_CNT)) {
			host_sdio_cmd_set_surprise(1);
			return _FAIL;
		}
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
	g_d.io_mock_err = 1;
	g_last_addr = g_io_count = 0;
}

void host_sdio_cmd_set_surprise(u8 on) { g_ad.surprise_removed = on ? 1 : 0; }
u8 host_sdio_cmd_surprise(void) { return g_ad.surprise_removed; }
u32 host_sdio_cmd_last_addr(void) { return g_last_addr; }
int host_sdio_cmd_io_count(void) { return g_io_count; }
struct host_dvobj *host_sdio_cmd_dvobj(void) { return &g_d; }
