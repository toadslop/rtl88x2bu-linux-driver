// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle slice: core/rtw_security_rest.c AES-CCMP software primitives (W3-11).
 */
#include "host_security_types.h"

#if defined(HOST_CCMP_PRIMITIVE_ORACLE_BUILD)

typedef int sint;

static u8 sbox_table[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static void xor_128(u8 *a, u8 *b, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = a[i] ^ b[i];
}

static void xor_32(u8 *a, u8 *b, u8 *out)
{
	sint i;

	for (i = 0; i < 4; i++)
		out[i] = a[i] ^ b[i];
}

static u8 sbox(u8 a)
{
	return sbox_table[(sint)a];
}

static void next_key(u8 *key, sint round)
{
	u8 rcon;
	u8 sbox_key[4];
	u8 rcon_table[12] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
		0x1b, 0x36, 0x36, 0x36
	};

	sbox_key[0] = sbox(key[13]);
	sbox_key[1] = sbox(key[14]);
	sbox_key[2] = sbox(key[15]);
	sbox_key[3] = sbox(key[12]);

	rcon = rcon_table[round];

	xor_32(&key[0], sbox_key, &key[0]);
	key[0] = key[0] ^ rcon;

	xor_32(&key[4], &key[0], &key[4]);
	xor_32(&key[8], &key[4], &key[8]);
	xor_32(&key[12], &key[8], &key[12]);
}

static void byte_sub(u8 *in, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = sbox(in[i]);
}

static void shift_row(u8 *in, u8 *out)
{
	out[0] =  in[0];
	out[1] =  in[5];
	out[2] =  in[10];
	out[3] =  in[15];
	out[4] =  in[4];
	out[5] =  in[9];
	out[6] =  in[14];
	out[7] =  in[3];
	out[8] =  in[8];
	out[9] =  in[13];
	out[10] = in[2];
	out[11] = in[7];
	out[12] = in[12];
	out[13] = in[1];
	out[14] = in[6];
	out[15] = in[11];
}

static void mix_column(u8 *in, u8 *out)
{
	sint i;
	u8 add1b[4];
	u8 add1bf7[4];
	u8 rotl[4];
	u8 swap_halfs[4];
	u8 andf7[4];
	u8 rotr[4];
	u8 temp[4];
	u8 tempb[4];

	for (i = 0; i < 4; i++) {
		if ((in[i] & 0x80) == 0x80)
			add1b[i] = 0x1b;
		else
			add1b[i] = 0x00;
	}

	swap_halfs[0] = in[2];
	swap_halfs[1] = in[3];
	swap_halfs[2] = in[0];
	swap_halfs[3] = in[1];

	rotl[0] = in[3];
	rotl[1] = in[0];
	rotl[2] = in[1];
	rotl[3] = in[2];

	andf7[0] = in[0] & 0x7f;
	andf7[1] = in[1] & 0x7f;
	andf7[2] = in[2] & 0x7f;
	andf7[3] = in[3] & 0x7f;

	for (i = 3; i > 0; i--) {
		andf7[i] = andf7[i] << 1;
		if ((andf7[i - 1] & 0x80) == 0x80)
			andf7[i] = (andf7[i] | 0x01);
	}
	andf7[0] = andf7[0] << 1;
	andf7[0] = andf7[0] & 0xfe;

	xor_32(add1b, andf7, add1bf7);
	xor_32(in, add1bf7, rotr);

	temp[0] = rotr[0];
	rotr[0] = rotr[1];
	rotr[1] = rotr[2];
	rotr[2] = rotr[3];
	rotr[3] = temp[0];

	xor_32(add1bf7, rotr, temp);
	xor_32(swap_halfs, rotl, tempb);
	xor_32(temp, tempb, out);
}

