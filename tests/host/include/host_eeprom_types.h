/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_EEPROM_TYPES_H
#define HOST_EEPROM_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define _TRUE 1
#define _FALSE 0
#define _EEDO 1u
#define _EEDI 2u
#define _EESK 4u
#define _EECS 8u
#define EE_9346CR 0x000A
#define CLOCK_RATE 50

#define _EEM0 0x40u
#define _EEM1 0x80u

#define EEPROM_READ_OPCODE 6
#define EEPROM_WRITE_OPCODE 5
#define EEPROM_EWEN_OPCODE 19
#define EEPROM_EWDS_OPCODE 16

struct host_eeprom_adapter {
	u8 surprise_removed;
	u8 EepromAddressSize;
};

void host_eeprom_reset(void);
void host_eeprom_set_surprise(u8 on);
void host_eeprom_set_addr_size(u8 bits);
void host_eeprom_set_reg(u8 v);
u8 host_eeprom_reg(void);
int host_eeprom_write_count(void);
void host_eeprom_set_read_sequence(const u8 *vals, int n);
struct host_eeprom_adapter *host_eeprom_adapter(void);

void up_clk(struct host_eeprom_adapter *padapter, u16 *x);
void down_clk(struct host_eeprom_adapter *padapter, u16 *x);
void shift_out_bits(struct host_eeprom_adapter *padapter, u16 data, u16 count);
u16 shift_in_bits(struct host_eeprom_adapter *padapter);
void standby(struct host_eeprom_adapter *padapter);
u16 wait_eeprom_cmd_done(struct host_eeprom_adapter *padapter);
void eeprom_clean(struct host_eeprom_adapter *padapter);

void eeprom_write16(struct host_eeprom_adapter *padapter, u16 reg, u16 data);
u16 eeprom_read16(struct host_eeprom_adapter *padapter, u16 reg);
void eeprom_read_sz(struct host_eeprom_adapter *padapter, u16 reg, u8 *data,
		    u32 sz);
u8 eeprom_read(struct host_eeprom_adapter *padapter, u32 addr_off, u8 sz,
	       u8 *rbuf);
void read_eeprom_content(struct host_eeprom_adapter *padapter);

#endif
