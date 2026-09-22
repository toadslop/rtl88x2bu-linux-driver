// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_eeprom_types.h"

static struct host_eeprom_adapter g_ad;
static u8 g_reg;
static int g_writes;
static u8 g_read_seq[64];
static int g_read_seq_len, g_read_seq_i;

static u8 rtw_read8(struct host_eeprom_adapter *p, u32 a)
{
	(void)p;
	(void)a;
	if (g_read_seq_len > 0)
		return g_read_seq[g_read_seq_i++ % g_read_seq_len];
	return g_reg;
}

static void rtw_write8(struct host_eeprom_adapter *p, u32 a, u8 v)
{
	(void)p;
	(void)a;
	g_reg = v;
	g_writes++;
}

static void rtw_udelay_os(u32 us) { (void)us; }
static u8 rtw_is_surprise_removed(struct host_eeprom_adapter *p)
{
	return p->surprise_removed;
}

void host_eeprom_reset(void)
{
	memset(&g_ad, 0, sizeof(g_ad));
	g_reg = 0;
	g_writes = g_read_seq_len = g_read_seq_i = 0;
}

void host_eeprom_set_surprise(u8 on) { g_ad.surprise_removed = on ? 1 : 0; }
void host_eeprom_set_reg(u8 v) { g_reg = v; }
u8 host_eeprom_reg(void) { return g_reg; }
int host_eeprom_write_count(void) { return g_writes; }

void host_eeprom_set_read_sequence(const u8 *vals, int n)
{
	g_read_seq_len = g_read_seq_i = 0;
	if (!vals || n <= 0)
		return;
	if (n > (int)sizeof(g_read_seq))
		n = (int)sizeof(g_read_seq);
	memcpy(g_read_seq, vals, (size_t)n);
	g_read_seq_len = n;
}

struct host_eeprom_adapter *host_eeprom_adapter(void) { return &g_ad; }

void up_clk(struct host_eeprom_adapter *padapter, u16 *x)
{
	*x = *x | _EESK;
	rtw_write8(padapter, EE_9346CR, (u8)*x);
	rtw_udelay_os(CLOCK_RATE);
}

void down_clk(struct host_eeprom_adapter *padapter, u16 *x)
{
	*x = *x & ~_EESK;
	rtw_write8(padapter, EE_9346CR, (u8)*x);
	rtw_udelay_os(CLOCK_RATE);
}

void shift_out_bits(struct host_eeprom_adapter *padapter, u16 data, u16 count)
{
	u16 x, mask;

	if (rtw_is_surprise_removed(padapter))
		return;
	mask = 0x01 << (count - 1);
	x = rtw_read8(padapter, EE_9346CR);
	x &= ~(_EEDO | _EEDI);
	do {
		x &= ~_EEDI;
		if (data & mask)
			x |= _EEDI;
		if (rtw_is_surprise_removed(padapter))
			return;
		rtw_write8(padapter, EE_9346CR, (u8)x);
		rtw_udelay_os(CLOCK_RATE);
		up_clk(padapter, &x);
		down_clk(padapter, &x);
		mask >>= 1;
	} while (mask);
	if (rtw_is_surprise_removed(padapter))
		return;
	x &= ~_EEDI;
	rtw_write8(padapter, EE_9346CR, (u8)x);
}

u16 shift_in_bits(struct host_eeprom_adapter *padapter)
{
	u16 x, d = 0, i;

	if (rtw_is_surprise_removed(padapter))
		return d;
	x = rtw_read8(padapter, EE_9346CR);
	x &= ~(_EEDO | _EEDI);
	for (i = 0; i < 16; i++) {
		d <<= 1;
		up_clk(padapter, &x);
		if (rtw_is_surprise_removed(padapter))
			return d;
		x = rtw_read8(padapter, EE_9346CR);
		x &= ~_EEDI;
		if (x & _EEDO)
			d |= 1;
		down_clk(padapter, &x);
	}
	return d;
}

void standby(struct host_eeprom_adapter *padapter)
{
	u8 x = rtw_read8(padapter, EE_9346CR);

	x &= ~(_EECS | _EESK);
	rtw_write8(padapter, EE_9346CR, x);
	rtw_udelay_os(CLOCK_RATE);
	x |= _EECS;
	rtw_write8(padapter, EE_9346CR, x);
	rtw_udelay_os(CLOCK_RATE);
}

u16 wait_eeprom_cmd_done(struct host_eeprom_adapter *padapter)
{
	u8 x;
	u16 i;

	standby(padapter);
	for (i = 0; i < 200; i++) {
		x = rtw_read8(padapter, EE_9346CR);
		if (x & _EEDO)
			return _TRUE;
		rtw_udelay_os(CLOCK_RATE);
	}
	return _FALSE;
}

void eeprom_clean(struct host_eeprom_adapter *padapter)
{
	u16 x;

	if (rtw_is_surprise_removed(padapter))
		return;
	x = rtw_read8(padapter, EE_9346CR);
	if (rtw_is_surprise_removed(padapter))
		return;
	x &= ~(_EECS | _EEDI);
	rtw_write8(padapter, EE_9346CR, (u8)x);
	if (rtw_is_surprise_removed(padapter))
		return;
	up_clk(padapter, &x);
	if (rtw_is_surprise_removed(padapter))
		return;
	down_clk(padapter, &x);
}
