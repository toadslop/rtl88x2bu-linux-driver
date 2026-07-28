/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define  _RTW_SECURITY_C_

#ifdef HOST_SECURITY_TEST
#include "host_security_types.h"
#else
#include <drv_types.h>
#include <rtw_swcrypto.h>
#endif

#ifdef DBG_SW_SEC_CNT
#define WEP_SW_ENC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->wep_sw_enc_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->wep_sw_enc_cnt_mc++; \
	else \
		sec->wep_sw_enc_cnt_uc++; \
	} while (0)

#define WEP_SW_DEC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->wep_sw_dec_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->wep_sw_dec_cnt_mc++; \
	else \
		sec->wep_sw_dec_cnt_uc++; \
	} while (0)

#define TKIP_SW_ENC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->tkip_sw_enc_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->tkip_sw_enc_cnt_mc++; \
	else \
		sec->tkip_sw_enc_cnt_uc++; \
	} while (0)

#define TKIP_SW_DEC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->tkip_sw_dec_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->tkip_sw_dec_cnt_mc++; \
	else \
		sec->tkip_sw_dec_cnt_uc++; \
	} while (0)

#define AES_SW_ENC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->aes_sw_enc_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->aes_sw_enc_cnt_mc++; \
	else \
		sec->aes_sw_enc_cnt_uc++; \
	} while (0)

#define AES_SW_DEC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->aes_sw_dec_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->aes_sw_dec_cnt_mc++; \
	else \
		sec->aes_sw_dec_cnt_uc++; \
	} while (0)

#define GCMP_SW_ENC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->gcmp_sw_enc_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->gcmp_sw_enc_cnt_mc++; \
	else \
		sec->gcmp_sw_enc_cnt_uc++; \
	} while (0)

#define GCMP_SW_DEC_CNT_INC(sec, ra) do {\
	if (is_broadcast_mac_addr(ra)) \
		sec->gcmp_sw_dec_cnt_bc++; \
	else if (is_multicast_mac_addr(ra)) \
		sec->gcmp_sw_dec_cnt_mc++; \
	else \
		sec->gcmp_sw_dec_cnt_uc++; \
	} while (0)
#else
#define WEP_SW_ENC_CNT_INC(sec, ra)
#define WEP_SW_DEC_CNT_INC(sec, ra)
#define TKIP_SW_ENC_CNT_INC(sec, ra)
#define TKIP_SW_DEC_CNT_INC(sec, ra)
#define AES_SW_ENC_CNT_INC(sec, ra)
#define AES_SW_DEC_CNT_INC(sec, ra)
#define GCMP_SW_ENC_CNT_INC(sec, ra)
#define GCMP_SW_DEC_CNT_INC(sec, ra)
#endif /* DBG_SW_SEC_CNT */

/* *****WEP related*****
 *
 * RC4 (arcfour_*) implements legacy 802.11 WEP40/WEP104 and TKIP keystream
 * generation per IEEE 802.11-1999/2003.  WEP is cryptographically broken; these
 * paths exist only for interoperability with peers that still require legacy
 * ciphers.  Modern associations use AES/CCMP/GCMP (migrated to Rust elsewhere).
 * W3-05: arcfour_* and getcrc32 live in rust/rtw_security.rs.
 */

/* Expands to a CodeQL suppression for the following line (legacy WEP/TKIP RC4). */
#define RTW_CODEQL_SUPPRESS_WEAK_RC4 /* codeql[cpp/weak-cryptographic-algorithm]: Legacy 802.11 WEP/TKIP RC4 (IEEE 802.11-1999/2003); interoperability-only — see .github/codeql/codeql-config.yml. */

struct arc4context {
	u32 x;
	u32 y;
	u8 state[256];
};

extern void arcfour_init(struct arc4context *parc4ctx, u8 *key, u32 key_len);
extern void arcfour_encrypt(struct arc4context *parc4ctx, u8 *dest, u8 *src, u32 len);
extern u32 getcrc32(u8 *buf, sint len);

/* W3-06: rtw_wep_encrypt/decrypt in rust/rtw_security.rs */
extern void rtw_wep_encrypt(_adapter *padapter, u8 *pxmitframe);
extern void rtw_wep_decrypt(_adapter *padapter, u8 *precvframe);

#ifdef CONFIG_RUST
#include <linux/types.h>
const size_t rtw_rust_wep_off_adapter_securitypriv = offsetof(_adapter, securitypriv);
const size_t rtw_rust_wep_off_adapter_xmitpriv = offsetof(_adapter, xmitpriv);
const size_t rtw_rust_wep_off_xmitpriv_frag_len = offsetof(struct xmit_priv, frag_len);
const size_t rtw_rust_wep_off_xmit_frame_attrib = offsetof(struct xmit_frame, attrib);
const size_t rtw_rust_wep_off_xmit_frame_buf_addr = offsetof(struct xmit_frame, buf_addr);
const size_t rtw_rust_wep_off_xmit_frame_pkt_offset = offsetof(struct xmit_frame, pkt_offset);
const size_t rtw_rust_wep_off_recv_frame_hdr = offsetof(union recv_frame, u.hdr);
const size_t rtw_rust_wep_off_recv_frame_hdr_attrib =
	offsetof(struct recv_frame_hdr, attrib);
const size_t rtw_rust_wep_off_recv_frame_hdr_len =
	offsetof(struct recv_frame_hdr, len);
const size_t rtw_rust_wep_off_recv_frame_hdr_rx_data =
	offsetof(struct recv_frame_hdr, rx_data);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_bc =
	offsetof(struct security_priv, wep_sw_enc_cnt_bc);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_mc =
	offsetof(struct security_priv, wep_sw_enc_cnt_mc);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_uc =
	offsetof(struct security_priv, wep_sw_enc_cnt_uc);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_bc =
	offsetof(struct security_priv, wep_sw_dec_cnt_bc);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_mc =
	offsetof(struct security_priv, wep_sw_dec_cnt_mc);
const size_t rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_uc =
	offsetof(struct security_priv, wep_sw_dec_cnt_uc);
