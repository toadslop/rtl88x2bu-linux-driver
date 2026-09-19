// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_ext_sitesurvey_cmd_types.h"
#include "host_vector_json.h"

extern void host_sitesurvey_cmd_reset_trace(void);

static _adapter g_adapter;

struct vector {
	char name[64];
	int initial_state, next_state, channel_idx;
	int ch_num, ch0, ch1, ch2, scan_ch_ms;
	int ps_annc;
	int expect_state, expect_next_state, expect_channel_idx;
	int expect_hw_survey, expect_igi, expect_msr, expect_site_survey, expect_pick_ch;
};

static void setup_adapter(struct vector *v)
{
	struct ss_res *ss = &g_adapter.mlmeextpriv.sitesurvey_res;
	struct sitesurvey_parm parm;

	memset(&g_adapter, 0, sizeof(g_adapter));
	ss->state = (u8)v->initial_state;
	ss->next_state = (u8)(v->next_state ? v->next_state : v->initial_state);
	ss->channel_idx = (u8)v->channel_idx;
	ss->scan_ch_ms = v->scan_ch_ms ? (u16)v->scan_ch_ms : 120;
	ss->scan_cnt_max = 8;
	ss->rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	ss->rx_ampdu_size = RX_AMPDU_SIZE_INVALID;

	memset(&parm, 0, sizeof(parm));
	parm.ch_num = (u8)(v->ch_num > 0 ? v->ch_num : 1);
	parm.ch[0].hw_value = (u16)(v->ch0 ? v->ch0 : 1);
	if (parm.ch_num > 1)
		parm.ch[1].hw_value = (u16)(v->ch1 ? v->ch1 : 6);
	if (parm.ch_num > 2)
		parm.ch[2].hw_value = (u16)(v->ch2 ? v->ch2 : 11);
	parm.scan_mode = SCAN_ACTIVE;

	host_sitesurvey_cmd_reset_trace();
	host_ps_annc_result = v->ps_annc ? 1 : 0;
	ss->ch_num = (u8)(v->ch_num > 0 ? v->ch_num : 1);
	if (ss->ch_num > 0)
		ss->ch[0].hw_value = (u16)(v->ch0 ? v->ch0 : 1);
	(void)sitesurvey_cmd_hdl(&g_adapter, (u8 *)&parm);
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(o, l, "initial_state", &v->initial_state);
	host_json_parse_int_in(o, l, "next_state", &v->next_state);
	host_json_parse_int_in(o, l, "channel_idx", &v->channel_idx);
	host_json_parse_int_in(o, l, "ch_num", &v->ch_num);
	host_json_parse_int_in(o, l, "ch0", &v->ch0);
	host_json_parse_int_in(o, l, "ch1", &v->ch1);
	host_json_parse_int_in(o, l, "ch2", &v->ch2);
	host_json_parse_int_in(o, l, "scan_ch_ms", &v->scan_ch_ms);
	host_json_parse_int_in(o, l, "ps_annc", &v->ps_annc);
	host_json_parse_int_in(o, l, "expect_state", &v->expect_state);
	host_json_parse_int_in(o, l, "expect_next_state", &v->expect_next_state);
	host_json_parse_int_in(o, l, "expect_channel_idx", &v->expect_channel_idx);
	host_json_parse_int_in(o, l, "expect_hw_survey", &v->expect_hw_survey);
	host_json_parse_int_in(o, l, "expect_igi", &v->expect_igi);
	host_json_parse_int_in(o, l, "expect_msr", &v->expect_msr);
	host_json_parse_int_in(o, l, "expect_site_survey", &v->expect_site_survey);
	host_json_parse_int_in(o, l, "expect_pick_ch", &v->expect_pick_ch);
	return 0;
}

static int run_vec(void *vv)
{
	struct vector *v = vv;
	struct ss_res *ss = &g_adapter.mlmeextpriv.sitesurvey_res;

	setup_adapter(v);
	if (v->expect_state && ss->state != (u8)v->expect_state) {
		fprintf(stderr, "%s: state got %u expect %d\n", v->name, ss->state,
			v->expect_state);
		return 1;
	}
	if (v->expect_next_state &&
	    ss->next_state != (u8)v->expect_next_state) {
		fprintf(stderr, "%s: next_state got %u expect %d\n", v->name,
			ss->next_state, v->expect_next_state);
		return 1;
	}
	if (v->expect_channel_idx &&
	    ss->channel_idx != (u8)v->expect_channel_idx) {
		fprintf(stderr, "%s: channel_idx got %u expect %d\n", v->name,
			ss->channel_idx, v->expect_channel_idx);
		return 1;
	}
	if (!!host_sitesurvey_cmd_trace.hw_survey_on != !!v->expect_hw_survey) {
		fprintf(stderr, "%s: hw_survey mismatch\n", v->name);
		return 1;
	}
	if (!!host_sitesurvey_cmd_trace.set_igi_enter != !!v->expect_igi) {
		fprintf(stderr, "%s: igi mismatch\n", v->name);
		return 1;
	}
	if (!!host_sitesurvey_cmd_trace.set_msr_enter != !!v->expect_msr) {
		fprintf(stderr, "%s: msr mismatch\n", v->name);
		return 1;
	}
	if (!!host_sitesurvey_cmd_trace.site_survey_ch != !!v->expect_site_survey) {
		fprintf(stderr, "%s: site_survey mismatch\n", v->name);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "mlme_ext_sitesurvey_cmd_vectors.json";
	struct vector vecs[16];
	size_t n = 0;
	size_t i;

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), sizeof(vecs) / sizeof(vecs[0]),
			      parse_vec, &n))
		return 1;
	for (i = 0; i < n; i++)
		if (run_vec(&vecs[i]))
			return 1;
#ifndef RUST_MLME_EXT_SITESURVEY_CMD_ORACLE
	printf("PASS %zu vectors (oracle: sitesurvey_cmd_host_oracle.c) (%s)\n", n, path);
#else
	printf("PASS %zu vectors (oracle: rust/rtw_mlme_ext_sitesurvey_cmd.rs) (%s)\n", n, path);
#endif
	return 0;
}
