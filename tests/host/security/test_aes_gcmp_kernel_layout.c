// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for the kernel layout of struct pkt_attrib on the AES/GCMP
 * software-TX path.
 *
 * rust/rtw_security_rest.rs used to overlay a 7-field stub that started at
 * `encrypt` on the real kernel struct (include/rtw_xmit.h), whose first
 * field is `type`. xmit_frame_attrib() returns offsetof(xmit_frame, attrib)
 * — the start of the struct — so (*pattrib).encrypt read type.
 *
 * WIFI_DATA_TYPE is BIT(3) == 8; _AES_ is 4 and _GCMP_ is 7. Every data
 * frame therefore took the "not our cipher" early return and left the
 * payload unencrypted. HW crypto is disabled when bswenc is set (EAPOL
 * 4/4 always sets it; so does sw_encrypt / hw_decrypted == false).
 *
 * This test compiles rust/rtw_security_rest.rs with the kernel cfgs (no
 * host_security_rest_test) and drives a real-order pkt_attrib fixture
 * whose type field is poisoned to WIFI_DATA_TYPE, so a regression back
 * to the overlay cannot pass.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define _SUCCESS 1
#define _FAIL 0
#define _AES_ 0x04
#define _GCMP_ 0x07
#define WIFI_DATA_TYPE 8
#define ETH_ALEN 6

/* Real kernel field order through ra (include/rtw_xmit.h; WDS/MESH/CSUM off). */
struct kernel_pkt_attrib {
	u8 type;
	u8 subtype;
	u8 bswenc;
	u8 dhcp_pkt;
	u16 ether_type;
	u16 seqnum;
	u8 hw_ssn_sel;
	u16 pkt_hdrlen;
	u16 hdrlen;
	u32 pktlen;
	u32 last_txcmdsz;
	u8 nr_frags;
	u8 encrypt;
	u8 bmc_camid;
	u8 iv_len;
	u8 icv_len;
	u8 iv[18];
	u8 icv[16];
	u8 priority;
	u8 ack_policy;
	u8 mac_id;
	u8 vcs_mode;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 uncst_key[32];
};

_Static_assert(offsetof(struct kernel_pkt_attrib, type) == 0, "type@0");
_Static_assert(offsetof(struct kernel_pkt_attrib, hdrlen) == 12, "hdrlen@12");
_Static_assert(offsetof(struct kernel_pkt_attrib, last_txcmdsz) == 20, "last_txcmdsz@20");
_Static_assert(offsetof(struct kernel_pkt_attrib, nr_frags) == 24, "nr_frags@24");
_Static_assert(offsetof(struct kernel_pkt_attrib, encrypt) == 25, "encrypt@25");
_Static_assert(offsetof(struct kernel_pkt_attrib, iv_len) == 27, "iv_len@27");
_Static_assert(offsetof(struct kernel_pkt_attrib, icv_len) == 28, "icv_len@28");
_Static_assert(offsetof(struct kernel_pkt_attrib, ra) == 85, "ra@85");

struct kernel_xmit_frame {
	u8 _pad0[16];
	struct kernel_pkt_attrib attrib;
	u8 *buf_addr;
	int8_t pkt_offset;
};

_Static_assert(offsetof(struct kernel_xmit_frame, attrib) == 16, "attrib@16");

struct kernel_xmit_priv {
	u32 frag_len;
};

struct kernel_security_priv {
	u32 grp_keyid;
	u8 grp_key[6][32];
	u64 aes_sw_enc_cnt_bc;
	u64 aes_sw_enc_cnt_mc;
	u64 aes_sw_enc_cnt_uc;
	u64 aes_sw_dec_cnt_bc;
	u64 aes_sw_dec_cnt_mc;
	u64 aes_sw_dec_cnt_uc;
	u64 gcmp_sw_enc_cnt_bc;
	u64 gcmp_sw_enc_cnt_mc;
	u64 gcmp_sw_enc_cnt_uc;
	u64 gcmp_sw_dec_cnt_bc;
	u64 gcmp_sw_dec_cnt_mc;
	u64 gcmp_sw_dec_cnt_uc;
	u32 privacy_algrthm;
	u32 privacy_key_index;
	u8 key_mask;
	u8 binstall_grpkey;
};

struct kernel_adapter {
	struct kernel_security_priv securitypriv;
	struct kernel_xmit_priv xmitpriv;
	u8 stapriv;
};

/* Offsets the Rust kernel path imports from core/rtw_security_rest.c. */
const size_t rtw_rust_wep_off_adapter_securitypriv =
	offsetof(struct kernel_adapter, securitypriv);