const size_t rtw_rust_tkip_off_securitypriv_dot118021XGrpKeyid =
	offsetof(struct security_priv, dot118021XGrpKeyid);
const size_t rtw_rust_tkip_off_securitypriv_dot118021XGrpKey =
	offsetof(struct security_priv, dot118021XGrpKey);
const size_t rtw_rust_tkip_off_pkt_attrib_dot118021x_UncstKey =
	offsetof(struct pkt_attrib, dot118021x_UncstKey);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_bc =
	offsetof(struct security_priv, tkip_sw_enc_cnt_bc);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_mc =
	offsetof(struct security_priv, tkip_sw_enc_cnt_mc);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_uc =
	offsetof(struct security_priv, tkip_sw_enc_cnt_uc);
const size_t rtw_rust_tkip_off_securitypriv_binstallGrpkey =
	offsetof(struct security_priv, binstallGrpkey);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_bc =
	offsetof(struct security_priv, tkip_sw_dec_cnt_bc);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_mc =
	offsetof(struct security_priv, tkip_sw_dec_cnt_mc);
const size_t rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_uc =
	offsetof(struct security_priv, tkip_sw_dec_cnt_uc);
const size_t rtw_rust_tkip_off_adapter_stapriv =
	offsetof(_adapter, stapriv);
