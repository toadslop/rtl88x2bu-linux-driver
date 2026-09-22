// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_mp_pmac_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_BITS 64

struct vector {
	char name[64];
	char op[16];
	char in_bits[MAX_BITS + 1];
	char out_bits[MAX_BITS + 1];
	char out_hex[32];
	u8 in_size;
	u8 mcs, b_spreamble;
	u32 pkt_len;
	u16 exp_sfd;
	u8 exp_sig, exp_svc;
	u32 exp_length;
	u8 exp_crc0, exp_crc1;
	u8 tx_rate, b_stbc;
	u8 exp_mcs, exp_nss, exp_nsts, exp_rate_hex, exp_m_stbc;
	u32 n_sym;
	u8 b_sgi;
	char exp_lsig_hex[16];
};

static int parse_bits(const char *s, bool *out, u8 cap, u8 *len_out)
{
	u8 n = 0;

	for (; s && *s && n < cap; s++) {
		if (*s == '0')
			out[n++] = 0;
		else if (*s == '1')
			out[n++] = 1;
		else if (*s != ' ')
			return -1;
	}
	*len_out = n;
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "in_bits", v->in_bits, sizeof(v->in_bits));
	host_json_parse_string_in(obj, len, "out_bits", v->out_bits, sizeof(v->out_bits));
	host_json_parse_string_in(obj, len, "out_hex", v->out_hex, sizeof(v->out_hex));
	if (!host_json_parse_int_in(obj, len, "in_size", &tmp))
		v->in_size = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "mcs", &tmp))
		v->mcs = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "b_spreamble", &tmp))
		v->b_spreamble = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "pkt_len", &tmp))
		v->pkt_len = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_sfd", &tmp))
		v->exp_sfd = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_sig", &tmp))
		v->exp_sig = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_svc", &tmp))
		v->exp_svc = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_length", &tmp))
		v->exp_length = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_crc0", &tmp))
		v->exp_crc0 = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_crc1", &tmp))
		v->exp_crc1 = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "tx_rate", &tmp))
		v->tx_rate = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "b_stbc", &tmp))
		v->b_stbc = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_mcs", &tmp))
		v->exp_mcs = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_nss", &tmp))
		v->exp_nss = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_nsts", &tmp))
		v->exp_nsts = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_rate_hex", &tmp))
		v->exp_rate_hex = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "exp_m_stbc", &tmp))
		v->exp_m_stbc = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "n_sym", &tmp))
		v->n_sym = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "b_sgi", &tmp))
		v->b_sgi = (u8)tmp;
	host_json_parse_string_in(obj, len, "exp_lsig_hex", v->exp_lsig_hex,
				  sizeof(v->exp_lsig_hex));
	return 0;
}

static int hex_eq(const u8 *got, size_t n, const char *expect_hex)
{
	unsigned char exp[16];
	size_t elen = 0;

	if (host_hex_decode(expect_hex, exp, sizeof(exp), &elen) || elen != n)
		return -1;
	return memcmp(got, exp, n) ? -1 : 0;
}

static int run_vec(struct vector *v)
{
	bool in[MAX_BITS], out[MAX_BITS];
	u8 nbits = 0, i;
	bool exp[MAX_BITS];

	if (strcmp(v->op, "byte_to_bit")) {
		if (parse_bits(v->in_bits, in, MAX_BITS, &nbits))
			return 1;
	}
	if (!strcmp(v->op, "crc16")) {
		u8 en = 0;

		CRC16_generator(out, in, 32);
		if (parse_bits(v->out_bits, exp, MAX_BITS, &en) || en != 16)
			return 1;
		for (i = 0; i < 16; i++)
			if (!!out[i] != !!exp[i])
				return 1;
	} else if (!strcmp(v->op, "crc8")) {
		u8 en = 0;

		CRC8_generator(out, in, nbits);
		if (parse_bits(v->out_bits, exp, MAX_BITS, &en) || en != 8)
			return 1;
		for (i = 0; i < 8; i++)
			if (!!out[i] != !!exp[i])
				return 1;
	} else if (!strcmp(v->op, "pkt_param")) {
		RT_PMAC_TX_INFO tx;
		RT_PMAC_PKT_INFO pkt;

		memset(&tx, 0, sizeof(tx));
		memset(&pkt, 0, sizeof(pkt));
		tx.TX_RATE = v->tx_rate;
		tx.bSTBC = v->b_stbc;
		PMAC_Get_Pkt_Param(&tx, &pkt);
		if (pkt.MCS != v->exp_mcs || pkt.Nss != v->exp_nss ||
		    pkt.Nsts != v->exp_nsts || tx.TX_RATE_HEX != v->exp_rate_hex ||
		    tx.m_STBC != v->exp_m_stbc)
			return 1;
	} else if (!strcmp(v->op, "l_sig")) {
		RT_PMAC_TX_INFO tx;
		RT_PMAC_PKT_INFO pkt;

		memset(&tx, 0, sizeof(tx));
		memset(&pkt, 0, sizeof(pkt));
		tx.TX_RATE = v->tx_rate;
		tx.PacketLength = v->pkt_len;
		tx.bSGI = v->b_sgi;
		pkt.MCS = v->exp_mcs;
		pkt.Nsts = v->exp_nsts;
		L_SIG_generator(v->n_sym, &tx, &pkt);
		if (hex_eq(tx.LSIG, 3, v->exp_lsig_hex))
			return 1;
	} else if (!strcmp(v->op, "cck")) {
		RT_PMAC_TX_INFO tx;
		RT_PMAC_PKT_INFO pkt;

		memset(&tx, 0, sizeof(tx));
		memset(&pkt, 0, sizeof(pkt));
		tx.PacketLength = v->pkt_len;
		tx.bSPreamble = v->b_spreamble;
		pkt.MCS = v->mcs;
		CCK_generator(&tx, &pkt);
		if (tx.SFD != v->exp_sfd || tx.SignalField != v->exp_sig ||
		    tx.LENGTH != v->exp_length || tx.ServiceField != v->exp_svc ||
		    tx.CRC16[0] != v->exp_crc0 || tx.CRC16[1] != v->exp_crc1)
			return 1;
	} else if (!strcmp(v->op, "byte_to_bit")) {
		u8 bytes[8];
		unsigned char expect[8];
		size_t elen = 0;

		if (parse_bits(v->in_bits, in, MAX_BITS, &nbits) ||
		    nbits != (u8)(v->in_size * 8))
			return 1;
		memset(bytes, 0, sizeof(bytes));
		ByteToBit(bytes, in, v->in_size);
		if (host_hex_decode(v->out_hex, expect, sizeof(expect), &elen) ||
		    elen != (size_t)v->in_size ||
		    memcmp(bytes, expect, elen))
			return 1;
	} else {
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	int failed = 0;
	const char *path = (argc > 1) ? argv[1] : "mp_pmac_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vec, &nvec))
		return 2;
	for (size_t j = 0; j < nvec; j++)
		failed += run_vec(&vectors[j]) != 0;
	printf(failed ? "" : "PASS %zu vectors (%s)\n", nvec, path);
	return failed ? 1 : 0;
}
