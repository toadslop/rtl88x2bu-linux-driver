// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-129 TSF iface enable/disable. */

#include "host_hal_com_tsf_types.h"
#include <stdio.h>
#include <string.h>

void host_hal_com_tsf_reset(void);
struct _adapter *host_hal_com_tsf_adapter(void);
extern struct host_hal_com_tsf_trace host_hal_com_tsf_trace;

void rtw_iface_enable_tsf_update_c_oracle(struct _adapter *adapter);
void rtw_iface_disable_tsf_update_c_oracle(struct _adapter *adapter);

#ifdef RUST_HAL_COM_TSF_ORACLE
void rtw_iface_enable_tsf_update(struct _adapter *adapter);
void rtw_iface_disable_tsf_update(struct _adapter *adapter);
#else
#define rtw_iface_enable_tsf_update rtw_iface_enable_tsf_update_c_oracle
#define rtw_iface_disable_tsf_update rtw_iface_disable_tsf_update_c_oracle
#endif

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void cmp_adapter(const char *tag, struct _adapter *a, struct _adapter *b,
			const struct host_hal_com_tsf_trace *ta,
			const struct host_hal_com_tsf_trace *tb)
{
	if (memcmp(a, b, sizeof(*a)) != 0)
		fail(tag);
	if (ta->set_tsf_update_count != tb->set_tsf_update_count ||
	    ta->last_set_tsf_en != tb->last_set_tsf_en)
		fail(tag);
}

static void run_pair(void (*under_test)(struct _adapter *),
		     void (*oracle)(struct _adapter *), u8 prime_en_hw)
{
	struct _adapter s, o;
	struct host_hal_com_tsf_trace tr_s, tr_o;

	host_hal_com_tsf_reset();
	s = *host_hal_com_tsf_adapter();
	o = s;
	s.mlmeextpriv.tsf_update_pause_stime = 99;
	s.mlmeextpriv.en_hw_update_tsf = prime_en_hw;
	s.mlmeextpriv.tsf_update_required = 1;
	o = s;

	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	under_test(&s);
	tr_s = host_hal_com_tsf_trace;

	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	oracle(&o);
	tr_o = host_hal_com_tsf_trace;

	cmp_adapter("pair", &s, &o, &tr_s, &tr_o);
}

int main(void)
{
	run_pair(rtw_iface_enable_tsf_update, rtw_iface_enable_tsf_update_c_oracle, 1);
	run_pair(rtw_iface_enable_tsf_update, rtw_iface_enable_tsf_update_c_oracle, 0);
	run_pair(rtw_iface_disable_tsf_update, rtw_iface_disable_tsf_update_c_oracle, 1);

	if (failures)
		return 1;
	puts("OK hal_com_tsf_iface L2");
	return 0;
}