// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_xmit_update_attrib_types.h"

static void setup_adapter(_adapter *a, u8 cur_wm, u16 rts_thresh, u32 frag_len)
{
	memset(a, 0, sizeof(*a));
	a->mlmeextpriv.cur_wireless_mode = cur_wm;
	a->registrypriv.rts_thresh = rts_thresh;
	a->registrypriv.vrtl_carrier_sense = AUTO_VCS;
	a->xmitpriv.frag_len = frag_len;
}

static int run_case(const char *name, u32 sz, u8 cur_wm, u8 ampdu, u8 expect)
{
	_adapter adapter;
	struct xmit_frame frame;

	setup_adapter(&adapter, cur_wm, 2347, 500);
	memset(&frame, 0, sizeof(frame));
	frame.attrib.nr_frags = 1;
	frame.attrib.last_txcmdsz = sz;
	frame.attrib.ampdu_en = ampdu;

	update_attrib_vcs_info(&adapter, &frame);
	if (frame.attrib.vcs_mode != expect) {
		fprintf(stderr, "%s: got %u expect %u\n", name,
			frame.attrib.vcs_mode, expect);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

int main(void)
{
	int fail = 0;

	fail |= run_case("vcs_legacy_rts_thresh", 3000, 3, 0, RTS_CTS);
	fail |= run_case("vcs_legacy_none", 100, 3, 0, NONE_VCS);
	fail |= run_case("vcs_ht_ampdu_rts", 100, WIRELESS_11_24N, 1, RTS_CTS);

	return fail ? 1 : 0;
}
