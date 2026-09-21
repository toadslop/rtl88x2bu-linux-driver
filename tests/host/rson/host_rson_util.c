// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"

u8 key_2char2num(u8 hch, u8 lch)
{
	hch = (hch >= 'a') ? (hch - 'a' + 10) : (hch - '0');
	lch = (lch >= 'a') ? (lch - 'a' + 10) : (lch - '0');
	return (hch << 4) | lch;
}
