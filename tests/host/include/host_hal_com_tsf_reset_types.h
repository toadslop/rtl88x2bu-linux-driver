/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for W4-143 TSF reset H2C offload L2 harness. */

#ifndef HOST_HAL_COM_TSF_RESET_TYPES_H
#define HOST_HAL_COM_TSF_RESET_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct _adapter;

#define H2C_RESET_TSF 0xC0
#define REG_FW_RESET_TSF_CNT_0 0x05FD
#define REG_FW_RESET_TSF_CNT_1 0x05FC
#define HW_PORT0 0
#define _SUCCESS 1
#define _FAIL 0
#define _FALSE 0
#define HOST_TSF_RESET_BWAIT_UNCALLED 0xFF

s32 rtw_hal_reset_tsf(struct _adapter *adapter, u8 reset_port);

#endif