static void aes128k128d(u8 *key, u8 *data, u8 *ciphertext)
{
	sint round;
	sint i;
	u8 intermediatea[16];
	u8 intermediateb[16];
	u8 round_key[16];

	for (i = 0; i < 16; i++)
		round_key[i] = key[i];

	for (round = 0; round < 11; round++) {
		if (round == 0) {
			xor_128(round_key, data, ciphertext);
			next_key(round_key, round);
		} else if (round == 10) {
			byte_sub(ciphertext, intermediatea);
			shift_row(intermediatea, intermediateb);
			xor_128(intermediateb, round_key, ciphertext);
		} else {
			byte_sub(ciphertext, intermediatea);
			shift_row(intermediatea, intermediateb);
			mix_column(&intermediateb[0], &intermediatea[0]);
			mix_column(&intermediateb[4], &intermediatea[4]);
			mix_column(&intermediateb[8], &intermediatea[8]);
			mix_column(&intermediateb[12], &intermediatea[12]);
			xor_128(intermediatea, round_key, ciphertext);
			next_key(round_key, round);
		}
	}
}

static void bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = ina[i] ^ inb[i];
}

void host_ccmp_aes128k128d(u8 *key, u8 *data, u8 *ciphertext)
{
	aes128k128d(key, data, ciphertext);
}

void host_ccmp_xor_128(u8 *a, u8 *b, u8 *out)
{
	xor_128(a, b, out);
}

void host_ccmp_xor_32(u8 *a, u8 *b, u8 *out)
{
	xor_32(a, b, out);
}

void host_ccmp_bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	bitwise_xor(ina, inb, out);
}

#endif /* HOST_CCMP_PRIMITIVE_ORACLE_BUILD */

#if defined(HOST_CCMP_CONSTRUCT_ORACLE_BUILD)

typedef int sint;
typedef unsigned int uint;

static void construct_mic_iv(
	u8 *mic_iv,
	sint qc_exists,
	sint a4_exists,
	u8 *mpdu,
	uint payload_length,
	u8 *pn_vector,
	uint frtype)
{
	sint i;

	mic_iv[0] = 0x59;
	if (qc_exists && a4_exists)
		mic_iv[1] = mpdu[30] & 0x0f;
	if (qc_exists && !a4_exists)
		mic_iv[1] = mpdu[24] & 0x0f;
	if (!qc_exists)
		mic_iv[1] = 0x00;
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	if (frtype == WIFI_MGT_TYPE)
		mic_iv[1] |= BIT(4);
#endif
	for (i = 2; i < 8; i++)
		mic_iv[i] = mpdu[i + 8];
#ifndef CONSISTENT_PN_ORDER
	for (i = 8; i < 14; i++)
		mic_iv[i] = pn_vector[13 - i];
#else
	for (i = 8; i < 14; i++)
		mic_iv[i] = pn_vector[i - 8];
#endif
	mic_iv[14] = (unsigned char)(payload_length / 256);
	mic_iv[15] = (unsigned char)(payload_length % 256);
}

static void construct_mic_header1(
	u8 *mic_header1,
	sint header_length,
	u8 *mpdu,
	uint frtype)
{
	mic_header1[0] = (u8)((header_length - 2) / 256);
	mic_header1[1] = (u8)((header_length - 2) % 256);
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	if (frtype == WIFI_MGT_TYPE)
		mic_header1[2] = mpdu[0];
	else
#endif
		mic_header1[2] = mpdu[0] & 0xcf;

	mic_header1[3] = mpdu[1] & 0xc7;
	mic_header1[4] = mpdu[4];
	mic_header1[5] = mpdu[5];
	mic_header1[6] = mpdu[6];
	mic_header1[7] = mpdu[7];
	mic_header1[8] = mpdu[8];
	mic_header1[9] = mpdu[9];
	mic_header1[10] = mpdu[10];
	mic_header1[11] = mpdu[11];
	mic_header1[12] = mpdu[12];
	mic_header1[13] = mpdu[13];
	mic_header1[14] = mpdu[14];
	mic_header1[15] = mpdu[15];
}

