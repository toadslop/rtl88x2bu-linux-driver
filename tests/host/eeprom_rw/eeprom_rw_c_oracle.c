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

void read_eeprom_content(struct host_eeprom_adapter *padapter)
{
	(void)padapter;
}
