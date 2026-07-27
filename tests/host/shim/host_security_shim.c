/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host oracle for WEP frame encrypt/decrypt (W3-06 / T5).
 * Faithful copy of core/rtw_security_rest.c WEP paths using host_security_frame.h.
 */

#include <stddef.h>
#include <stdint.h>

#include "host_security_frame.h"

#define CRC32_POLY 0x04c11db7

struct arc4context {
	uint32_t x;
	uint32_t y;
	uint8_t state[256];
};

static void arcfour_init(struct arc4context *parc4ctx, uint8_t *key, uint32_t key_len)
{
	uint32_t t, u;
	uint32_t keyindex;
	uint32_t stateindex;
	uint8_t *state;
	uint32_t counter;

	state = parc4ctx->state;
	parc4ctx->x = 0;
	parc4ctx->y = 0;
	for (counter = 0; counter < 256; counter++)
		state[counter] = (uint8_t)counter;
	keyindex = 0;
	stateindex = 0;
	for (counter = 0; counter < 256; counter++) {
		t = state[counter];
		stateindex = (stateindex + key[keyindex] + t) & 0xff;
		u = state[stateindex];
		state[stateindex] = (uint8_t)t;
		state[counter] = (uint8_t)u;
		if (++keyindex >= key_len)
			keyindex = 0;
	}
}

static uint32_t arcfour_byte(struct arc4context *parc4ctx)
{
	uint32_t x;
	uint32_t y;
	uint32_t sx, sy;
	uint8_t *state;

	state = parc4ctx->state;
	x = (parc4ctx->x + 1) & 0xff;
	sx = state[x];
	y = (sx + parc4ctx->y) & 0xff;
	sy = state[y];
	parc4ctx->x = x;
	parc4ctx->y = y;
	state[y] = (uint8_t)sx;
	state[x] = (uint8_t)sy;
	return state[(sx + sy) & 0xff];
}

static void arcfour_encrypt(struct arc4context *parc4ctx, uint8_t *dest, uint8_t *src,
			    uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dest[i] = src[i] ^ (unsigned char)arcfour_byte(parc4ctx);
}

static int bcrc32initialized;
static uint32_t crc32_table[256];

static uint8_t crc32_reverse_bit(uint8_t data)
{
	return (uint8_t)((data << 7) & 0x80) | ((data << 5) & 0x40) |
	       ((data << 3) & 0x20) | ((data << 1) & 0x10) |
	       ((data >> 1) & 0x08) | ((data >> 3) & 0x04) |
	       ((data >> 5) & 0x02) | ((data >> 7) & 0x01);
}

static void crc32_init(void)
{
	int i, j;
	uint32_t c;
	uint8_t *p = (uint8_t *)&c, *p1;
	uint8_t k;

	if (bcrc32initialized == 1)
		return;

	c = 0x12340000;
	for (i = 0; i < 256; ++i) {
		k = crc32_reverse_bit((uint8_t)i);
		for (c = ((uint32_t)k) << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		p1 = (uint8_t *)&crc32_table[i];
		p1[0] = crc32_reverse_bit(p[3]);
		p1[1] = crc32_reverse_bit(p[2]);
		p1[2] = crc32_reverse_bit(p[1]);
		p1[3] = crc32_reverse_bit(p[0]);
	}
	bcrc32initialized = 1;
}

static uint32_t getcrc32(uint8_t *buf, int len)
{
	uint8_t *p;
	uint32_t crc;

	if (bcrc32initialized == 0)
		crc32_init();

	crc = 0xffffffff;
	for (p = buf; len > 0; ++p, --len)
		crc = crc32_table[(crc ^ *p) & 0xff] ^ (crc >> 8);
	return ~crc;
}

static void host_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	while (n--)
		*d++ = *s++;
}

#define RND4(x) ((((uintptr_t)(x) >> 2) + ((((uintptr_t)(x) & 3) == 0) ? 0 : 1)) << 2)