static void construct_mic_header2(
	u8 *mic_header2,
	u8 *mpdu,
	sint a4_exists,
	sint qc_exists)
{
	sint i;

	for (i = 0; i < 16; i++)
		mic_header2[i] = 0x00;

	mic_header2[0] = mpdu[16];
	mic_header2[1] = mpdu[17];
	mic_header2[2] = mpdu[18];
	mic_header2[3] = mpdu[19];
	mic_header2[4] = mpdu[20];
	mic_header2[5] = mpdu[21];
	mic_header2[6] = 0x00;
	mic_header2[7] = 0x00;

	if (!qc_exists && a4_exists) {
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i];
	}

	if (qc_exists && !a4_exists) {
		mic_header2[8] = mpdu[24] & 0x0f;
		mic_header2[9] = mpdu[25] & 0x00;
	}

	if (qc_exists && a4_exists) {
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i];

		mic_header2[14] = mpdu[30] & 0x0f;
		mic_header2[15] = mpdu[31] & 0x00;
	}
}

static void construct_ctr_preload(
	u8 *ctr_preload,
	sint a4_exists,
	sint qc_exists,
	u8 *mpdu,
	u8 *pn_vector,
	sint c,
	uint frtype)
{
	sint i;

	for (i = 0; i < 16; i++)
		ctr_preload[i] = 0x00;

	ctr_preload[0] = 0x01;
	if (qc_exists && a4_exists)
		ctr_preload[1] = mpdu[30] & 0x0f;
	if (qc_exists && !a4_exists)
		ctr_preload[1] = mpdu[24] & 0x0f;
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	if (frtype == WIFI_MGT_TYPE)
		ctr_preload[1] |= BIT(4);
#endif
	for (i = 2; i < 8; i++)
		ctr_preload[i] = mpdu[i + 8];
#ifndef CONSISTENT_PN_ORDER
	for (i = 8; i < 14; i++)
		ctr_preload[i] = pn_vector[13 - i];
#else
	for (i = 8; i < 14; i++)
		ctr_preload[i] = pn_vector[i - 8];
#endif
	ctr_preload[14] = (unsigned char)(c / 256);
	ctr_preload[15] = (unsigned char)(c % 256);
}

void host_ccmp_construct_mic_iv(
	u8 *mic_iv,
	int qc_exists,
	int a4_exists,
	u8 *mpdu,
	unsigned int payload_length,
	u8 *pn_vector,
	unsigned int frtype)
{
	construct_mic_iv(mic_iv, qc_exists, a4_exists, mpdu, payload_length,
			 pn_vector, frtype);
}

void host_ccmp_construct_mic_header1(
	u8 *mic_header1,
	int header_length,
	u8 *mpdu,
	unsigned int frtype)
{
	construct_mic_header1(mic_header1, header_length, mpdu, frtype);
}

void host_ccmp_construct_mic_header2(
	u8 *mic_header2,
	u8 *mpdu,
	int a4_exists,
	int qc_exists)
{
	construct_mic_header2(mic_header2, mpdu, a4_exists, qc_exists);
}

void host_ccmp_construct_ctr_preload(
	u8 *ctr_preload,
	int a4_exists,
	int qc_exists,
	u8 *mpdu,
	u8 *pn_vector,
	int c,
	unsigned int frtype)
{
	construct_ctr_preload(ctr_preload, a4_exists, qc_exists, mpdu, pn_vector,
			      c, frtype);
}

#endif /* HOST_CCMP_CONSTRUCT_ORACLE_BUILD */

#if defined(HOST_CCMP_FRAME_ORACLE_BUILD)

typedef int sint;

#define _SUCCESS 1
#define WLAN_HDR_A3_LEN 24
#define WLAN_HDR_A3_QOS_LEN 26
#define WLAN_HDR_A4_QOS_LEN 32

#define WIFI_DATA_CFACK (BIT(4) | WIFI_DATA_TYPE)
#define WIFI_DATA_CFPOLL (BIT(5) | WIFI_DATA_TYPE)
#define WIFI_DATA_CFACKPOLL (BIT(5) | BIT(4) | WIFI_DATA_TYPE)

static inline u16 host_le16_to_cpu(u16 v)
{
	return (u16)((v & 0xffU) << 8 | (v >> 8));
}

static unsigned int host_get_frame_type(u8 *pbuf)
{
	return host_le16_to_cpu(*(u16 *)pbuf) & (BIT(3) | BIT(2));
}

static unsigned int host_get_frame_sub_type(u8 *pbuf)
{
	return host_le16_to_cpu(*(u16 *)pbuf) &
	       (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2));
}

