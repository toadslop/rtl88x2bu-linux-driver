/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_EEPROM_TYPES_H
#define HOST_EEPROM_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct host_eeprom_adapter { u8 surprise_removed; };

void host_eeprom_reset(void);
void host_eeprom_set_reg(u8 v);
u8 host_eeprom_reg(void);
int host_eeprom_write_count(void);
struct host_eeprom_adapter *host_eeprom_adapter(void);

void up_clk(struct host_eeprom_adapter *padapter, u16 *x);
void down_clk(struct host_eeprom_adapter *padapter, u16 *x);
void standby(struct host_eeprom_adapter *padapter);

#endif
