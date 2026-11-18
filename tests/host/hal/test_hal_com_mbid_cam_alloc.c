// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-120 MBSSID CAM alloc/assign/clean. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdio.h>
#include <string.h>

void host_mbid_cam_reset_fixture(void);
struct _adapter *host_mbid_cam_adapter(void);
struct dvobj_priv *host_mbid_cam_dvobj(void);
void host_mbid_cam_prime_entry(u8 slot, u8 iface_id, const u8 *mac, u8 set_bitmap);
void host_mbid_cam_set_entry_num(u8 n);

#ifdef RUST_HAL_COM_MBID_CAM_ORACLE
u8 rtw_mbid_camid_alloc(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_info_change(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_assign(struct _adapter *adapter, u8 *mac_addr, u8 camid);
void rtw_mbid_camid_clean(struct _adapter *adapter, u8 mbss_camid);
u8 rtw_get_mbid_cam_entry_num(struct _adapter *adapter);
u8 rtw_mbid_cam_search_by_macaddr(struct _adapter *adapter, u8 *mac_addr);
#else
#define rtw_mbid_camid_alloc rtw_mbid_camid_alloc_c_oracle
#define rtw_mbid_cam_info_change rtw_mbid_cam_info_change_c_oracle
#define rtw_mbid_cam_assign rtw_mbid_cam_assign_c_oracle
#define rtw_mbid_camid_clean rtw_mbid_camid_clean_c_oracle
#define rtw_get_mbid_cam_entry_num rtw_get_mbid_cam_entry_num_c_oracle
#define rtw_mbid_cam_search_by_macaddr rtw_mbid_cam_search_by_macaddr_c_oracle
#endif

u8 rtw_mbid_camid_alloc_c_oracle(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_info_change_c_oracle(struct _adapter *adapter, u8 *mac_addr);
u8 rtw_mbid_cam_assign_c_oracle(struct _adapter *adapter, u8 *mac_addr, u8 camid);
void rtw_mbid_camid_clean_c_oracle(struct _adapter *adapter, u8 mbss_camid);
u8 rtw_get_mbid_cam_entry_num_c_oracle(struct _adapter *adapter);
u8 rtw_mbid_cam_search_by_macaddr_c_oracle(struct _adapter *adapter, u8 *mac_addr);

static int failures;

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void cmp_oracle(const char *tag, u8 subj, u8 orc)
{
	eq_u8(tag, subj, orc);
}

int main(void)
{
	static const u8 mac_a[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	static const u8 mac_b[ETH_ALEN] = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};
	struct _adapter *adap = host_mbid_cam_adapter();
	u8 subj, orc;

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	cmp_oracle("alloc", subj, orc);

	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	cmp_oracle("dup", subj, orc);

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(1, 2, mac_a, 1);
	adap->iface_id = 2;
	subj = rtw_mbid_cam_info_change(adap, (u8 *)mac_b);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(1, 2, mac_a, 1);
	adap->iface_id = 2;
	orc = rtw_mbid_cam_info_change_c_oracle(adap, (u8 *)mac_b);
	cmp_oracle("info", subj, orc);

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(3, 0, mac_a, 1);
	subj = rtw_mbid_cam_assign(adap, (u8 *)mac_b, 3);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(3, 0, mac_a, 1);
	orc = rtw_mbid_cam_assign_c_oracle(adap, (u8 *)mac_b, 3);
	cmp_oracle("assign_busy", subj, orc);

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_cam_assign(adap, (u8 *)mac_b, 2);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_cam_assign_c_oracle(adap, (u8 *)mac_b, 2);
	cmp_oracle("assign_ok", subj, orc);

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean(adap, subj);
	subj = rtw_get_mbid_cam_entry_num(adap);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean_c_oracle(adap, orc);
	orc = rtw_get_mbid_cam_entry_num_c_oracle(adap);
	cmp_oracle("clean_ent", subj, orc);

	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean(adap, 0);
	subj = rtw_mbid_cam_search_by_macaddr(adap, (u8 *)mac_a);
	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean_c_oracle(adap, 0);
	orc = rtw_mbid_cam_search_by_macaddr_c_oracle(adap, (u8 *)mac_a);
	cmp_oracle("clean_mac", subj, orc);

	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam alloc L2");
	return 0;
}