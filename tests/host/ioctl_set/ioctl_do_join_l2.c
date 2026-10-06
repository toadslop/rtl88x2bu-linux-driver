// SPDX-License-Identifier: GPL-2.0
/* W3-124 follow-up (#979) PR6: L2 C oracle for rtw_do_join. */
#include <stdio.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define WIFI_ADHOC_STATE 0x00000020
#define WIFI_ADHOC_MASTER_STATE 0x00000040
#define WIFI_UNDER_LINKING 0x00000080
#define SS_DENY_BUSY_TRAFFIC 12
#define SS_ALLOW 13
#define MAX_JOIN_TIMEOUT 6500

typedef struct { u32 SsidLength; u8 Ssid[32]; } NDIS_802_11_SSID;
typedef struct { u8 bBusyTraffic; } LinkDetectInfo;
struct sitesurvey_parm {
	NDIS_802_11_SSID ssid[4];
	u8 ssid_num, ch_num;
	struct { u8 hw_value, flags; } ch[4];
};
struct mlme_priv {
	u32 fw_state;
	u8 to_join;
	u16 assoc_ch;
	NDIS_802_11_SSID assoc_ssid;
	s8 join_res;
	int scanned_lock_depth;
	u8 queue_empty;
	LinkDetectInfo LinkDetectInfo;
	u32 assoc_timer_ms;
};
struct adapter {
	struct mlme_priv mlmepriv;
	s8 to_roam;
	u8 ssc_chk, sitesurvey_ret;
	s8 select_ret;
	u8 create_ibss_ret;
	u32 ss_calls, select_calls, create_ibss_calls;
};

#ifdef HOST_IOCTL_DO_JOIN_RUST
extern u8 rtw_do_join_rust(struct adapter *p);
#define DO_JOIN rtw_do_join_rust
#else
#define LOCK(m) ((m)->scanned_lock_depth++)
#define UNLOCK(m) ((m)->scanned_lock_depth--)
#define SET_FW(m, s) ((m)->fw_state |= (s))
#define CLR_FW(m, s) ((m)->fw_state &= ~(s))

static u8 DO_JOIN(struct adapter *a)
{
	struct mlme_priv *m = &a->mlmepriv;
	struct sitesurvey_parm parm;
	u8 ret = _SUCCESS;

	LOCK(m);
	m->join_res = -2;
	SET_FW(m, WIFI_UNDER_LINKING);
	m->to_join = _TRUE;
	memset(&parm, 0, sizeof(parm));
	memcpy(&parm.ssid[0], &m->assoc_ssid, sizeof(NDIS_802_11_SSID));
	parm.ssid_num = 1;
	if (m->assoc_ch) {
		parm.ch_num = 1;
		parm.ch[0].hw_value = (u8)m->assoc_ch;
	}

	if (m->queue_empty) {
		UNLOCK(m);
		CLR_FW(m, WIFI_UNDER_LINKING);
		if (!m->LinkDetectInfo.bBusyTraffic || a->to_roam > 0) {
			if (a->ssc_chk == SS_ALLOW || a->ssc_chk == SS_DENY_BUSY_TRAFFIC) {
				a->ss_calls++;
				ret = a->sitesurvey_ret;
				if (ret != _SUCCESS)
					m->to_join = 0;
			} else {
				m->to_join = 0;
				ret = _FAIL;
			}
		} else {
			m->to_join = 0;
			ret = _FAIL;
		}
		return ret;
	}

	UNLOCK(m);
	a->select_calls++;
	if (a->select_ret == _SUCCESS) {
		m->to_join = 0;
		m->assoc_timer_ms = MAX_JOIN_TIMEOUT;
		return _SUCCESS;
	}

	if (m->fw_state & WIFI_ADHOC_STATE) {
		/* CONFIG_AP_MODE=y: IBSS master path (core/rtw_ioctl_set.c), not resurvey. */
		m->fw_state = WIFI_ADHOC_MASTER_STATE;
		a->create_ibss_calls++;
		if (a->create_ibss_ret != _SUCCESS) {
			ret = _FAIL;
			return ret;
		}
		m->to_join = 0;
		return _SUCCESS;
	}

	CLR_FW(m, WIFI_UNDER_LINKING);
	if (!m->LinkDetectInfo.bBusyTraffic || a->to_roam > 0) {
		if (a->ssc_chk == SS_ALLOW || a->ssc_chk == SS_DENY_BUSY_TRAFFIC) {
			a->ss_calls++;
			ret = a->sitesurvey_ret;
			if (ret != _SUCCESS)
				m->to_join = 0;
		} else {
			ret = _FAIL;
			m->to_join = 0;
		}
	} else {
		ret = _FAIL;
		m->to_join = 0;
	}
	return ret;
}
#endif