const size_t rtw_rust_tkip_off_sta_info_dot118021x_UncstKey =
	offsetof(struct sta_info, dot118021x_UncstKey);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_bc =
	offsetof(struct security_priv, aes_sw_enc_cnt_bc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_mc =
	offsetof(struct security_priv, aes_sw_enc_cnt_mc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_uc =
	offsetof(struct security_priv, aes_sw_enc_cnt_uc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_bc =
	offsetof(struct security_priv, aes_sw_dec_cnt_bc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_mc =
	offsetof(struct security_priv, aes_sw_dec_cnt_mc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_uc =
	offsetof(struct security_priv, aes_sw_dec_cnt_uc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_bc =
	offsetof(struct security_priv, gcmp_sw_enc_cnt_bc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_mc =
	offsetof(struct security_priv, gcmp_sw_enc_cnt_mc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_uc =
	offsetof(struct security_priv, gcmp_sw_enc_cnt_uc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_bc =
	offsetof(struct security_priv, gcmp_sw_dec_cnt_bc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_mc =
	offsetof(struct security_priv, gcmp_sw_dec_cnt_mc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_uc =
	offsetof(struct security_priv, gcmp_sw_dec_cnt_uc);
#endif

/* 3		=====TKIP related===== (W3-07a: MIC helpers in rust/rtw_security.rs) */

extern void rtw_secmicsetkey(struct mic_data *pmicdata, u8 *key);
extern void rtw_secmicappendbyte(struct mic_data *pmicdata, u8 b);
extern void rtw_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes);
extern void rtw_secgetmic(struct mic_data *pmicdata, u8 *dst);
extern void rtw_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len,
			       u8 *mic_code, u8 pri);

/* TKIP phase1/phase2 (W3-07b: in rust/rtw_security.rs) */
extern void phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32);
extern void phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16);

/* W3-10a: rtw_tkip_encrypt in rust/rtw_security.rs */
extern u32 rtw_tkip_encrypt(_adapter *padapter, u8 *pxmitframe);
/* W3-10d: rtw_tkip_decrypt in rust/rtw_security.rs */
extern u32 rtw_tkip_decrypt(_adapter *padapter, u8 *precvframe);

/*
 * Multicast/broadcast group-key readiness check with rate-limited dmesg
 * diagnostics (restores legacy C rtw_tkip_decrypt logging).
 * Returns _TRUE when decrypt should fail (group key not installed).
 */
u8 rtw_tkip_decrypt_mcast_gkey_check(_adapter *padapter, u8 *ra, u8 grpkey_installed)
{
	static systime start = 0;
	static u32 no_gkey_bc_cnt = 0;
	static u32 no_gkey_mc_cnt = 0;

	if (grpkey_installed == _FALSE) {
		if (start == 0)
			start = rtw_get_current_time();

		if (is_broadcast_mac_addr(ra))
			no_gkey_bc_cnt++;
		else
			no_gkey_mc_cnt++;

		if (rtw_get_passing_time_ms(start) > 1000) {
			if (no_gkey_bc_cnt || no_gkey_mc_cnt) {
				RTW_PRINT(FUNC_ADPT_FMT" no_gkey_bc_cnt:%u, no_gkey_mc_cnt:%u\n",
					FUNC_ADPT_ARG(padapter), no_gkey_bc_cnt, no_gkey_mc_cnt);
			}
			start = rtw_get_current_time();
			no_gkey_bc_cnt = 0;
			no_gkey_mc_cnt = 0;
		}
		return _TRUE;
	}

	if (no_gkey_bc_cnt || no_gkey_mc_cnt) {
		RTW_PRINT(FUNC_ADPT_FMT" gkey installed. no_gkey_bc_cnt:%u, no_gkey_mc_cnt:%u\n",
			FUNC_ADPT_ARG(padapter), no_gkey_bc_cnt, no_gkey_mc_cnt);
	}
	start = 0;
	no_gkey_bc_cnt = 0;
	no_gkey_mc_cnt = 0;
	return _FALSE;
}

/*
 * GCMP multicast group-key readiness check with rate-limited dmesg diagnostics.
 * Separate static state from rtw_tkip_decrypt_mcast_gkey_check (GCMP legacy path).
 * Returns _TRUE when decrypt should fail (group key not installed).
 */
u8 rtw_gcmp_decrypt_mcast_gkey_check(_adapter *padapter, u8 *ra, u8 grpkey_installed)
{
	static systime start = 0;
	static u32 no_gkey_bc_cnt = 0;
	static u32 no_gkey_mc_cnt = 0;

	if (grpkey_installed == _FALSE) {
		if (start == 0)
			start = rtw_get_current_time();

		if (is_broadcast_mac_addr(ra))
			no_gkey_bc_cnt++;
		else
			no_gkey_mc_cnt++;

		if (rtw_get_passing_time_ms(start) > 1000) {
			if (no_gkey_bc_cnt || no_gkey_mc_cnt) {
				RTW_PRINT(FUNC_ADPT_FMT" no_gkey_bc_cnt:%u, no_gkey_mc_cnt:%u\n",
					FUNC_ADPT_ARG(padapter), no_gkey_bc_cnt, no_gkey_mc_cnt);
			}
			start = rtw_get_current_time();
			no_gkey_bc_cnt = 0;
			no_gkey_mc_cnt = 0;
		}
		return _TRUE;
	}

	if (no_gkey_bc_cnt || no_gkey_mc_cnt) {
		RTW_PRINT(FUNC_ADPT_FMT" gkey installed. no_gkey_bc_cnt:%u, no_gkey_mc_cnt:%u\n",
			FUNC_ADPT_ARG(padapter), no_gkey_bc_cnt, no_gkey_mc_cnt);
	}
	start = 0;
	no_gkey_bc_cnt = 0;
	no_gkey_mc_cnt = 0;
	return _FALSE;
}

void rtw_gcmp_decrypt_key_index_mismatch_dbg(u8 packet_index, u8 install_index)
{
	RTW_DBG("not match packet_index=%d, install_index=%d\n",
		packet_index, install_index);
}

/* 3			=====AES related===== */
#if (NEW_CRYPTO == 0)

#define MAX_MSG_SIZE	2048
/*****************************/
/******** SBOX Table *********/
/*****************************/

static  u8 sbox_table[256] = {
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

/*****************************/
/**** Function Prototypes ****/
/*****************************/

static void bitwise_xor(u8 *ina, u8 *inb, u8 *out);
static void construct_mic_iv(
	u8 *mic_header1,
	sint qc_exists,
	sint a4_exists,
	u8 *mpdu,
	uint payload_length,
	u8 *pn_vector,
	uint frtype);/* add for CONFIG_IEEE80211W, none 11w also can use */
static void construct_mic_header1(
	u8 *mic_header1,
	sint header_length,
	u8 *mpdu,
	uint frtype);/* add for CONFIG_IEEE80211W, none 11w also can use */
static void construct_mic_header2(
	u8 *mic_header2,
	u8 *mpdu,
	sint a4_exists,
	sint qc_exists);
static void construct_ctr_preload(
	u8 *ctr_preload,
	sint a4_exists,
	sint qc_exists,
	u8 *mpdu,
	u8 *pn_vector,
	sint c,
	uint frtype);/* add for CONFIG_IEEE80211W, none 11w also can use */
static void xor_128(u8 *a, u8 *b, u8 *out);
static void xor_32(u8 *a, u8 *b, u8 *out);
static u8 sbox(u8 a);
static void next_key(u8 *key, sint round);
static void byte_sub(u8 *in, u8 *out);
static void shift_row(u8 *in, u8 *out);
static void mix_column(u8 *in, u8 *out);
static void aes128k128d(u8 *key, u8 *data, u8 *ciphertext);


/****************************************/
/* aes128k128d()                       */
/* Performs a 128 bit AES encrypt with */
/* 128 bit data.                       */
/****************************************/
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
	for (i = 0 ; i < 4; i++) {
		if ((in[i] & 0x80) == 0x80)
			add1b[i] = 0x1b;
		else
			add1b[i] = 0x00;
	}

	swap_halfs[0] = in[2];    /* Swap halfs */
	swap_halfs[1] = in[3];
	swap_halfs[2] = in[0];
	swap_halfs[3] = in[1];

	rotl[0] = in[3];        /* Rotate left 8 bits */
	rotl[1] = in[0];
	rotl[2] = in[1];
	rotl[3] = in[2];

	andf7[0] = in[0] & 0x7f;
	andf7[1] = in[1] & 0x7f;
	andf7[2] = in[2] & 0x7f;
	andf7[3] = in[3] & 0x7f;

	for (i = 3; i > 0; i--) { /* logical shift left 1 bit */
		andf7[i] = andf7[i] << 1;
		if ((andf7[i - 1] & 0x80) == 0x80)
			andf7[i] = (andf7[i] | 0x01);
	}
	andf7[0] = andf7[0] << 1;
	andf7[0] = andf7[0] & 0xfe;

	xor_32(add1b, andf7, add1bf7);

	xor_32(in, add1bf7, rotr);

	temp[0] = rotr[0];         /* Rotate right 8 bits */
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
		} else { /* 1 - 9 */
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


/************************************************/
/* construct_mic_iv()                          */
/* Builds the MIC IV from header fields and PN */
/* Baron think the function is construct CCM   */
/* nonce                                       */
/************************************************/
static void construct_mic_iv(
	u8 *mic_iv,
	sint qc_exists,
	sint a4_exists,
	u8 *mpdu,
	uint payload_length,
	u8 *pn_vector,
	uint frtype/* add for CONFIG_IEEE80211W, none 11w also can use */
)
{
	sint i;
	mic_iv[0] = 0x59;
	if (qc_exists && a4_exists)
		mic_iv[1] = mpdu[30] & 0x0f;    /* QoS_TC          */
	if (qc_exists && !a4_exists)
		mic_iv[1] = mpdu[24] & 0x0f;   /* mute bits 7-4   */
	if (!qc_exists)
		mic_iv[1] = 0x00;
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	/* 802.11w management frame should set management bit(4) */
	if (frtype == WIFI_MGT_TYPE)
		mic_iv[1] |= BIT(4);
#endif
	for (i = 2; i < 8; i++)
		mic_iv[i] = mpdu[i + 8];                    /* mic_iv[2:7] = A2[0:5] = mpdu[10:15] */
#ifdef CONSISTENT_PN_ORDER
	for (i = 8; i < 14; i++)
		mic_iv[i] = pn_vector[i - 8];           /* mic_iv[8:13] = PN[0:5] */
#else
	for (i = 8; i < 14; i++)
		mic_iv[i] = pn_vector[13 - i];          /* mic_iv[8:13] = PN[5:0] */
#endif
	mic_iv[14] = (unsigned char)(payload_length / 256);
	mic_iv[15] = (unsigned char)(payload_length % 256);
}


/************************************************/
/* construct_mic_header1()                     */
/* Builds the first MIC header block from      */
/* header fields.                              */
/* Build AAD SC,A1,A2                          */
/************************************************/
static void construct_mic_header1(
	u8 *mic_header1,
	sint header_length,
	u8 *mpdu,
	uint frtype/* add for CONFIG_IEEE80211W, none 11w also can use */
)
{
	mic_header1[0] = (u8)((header_length - 2) / 256);
	mic_header1[1] = (u8)((header_length - 2) % 256);
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	/* 802.11w management frame don't AND subtype bits 4,5,6 of frame control field */
	if (frtype == WIFI_MGT_TYPE)
		mic_header1[2] = mpdu[0];
	else
#endif
		mic_header1[2] = mpdu[0] & 0xcf;    /* Mute CF poll & CF ack bits */

	mic_header1[3] = mpdu[1] & 0xc7;    /* Mute retry, more data and pwr mgt bits */
	mic_header1[4] = mpdu[4];       /* A1 */
	mic_header1[5] = mpdu[5];
	mic_header1[6] = mpdu[6];
	mic_header1[7] = mpdu[7];
	mic_header1[8] = mpdu[8];
	mic_header1[9] = mpdu[9];
	mic_header1[10] = mpdu[10];     /* A2 */
	mic_header1[11] = mpdu[11];
	mic_header1[12] = mpdu[12];
	mic_header1[13] = mpdu[13];
	mic_header1[14] = mpdu[14];
	mic_header1[15] = mpdu[15];
}


/************************************************/
/* construct_mic_header2()                     */
/* Builds the last MIC header block from       */
/* header fields.                              */
/************************************************/
static void construct_mic_header2(
	u8 *mic_header2,
	u8 *mpdu,
	sint a4_exists,
	sint qc_exists
)
{
	sint i;
	for (i = 0; i < 16; i++)
		mic_header2[i] = 0x00;

	mic_header2[0] = mpdu[16];    /* A3 */
	mic_header2[1] = mpdu[17];
	mic_header2[2] = mpdu[18];
	mic_header2[3] = mpdu[19];
	mic_header2[4] = mpdu[20];
	mic_header2[5] = mpdu[21];

	/* mic_header2[6] = mpdu[22] & 0xf0;    SC */
	mic_header2[6] = 0x00;
	mic_header2[7] = 0x00; /* mpdu[23]; */


	if (!qc_exists && a4_exists) {
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i]; /* A4 */

	}

	if (qc_exists && !a4_exists) {
		mic_header2[8] = mpdu[24] & 0x0f; /* mute bits 15 - 4 */
		mic_header2[9] = mpdu[25] & 0x00;
	}

	if (qc_exists && a4_exists) {
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i]; /* A4 */

		mic_header2[14] = mpdu[30] & 0x0f;
		mic_header2[15] = mpdu[31] & 0x00;
	}

}


/************************************************/
/* construct_mic_header2()                     */
/* Builds the last MIC header block from       */
/* header fields.                              */
/* Baron think the function is construct CCM   */
/* nonce                                       */
/************************************************/
static void construct_ctr_preload(
	u8 *ctr_preload,
	sint a4_exists,
	sint qc_exists,
	u8 *mpdu,
	u8 *pn_vector,
	sint c,
	uint frtype /* add for CONFIG_IEEE80211W, none 11w also can use */
)
{
	sint i = 0;
	for (i = 0; i < 16; i++)
		ctr_preload[i] = 0x00;
	i = 0;

	ctr_preload[0] = 0x01;                                  /* flag */
	if (qc_exists && a4_exists)
		ctr_preload[1] = mpdu[30] & 0x0f;   /* QoC_Control */
	if (qc_exists && !a4_exists)
		ctr_preload[1] = mpdu[24] & 0x0f;
#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	/* 802.11w management frame should set management bit(4) */
	if (frtype == WIFI_MGT_TYPE)
		ctr_preload[1] |= BIT(4);
#endif
	for (i = 2; i < 8; i++)
		ctr_preload[i] = mpdu[i + 8];                       /* ctr_preload[2:7] = A2[0:5] = mpdu[10:15] */
#ifdef CONSISTENT_PN_ORDER
	for (i = 8; i < 14; i++)
		ctr_preload[i] =    pn_vector[i - 8];           /* ctr_preload[8:13] = PN[0:5] */
#else
	for (i = 8; i < 14; i++)
		ctr_preload[i] =    pn_vector[13 - i];          /* ctr_preload[8:13] = PN[5:0] */
#endif
	ctr_preload[14] = (unsigned char)(c / 256);   /* Ctr */
	ctr_preload[15] = (unsigned char)(c % 256);
}


/************************************/
/* bitwise_xor()                   */
/* A 128 bit, bitwise exclusive or */
/************************************/
static void bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	sint i;
	for (i = 0; i < 16; i++)
		out[i] = ina[i] ^ inb[i];
}


static sint aes_cipher(u8 *key, uint	hdrlen,
		       u8 *pframe, uint plen)
{
	/*	static unsigned char	message[MAX_MSG_SIZE]; */
	uint	qc_exists, a4_exists, i, j, payload_remainder,
		num_blocks, payload_index;

	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];

	/* Intermediate Buffers */
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];
	u8 mic[8];
	/*	uint	offset = 0; */
	uint	frtype  = GetFrameType(pframe);
	uint	frsubtype  = get_frame_sub_type(pframe);

	frsubtype = frsubtype >> 4;


	_rtw_memset((void *)mic_iv, 0, 16);
	_rtw_memset((void *)mic_header1, 0, 16);
	_rtw_memset((void *)mic_header2, 0, 16);
	_rtw_memset((void *)ctr_preload, 0, 16);
	_rtw_memset((void *)chain_buffer, 0, 16);
	_rtw_memset((void *)aes_out, 0, 16);
	_rtw_memset((void *)padded_buffer, 0, 16);

	if ((hdrlen == WLAN_HDR_A3_LEN) || (hdrlen ==  WLAN_HDR_A3_QOS_LEN))
		a4_exists = 0;
	else
		a4_exists = 1;

	if (
		((frtype | frsubtype) == WIFI_DATA_CFACK) ||
		((frtype | frsubtype) == WIFI_DATA_CFPOLL) ||
		((frtype | frsubtype) == WIFI_DATA_CFACKPOLL)) {
		qc_exists = 1;
		if (hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN)
			hdrlen += 2;
	}
	/* add for CONFIG_IEEE80211W, none 11w also can use */
	else if ((frtype == WIFI_DATA) &&
		 ((frsubtype == 0x08) ||
		  (frsubtype == 0x09) ||
		  (frsubtype == 0x0a) ||
		  (frsubtype == 0x0b))) {
		if (hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN)
			hdrlen += 2;
		qc_exists = 1;
	} else
		qc_exists = 0;

	pn_vector[0] = pframe[hdrlen];
	pn_vector[1] = pframe[hdrlen + 1];
	pn_vector[2] = pframe[hdrlen + 4];
	pn_vector[3] = pframe[hdrlen + 5];
	pn_vector[4] = pframe[hdrlen + 6];
	pn_vector[5] = pframe[hdrlen + 7];

	construct_mic_iv(
		mic_iv,
		qc_exists,
		a4_exists,
		pframe,	 /* message, */
		plen,
		pn_vector,
		frtype /* add for CONFIG_IEEE80211W, none 11w also can use */
	);

	construct_mic_header1(
		mic_header1,
		hdrlen,
		pframe,	/* message */
		frtype /* add for CONFIG_IEEE80211W, none 11w also can use */
	);
	construct_mic_header2(
		mic_header2,
		pframe,	/* message, */
		a4_exists,
		qc_exists
	);


	payload_remainder = plen % 16;
	num_blocks = plen / 16;

	/* Find start of payload */
	payload_index = (hdrlen + 8);

	/* Calculate MIC */
	aes128k128d(key, mic_iv, aes_out);
	bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);

	for (i = 0; i < num_blocks; i++) {
		bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);/* bitwise_xor(aes_out, &message[payload_index], chain_buffer); */

		payload_index += 16;
		aes128k128d(key, chain_buffer, aes_out);
	}

	/* Add on the final payload block if it needs padding */
	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index++];/* padded_buffer[j] = message[payload_index++]; */
		}
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes128k128d(key, chain_buffer, aes_out);

	}

	for (j = 0 ; j < 8; j++)
		mic[j] = aes_out[j];

	/* Insert MIC into payload */
	for (j = 0; j < 8; j++)
		pframe[payload_index + j] = mic[j];	/* message[payload_index+j] = mic[j]; */

	payload_index = hdrlen + 8;
	for (i = 0; i < num_blocks; i++) {
		construct_ctr_preload(
			ctr_preload,
			a4_exists,
			qc_exists,
			pframe,	/* message, */
			pn_vector,
			i + 1,
			frtype); /* add for CONFIG_IEEE80211W, none 11w also can use */
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);/* bitwise_xor(aes_out, &message[payload_index], chain_buffer); */
		for (j = 0; j < 16; j++)
			pframe[payload_index++] = chain_buffer[j];/* for (j=0; j<16;j++) message[payload_index++] = chain_buffer[j]; */
	}

	if (payload_remainder > 0) {        /* If there is a short final block, then pad it,*/
		/* encrypt it and copy the unpadded part back  */
		construct_ctr_preload(
			ctr_preload,
			a4_exists,
			qc_exists,
			pframe,	/* message, */
			pn_vector,
			num_blocks + 1,
			frtype); /* add for CONFIG_IEEE80211W, none 11w also can use */

		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index + j]; /* padded_buffer[j] = message[payload_index+j]; */
		}
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++)
			pframe[payload_index++] = chain_buffer[j];/* for (j=0; j<payload_remainder;j++) message[payload_index++] = chain_buffer[j]; */
	}

	/* Encrypt the MIC */
	construct_ctr_preload(
		ctr_preload,
		a4_exists,
		qc_exists,
		pframe,	/* message, */
		pn_vector,
		0,
		frtype); /* add for CONFIG_IEEE80211W, none 11w also can use */

	for (j = 0; j < 16; j++)
		padded_buffer[j] = 0x00;
	for (j = 0; j < 8; j++) {
		padded_buffer[j] = pframe[j + hdrlen + 8 + plen]; /* padded_buffer[j] = message[j+hdrlen+8+plen]; */
	}

	aes128k128d(key, ctr_preload, aes_out);
	bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++)
		pframe[payload_index++] = chain_buffer[j];/* for (j=0; j<8;j++) message[payload_index++] = chain_buffer[j]; */
	return _SUCCESS;
}
#endif /* (NEW_CRYPTO == 0) */

