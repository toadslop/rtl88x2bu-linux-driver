// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_wnm_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 64
#define MAX_FRAME 128

struct vector {
	char name[MAX_NAME];
	char op[24];
	char frame_hex[512];
	int expect_offset;
	int expect_dialog;
	int expect_req_mode;
	int expect_disassoc;
	int expect_validity;
	int expect_term_id;
	int expect_term_len;
	char expect_tsf_hex[32];
	int expect_duration;
	int expect_valid;
	int passing_ms;
	int validity_time;
	int disassoc_time;
	int flag;
	int frame_len;
	int expect_sz;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "frame_hex", v->frame_hex,
				  sizeof(v->frame_hex));
	host_json_parse_int_in(obj, len, "expect_offset", &v->expect_offset);
	host_json_parse_int_in(obj, len, "expect_dialog", &v->expect_dialog);
	host_json_parse_int_in(obj, len, "expect_req_mode", &v->expect_req_mode);
	host_json_parse_int_in(obj, len, "expect_disassoc", &v->expect_disassoc);
	host_json_parse_int_in(obj, len, "expect_validity", &v->expect_validity);
	host_json_parse_int_in(obj, len, "expect_term_id", &v->expect_term_id);
	host_json_parse_int_in(obj, len, "expect_term_len", &v->expect_term_len);
	host_json_parse_string_in(obj, len, "expect_tsf_hex", v->expect_tsf_hex,
				  sizeof(v->expect_tsf_hex));
	host_json_parse_int_in(obj, len, "expect_duration", &v->expect_duration);
	host_json_parse_int_in(obj, len, "expect_valid", &v->expect_valid);
	host_json_parse_int_in(obj, len, "passing_ms", &v->passing_ms);
	host_json_parse_int_in(obj, len, "validity_time", &v->validity_time);
	host_json_parse_int_in(obj, len, "disassoc_time", &v->disassoc_time);
	host_json_parse_int_in(obj, len, "flag", &v->flag);
	host_json_parse_int_in(obj, len, "frame_len", &v->frame_len);
	host_json_parse_int_in(obj, len, "expect_sz", &v->expect_sz);
	return 0;
}

static int run_vec(struct vector *v)
{
	u8 frame[MAX_FRAME];
	size_t frame_len = 0;
	struct btm_req_hdr hdr;

	if (v->frame_hex[0] &&
	    host_hex_decode(v->frame_hex, frame, sizeof(frame), &frame_len))
		return 1;

	if (!strcmp(v->op, "candidates_offset")) {
		u32 off = host_wnm_btm_candidates_offset_get(frame);

		if ((int)off != v->expect_offset)
			return 1;
	} else if (!strcmp(v->op, "req_hdr_parsing")) {
		memset(&hdr, 0xff, sizeof(hdr));
		host_wnm_btm_req_hdr_parsing(frame, &hdr);
		if (hdr.dialog_token != (u8)v->expect_dialog ||
		    hdr.req_mode != (u8)v->expect_req_mode ||
		    hdr.disassoc_timer != (u16)v->expect_disassoc ||
		    hdr.validity_interval != (u8)v->expect_validity)
			return 1;
		if (v->expect_term_id > 0 &&
		    (hdr.term_duration.id != (u8)v->expect_term_id ||
		     hdr.term_duration.len != (u8)v->expect_term_len ||
		     hdr.term_duration.duration != (u16)v->expect_duration))
			return 1;
		if (v->expect_tsf_hex[0]) {
			u8 tsf[8];
			size_t tsf_len = 0;

			if (host_hex_decode(v->expect_tsf_hex, tsf, sizeof(tsf),
					    &tsf_len) ||
			    tsf_len != 8 ||
			    memcmp(&hdr.term_duration.tsf, tsf, 8))
				return 1;
		}
	} else if (!strcmp(v->op, "candidate_validity")) {
		struct btm_rpt_cache cache = { .req_stime = 1 };

		cache.validity_time = (u32)v->validity_time;
		cache.disassoc_time = (u32)v->disassoc_time;
		host_wnm_set_passing_ms((u32)v->passing_ms);
		if (host_wnm_btm_candidate_validity(&cache, (u8)v->flag) !=
		    (u8)v->expect_valid)
			return 1;
	} else if (!strcmp(v->op, "rsp_candidates_sz")) {
		_adapter a;
		u32 flen = v->frame_len ? (u32)v->frame_len : (u32)frame_len;
		u32 sz = host_wnm_btm_rsp_candidates_sz_get(&a, frame, flen);

		if (sz != (u32)v->expect_sz)
			return 1;
	} else {
		return 1;
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "wnm_btm_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec,
			      &n)) {
		fprintf(stderr, "load vectors failed\n");
		return 1;
	}
	for (size_t i = 0; i < n; i++)
		if (run_vec(&vecs[i]))
			bad++;
	if (bad)
		fprintf(stderr, "%d/%zu vectors failed\n", bad, n);
	return bad ? 1 : 0;
}
