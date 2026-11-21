// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-126 MBSSID CAM HW push. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

void host_mbid_cam_hw_oracle_reset(void);
void host_mbid_cam_hw_oracle_set_reg(u8 val);
struct host_mbid_cam_hw_trace *host_mbid_cam_hw_oracle_trace(void);

struct _adapter *host_mbid_cam_adapter(void);
struct dvobj_priv *host_mbid_cam_dvobj(void);
void host_mbid_cam_reset_fixture(void);
void host_mbid_cam_prime_entry(u8 slot, u8 iface_id, const u8 *mac, u8 set_bitmap);

void rtw_ap_set_mbid_num_c_oracle(struct _adapter *adapter, u8 ap_num);
void rtw_mi_set_mbid_cam_c_oracle(struct _adapter *adapter);

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
void rtw_ap_set_mbid_num(struct _adapter *adapter, u8 ap_num);
void rtw_mi_set_mbid_cam(struct _adapter *adapter);
#endif

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void eq_u32(const char *tag, u32 a, u32 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%#x b=%#x\n", tag, a, b);
		failures++;
	}
}

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
static void eq_trace(const char *tag, const struct host_mbid_cam_hw_trace *r,
		     const struct host_mbid_cam_hw_trace *o)
{
	unsigned i;

	eq_u8(tag, r->reg_mbid_num, o->reg_mbid_num);
	if (r->hwreg_count != o->hwreg_count) {
		fprintf(stderr, "FAIL %s hwreg_count %u %u\n", tag,
			r->hwreg_count, o->hwreg_count);
		failures++;
	}
	if (r->rcr_add_count != o->rcr_add_count) {
		fprintf(stderr, "FAIL %s rcr_add_count %u %u\n", tag,
			r->rcr_add_count, o->rcr_add_count);
		failures++;
	}
	eq_u32(tag, r->last_rcr_add, o->last_rcr_add);
	for (i = 0; i < r->hwreg_count && i < TOTAL_MBID_CAM_NUM; i++) {
		eq_u32("hw cam0", r->writes[i].cam_val[0], o->writes[i].cam_val[0]);
		eq_u32("hw cam1", r->writes[i].cam_val[1], o->writes[i].cam_val[1]);
		eq_u8("hw slot", r->writes[i].cam_id, o->writes[i].cam_id);
	}
}
#endif

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
static void test_ap_set_mbid_num_rust(void)
{
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_hw_trace.reg_mbid_num = 0x55;
	rtw_ap_set_mbid_num(adap, 3);
	host_mbid_cam_hw_oracle_reset();
	host_mbid_cam_hw_oracle_set_reg(0x55);
	rtw_ap_set_mbid_num_c_oracle(adap, 3);
	eq_trace("ap_set_mbid_num", &host_mbid_cam_hw_trace,
		 host_mbid_cam_hw_oracle_trace());
}

static void test_mi_set_mbid_cam_rust(void)
{
	static const u8 mac0[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	static const u8 mac1[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	struct _adapter *adap = host_mbid_cam_adapter();
	struct host_mbid_cam_hw_trace oracle_snapshot;

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(0, 0, mac0, 1);
	host_mbid_cam_prime_entry(1, 1, mac1, 1);
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0x03;

	host_mbid_cam_hw_oracle_reset();
	rtw_mi_set_mbid_cam_c_oracle(adap);
	memcpy(&oracle_snapshot, host_mbid_cam_hw_oracle_trace(),
	       sizeof(oracle_snapshot));
	memset(&host_mbid_cam_hw_trace, 0, sizeof(host_mbid_cam_hw_trace));
	rtw_mi_set_mbid_cam(adap);
	eq_trace("mi_set_mbid_cam", &host_mbid_cam_hw_trace, &oracle_snapshot);
}
#else
static void test_ap_set_mbid_num_oracle(void)
{
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_hw_oracle_reset();
	host_mbid_cam_hw_oracle_set_reg(0x55);
	rtw_ap_set_mbid_num_c_oracle(adap, 3);
	eq_u8("ap_set_mbid_num", host_mbid_cam_hw_oracle_trace()->reg_mbid_num, 0x52);
}

static void test_mi_set_mbid_cam_oracle(void)
{
	static const u8 mac0[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	static const u8 mac1[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(0, 0, mac0, 1);
	host_mbid_cam_prime_entry(1, 1, mac1, 1);
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0x03;

	host_mbid_cam_hw_oracle_reset();
	rtw_mi_set_mbid_cam_c_oracle(adap);
	if (host_mbid_cam_hw_oracle_trace()->hwreg_count != 2) {
		fprintf(stderr, "FAIL mi_set hwreg_count\n");
		failures++;
	}
	eq_u32("rcr", host_mbid_cam_hw_oracle_trace()->last_rcr_add, HOST_RCR_ENMBID);
}
#endif

int main(void)
{
#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
	test_ap_set_mbid_num_rust();
	test_mi_set_mbid_cam_rust();
#else
	test_ap_set_mbid_num_oracle();
	test_mi_set_mbid_cam_oracle();
#endif

	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam hw L2");
	return 0;
}