/* W3-12c: rtw_aes_encrypt in rust/rtw_security_rest.rs */

/* W3-13: aes_decipher in rust/rtw_security_rest.rs */
#if (NEW_CRYPTO == 0)
extern sint aes_decipher(u8 *key, uint hdrlen, u8 *pframe, uint plen);
#endif /* (NEW_CRYPTO == 0) */

void rtw_aes_decipher_log_mic_mismatch(int i, u8 pframe_byte, u8 message_byte)
{
	RTW_INFO("aes_decipher:mic check error mic[%d]: pframe(%x) != message(%x)\n",
		 i, pframe_byte, message_byte);
}

/* W3-13: rtw_aes_decrypt in rust/rtw_security_rest.rs */

#ifdef CONFIG_RTW_MESH_AEK
/* for AES-SIV, wrapper to ase_siv_encrypt and aes_siv_decrypt */
int rtw_aes_siv_encrypt(const u8 *key, size_t key_len, const u8 *pw,
	size_t pwlen, size_t num_elem,
	const u8 *addr[], const size_t *len, u8 *out)
{
	return _aes_siv_encrypt(key, key_len, pw, pwlen,
		num_elem, addr, len, out);
}

int rtw_aes_siv_decrypt(const u8 *key, size_t key_len, const u8 *iv_crypt, size_t iv_c_len,
	size_t num_elem, const u8 *addr[], const size_t *len, u8 *out)
{
	return _aes_siv_decrypt(key, key_len, iv_crypt,
		iv_c_len, num_elem, addr, len, out);
}
#endif /* CONFIG_RTW_MESH_AEK */

