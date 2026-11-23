// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-133 MBSSID MAC address set/change. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

void host_mbid_cam_hw_oracle_reset(void);
struct host_mbid_cam_hw_trace *host_mbid_cam_hw_oracle_trace(void);
unsigned host_mbid_cam_oracle_warn_count(void);

struct _adapter *host_mbid_cam_adapter(void);
struct dvobj_priv *host_mbid_cam_dvobj(void);
void host_mbid_cam_reset_fixture(void);
void host_mbid_cam_set_entry_num(u8 n);

void rtw_hal_set_macaddr_mbid_c_oracle(struct _adapter *adapter, u8 *mac_addr);
void rtw_hal_change_macaddr_mbid_c_oracle(struct _adapter *adapter, u8 *mac_addr);

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
void rtw_hal_set_macaddr_mbid(struct _adapter *adapter, u8 *mac_addr);
void rtw_hal_change_macaddr_mbid(struct _adapter *adapter, u8 *mac_addr);
#endif

extern struct host_mbid_cam_hw_trace host_mbid_cam_hw_trace;

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
static void eq_u32(const char *tag, u32 a, u32 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%#x b=%#x\n", tag, a, b);
		failures++;
	}
}
#endif

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
static void eq_trace(const char *tag, const struct host_mbid_cam_hw_trace *r,
		     const struct host_mbid_cam_hw_trace *o)
{
	unsigned i;

	if (r->hwreg_count != o->hwreg_count) {
		fprintf(stderr, "FAIL %s hwreg_count %u %u\n", tag,
			r->hwreg_count, o->hwreg_count);
		failures++;
	}
	for (i = 0; i < r->hwreg_count && i < TOTAL_MBID_CAM_NUM; i++) {
		eq_u32("hw cam0", r->writes[i].cam_val[0], o->writes[i].cam_val[0]);
		eq_u32("hw cam1", r->writes[i].cam_val[1], o->writes[i].cam_val[1]);
		eq_u8("hw slot", r->writes[i].cam_id, o->writes[i].cam_id);
	}
}
#endif

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
static void test_set_macaddr_mbid_rust(void)
{
	static u8 mac[ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	struct _adapter *adap = host_mbid_cam_adapter();
	struct host_mbid_cam_hw_trace oracle_snapshot;

	host_mbid_cam_reset_fixture();
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0;

	host_mbid_cam_hw_oracle_reset();
	rtw_hal_set_macaddr_mbid_c_oracle(adap, mac);
	memcpy(&oracle_snapshot, host_mbid_cam_hw_oracle_trace(),
	       sizeof(oracle_snapshot));

	memset(&host_mbid_cam_hw_trace, 0, sizeof(host_mbid_cam_hw_trace));
	rtw_hal_set_macaddr_mbid(adap, mac);
	eq_trace("set_macaddr_mbid", &host_mbid_cam_hw_trace, &oracle_snapshot);
}

static void test_change_macaddr_mbid_rust(void)
{
	static u8 mac0[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	static u8 mac1[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	struct _adapter *adap = host_mbid_cam_adapter();
	struct host_mbid_cam_hw_trace oracle_snapshot;

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(0, 0, mac0, 1);
	host_mbid_cam_set_entry_num(1);

	host_mbid_cam_hw_oracle_reset();
	rtw_hal_change_macaddr_mbid_c_oracle(adap, mac1);
	memcpy(&oracle_snapshot, host_mbid_cam_hw_oracle_trace(),
	       sizeof(oracle_snapshot));

	memset(&host_mbid_cam_hw_trace, 0, sizeof(host_mbid_cam_hw_trace));
	rtw_hal_change_macaddr_mbid(adap, mac1);
	eq_trace("change_macaddr_mbid", &host_mbid_cam_hw_trace, &oracle_snapshot);
}

static void test_change_macaddr_null_rust(void)
{
	struct _adapter *adap = host_mbid_cam_adapter();
	unsigned warns_before = host_mbid_cam_oracle_warn_count();

	host_mbid_cam_reset_fixture();
	rtw_hal_change_macaddr_mbid(adap, NULL);
	eq_u8("change_null_warn", (u8)(host_mbid_cam_oracle_warn_count() - warns_before),
	      0);
}
#endif

#ifndef RUST_HAL_COM_MBID_CAM_ORACLE
void host_mbid_cam_prime_entry(u8 slot, u8 iface_id, const u8 *mac, u8 set_bitmap);

static void test_set_macaddr_mbid_oracle(void)
{
	static u8 mac[ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0;

	host_mbid_cam_hw_oracle_reset();
	rtw_hal_set_macaddr_mbid_c_oracle(adap, mac);
	eq_u8("set_hwreg_count",
	      (u8)host_mbid_cam_hw_oracle_trace()->hwreg_count, 1);
	eq_u8("set_cam_id", host_mbid_cam_hw_oracle_trace()->writes[0].cam_id, 0);
}

static void test_change_macaddr_mbid_oracle(void)
{
	static u8 mac0[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	static u8 mac1[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(0, 0, mac0, 1);
	host_mbid_cam_set_entry_num(1);

	host_mbid_cam_hw_oracle_reset();
	rtw_hal_change_macaddr_mbid_c_oracle(adap, mac1);
	eq_u8("change_hwreg_count",
	      (u8)host_mbid_cam_hw_oracle_trace()->hwreg_count, 1);
	eq_u8("change_cam_id", host_mbid_cam_hw_oracle_trace()->writes[0].cam_id, 0);
}

static void test_change_macaddr_null_oracle(void)
{
	struct _adapter *adap = host_mbid_cam_adapter();
	unsigned warns_before = host_mbid_cam_oracle_warn_count();

	host_mbid_cam_reset_fixture();
	rtw_hal_change_macaddr_mbid_c_oracle(adap, NULL);
	eq_u8("change_null_warn", (u8)(host_mbid_cam_oracle_warn_count() - warns_before),
	      1);
}
#endif /* !RUST_HAL_COM_MBID_CAM_ORACLE */

int main(void)
{
#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
	test_set_macaddr_mbid_rust();
	test_change_macaddr_mbid_rust();
	test_change_macaddr_null_rust();
#else
	test_set_macaddr_mbid_oracle();
	test_change_macaddr_mbid_oracle();
	test_change_macaddr_null_oracle();
#endif

	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam macaddr L2");
	return 0;
}