void rtw_wep_encrypt(struct host_adapter *padapter, uint8_t *pxmitframe)
{
	unsigned char crc[4];
	struct arc4context mycontext;
	int curfragnum, length;
	uint32_t keylength;
	uint8_t *pframe, *payload, *iv;
	uint8_t wepkey[16];
	uint8_t hw_hdr_offset = HOST_TXDESC_OFFSET;
	struct host_pkt_attrib *pattrib =
		&((struct host_xmit_frame *)pxmitframe)->attrib;
	struct host_security_priv *psecuritypriv = &padapter->securitypriv;
	struct host_xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (((struct host_xmit_frame *)pxmitframe)->buf_addr == NULL)
		return;

	hw_hdr_offset = HOST_TXDESC_OFFSET +
		(((struct host_xmit_frame *)pxmitframe)->pkt_offset * 8);

	pframe = ((struct host_xmit_frame *)pxmitframe)->buf_addr + hw_hdr_offset;

	if ((pattrib->encrypt == _WEP40_) || (pattrib->encrypt == _WEP104_)) {
		keylength =
			psecuritypriv->dot11_def_keylen[psecuritypriv->dot11_privacy_key_index];

		for (curfragnum = 0; curfragnum < pattrib->nr_frags; curfragnum++) {
			iv = pframe + pattrib->hdrlen;
			host_memcpy(&wepkey[0], iv, 3);
			host_memcpy(&wepkey[3],
				    &psecuritypriv->dot11_def_key[psecuritypriv->dot11_privacy_key_index].skey[0],
				    keylength);
			payload = pframe + pattrib->iv_len + pattrib->hdrlen;

			if ((curfragnum + 1) == pattrib->nr_frags) {
				length = pattrib->last_txcmdsz - pattrib->hdrlen -
					 pattrib->iv_len - pattrib->icv_len;

				*((uint32_t *)crc) = getcrc32(payload, length);
				arcfour_init(&mycontext, wepkey, 3 + keylength);
				arcfour_encrypt(&mycontext, payload, payload, length);
				arcfour_encrypt(&mycontext, payload + length, crc, 4);
			} else {
				length = pxmitpriv->frag_len - pattrib->hdrlen -
					 pattrib->iv_len - pattrib->icv_len;
				*((uint32_t *)crc) = getcrc32(payload, length);
				arcfour_init(&mycontext, wepkey, 3 + keylength);
				arcfour_encrypt(&mycontext, payload, payload, length);
				arcfour_encrypt(&mycontext, payload + length, crc, 4);

				pframe += pxmitpriv->frag_len;
				pframe = (uint8_t *)RND4((uintptr_t)(pframe));
			}
		}
	}
}

void rtw_wep_decrypt(struct host_adapter *padapter, uint8_t *precvframe)
{
	uint8_t crc[4];
	struct arc4context mycontext;
	int length;
	uint32_t keylength;
	uint8_t *pframe, *payload, *iv, wepkey[16];
	uint8_t keyindex;
	struct host_rx_pkt_attrib *prxattrib =
		&(((union host_recv_frame *)precvframe)->u.hdr.attrib);
	struct host_security_priv *psecuritypriv = &padapter->securitypriv;

	pframe = (uint8_t *)((union host_recv_frame *)precvframe)->u.hdr.rx_data;

	if ((prxattrib->encrypt == _WEP40_) || (prxattrib->encrypt == _WEP104_)) {
		iv = pframe + prxattrib->hdrlen;
		keyindex = prxattrib->key_index;
		keylength = psecuritypriv->dot11_def_keylen[keyindex];
		host_memcpy(&wepkey[0], iv, 3);
		host_memcpy(&wepkey[3], &psecuritypriv->dot11_def_key[keyindex].skey[0],
			    keylength);
		length = ((union host_recv_frame *)precvframe)->u.hdr.len -
			 prxattrib->hdrlen - prxattrib->iv_len;

		payload = pframe + prxattrib->iv_len + prxattrib->hdrlen;

		arcfour_init(&mycontext, wepkey, 3 + keylength);
		arcfour_encrypt(&mycontext, payload, payload, length);

		*((uint32_t *)crc) = getcrc32(payload, length - 4);
	}
}

#if defined(HOST_TKIP_FRAME_ORACLE_BUILD)

/* ----- TKIP frame encrypt (W3-10 / T5) ----- */

union host_pn48 {
	uint64_t val;
	struct {
		uint8_t TSC0;
		uint8_t TSC1;
		uint8_t TSC2;
		uint8_t TSC3;
		uint8_t TSC4;
		uint8_t TSC5;
		uint8_t TSC6;
		uint8_t TSC7;
	} _byte_;
};