#ifdef CONFIG_TDLS
void wpa_tdls_generate_tpk(_adapter *padapter, void *sta)
{
	struct sta_info *psta = (struct sta_info *)sta;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	_tdls_generate_tpk(psta, adapter_mac_addr(padapter), get_bssid(pmlmepriv));
}

/**
 * wpa_tdls_ftie_mic - Calculate TDLS FTIE MIC
 * @kck: TPK-KCK
 * @lnkid: Pointer to the beginning of Link Identifier IE
 * @rsnie: Pointer to the beginning of RSN IE used for handshake
 * @timeoutie: Pointer to the beginning of Timeout IE used for handshake
 * @ftie: Pointer to the beginning of FT IE
 * @mic: Pointer for writing MIC
 *
 * Calculate MIC for TDLS frame.
 */
int wpa_tdls_ftie_mic(u8 *kck, u8 trans_seq,
		      u8 *lnkid, u8 *rsnie, u8 *timeoutie, u8 *ftie,
		      u8 *mic)
{
	u8 *buf, *pos;
	struct wpa_tdls_ftie *_ftie;
	struct wpa_tdls_lnkid *_lnkid;
	int ret;
	int len = 2 * ETH_ALEN + 1 + 2 + lnkid[1] + 2 + rsnie[1] +
		  2 + timeoutie[1] + 2 + ftie[1];
	buf = rtw_zmalloc(len);
	if (!buf) {
		RTW_INFO("TDLS: No memory for MIC calculation\n");
		return -1;
	}

	pos = buf;
	_lnkid = (struct wpa_tdls_lnkid *) lnkid;
	/* 1) TDLS initiator STA MAC address */
	_rtw_memcpy(pos, _lnkid->init_sta, ETH_ALEN);
	pos += ETH_ALEN;
	/* 2) TDLS responder STA MAC address */
	_rtw_memcpy(pos, _lnkid->resp_sta, ETH_ALEN);
	pos += ETH_ALEN;
	/* 3) Transaction Sequence number */
	*pos++ = trans_seq;
	/* 4) Link Identifier IE */
	_rtw_memcpy(pos, lnkid, 2 + lnkid[1]);
	pos += 2 + lnkid[1];
	/* 5) RSN IE */
	_rtw_memcpy(pos, rsnie, 2 + rsnie[1]);
	pos += 2 + rsnie[1];
	/* 6) Timeout Interval IE */
	_rtw_memcpy(pos, timeoutie, 2 + timeoutie[1]);
	pos += 2 + timeoutie[1];
	/* 7) FTIE, with the MIC field of the FTIE set to 0 */
	_rtw_memcpy(pos, ftie, 2 + ftie[1]);
	_ftie = (struct wpa_tdls_ftie *) pos;
	_rtw_memset(_ftie->mic, 0, TDLS_MIC_LEN);
	pos += 2 + ftie[1];

	/* ret = omac1_aes_128(kck, buf, pos - buf, mic); */
	ret = _bip_ccmp_protect(kck, 16, buf, pos - buf, mic);
	rtw_mfree(buf, len);
	return ret;

}

