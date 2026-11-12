// SPDX-License-Identifier: GPL-2.0
/* FX-19: c2h_evt_read_88xx clamps PLEN 0xFF to 12 payload bytes (host L2). */

#include <stdio.h>
#include <string.h>

typedef unsigned char u8;

#define REG_MSG 0x01a0
#define REG_SEQ 0x01a1
#define REG_PLEN 0x01ae
#define REG_CLEAR 0x01af
#define C2H_HOST_CLOSE 0x00
#define C2H_FW_CLOSE 0xff
#define C2H_REG_LEN 16
#define C2H_PAYLOAD_MAX 12

static struct {
	u8 trigger, id, seq, plen, payload;
} mmio;

extern u8 rtw_rust_hal_com_read8(void *a, unsigned int addr);
extern void rtw_rust_hal_com_write8(void *a, unsigned int addr, u8 v);
extern int c2h_evt_read_88xx(void *a, u8 *buf);

u8 rtw_rust_hal_com_read8(void *a, unsigned int addr)
{
	(void)a;
	if (addr == REG_CLEAR)
		return mmio.trigger;
	if (addr == REG_MSG)
		return mmio.id;
	if (addr == REG_SEQ)
		return mmio.seq;
	if (addr == REG_PLEN)
		return mmio.plen;
	if (addr >= REG_MSG + 2)
		return mmio.payload;
	return 0;
}

void rtw_rust_hal_com_write8(void *a, unsigned int addr, u8 v)
{
	(void)a;
	if (addr == REG_CLEAR)
		mmio.trigger = v;
}

static void prime(u8 plen)
{
	mmio.trigger = C2H_FW_CLOSE;
	mmio.id = 0x5a;
	mmio.seq = 0xa5;
	mmio.plen = plen;
	mmio.payload = 0x3c;
}

int main(void)
{
	u8 buf[C2H_REG_LEN];
	unsigned int i;

	prime(0xff);
	memset(buf, 0xaa, sizeof(buf));
	if (c2h_evt_read_88xx((void *)1, buf) != 1) {
		fputs("c2h read failed\n", stderr);
		return 1;
	}
	if (buf[14] != 0xff || buf[15] != 0) {
		fputs("header bytes wrong\n", stderr);
		return 1;
	}
	for (i = 2; i < 2 + C2H_PAYLOAD_MAX; i++) {
		if (buf[i] != 0x3c) {
			fputs("payload clamp mismatch\n", stderr);
			return 1;
		}
	}
	puts("ok hal_com_c2h_evt_fx19");
	return 0;
}
