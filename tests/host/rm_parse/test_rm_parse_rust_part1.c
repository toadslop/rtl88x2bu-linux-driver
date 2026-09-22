// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rm_parse_types.h"

int rm_parse_ch_load_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);
int rm_parse_noise_histo_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);
int rm_parse_bcn_req_s_elem(struct rm_obj *prm, u8 *pbody, int req_len);

int main(void)
{
	struct rm_obj prm;
	u8 cl[] = { 0x01, 0x02, 0x00, 0x0a };
	u8 nh[] = { 0x01, 0x02, 0x00, 0x0b };
	u8 bc[] = { 0x00, 0x04, 't', 'e', 's', 't', 0x01, 0x02,
		    0x00, 0x01, 0x02, 0x01, 0x01 };

	memset(&prm, 0, sizeof(prm));
	if (rm_parse_ch_load_s_elem(&prm, cl, sizeof(cl)) != _SUCCESS ||
	    prm.q.opt.clm.rep_cond.threshold != 10)
		return 1;
	memset(&prm, 0, sizeof(prm));
	if (rm_parse_noise_histo_s_elem(&prm, nh, sizeof(nh)) != _SUCCESS ||
	    prm.q.opt.nhm.rep_cond.threshold != 11)
		return 1;
	memset(&prm, 0, sizeof(prm));
	if (rm_parse_bcn_req_s_elem(&prm, bc, sizeof(bc)) != _SUCCESS ||
	    prm.q.opt.bcn.rep_detail != 1)
		return 1;
	puts("rm_parse rust part1: 3 vectors OK");
	return 0;
}
