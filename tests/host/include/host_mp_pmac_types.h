/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for rtw_mp PMAC sig generators (W3-113).
 * Rate / PMAC struct definitions expand in PR2+ generator oracles.
 */
#ifndef HOST_MP_PMAC_TYPES_H
#define HOST_MP_PMAC_TYPES_H

#include <stdbool.h>

#include "host_types.h"

void ByteToBit(u8 *out, bool *in, u8 in_size);
void CRC16_generator(bool *out, bool *in, u8 in_size);
void CRC8_generator(bool *out, bool *in, u8 in_size);

#endif /* HOST_MP_PMAC_TYPES_H */
