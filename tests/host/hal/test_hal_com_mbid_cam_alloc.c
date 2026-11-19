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
unsigned host_mbid_cam_oracle_warn_count(void);

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

struct mbid_fixture_snap {
	u8 bitmap;
	u8 entry_num;
	u8 cache_mac[TOTAL_MBID_CAM_NUM][ETH_ALEN];
	u8 cache_iface[TOTAL_MBID_CAM_NUM];
};

static void snap_fixture(struct mbid_fixture_snap *s)
{
	struct dvobj_priv *d = host_mbid_cam_dvobj();
	u8 i;

	s->bitmap = d->mbid_cam_ctl.bitmap;
	s->entry_num = (u8)ATOMIC_READ(&d->mbid_cam_ctl.mbid_entry_num);
	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		memcpy(s->cache_mac[i], d->mbid_cam_cache[i].mac_addr, ETH_ALEN);
		s->cache_iface[i] = d->mbid_cam_cache[i].iface_id;
	}
}

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void cmp_snap(const char *tag, const struct mbid_fixture_snap *a,
		     const struct mbid_fixture_snap *b)
{
	u8 i;

	eq_u8(tag, a->bitmap, b->bitmap);
	eq_u8(tag, a->entry_num, b->entry_num);
	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		if (memcmp(a->cache_mac[i], b->cache_mac[i], ETH_ALEN)) {
			fprintf(stderr, "FAIL %s mac slot %u\n", tag, i);
			failures++;
		}
		eq_u8(tag, a->cache_iface[i], b->cache_iface[i]);
	}
}

static void run_pair(const char *tag, u8 subj, u8 orc,
		     const struct mbid_fixture_snap *subj_snap,
		     const struct mbid_fixture_snap *orc_snap)
{
	eq_u8(tag, subj, orc);
	cmp_snap(tag, subj_snap, orc_snap);
}

int main(void)
{
	static const u8 mac_a[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	static const u8 mac_b[ETH_ALEN] = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};
	struct _adapter *adap = host_mbid_cam_adapter();
	struct mbid_fixture_snap subj_snap, orc_snap;
	u8 subj, orc;

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	snap_fixture(&orc_snap);
	run_pair("alloc", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	snap_fixture(&orc_snap);
	run_pair("dup", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(1, 2, mac_a, 1);
	adap->iface_id = 2;
	subj = rtw_mbid_cam_info_change(adap, (u8 *)mac_b);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(1, 2, mac_a, 1);
	adap->iface_id = 2;
	orc = rtw_mbid_cam_info_change_c_oracle(adap, (u8 *)mac_b);
	snap_fixture(&orc_snap);
	run_pair("info", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(3, 0, mac_a, 1);
	subj = rtw_mbid_cam_assign(adap, (u8 *)mac_b, 3);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_prime_entry(3, 0, mac_a, 1);
	orc = rtw_mbid_cam_assign_c_oracle(adap, (u8 *)mac_b, 3);
	snap_fixture(&orc_snap);
	run_pair("assign_busy", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_cam_assign(adap, (u8 *)mac_b, 2);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_cam_assign_c_oracle(adap, (u8 *)mac_b, 2);
	snap_fixture(&orc_snap);
	run_pair("assign_ok", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean(adap, subj);
	subj = rtw_get_mbid_cam_entry_num(adap);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean_c_oracle(adap, orc);
	orc = rtw_get_mbid_cam_entry_num_c_oracle(adap);
	snap_fixture(&orc_snap);
	run_pair("clean_ent", subj, orc, &subj_snap, &orc_snap);

	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean(adap, 0);
	subj = rtw_mbid_cam_search_by_macaddr(adap, (u8 *)mac_a);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_a);
	rtw_mbid_camid_clean_c_oracle(adap, 0);
	orc = rtw_mbid_cam_search_by_macaddr_c_oracle(adap, (u8 *)mac_a);
	snap_fixture(&orc_snap);
	run_pair("clean_mac", subj, orc, &subj_snap, &orc_snap);

	/* Counter ahead of bitmap: driver warns but still allocates free slot. */
	host_mbid_cam_reset_fixture();
	host_mbid_cam_set_entry_num(8);
	{
		u8 i;

		for (i = 0; i < 7; i++)
			host_mbid_cam_prime_entry(i, 0, mac_a, 1);
	}
	subj = rtw_mbid_camid_alloc(adap, (u8 *)mac_b);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_set_entry_num(8);
	{
		u8 i;

		for (i = 0; i < 7; i++)
			host_mbid_cam_prime_entry(i, 0, mac_a, 1);
	}
	orc = rtw_mbid_camid_alloc_c_oracle(adap, (u8 *)mac_b);
	snap_fixture(&orc_snap);
	run_pair("alloc_cnt_ahead", subj, orc, &subj_snap, &orc_snap);

	/* Invalid cam id: warn path still updates counter on assign. */
	host_mbid_cam_reset_fixture();
	subj = rtw_mbid_cam_assign(adap, (u8 *)mac_b, 10);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	orc = rtw_mbid_cam_assign_c_oracle(adap, (u8 *)mac_b, 10);
	snap_fixture(&orc_snap);
	run_pair("assign_invalid", subj, orc, &subj_snap, &orc_snap);

	/* Invalid clean id: warn path still decrements entry count. */
	host_mbid_cam_reset_fixture();
	host_mbid_cam_set_entry_num(1);
	rtw_mbid_camid_clean(adap, 10);
	subj = rtw_get_mbid_cam_entry_num(adap);
	snap_fixture(&subj_snap);
	host_mbid_cam_reset_fixture();
	host_mbid_cam_set_entry_num(1);
	rtw_mbid_camid_clean_c_oracle(adap, 10);
	orc = rtw_get_mbid_cam_entry_num_c_oracle(adap);
	snap_fixture(&orc_snap);
	run_pair("clean_invalid", subj, orc, &subj_snap, &orc_snap);

	if (failures)
		return 1;
	puts("OK hal_com_mbid_cam alloc L2");
	return 0;
}