const size_t rtw_rust_wep_off_adapter_xmitpriv =
	offsetof(struct kernel_adapter, xmitpriv);
const size_t rtw_rust_wep_off_xmitpriv_frag_len =
	offsetof(struct kernel_xmit_priv, frag_len);
const size_t rtw_rust_wep_off_xmit_frame_attrib =
	offsetof(struct kernel_xmit_frame, attrib);
const size_t rtw_rust_wep_off_xmit_frame_buf_addr =
	offsetof(struct kernel_xmit_frame, buf_addr);
const size_t rtw_rust_wep_off_xmit_frame_pkt_offset =
	offsetof(struct kernel_xmit_frame, pkt_offset);
const size_t rtw_rust_tkip_off_securitypriv_dot118021XGrpKeyid =
	offsetof(struct kernel_security_priv, grp_keyid);
const size_t rtw_rust_tkip_off_securitypriv_dot118021XGrpKey =
	offsetof(struct kernel_security_priv, grp_key);
const size_t rtw_rust_tkip_off_pkt_attrib_dot118021x_UncstKey =
	offsetof(struct kernel_pkt_attrib, uncst_key);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_bc =
	offsetof(struct kernel_security_priv, aes_sw_enc_cnt_bc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_mc =
	offsetof(struct kernel_security_priv, aes_sw_enc_cnt_mc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_uc =
	offsetof(struct kernel_security_priv, aes_sw_enc_cnt_uc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_bc =
	offsetof(struct kernel_security_priv, aes_sw_dec_cnt_bc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_mc =
	offsetof(struct kernel_security_priv, aes_sw_dec_cnt_mc);
const size_t rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_uc =
	offsetof(struct kernel_security_priv, aes_sw_dec_cnt_uc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_bc =
	offsetof(struct kernel_security_priv, gcmp_sw_enc_cnt_bc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_mc =
	offsetof(struct kernel_security_priv, gcmp_sw_enc_cnt_mc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_uc =
	offsetof(struct kernel_security_priv, gcmp_sw_enc_cnt_uc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_bc =
	offsetof(struct kernel_security_priv, gcmp_sw_dec_cnt_bc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_mc =
	offsetof(struct kernel_security_priv, gcmp_sw_dec_cnt_mc);
const size_t rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_uc =
	offsetof(struct kernel_security_priv, gcmp_sw_dec_cnt_uc);
const size_t rtw_rust_wep_off_recv_frame_hdr = 0;
const size_t rtw_rust_wep_off_recv_frame_hdr_attrib = 0;
const size_t rtw_rust_wep_off_recv_frame_hdr_len = 8;
const size_t rtw_rust_wep_off_recv_frame_hdr_rx_data = 16;
const size_t rtw_rust_tkip_off_securitypriv_binstallGrpkey =
	offsetof(struct kernel_security_priv, binstall_grpkey);
const size_t rtw_rust_tkip_off_adapter_stapriv =
	offsetof(struct kernel_adapter, stapriv);
const size_t rtw_rust_tkip_off_sta_info_dot118021x_UncstKey = 0;
const size_t rtw_rust_wep_restore_off_securitypriv_dot11PrivacyAlgrthm =
	offsetof(struct kernel_security_priv, privacy_algrthm);
const size_t rtw_rust_wep_restore_off_securitypriv_dot11PrivacyKeyIndex =
	offsetof(struct kernel_security_priv, privacy_key_index);
const size_t rtw_rust_wep_restore_off_securitypriv_key_mask =
	offsetof(struct kernel_security_priv, key_mask);

/* C accessors — same contract as core/rtw_security_rest.c. */
u8 rtw_rust_pkt_attrib_encrypt(const struct kernel_pkt_attrib *a)
{
	return a->encrypt;
}

u8 rtw_rust_pkt_attrib_nr_frags(const struct kernel_pkt_attrib *a)
{
	return a->nr_frags;
}

u16 rtw_rust_pkt_attrib_hdrlen(const struct kernel_pkt_attrib *a)
{
	return a->hdrlen;
}

u32 rtw_rust_pkt_attrib_last_txcmdsz(const struct kernel_pkt_attrib *a)
{
	return a->last_txcmdsz;
}

u8 rtw_rust_pkt_attrib_iv_len(const struct kernel_pkt_attrib *a)
{
	return a->iv_len;
}

u8 rtw_rust_pkt_attrib_icv_len(const struct kernel_pkt_attrib *a)
{
	return a->icv_len;
}

void rtw_rust_pkt_attrib_ra(const struct kernel_pkt_attrib *a, u8 *out)
{
	memcpy(out, a->ra, ETH_ALEN);
}

u32 rtw_aes_encrypt(void *padapter, u8 *pxmitframe);
u32 rtw_gcmp_encrypt(void *padapter, u8 *pxmitframe);

static int g_ccmp_calls;
static int g_gcmp_calls;
static u32 g_last_hdrlen;
static u32 g_last_plen;
static u32 g_last_key_len;
static u8 g_last_key[32];
static u8 *g_last_frame;

int _rtw_ccmp_encrypt(void *padapter, u8 *key, u32 key_len, u32 hdrlen,
		      u8 *frame, u32 plen)
{
	(void)padapter;
	g_ccmp_calls++;
	g_last_hdrlen = hdrlen;
	g_last_plen = plen;
	g_last_key_len = key_len;
	g_last_frame = frame;
	if (key && key_len)
		memcpy(g_last_key, key, key_len > 32 ? 32 : key_len);
	return 1;
}

int _rtw_gcmp_encrypt(void *padapter, u8 *key, u32 key_len, u32 hdrlen,
		      u8 *frame, u32 plen)
{
	(void)padapter;
	g_gcmp_calls++;
	g_last_hdrlen = hdrlen;
	g_last_plen = plen;
	g_last_key_len = key_len;
	g_last_frame = frame;
	if (key && key_len)
		memcpy(g_last_key, key, key_len > 32 ? 32 : key_len);
	return 1;
}

int _rtw_ccmp_decrypt(void *padapter, u8 *key, u32 key_len, u32 hdrlen,
		      u8 *frame, u32 plen)
{
	(void)padapter;
	(void)key;
	(void)key_len;
	(void)hdrlen;
	(void)frame;
	(void)plen;
	return 1;
}

int _rtw_gcmp_decrypt(void *padapter, u8 *key, u32 key_len, u32 hdrlen,
		      u8 *frame, u32 plen)
{
	(void)padapter;
	(void)key;
	(void)key_len;
	(void)hdrlen;
	(void)frame;
	(void)plen;
	return 1;
}

void *rtw_get_stainfo(void *stapriv, u8 *hwaddr)
{
	(void)stapriv;
	(void)hwaddr;
	return NULL;
}

u8 rtw_tkip_decrypt_mcast_gkey_check(void *padapter, const u8 *ra,
				     u8 grpkey_installed)
{
	(void)padapter;
	(void)ra;
	(void)grpkey_installed;
	return 0;
}

u8 rtw_gcmp_decrypt_mcast_gkey_check(void *padapter, const u8 *ra,
				     u8 grpkey_installed)
{
	(void)padapter;
	(void)ra;
	(void)grpkey_installed;
	return 0;
}

void rtw_gcmp_decrypt_key_index_mismatch_dbg(u8 packet_index, u8 install_index)
{
	(void)packet_index;
	(void)install_index;
}

void rtw_aes_decipher_log_mic_mismatch(int i, u8 pframe_byte, u8 message_byte)
{
	(void)i;
	(void)pframe_byte;
	(void)message_byte;
}

u32 getcrc32(u8 *buf, int32_t len)
{
	(void)buf;
	(void)len;
	return 0;
}

int _aes_siv_encrypt(const u8 *key, size_t key_len, const u8 *pw, size_t pwlen,
		     size_t num_elem, const u8 *const *addr, const size_t *len,
		     u8 *out)
{
	(void)key;
	(void)key_len;
	(void)pw;
	(void)pwlen;
	(void)num_elem;
	(void)addr;
	(void)len;
	(void)out;
	return 0;
}

int _aes_siv_decrypt(const u8 *key, size_t key_len, const u8 *iv_crypt,
		     size_t iv_c_len, size_t num_elem, const u8 *const *addr,
		     const size_t *len, u8 *out)
{
	(void)key;
	(void)key_len;
	(void)iv_crypt;
	(void)iv_c_len;
	(void)num_elem;
	(void)addr;
	(void)len;
	(void)out;
	return 0;
}

int rtw_set_key(void *adapter, u8 *securitypriv, int32_t keyid, u8 set_tx,
		u8 enqueue)
{
	(void)adapter;
	(void)securitypriv;
	(void)keyid;
	(void)set_tx;
	(void)enqueue;
	return 1;
}

static int fail(const char *msg)
{
	fprintf(stderr, "FAIL: %s\n", msg);
	return 1;
}

static u8 g_frame[256];
static const u8 k_key[16] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};
static const u8 k_ra[ETH_ALEN] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

static void reset_counts(void)
{
	g_ccmp_calls = 0;
	g_gcmp_calls = 0;
	g_last_hdrlen = 0;
	g_last_plen = 0;
	g_last_key_len = 0;
	g_last_frame = NULL;
	memset(g_last_key, 0, sizeof(g_last_key));
}

static void init_fixture(struct kernel_adapter *adp, struct kernel_xmit_frame *xf,
			 u8 encrypt)
{
	memset(adp, 0, sizeof(*adp));
	memset(xf, 0, sizeof(*xf));
	memset(g_frame, 0xa5, sizeof(g_frame));

	adp->xmitpriv.frag_len = 2346;
	xf->buf_addr = g_frame;
	xf->pkt_offset = 0;

	xf->attrib.type = WIFI_DATA_TYPE; /* old overlay read this as encrypt */
	xf->attrib.encrypt = encrypt;
	xf->attrib.nr_frags = 1;
	xf->attrib.hdrlen = 24;
	xf->attrib.last_txcmdsz = 56;
	xf->attrib.iv_len = 8;
	xf->attrib.icv_len = 8;
	memcpy(xf->attrib.ra, k_ra, ETH_ALEN);
	memcpy(xf->attrib.uncst_key, k_key, sizeof(k_key));
}

int main(void)
{
	struct kernel_adapter adp;
	struct kernel_xmit_frame xf;

	/* 1) AES TX must encrypt when type==WIFI_DATA_TYPE and encrypt==_AES_.
	 * The old overlay would see type (8) and skip. */
	reset_counts();
	init_fixture(&adp, &xf, _AES_);
	if (rtw_aes_encrypt(&adp, (u8 *)&xf) != _SUCCESS)
		return fail("rtw_aes_encrypt returned fail");
	if (g_ccmp_calls != 1)
		return fail("rtw_aes_encrypt skipped CCMP (read encrypt from type@0)");
	if (g_last_hdrlen != 24)
		return fail("rtw_aes_encrypt used the wrong hdrlen");
	if (g_last_plen != 16)
		return fail("rtw_aes_encrypt used the wrong payload len");
	if (g_last_key_len != 16)
		return fail("rtw_aes_encrypt used the wrong key len");
	if (memcmp(g_last_key, k_key, 16) != 0)
		return fail("rtw_aes_encrypt used the wrong key");
	if (g_last_frame != g_frame + 48)
		return fail("rtw_aes_encrypt used the wrong frame offset");
	if (adp.securitypriv.aes_sw_enc_cnt_uc != 1)
		return fail("rtw_aes_encrypt did not bump the UC SW-enc counter");

	/* 2) type==_AES_ must not fake a cipher when the real encrypt is 0. */
	reset_counts();
	init_fixture(&adp, &xf, 0);
	xf.attrib.type = _AES_;
	if (rtw_aes_encrypt(&adp, (u8 *)&xf) != _SUCCESS)
		return fail("rtw_aes_encrypt(open) returned fail");
	if (g_ccmp_calls != 0)
		return fail("rtw_aes_encrypt honored poisoned type@0 as encrypt");

	/* 3) GCMP TX, same type-vs-encrypt split. */
	reset_counts();
	init_fixture(&adp, &xf, _GCMP_);
	if (rtw_gcmp_encrypt(&adp, (u8 *)&xf) != _SUCCESS)
		return fail("rtw_gcmp_encrypt returned fail");
	if (g_gcmp_calls != 1)
		return fail("rtw_gcmp_encrypt skipped GCMP (read encrypt from type@0)");
	if (g_last_hdrlen != 24)
		return fail("rtw_gcmp_encrypt used the wrong hdrlen");
	if (g_last_plen != 16)
		return fail("rtw_gcmp_encrypt used the wrong payload len");
	if (g_last_key_len != 16)
		return fail("rtw_gcmp_encrypt used the wrong key len");
	if (memcmp(g_last_key, k_key, 16) != 0)
		return fail("rtw_gcmp_encrypt used the wrong key");
	if (adp.securitypriv.gcmp_sw_enc_cnt_uc != 1)
		return fail("rtw_gcmp_encrypt did not bump the UC SW-enc counter");

	/* 4) type==_GCMP_ must not fake a cipher when encrypt is 0. */
	reset_counts();
	init_fixture(&adp, &xf, 0);
	xf.attrib.type = _GCMP_;
	if (rtw_gcmp_encrypt(&adp, (u8 *)&xf) != _SUCCESS)
		return fail("rtw_gcmp_encrypt(open) returned fail");
	if (g_gcmp_calls != 0)
		return fail("rtw_gcmp_encrypt honored poisoned type@0 as encrypt");

	printf("PASS: kernel pkt_attrib layout (encrypt@%zu, ra@%zu, type@0)\n",
	       (size_t)offsetof(struct kernel_pkt_attrib, encrypt),
	       (size_t)offsetof(struct kernel_pkt_attrib, ra));
	return 0;
}
