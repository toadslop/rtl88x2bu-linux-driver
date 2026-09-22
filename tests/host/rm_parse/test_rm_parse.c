// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rm_parse_types.h"

int rm_parse_ch_load_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);
int rm_parse_noise_histo_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);
int rm_parse_bcn_req_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);
int rm_parse_meas_req(struct rm_obj *prm, u8 *pbody);

static int test_ch_load(void)
{
	struct rm_obj prm;
	u8 body[] = { 0x01, 0x02, 0x00, 0x0a };

	memset(&prm, 0, sizeof(prm));
	if (rm_parse_ch_load_s_elem(&prm, body, sizeof(body)) != _SUCCESS)
		return 1;
	return prm.q.opt.clm.rep_cond.cond == 0 &&
	       prm.q.opt.clm.rep_cond.threshold == 10 ? 0 : 1;
}

static int test_noise(void)
{
	struct rm_obj prm;
	u8 body[] = { 0x01, 0x02, 0x00, 0x0b };

	memset(&prm, 0, sizeof(prm));
	if (rm_parse_noise_histo_s_elem(&prm, body, sizeof(body)) != _SUCCESS)
		return 1;
	return prm.q.opt.nhm.rep_cond.cond == 0 &&
	       prm.q.opt.nhm.rep_cond.threshold == 11 ? 0 : 1;
}

static int test_bcn(void)
{
	struct rm_obj prm;
	u8 body[] = { 0x00, 0x04, 't', 'e', 's', 't', 0x01, 0x02,
		      0x00, 0x01, 0x02, 0x01, 0x01 };

	memset(&prm, 0, sizeof(prm));
	if (rm_parse_bcn_req_s_elem(&prm, body, sizeof(body)) != _SUCCESS)
		return 1;
	if (prm.q.opt.bcn.opt_id_num != 2 || prm.q.opt.bcn.rep_detail != 1)
		return 1;
	return strncmp((char *)prm.q.opt.bcn.ssid.Ssid, "test", 4) ? 1 : 0;
}

static int test_meas_ch_load(void)
{
	struct rm_obj prm;
	u8 body[] = { 0x38, 0x0d, 0x01, 0x00, 0x03, 0x01, 0x06,
		      0x00, 0x00, 0x0a, 0x00, 0x01, 0x02, 0x00, 0x0c };

	memset(&prm, 0, sizeof(prm));
	prm.q.m_type = ch_load_req;
	if (rm_parse_meas_req(&prm, body) != _SUCCESS)
		return 1;
	return prm.q.op_class == 1 && prm.q.ch_num == 6 && prm.q.meas_dur == 10 &&
	       prm.q.opt.clm.rep_cond.threshold == 12
		   ? 0
		   : 1;
}

static int test_meas_bcn(void)
{
	struct rm_obj prm;
	u8 body[] = { 0x38, 0x13, 0x42, 0x00, 0x05, 0x01, 0x06, 0x00, 0x00,
		      0x0a, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		      0x02, 0x01, 0x01 };

	memset(&prm, 0, sizeof(prm));
	prm.q.m_type = bcn_req;
	if (rm_parse_meas_req(&prm, body) != _SUCCESS)
		return 1;
	return prm.q.op_class == 1 && prm.q.ch_num == 6 && prm.q.opt.bcn.rep_detail == 1
		   ? 0
		   : 1;
}

int main(void)
{
	if (test_ch_load() || test_noise() || test_bcn() || test_meas_ch_load() ||
	    test_meas_bcn()) {
		fprintf(stderr, "rm_parse vectors failed\n");
		return 1;
	}
	puts("rm_parse: 5 vectors OK");
	return 0;
}
