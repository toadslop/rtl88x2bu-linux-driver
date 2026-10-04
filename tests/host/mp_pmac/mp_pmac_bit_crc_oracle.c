// SPDX-License-Identifier: GPL-2.0
/* C oracle — ByteToBit / CRC16 / CRC8 (W3-113 PR1). */
#include "host_mp_pmac_types.h"

void ByteToBit(u8 *out, bool *in, u8 in_size)
{
	u8 i = 0, j = 0;

	for (i = 0; i < in_size; i++) {
		for (j = 0; j < 8; j++) {
			if (in[8 * i + j])
				out[i] |= (1 << j);
		}
	}
}

void CRC16_generator(bool *out, bool *in, u8 in_size)
{
	u8 i = 0;
	bool temp = 0, reg[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	for (i = 0; i < in_size; i++) {
		temp = in[i] ^ reg[15];
		reg[15] = reg[14];
		reg[14] = reg[13];
		reg[13] = reg[12];
		reg[12] = reg[11];
		reg[11] = reg[10];
		reg[10] = reg[9];
		reg[9] = reg[8];
		reg[8] = reg[7];
		reg[7] = reg[6];
		reg[6] = reg[5];
		reg[5] = reg[4];
		reg[4] = reg[3];
		reg[3] = reg[2];
		reg[2] = reg[1];
		reg[1] = reg[0];
		reg[12] = reg[12] ^ temp;
		reg[5] = reg[5] ^ temp;
		reg[0] = temp;
	}
	for (i = 0; i < 16; i++)
		out[i] = 1 - reg[15 - i];
}

void CRC8_generator(bool *out, bool *in, u8 in_size)
{
	u8 i = 0;
	bool temp = 0, reg[] = {1, 1, 1, 1, 1, 1, 1, 1};

	for (i = 0; i < in_size; i++) {
		temp = in[i] ^ reg[7];
		reg[7] = reg[6];
		reg[6] = reg[5];
		reg[5] = reg[4];
		reg[4] = reg[3];
		reg[3] = reg[2];
		reg[2] = reg[1] ^ temp;
		reg[1] = reg[0] ^ temp;
		reg[0] = temp;
	}
	for (i = 0; i < 8; i++)
		out[i] = reg[7 - i] ^ 1;
}