/**
 * wpa_tdls_teardown_ftie_mic - Calculate TDLS TEARDOWN FTIE MIC
 * @kck: TPK-KCK
 * @lnkid: Pointer to the beginning of Link Identifier IE
 * @reason: Reason code of TDLS Teardown
 * @dialog_token: Dialog token that was used in the MIC calculation for TPK Handshake Message 3
 * @trans_seq: Transaction Sequence number (1 octet) which shall be set to the value 4
 * @ftie: Pointer to the beginning of FT IE
 * @mic: Pointer for writing MIC
 *
 * Calculate MIC for TDLS TEARDOWN frame according to Section 10.22.5 in IEEE 802.11 - 2012.
 */
int wpa_tdls_teardown_ftie_mic(u8 *kck, u8 *lnkid, u16 reason,
			       u8 dialog_token, u8 trans_seq, u8 *ftie, u8 *mic)
{
	u8 *buf, *pos;
	struct wpa_tdls_ftie *_ftie;
	int ret;
	int len = 2 + lnkid[1] + 2 + 1 + 1 + 2 + ftie[1];

	buf = rtw_zmalloc(len);
	if (!buf) {
		RTW_INFO("TDLS: No memory for MIC calculation\n");
		return -1;
	}

	pos = buf;
	/* 1) Link Identifier IE */
	_rtw_memcpy(pos, lnkid, 2 + lnkid[1]);
	pos += 2 + lnkid[1];
	/* 2) Reason Code */
	_rtw_memcpy(pos, (u8 *)&reason, 2);
	pos += 2;
	/* 3) Dialog Token */
	*pos++ = dialog_token;
	/* 4) Transaction Sequence number */
	*pos++ = trans_seq;
	/* 5) FTIE, with the MIC field of the FTIE set to 0 */
	_rtw_memcpy(pos, ftie, 2 + ftie[1]);
	_ftie = (struct wpa_tdls_ftie *) pos;
	_rtw_memset(_ftie->mic, 0, TDLS_MIC_LEN);
	pos += 2 + ftie[1];

	/* ret = omac1_aes_128(kck, buf, pos - buf, mic); */
	ret = _bip_ccmp_protect(kck, 16, buf, pos - buf, mic);
	rtw_mfree(buf, len);
	return ret;

}

