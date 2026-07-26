// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle slice: core/rtw_security.c type-string helpers (T5 / W3-04).
 */
#include "host_security_types.h"

static const char *_security_type_str[] = {
	"N/A",
	"WEP40",
	"TKIP",
	"TKIP_WM",
	"AES",
	"WEP104",
	"SMS4",
	"GCMP",
};

static const char *_security_type_bip_str[] = {
	"BIP_CMAC_128",
	"BIP_GMAC_128",
	"BIP_GMAC_256",
	"BIP_CMAC_256",
};

const char *security_type_str(u8 value)
{
#ifdef CONFIG_IEEE80211W
	if ((_BIP_MAX_ > value) && (value >= _BIP_CMAC_128_))
		return _security_type_bip_str[value & ~_SEC_TYPE_BIT_];
#endif

	if (_CCMP_256_ == value)
		return "CCMP_256";
	if (_GCMP_256_ == value)
		return "GCMP_256";

	if (_SEC_TYPE_MAX_ > value)
		return _security_type_str[value];

	return NULL;
}

#ifdef CONFIG_IEEE80211W
u32 security_type_bip_to_gmcs(enum security_type type)
{
	switch (type) {
	case _BIP_CMAC_128_:
		return WPA_CIPHER_BIP_CMAC_128;
	case _BIP_GMAC_128_:
		return WPA_CIPHER_BIP_GMAC_128;
	case _BIP_GMAC_256_:
		return WPA_CIPHER_BIP_GMAC_256;
	case _BIP_CMAC_256_:
		return WPA_CIPHER_BIP_CMAC_256;
	default:
		return 0;
	}
}
#endif

/* ----- WEP primitives (ARC4 + CRC32) from core/rtw_security.c ----- */
/* codeql[cpp/weak-cryptographic-algorithm]: Host L2 oracle for legacy WEP characterization only. */

typedef int sint;

#define CRC32_POLY 0x04c11db7

struct arc4context {
	u32 x;
	u32 y;
	u8 state[256];
};

static void arcfour_init(struct arc4context *parc4ctx, u8 *key, u32 key_len)
{
	u32 t, u;
	u32 keyindex;
	u32 stateindex;
	u8 *state;
	u32 counter;

	state = parc4ctx->state;
	parc4ctx->x = 0;
	parc4ctx->y = 0;
	for (counter = 0; counter < 256; counter++)
		state[counter] = (u8)counter;
	keyindex = 0;
	stateindex = 0;
	for (counter = 0; counter < 256; counter++) {
		t = state[counter];
		stateindex = (stateindex + key[keyindex] + t) & 0xff;
		u = state[stateindex];
		state[stateindex] = (u8)t;
		state[counter] = (u8)u;
		if (++keyindex >= key_len)
			keyindex = 0;
	}
}

static u32 arcfour_byte(struct arc4context *parc4ctx)
{
	u32 x;
	u32 y;
	u32 sx, sy;
	u8 *state;

	state = parc4ctx->state;
	x = (parc4ctx->x + 1) & 0xff;
	sx = state[x];
	y = (sx + parc4ctx->y) & 0xff;
	sy = state[y];
	parc4ctx->x = x;
	parc4ctx->y = y;
	state[y] = (u8)sx;
	state[x] = (u8)sy;
	return state[(sx + sy) & 0xff];
}

static void arcfour_encrypt(struct arc4context *parc4ctx, u8 *dest, u8 *src,
			    u32 len)
{
	u32 i;

	for (i = 0; i < len; i++)
		dest[i] = src[i] ^ (unsigned char)arcfour_byte(parc4ctx);
}

static sint bcrc32initialized;
static u32 crc32_table[256];

static u8 crc32_reverseBit(u8 data)
{
	return (u8)((data << 7) & 0x80) | ((data << 5) & 0x40) |
	       ((data << 3) & 0x20) | ((data << 1) & 0x10) |
	       ((data >> 1) & 0x08) | ((data >> 3) & 0x04) |
	       ((data >> 5) & 0x02) | ((data >> 7) & 0x01);
}

static void crc32_init(void)
{
	sint i, j;
	u32 c;
	u8 *p = (u8 *)&c, *p1;
	u8 k;

	if (bcrc32initialized == 1)
		return;

	c = 0x12340000;
	for (i = 0; i < 256; ++i) {
		k = crc32_reverseBit((u8)i);
		for (c = ((u32)k) << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		p1 = (u8 *)&crc32_table[i];
		p1[0] = crc32_reverseBit(p[3]);
		p1[1] = crc32_reverseBit(p[2]);
		p1[2] = crc32_reverseBit(p[1]);
		p1[3] = crc32_reverseBit(p[0]);
	}
	bcrc32initialized = 1;
}

static u32 getcrc32(u8 *buf, sint len)
{
	u8 *p;
	u32 crc;

	if (bcrc32initialized == 0)
		crc32_init();

	crc = 0xffffffff;
	for (p = buf; len > 0; ++p, --len)
		crc = crc32_table[(crc ^ *p) & 0xff] ^ (crc >> 8);
	return ~crc;
}

void host_wep_arcfour_crypt(const u8 *key, u32 key_len, const u8 *src, u8 *dest,
			    u32 len)
{
	struct arc4context ctx;

	// codeql[cpp/weak-cryptographic-algorithm]
	arcfour_init(&ctx, (u8 *)key, key_len);
	arcfour_encrypt(&ctx, dest, (u8 *)src, len);
}

u32 host_wep_getcrc32(u8 *buf, sint len)
{
	return getcrc32(buf, len);
}