#define HOST_GET_TKIP_PN(iv, dot11txpn)                                          \
	do {                                                                     \
		(dot11txpn)._byte_.TSC0 = (iv)[2];                               \
		(dot11txpn)._byte_.TSC1 = (iv)[0];                               \
		(dot11txpn)._byte_.TSC2 = (iv)[4];                               \
		(dot11txpn)._byte_.TSC3 = (iv)[5];                               \
		(dot11txpn)._byte_.TSC4 = (iv)[6];                               \
		(dot11txpn)._byte_.TSC5 = (iv)[7];                               \
	} while (0)

#define HOST_SUCCESS 0
#define HOST_FAIL 1

extern void host_tkip_phase1(uint16_t *p1k, const uint8_t *tk, const uint8_t *ta,
			     uint32_t iv32);
extern void host_tkip_phase2(uint8_t *rc4key, const uint8_t *tk, const uint16_t *p1k,
			     uint16_t iv16);

static uint32_t host_cpu_to_le32(uint32_t v)
{
	return v;
}

uint32_t rtw_tkip_encrypt(struct host_adapter *padapter, uint8_t *pxmitframe)
{
	uint16_t pnl;
	uint32_t pnh;
	uint8_t rc4key[16];
	uint8_t ttkey[16];
	uint8_t crc[4];
	uint8_t hw_hdr_offset;
	struct arc4context mycontext;
	int curfragnum, length;
	uint8_t *pframe, *payload, *iv, *prwskey;
	union host_pn48 dot11txpn;
	struct host_pkt_attrib *pattrib =
		&((struct host_xmit_frame *)pxmitframe)->attrib;
	struct host_security_priv *psecuritypriv = &padapter->securitypriv;
	struct host_xmit_priv *pxmitpriv = &padapter->xmitpriv;
	uint32_t res = HOST_SUCCESS;

	if (((struct host_xmit_frame *)pxmitframe)->buf_addr == NULL)
		return HOST_FAIL;

	hw_hdr_offset = HOST_TXDESC_OFFSET +
		(((struct host_xmit_frame *)pxmitframe)->pkt_offset * 8);

	pframe = ((struct host_xmit_frame *)pxmitframe)->buf_addr + hw_hdr_offset;

	if (pattrib->encrypt == _TKIP_) {
		if (HOST_IS_MCAST(pattrib->ra))
			prwskey = psecuritypriv->dot118021XGrpKey[psecuritypriv->dot118021XGrpKeyid].skey;
		else
			prwskey = pattrib->dot118021x_UncstKey.skey;

		for (curfragnum = 0; curfragnum < pattrib->nr_frags; curfragnum++) {
			iv = pframe + pattrib->hdrlen;
			payload = pframe + pattrib->iv_len + pattrib->hdrlen;

			HOST_GET_TKIP_PN(iv, dot11txpn);

			pnl = (uint16_t)(dot11txpn.val);
			pnh = (uint32_t)(dot11txpn.val >> 16);

			host_tkip_phase1((uint16_t *)&ttkey[0], prwskey, &pattrib->ta[0], pnh);
			host_tkip_phase2(&rc4key[0], prwskey, (uint16_t *)&ttkey[0], pnl);

			if ((curfragnum + 1) == pattrib->nr_frags) {
				length = pattrib->last_txcmdsz - pattrib->hdrlen -
					 pattrib->iv_len - pattrib->icv_len;
				*((uint32_t *)crc) =
					host_cpu_to_le32(getcrc32(payload, length));
				arcfour_init(&mycontext, rc4key, 16);
				arcfour_encrypt(&mycontext, payload, payload, length);
				arcfour_encrypt(&mycontext, payload + length, crc, 4);
			} else {
				length = pxmitpriv->frag_len - pattrib->hdrlen -
					 pattrib->iv_len - pattrib->icv_len;
				*((uint32_t *)crc) =
					host_cpu_to_le32(getcrc32(payload, length));
				arcfour_init(&mycontext, rc4key, 16);
				arcfour_encrypt(&mycontext, payload, payload, length);
				arcfour_encrypt(&mycontext, payload + length, crc, 4);

				pframe += pxmitpriv->frag_len;
				pframe = (uint8_t *)RND4((uintptr_t)(pframe));
			}
		}
	}

	return res;
}

#endif /* HOST_TKIP_FRAME_ORACLE_BUILD */