int tdls_verify_mic(u8 *kck, u8 trans_seq,
		    u8 *lnkid, u8 *rsnie, u8 *timeoutie, u8 *ftie)
{
	u8 *buf, *pos;
	int len;
	u8 mic[16];
	int ret;
	u8 *rx_ftie, *tmp_ftie;

	if (lnkid == NULL || rsnie == NULL ||
	    timeoutie == NULL || ftie == NULL)
		return _FAIL;

	len = 2 * ETH_ALEN + 1 + 2 + 18 + 2 + *(rsnie + 1) + 2 + *(timeoutie + 1) + 2 + *(ftie + 1);

	buf = rtw_zmalloc(len);
	if (buf == NULL)
		return _FAIL;

	pos = buf;
	/* 1) TDLS initiator STA MAC address */
	_rtw_memcpy(pos, lnkid + ETH_ALEN + 2, ETH_ALEN);
	pos += ETH_ALEN;
	/* 2) TDLS responder STA MAC address */
	_rtw_memcpy(pos, lnkid + 2 * ETH_ALEN + 2, ETH_ALEN);
	pos += ETH_ALEN;
	/* 3) Transaction Sequence number */
	*pos++ = trans_seq;
	/* 4) Link Identifier IE */
	_rtw_memcpy(pos, lnkid, 2 + 18);
	pos += 2 + 18;
	/* 5) RSN IE */
	_rtw_memcpy(pos, rsnie, 2 + *(rsnie + 1));
	pos += 2 + *(rsnie + 1);
	/* 6) Timeout Interval IE */
	_rtw_memcpy(pos, timeoutie, 2 + *(timeoutie + 1));
	pos += 2 + *(timeoutie + 1);
	/* 7) FTIE, with the MIC field of the FTIE set to 0 */
	_rtw_memcpy(pos, ftie, 2 + *(ftie + 1));
	pos += 2;
	tmp_ftie = (u8 *)(pos + 2);
	_rtw_memset(tmp_ftie, 0, 16);
	pos += *(ftie + 1);

	/* ret = omac1_aes_128(kck, buf, pos - buf, mic); */
	ret = _bip_ccmp_protect(kck, 16, buf, pos - buf, mic);
	rtw_mfree(buf, len);
	if (ret == _FAIL)
		return _FAIL;
	rx_ftie = ftie + 4;

	if (_rtw_memcmp2(mic, rx_ftie, 16) == 0) {
		/* Valid MIC */
		return _SUCCESS;
	}

	/* Invalid MIC */
	RTW_INFO("[%s] Invalid MIC\n", __FUNCTION__);
	return _FAIL;

}
#endif /* CONFIG_TDLS */

/* Restore HW wep key setting according to key_mask */
void rtw_sec_restore_wep_key(_adapter *adapter)
{
	struct security_priv *securitypriv = &(adapter->securitypriv);
	sint keyid;

	if ((_WEP40_ == securitypriv->dot11PrivacyAlgrthm) || (_WEP104_ == securitypriv->dot11PrivacyAlgrthm)) {
		for (keyid = 0; keyid < 4; keyid++) {
			if (securitypriv->key_mask & BIT(keyid)) {
				if (keyid == securitypriv->dot11PrivacyKeyIndex)
					rtw_set_key(adapter, securitypriv, keyid, 1, _FALSE);
				else
					rtw_set_key(adapter, securitypriv, keyid, 0, _FALSE);
			}
		}
	}
}

u8 rtw_handle_tkip_countermeasure(_adapter *adapter, const char *caller)
{
	struct security_priv *securitypriv = &(adapter->securitypriv);
	u8 status = _SUCCESS;

	if (securitypriv->btkip_countermeasure == _TRUE) {
		u32 passing_ms = rtw_get_passing_time_ms(securitypriv->btkip_countermeasure_time);
		if (passing_ms > 60 * 1000) {
			RTW_PRINT("%s("ADPT_FMT") countermeasure time:%ds > 60s\n",
				  caller, ADPT_ARG(adapter), passing_ms / 1000);
			securitypriv->btkip_countermeasure = _FALSE;
			securitypriv->btkip_countermeasure_time = 0;
		} else {
			RTW_PRINT("%s("ADPT_FMT") countermeasure time:%ds < 60s\n",
				  caller, ADPT_ARG(adapter), passing_ms / 1000);
			status = _FAIL;
		}
	}

	return status;
}

#ifdef CONFIG_WOWLAN
u16 rtw_cal_crc16(u8 data, u16 crc)
{
	u8 shift_in, data_bit;
	u8 crc_bit4, crc_bit11, crc_bit15;
	u16 crc_result;
	int index;

	for (index = 0; index < 8; index++) {
		crc_bit15 = ((crc & BIT15) ? 1 : 0);
		data_bit = (data & (BIT0 << index) ? 1 : 0);
		shift_in = crc_bit15 ^ data_bit;
		/*printf("crc_bit15=%d, DataBit=%d, shift_in=%d\n",
		 * crc_bit15, data_bit, shift_in);*/

		crc_result = crc << 1;

		if (shift_in == 0)
			crc_result &= (~BIT0);
		else
			crc_result |= BIT0;
		/*printf("CRC =%x\n",CRC_Result);*/

		crc_bit11 = ((crc & BIT11) ? 1 : 0) ^ shift_in;

		if (crc_bit11 == 0)
			crc_result &= (~BIT12);
		else
			crc_result |= BIT12;

		/*printf("bit12 CRC =%x\n",CRC_Result);*/

		crc_bit4 = ((crc & BIT4) ? 1 : 0) ^ shift_in;

		if (crc_bit4 == 0)
			crc_result &= (~BIT5);
		else
			crc_result |= BIT5;

		/* printf("bit5 CRC =%x\n",CRC_Result); */
		/* repeat using the last result*/
		crc = crc_result;
	}
	return crc;
}

/*
 * function name :rtw_calc_crc
 *
 * input: char* pattern , pattern size
 *
 */