static void host_bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = ina[i] ^ inb[i];
}

static sint host_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe, unsigned int plen)
{
	unsigned int qc_exists, a4_exists, i, j, payload_remainder, num_blocks,
		payload_index;
	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];
	u8 mic[8];
	unsigned int frtype = host_get_frame_type(pframe);
	unsigned int frsubtype = host_get_frame_sub_type(pframe);

	frsubtype >>= 4;

	_rtw_memset(mic_iv, 0, 16);
	_rtw_memset(mic_header1, 0, 16);
	_rtw_memset(mic_header2, 0, 16);
	_rtw_memset(ctr_preload, 0, 16);
	_rtw_memset(chain_buffer, 0, 16);
	_rtw_memset(aes_out, 0, 16);
	_rtw_memset(padded_buffer, 0, 16);

	if (hdrlen == WLAN_HDR_A3_LEN || hdrlen == WLAN_HDR_A3_QOS_LEN)
		a4_exists = 0;
	else
		a4_exists = 1;

	if (((frtype | frsubtype) == WIFI_DATA_CFACK) ||
	    ((frtype | frsubtype) == WIFI_DATA_CFPOLL) ||
	    ((frtype | frsubtype) == WIFI_DATA_CFACKPOLL)) {
		qc_exists = 1;
		if (hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN)
			hdrlen += 2;
	} else if (frtype == WIFI_DATA_TYPE &&
		   (frsubtype == 0x08 || frsubtype == 0x09 || frsubtype == 0x0a ||
		    frsubtype == 0x0b)) {
		if (hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN)
			hdrlen += 2;
		qc_exists = 1;
	} else {
		qc_exists = 0;
	}

	pn_vector[0] = pframe[hdrlen];
	pn_vector[1] = pframe[hdrlen + 1];
	pn_vector[2] = pframe[hdrlen + 4];
	pn_vector[3] = pframe[hdrlen + 5];
	pn_vector[4] = pframe[hdrlen + 6];
	pn_vector[5] = pframe[hdrlen + 7];

	construct_mic_iv(mic_iv, qc_exists, a4_exists, pframe, plen, pn_vector,
			 frtype);
	construct_mic_header1(mic_header1, hdrlen, pframe, frtype);
	construct_mic_header2(mic_header2, pframe, a4_exists, qc_exists);

	payload_remainder = plen % 16;
	num_blocks = plen / 16;
	payload_index = hdrlen + 8;

	aes128k128d(key, mic_iv, aes_out);
	host_bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	host_bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);

	for (i = 0; i < num_blocks; i++) {
		host_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);
		payload_index += 16;
		aes128k128d(key, chain_buffer, aes_out);
	}

	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = pframe[payload_index++];
		host_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes128k128d(key, chain_buffer, aes_out);
	}

	for (j = 0; j < 8; j++)
		mic[j] = aes_out[j];

	for (j = 0; j < 8; j++)
		pframe[payload_index + j] = mic[j];

	payload_index = hdrlen + 8;
	for (i = 0; i < num_blocks; i++) {
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe,
				      pn_vector, i + 1, frtype);
		aes128k128d(key, ctr_preload, aes_out);
		host_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);
		for (j = 0; j < 16; j++)
			pframe[payload_index++] = chain_buffer[j];
	}

	if (payload_remainder > 0) {
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe,
				      pn_vector, num_blocks + 1, frtype);
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = pframe[payload_index + j];
		aes128k128d(key, ctr_preload, aes_out);
		host_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++)
			pframe[payload_index++] = chain_buffer[j];
	}

	construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector,
			      0, frtype);
	for (j = 0; j < 16; j++)
		padded_buffer[j] = 0x00;
	for (j = 0; j < 8; j++)
		padded_buffer[j] = pframe[j + hdrlen + 8 + plen];

	aes128k128d(key, ctr_preload, aes_out);
	host_bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++)
		pframe[payload_index++] = chain_buffer[j];

	return _SUCCESS;
}

sint host_ccmp_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe,
			  unsigned int plen)
{
	return host_aes_cipher(key, hdrlen, pframe, plen);
}

#endif /* HOST_CCMP_FRAME_ORACLE_BUILD */
