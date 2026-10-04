// SPDX-License-Identifier: GPL-2.0
#include "host_eeprom_types.h"

#ifndef BIT
#define BIT(x) (1u << (x))
#endif

#define _EESK BIT(2)
#define _EECS BIT(3)
#define EE_9346CR 0x000A
#define CLOCK_RATE 50

static struct host_eeprom_adapter g_ad;
static u8 g_reg;
static int g_writes;

static u8 rtw_read8(struct host_eeprom_adapter *p, u32 a)
{
	(void)p;
	(void)a;
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

void host_eeprom_reset(void)
{
	g_ad.surprise_removed = 0;
	g_reg = 0;
	g_writes = 0;
}

void host_eeprom_set_reg(u8 v) { g_reg = v; }
u8 host_eeprom_reg(void) { return g_reg; }
int host_eeprom_write_count(void) { return g_writes; }
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
