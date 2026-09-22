/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SDIO_CMD_TYPES_H
#define HOST_SDIO_CMD_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define _FAIL 0
#define _SUCCESS 1
#define _TRUE 1
#define _FALSE 0
#define RTW_SDIO_ADDR_CMD52_GEN(a) ((a) | (1u << 17))
#define RTW_SDIO_ADDR_F0_GEN(a) ((a) | (1u << 18))
#define SD_IO_TRY_CNT 8

struct host_adapter { u8 surprise_removed; };
struct host_dvobj;
struct host_sdio_if_ops {
	int (*read)(struct host_dvobj *d, unsigned int addr, void *buf, size_t len, int fixed);
	int (*write)(struct host_dvobj *d, unsigned int addr, void *buf, size_t len, int fixed);
};

struct host_dvobj {
	struct host_adapter *primary_adapter;
	struct host_sdio_if_ops *intf_ops;
	int continual_io_error, io_fail_remaining;
	u8 read_fill;
};

void host_sdio_cmd_reset(void);
void host_sdio_cmd_set_surprise(u8 on);
u32 host_sdio_cmd_last_addr(void);
int host_sdio_cmd_io_count(void);
struct host_dvobj *host_sdio_cmd_dvobj(void);

u8 rtw_sdio_read_cmd52(struct host_dvobj *d, u32 addr, void *buf, size_t len);
u8 rtw_sdio_read_cmd53(struct host_dvobj *d, u32 addr, void *buf, size_t len);
u8 rtw_sdio_write_cmd52(struct host_dvobj *d, u32 addr, void *buf, size_t len);
u8 rtw_sdio_write_cmd53(struct host_dvobj *d, u32 addr, void *buf, size_t len);
u8 rtw_sdio_f0_read(struct host_dvobj *d, u32 addr, void *buf, size_t len);

#endif
