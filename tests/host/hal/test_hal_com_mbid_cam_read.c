// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-141 MBSSID CAM HW read. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

struct _adapter *host_mbid_cam_adapter(void);
void host_mbid_cam_reset_fixture(void);
void host_mbid_cam_read_reset(void);
void host_mbid_cam_read_seed(u8 cam_addr, const u8 *mac, u8 poll_reads_before_clear);
void host_sel_reset(struct host_sel_capture *cap);

void read_mbssid_cam_probe_c_oracle(void *adapter, u8 cam_addr, u8 *mac);
int rtw_mbid_cam_dump_c_oracle(void *sel, const char *fun_name, void *adapter);

#ifdef RUST_HAL_COM_MBID_CAM_READ_ORACLE
void read_mbssid_cam_probe(void *adapter, u8 cam_addr, u8 *mac);
int rtw_mbid_cam_dump(void *sel, const char *fun_name, void *adapter);
#endif

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void eq_u32(const char *tag, unsigned a, unsigned b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void eq_mac(const char *tag, const u8 *a, const u8 *b)
{
	unsigned i;

	for (i = 0; i < ETH_ALEN; i++) {
		if (a[i] != b[i]) {
			fprintf(stderr, "FAIL %s byte %u %#x %#x\n", tag, i, a[i], b[i]);
			failures++;
			return;
		}
	}
}

static void expect_mac_zero(const char *tag, const u8 *mac)
{
	static const u8 zero[ETH_ALEN];

	eq_mac(tag, mac, zero);
}

static void eq_trace_slot(const char *tag, u8 cam, unsigned cfg2_reads)
{
	struct host_mbid_cam_read_trace *tr = host_mbid_cam_read_trace_ptr();

	if (!tr->cfg2_write_count) {
		fprintf(stderr, "FAIL %s no cfg2 write\n", tag);
		failures++;
		return;
	}
	eq_u8(tag, tr->last_cam_addr, cam);
	if (tr->cfg2_read32_count != cfg2_reads) {
		fprintf(stderr, "FAIL %s cfg2_read32 want %u got %u\n",
			tag, cfg2_reads, tr->cfg2_read32_count);
		failures++;
	}
	if (!tr->cfg1_read32_count) {
		fprintf(stderr, "FAIL %s no cfg1 read\n", tag);
		failures++;
	}
	if (!tr->cfg2_read16_count) {
		fprintf(stderr, "FAIL %s no cfg2 read16\n", tag);
		failures++;
	}
}

static void run_probe_dut(void *adapter, u8 cam, u8 *mac)
{
#ifdef RUST_HAL_COM_MBID_CAM_READ_ORACLE
	read_mbssid_cam_probe(adapter, cam, mac);
#else
	read_mbssid_cam_probe_c_oracle(adapter, cam, mac);
#endif
}

static void test_read_slot3_immediate(void)
{
	static const u8 mac[ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	u8 got[ETH_ALEN];
	u8 oracle[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	memset(got, 0xcc, sizeof(got));
	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(3, mac, 1);
	run_probe_dut(adap, 3, got);
	eq_trace_slot("slot3", 3, 1);
	eq_mac("slot3_mac", got, mac);

	host_mbid_cam_read_reset();
	host_mbid_cam_read_seed(3, mac, 1);
	memset(oracle, 0xcc, sizeof(oracle));
	read_mbssid_cam_probe_c_oracle(adap, 3, oracle);
	eq_mac("slot3_oracle", got, oracle);
}

static void test_read_slot5_delayed_poll(void)
{
	static const u8 mac[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	u8 got[ETH_ALEN];
	u8 oracle[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	memset(got, 0xcc, sizeof(got));
	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(5, mac, 3);
	run_probe_dut(adap, 5, got);
	eq_trace_slot("slot5", 5, 3);
	eq_mac("slot5_mac", got, mac);

	host_mbid_cam_read_reset();
	host_mbid_cam_read_seed(5, mac, 3);
	memset(oracle, 0xcc, sizeof(oracle));
	read_mbssid_cam_probe_c_oracle(adap, 5, oracle);
	eq_mac("slot5_oracle", got, oracle);
}

static void test_read_poll_budget_timeout(void)
{
	static const u8 mac[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	u8 got[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	memset(got, 0, sizeof(got));
	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(2, mac, 10);
	run_probe_dut(adap, 2, got);
	eq_u32("timeout_cfg2_reads", host_mbid_cam_read_trace_ptr()->cfg2_read32_count, 9);
	expect_mac_zero("timeout_mac", got);
}

static void test_cannot_run_before_poll_write(void)
{
	static const u8 mac[ETH_ALEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	u8 got[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	memset(got, 0xab, sizeof(got));
	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(4, mac, 1);
	host_mbid_cam_set_force_cannot_run(1);
	run_probe_dut(adap, 4, got);
	eq_u32("cannot_run_writes", host_mbid_cam_read_trace_ptr()->cfg2_write_count, 0);
	{
		static const u8 unchanged[ETH_ALEN] = {0xab, 0xab, 0xab, 0xab, 0xab, 0xab};

		eq_mac("cannot_run_mac", got, unchanged);
	}
}

static void test_cannot_run_mid_poll(void)
{
	static const u8 mac[ETH_ALEN] = {0xde, 0xad, 0xbe, 0xef, 0x00, 0x01};
	u8 got[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	memset(got, 0, sizeof(got));
	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(6, mac, 5);
	host_mbid_cam_set_cannot_run_after_poll_reads(2);
	run_probe_dut(adap, 6, got);
	eq_u32("mid_cannot_cfg2_reads", host_mbid_cam_read_trace_ptr()->cfg2_read32_count, 2);
	expect_mac_zero("mid_cannot_mac", got);
}

static int run_dump_dut(struct host_sel_capture *sel, const char *fun, void *adap)
{
#ifdef RUST_HAL_COM_MBID_CAM_READ_ORACLE
	return rtw_mbid_cam_dump(sel, fun, adap);
#else
	return rtw_mbid_cam_dump_c_oracle(sel, fun, adap);
#endif
}

static void test_dump_full_capture(void)
{
	static const u8 mac1[ETH_ALEN] = {0x48, 0x89, 0x4a, 0x2b, 0x10, 0x99};
	static const u8 mac3[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	struct host_sel_capture got;
	struct host_sel_capture oracle;
	char got_buf[sizeof(got.buf)];
	char oracle_buf[sizeof(oracle.buf)];
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(1, mac1, 1);
	host_mbid_cam_read_seed(3, mac3, 1);
	host_mbid_cam_read_seed(4, mac3, 10);

	host_sel_reset(&got);
	run_dump_dut(&got, "test_dump", adap);
	if (host_sel_string(&got, got_buf, sizeof(got_buf)) < 0) {
		fprintf(stderr, "FAIL dump got overflow\n");
		failures++;
		return;
	}

	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(1, mac1, 1);
	host_mbid_cam_read_seed(3, mac3, 1);
	host_mbid_cam_read_seed(4, mac3, 10);
	host_sel_reset(&oracle);
	rtw_mbid_cam_dump_c_oracle(&oracle, "test_dump", adap);
	if (host_sel_string(&oracle, oracle_buf, sizeof(oracle_buf)) < 0) {
		fprintf(stderr, "FAIL dump oracle overflow\n");
		failures++;
		return;
	}
	if (strcmp(got_buf, oracle_buf) != 0) {
		fprintf(stderr, "FAIL dump capture mismatch\n");
		failures++;
	}
}

int main(void)
{
	test_read_slot3_immediate();
	test_read_slot5_delayed_poll();
	test_read_poll_budget_timeout();
	test_cannot_run_before_poll_write();
	test_cannot_run_mid_poll();
	test_dump_full_capture();
	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam_read L2");
	return 0;
}