u16 rtw_calc_crc(u8  *pdata, int length)
{
	u16 crc = 0xffff;
	int i;

	for (i = 0; i < length; i++)
		crc = rtw_cal_crc16(pdata[i], crc);
	/* get 1' complement */
	crc = ~crc;

	return crc;
}
#endif /*CONFIG_WOWLAN*/

u32 rtw_calc_crc32(u8 *data, size_t len)
{
	return getcrc32(data, (sint)len);
}


/* W3-14a: rtw_gcmp_encrypt in rust/rtw_security_rest.rs */

/* W3-14b: rtw_gcmp_decrypt in rust/rtw_security_rest.rs */

#ifdef CONFIG_IEEE80211W
u8 rtw_calculate_bip_mic(enum security_type gmcs, u8 *whdr_pos, s32 len,
	const u8 *key, const u8 *data, size_t data_len, u8 *mic)
{
	u8 res = _SUCCESS;

	if (gmcs == _BIP_CMAC_128_) {
		if (_bip_ccmp_protect(key, 16, data, data_len, mic) == _FALSE) {
			res = _FAIL;
			RTW_ERR("%s : _bip_ccmp_protect(128) fail!", __func__);
		}
	} else if (gmcs == _BIP_CMAC_256_) {
		if (_bip_ccmp_protect(key, 32, data, data_len, mic) == _FALSE) {
			res = _FAIL;
			RTW_ERR("%s : _bip_ccmp_protect(256) fail!", __func__);
		}
	} else if (gmcs == _BIP_GMAC_128_) {
		if (_bip_gcmp_protect(whdr_pos, len, key, 16,
				data, data_len, mic) == _FALSE) {
			res = _FAIL;
			RTW_ERR("%s : _bip_gcmp_protect(128) fail!", __func__);
		}
	} else if (gmcs == _BIP_GMAC_256_) {
		if (_bip_gcmp_protect(whdr_pos, len, key, 32,
				data, data_len, mic) == _FALSE) {
			res = _FAIL;
			RTW_ERR("%s : _bip_gcmp_protect(256) fail!", __func__);
		}
	} else {
		res = _FAIL;
		RTW_ERR("%s : unsupport dot11wCipher !\n", __func__);
	}

	return res;
}


u32 rtw_bip_verify(enum security_type gmcs, u16 pkt_len,
	u8 *whdr_pos, sint flen, const u8 *key, u16 keyid, u64 *ipn)
{
	u8 * BIP_AAD,*mme;
	u32 res = _FAIL;
	uint len, ori_len;
	u16 pkt_keyid = 0;
	u64 pkt_ipn = 0;
	struct rtw_ieee80211_hdr *pwlanhdr;
	u8 mic[16];
	u8 mic_len, mme_offset;

	mic_len = (gmcs == _BIP_CMAC_128_) ? 8 : 16;

	if (flen < WLAN_HDR_A3_LEN || flen - WLAN_HDR_A3_LEN < mic_len)
		return RTW_RX_HANDLED;

	mme_offset = (mic_len == 8) ? 18 : 26;
	mme = whdr_pos + flen - mme_offset;
	if (*mme != _MME_IE_)
		return RTW_RX_HANDLED;

	/* copy key index */
	_rtw_memcpy(&pkt_keyid, mme + 2, 2);
	pkt_keyid = le16_to_cpu(pkt_keyid);
	if (pkt_keyid != keyid) {
		RTW_INFO("BIP key index error!\n");
		return _FAIL;
	}

	/* save packet number */
	_rtw_memcpy(&pkt_ipn, mme + 4, 6);
	pkt_ipn = le64_to_cpu(pkt_ipn);
	/* BIP packet number should bigger than previous BIP packet */
	if (pkt_ipn <= *ipn) { /* wrap around? */
		RTW_INFO("replay BIP packet\n");
		return _FAIL;
	}

	ori_len = flen - WLAN_HDR_A3_LEN + BIP_AAD_SIZE;
	BIP_AAD = rtw_zmalloc(ori_len);
	if (BIP_AAD == NULL) {
		RTW_INFO("BIP AAD allocate fail\n");
		return _FAIL;
	}

	/* mapping to wlan header */
	pwlanhdr = (struct rtw_ieee80211_hdr *)whdr_pos;

	/* save the frame body + MME (w/o mic) */
	_rtw_memcpy(BIP_AAD + BIP_AAD_SIZE,
		whdr_pos + WLAN_HDR_A3_LEN,
		flen - WLAN_HDR_A3_LEN - mic_len);

	/* conscruct AAD, copy frame control field */
	_rtw_memcpy(BIP_AAD, &pwlanhdr->frame_ctl, 2);
	ClearRetry(BIP_AAD);
	ClearPwrMgt(BIP_AAD);
	ClearMData(BIP_AAD);
	/* conscruct AAD, copy address 1 to address 3 */
	_rtw_memcpy(BIP_AAD + 2, GetAddr1Ptr((u8 *)pwlanhdr), 18);

	if (rtw_calculate_bip_mic(gmcs, whdr_pos,
			pkt_len, key, BIP_AAD, ori_len, mic) == _FAIL)
		goto BIP_exit;

	/* MIC field should be last 8 bytes of packet (packet without FCS) */
	if (_rtw_memcmp(mic, whdr_pos + flen - mic_len, mic_len)) {
		*ipn = pkt_ipn;
		res = _SUCCESS;
	} else
		RTW_INFO("BIP MIC error!\n");

#if 0
	/* management packet content */
	{
		int pp;
		RTW_INFO("pkt: ");
		RTW_INFO_DUMP("", whdr_pos, flen);
		RTW_INFO("\n");
		/* BIP AAD + management frame body + MME(MIC is zero) */
		RTW_INFO("AAD+PKT: ");
		RTW_INFO_DUMP("", BIP_AAD, ori_len);
		RTW_INFO("\n");
		/* show the MIC result */
		RTW_INFO("mic: ");
		RTW_INFO_DUMP("", mic, mic_len);
		RTW_INFO("\n");
	}
#endif

BIP_exit:

	rtw_mfree(BIP_AAD, ori_len);
	return res;
}

#endif /* CONFIG_IEEE80211W */

