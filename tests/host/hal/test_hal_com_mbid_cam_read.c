// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-141 MBSSID CAM HW read. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

struct _adapter *host_mbid_cam_adapter(void);
void host_mbid_cam_reset_fixture(void);
void host_mbid_cam_read_reset(void);
void host_mbid_cam_read_seed(u8 cam_addr, const u8 *mac, u8 poll_reads_before_clear);

void read_mbssid_cam_probe_c_oracle(void *adapter, u8 cam_addr, u8 *mac);

#ifdef RUST_HAL_COM_MBID_CAM_READ_ORACLE
void read_mbssid_cam_probe(void *adapter, u8 cam_addr, u8 *mac);
#endif

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
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

static void eq_trace_slot(const char *tag, u8 cam)
{
	struct host_mbid_cam_read_trace *tr = host_mbid_cam_read_trace_ptr();

	if (!tr->cfg2_write_count) {
		fprintf(stderr, "FAIL %s no cfg2 write\n", tag);
		failures++;
		return;
	}
	eq_u8(tag, tr->last_cam_addr, cam);
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

	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(3, mac, 1);
	run_probe_dut(adap, 3, got);
	eq_trace_slot("slot3", 3);
	eq_mac("slot3_mac", got, mac);

	host_mbid_cam_read_reset();
	host_mbid_cam_read_seed(3, mac, 1);
	read_mbssid_cam_probe_c_oracle(adap, 3, oracle);
	eq_mac("slot3_oracle", got, oracle);
}

static void test_read_slot5_delayed_poll(void)
{
	static const u8 mac[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	u8 got[ETH_ALEN];
	u8 oracle[ETH_ALEN];
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_read_seed(5, mac, 3);
	run_probe_dut(adap, 5, got);
	eq_trace_slot("slot5", 5);

	host_mbid_cam_read_reset();
	host_mbid_cam_read_seed(5, mac, 3);
	read_mbssid_cam_probe_c_oracle(adap, 5, oracle);
	eq_mac("slot5_oracle", got, oracle);
}

int main(void)
{
	test_read_slot3_immediate();
	test_read_slot5_delayed_poll();
	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam_read L2");
	return 0;
}