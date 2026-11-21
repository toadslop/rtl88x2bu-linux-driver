// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-119 MBSSID CAM init/search. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

void rtw_mbid_cam_cache_dump_c_oracle(void *sel, const char *fn, struct _adapter *a);
#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
int rtw_mbid_cam_cache_dump(void *sel, const char *fn, struct _adapter *a);
#else
#define rtw_mbid_cam_cache_dump rtw_mbid_cam_cache_dump_c_oracle
#endif

void host_mbid_cam_reset_fixture(void);
struct _adapter *host_mbid_cam_adapter(void);
struct dvobj_priv *host_mbid_cam_dvobj(void);
void host_mbid_cam_prime_entry(u8 slot, u8 iface_id, const u8 *mac, u8 set_bitmap);
void host_mbid_cam_set_entry_num(u8 n);
#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
void rtw_mbid_cam_init(struct dvobj_priv *dvobj);
void rtw_mbid_cam_reset(struct _adapter *adapter);
u8 rtw_mbid_cam_search_by_macaddr(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_search_by_ifaceid(struct _adapter *adapter, u8 iface_id);
u8 rtw_get_max_mbid_cam_id(struct _adapter *adapter);
u8 rtw_get_mbid_cam_entry_num(struct _adapter *adapter);
#else
#define rtw_mbid_cam_init rtw_mbid_cam_init_c_oracle
#define rtw_mbid_cam_reset rtw_mbid_cam_reset_c_oracle
#define rtw_mbid_cam_search_by_macaddr rtw_mbid_cam_search_by_macaddr_c_oracle
#define rtw_mbid_cam_search_by_ifaceid rtw_mbid_cam_search_by_ifaceid_c_oracle
#define rtw_get_max_mbid_cam_id rtw_get_max_mbid_cam_id_c_oracle
#define rtw_get_mbid_cam_entry_num rtw_get_mbid_cam_entry_num_c_oracle
#endif

void rtw_mbid_cam_init_c_oracle(struct dvobj_priv *dvobj);
void rtw_mbid_cam_reset_c_oracle(struct _adapter *adapter);
u8 rtw_mbid_cam_search_by_macaddr_c_oracle(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_search_by_ifaceid_c_oracle(struct _adapter *adapter, u8 iface_id);
u8 rtw_get_max_mbid_cam_id_c_oracle(struct _adapter *adapter);
u8 rtw_get_mbid_cam_entry_num_c_oracle(struct _adapter *adapter);

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void expect_cam_cleared(const char *tag, struct dvobj_priv *d)
{
	u8 i;

	eq_u8(tag, d->mbid_cam_ctl.bitmap, 0);
	eq_u8(tag, (u8)ATOMIC_READ(&d->mbid_cam_ctl.mbid_entry_num), 0);
	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		if (d->mbid_cam_cache[i].iface_id != 0 ||
		    memcmp(d->mbid_cam_cache[i].mac_addr, "\0\0\0\0\0\0", ETH_ALEN)) {
			fprintf(stderr, "FAIL %s cache slot %u not zero\n", tag, i);
			failures++;
		}
	}
}

int main(void)
{
	static const u8 mac2[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	struct _adapter *adap = host_mbid_cam_adapter();

	host_mbid_cam_reset_fixture();
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0x55;
	host_mbid_cam_set_entry_num(3);
	host_mbid_cam_prime_entry(1, 5, mac2, 0);
	rtw_mbid_cam_init(host_mbid_cam_dvobj());
	expect_cam_cleared("init", host_mbid_cam_dvobj());

	host_mbid_cam_reset_fixture();
	host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0x05;
	host_mbid_cam_prime_entry(2, 3, mac2, 0);
	eq_u8("mac", rtw_mbid_cam_search_by_macaddr(adap, (u8 *)mac2),
	      rtw_mbid_cam_search_by_macaddr_c_oracle(adap, (u8 *)mac2));
	adap->iface_id = 3;
	eq_u8("iface", rtw_mbid_cam_search_by_ifaceid(adap, 3),
	      rtw_mbid_cam_search_by_ifaceid_c_oracle(adap, 3));
	eq_u8("max", rtw_get_max_mbid_cam_id(adap), rtw_get_max_mbid_cam_id_c_oracle(adap));
	rtw_mbid_cam_reset(adap);
	expect_cam_cleared("reset", host_mbid_cam_dvobj());

	host_mbid_cam_reset_fixture();
	{
		static const u8 mac_sta[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
		static const u8 mac_ap[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
		struct host_sel_capture cap_rust;
		struct host_sel_capture cap_c;
		char got[4096];
		char expect[4096];

		host_mbid_cam_set_entry_num(3);
		host_mbid_cam_dvobj()->mbid_cam_ctl.bitmap = 0x25;
		host_mbid_cam_prime_entry(0, 0, mac_sta, 0);
		host_mbid_cam_prime_entry(2, 1, mac_ap, 0);
		host_mbid_cam_prime_entry(5, 2, mac_ap, 0);
		host_mbid_cam_set_iface_role(0, WIFI_STATION_STATE);
		host_mbid_cam_set_iface_role(1, WIFI_AP_STATE);
		host_mbid_cam_set_iface_role(2, WIFI_MESH_STATE);

		host_sel_reset(&cap_rust);
		rtw_mbid_cam_cache_dump(&cap_rust, "host_l2", adap);
		host_sel_reset(&cap_c);
		rtw_mbid_cam_cache_dump_c_oracle(&cap_c, "host_l2", adap);
		if (host_sel_string(&cap_rust, got, sizeof(got)) < 0 ||
		    host_sel_string(&cap_c, expect, sizeof(expect)) < 0 ||
		    strcmp(got, expect)) {
			fprintf(stderr, "FAIL mbid_cam_cache_dump\n--- got ---\n%s--- expect ---\n%s",
				got, expect);
			failures++;
		}
	}

	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam L2");
	return 0;
}