struct vector {
	char name[64];
	int queue_empty, busy_traffic, to_roam, ssc_chk, sitesurvey_ret, select_ret;
	int adhoc_state, create_ibss_ret, assoc_ch, ssid_len, expect_ret, expect_to_join;
	int expect_ss_calls, expect_select_calls, expect_create_ibss_calls;
	int expect_join_res, expect_fw_linking, expect_fw_adhoc_master, expect_timer_ms;
	int expect_lock_depth;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *k[] = { "queue_empty", "busy_traffic", "to_roam", "ssc_chk",
			    "sitesurvey_ret", "select_ret", "assoc_ch", "ssid_len",
			    "expect_ret", "expect_to_join", "expect_ss_calls",
			    "expect_select_calls", "expect_join_res", "expect_fw_linking" };
	int *p[] = { &v->queue_empty, &v->busy_traffic, &v->to_roam, &v->ssc_chk,
		     &v->sitesurvey_ret, &v->select_ret, &v->assoc_ch, &v->ssid_len,
		     &v->expect_ret, &v->expect_to_join, &v->expect_ss_calls,
		     &v->expect_select_calls, &v->expect_join_res, &v->expect_fw_linking };
	size_t i;

	memset(v, 0, sizeof(*v));
	v->expect_timer_ms = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (i = 0; i < sizeof(k) / sizeof(k[0]); i++)
		if (host_json_parse_int_in(obj, len, k[i], p[i]))
			return -1;
	host_json_parse_int_in(obj, len, "adhoc_state", &v->adhoc_state);
	host_json_parse_int_in(obj, len, "create_ibss_ret", &v->create_ibss_ret);
	host_json_parse_int_in(obj, len, "expect_create_ibss_calls", &v->expect_create_ibss_calls);
	host_json_parse_int_in(obj, len, "expect_fw_adhoc_master", &v->expect_fw_adhoc_master);
	host_json_parse_int_in(obj, len, "expect_lock_depth", &v->expect_lock_depth);
	host_json_parse_int_in(obj, len, "expect_timer_ms", &v->expect_timer_ms);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct adapter a;

	memset(&a, 0, sizeof(a));
	a.mlmepriv.queue_empty = (u8)v->queue_empty;
	a.mlmepriv.LinkDetectInfo.bBusyTraffic = (u8)v->busy_traffic;
	a.mlmepriv.assoc_ch = (u16)v->assoc_ch;
	a.mlmepriv.assoc_ssid.SsidLength = (u32)v->ssid_len;
	a.to_roam = (s8)v->to_roam;
	a.ssc_chk = (u8)v->ssc_chk;
	a.sitesurvey_ret = (u8)v->sitesurvey_ret;
	a.select_ret = (s8)v->select_ret;
	a.create_ibss_ret = (u8)(v->create_ibss_ret ? v->create_ibss_ret : _SUCCESS);
	if (v->adhoc_state)
		a.mlmepriv.fw_state |= WIFI_ADHOC_STATE;

	u8 got = DO_JOIN(&a);

	if (got != (u8)v->expect_ret || a.mlmepriv.to_join != (u8)v->expect_to_join ||
	    (int)a.ss_calls != v->expect_ss_calls ||
	    (int)a.select_calls != v->expect_select_calls ||
	    (int)a.create_ibss_calls != v->expect_create_ibss_calls ||
	    a.mlmepriv.join_res != (s8)v->expect_join_res ||
	    (((a.mlmepriv.fw_state & WIFI_UNDER_LINKING) != 0) !=
	     (v->expect_fw_linking != 0)) ||
	    (((a.mlmepriv.fw_state & WIFI_ADHOC_MASTER_STATE) != 0) !=
	     (v->expect_fw_adhoc_master != 0)) ||
	    a.mlmepriv.scanned_lock_depth != v->expect_lock_depth ||
	    (v->expect_timer_ms >= 0 &&
	     (int)a.mlmepriv.assoc_timer_ms != v->expect_timer_ms)) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_do_join_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 16, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
