// SPDX-License-Identifier: GPL-2.0
#include "host_eeprom_types.h"

u8 rtw_read8(struct host_eeprom_adapter *p, u32 a);
void rtw_write8(struct host_eeprom_adapter *p, u32 a, u8 v);
void rtw_udelay_os(u32 us);
u8 rtw_is_surprise_removed(struct host_eeprom_adapter *p);

void eeprom_write16(struct host_eeprom_adapter *padapter, u16 reg, u16 data)
{
	u8 x;

	x = rtw_read8(padapter, EE_9346CR);
	x &= ~(_EEDI | _EEDO | _EESK | _EEM0);
	x |= _EEM1 | _EECS;
	rtw_write8(padapter, EE_9346CR, x);

	shift_out_bits(padapter, EEPROM_EWEN_OPCODE, 5);

	if (padapter->EepromAddressSize == 8)
		shift_out_bits(padapter, 0, 6);
	else
		shift_out_bits(padapter, 0, 4);

	standby(padapter);
	standby(padapter);

	shift_out_bits(padapter, EEPROM_WRITE_OPCODE, 3);
	shift_out_bits(padapter, reg, padapter->EepromAddressSize);
	shift_out_bits(padapter, data, 16);

	if (wait_eeprom_cmd_done(padapter) == _FALSE)
		goto exit;

	standby(padapter);

	shift_out_bits(padapter, EEPROM_EWDS_OPCODE, 5);
	shift_out_bits(padapter, reg, 4);

	eeprom_clean(padapter);
exit:
	return;
}

u16 eeprom_read16(struct host_eeprom_adapter *padapter, u16 reg)
{
	u16 x;
	u16 data = 0;

	if (rtw_is_surprise_removed(padapter))
		goto out;
	x = rtw_read8(padapter, EE_9346CR);

	if (rtw_is_surprise_removed(padapter))
		goto out;

	x &= ~(_EEDI | _EEDO | _EESK | _EEM0);
	x |= _EEM1 | _EECS;
	rtw_write8(padapter, EE_9346CR, (unsigned char)x);

	shift_out_bits(padapter, EEPROM_READ_OPCODE, 3);
	shift_out_bits(padapter, reg, padapter->EepromAddressSize);

	data = shift_in_bits(padapter);

	eeprom_clean(padapter);
out:
	return data;
}

void eeprom_read_sz(struct host_eeprom_adapter *padapter, u16 reg, u8 *data,
		    u32 sz)
{
	u16 x, data16;
	u32 i;

	if (rtw_is_surprise_removed(padapter))
		goto out;
	x = rtw_read8(padapter, EE_9346CR);

	if (rtw_is_surprise_removed(padapter))
		goto out;

	x &= ~(_EEDI | _EEDO | _EESK | _EEM0);
	x |= _EEM1 | _EECS;
	rtw_write8(padapter, EE_9346CR, (unsigned char)x);

	shift_out_bits(padapter, EEPROM_READ_OPCODE, 3);
	shift_out_bits(padapter, reg, padapter->EepromAddressSize);

	for (i = 0; i < sz; i += 2) {
		data16 = shift_in_bits(padapter);
		data[i] = data16 & 0xff;
		data[i + 1] = data16 >> 8;
	}

	eeprom_clean(padapter);
out:
	return;
}

u8 eeprom_read(struct host_eeprom_adapter *padapter, u32 addr_off, u8 sz,
	       u8 *rbuf)
{
	u8 quotient, remainder, addr_2align_odd;
	u16 reg, stmp, i = 0, idx = 0;

	reg = (u16)(addr_off >> 1);
	addr_2align_odd = (u8)(addr_off & 0x1);

	if (addr_2align_odd) {
		stmp = eeprom_read16(padapter, reg);
		rbuf[idx++] = (u8)((stmp >> 8) & 0xff);
		reg++;
		sz--;
	}

	quotient = sz >> 1;
	remainder = sz & 0x1;

	for (i = 0; i < quotient; i++) {
		stmp = eeprom_read16(padapter, reg + i);
		rbuf[idx++] = (u8)(stmp & 0xff);
		rbuf[idx++] = (u8)((stmp >> 8) & 0xff);
	}

	reg = reg + i;
	if (remainder) {
		stmp = eeprom_read16(padapter, reg);
		rbuf[idx] = (u8)(stmp & 0xff);
	}
	return _TRUE;
}

void read_eeprom_content(struct host_eeprom_adapter *padapter)
{
	(void)padapter;
}
