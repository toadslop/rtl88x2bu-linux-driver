// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-212 proc TDLS enable + function info. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_tdls_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 2048

struct vector {
	char name[MAX_NAME];
	u8 en_tdls;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	struct tdls_info tdlsinfo;
	int has_tdlsinfo;
	u32 fw_state;
	char ssid[33];
	u8 mac[ETH_ALEN];
	u8 wireless_mode;
	u32 privacy;
	u8 channel;
	u8 ch_offset;
	u8 bwmode;
	int has_network;
	struct sta_info tdls_sta;
	int has_tdls_sta;
};

static void unescape_expect(char *s)
{
	char *r = s;
	char *w = s;

	while (*r) {
		if (r[0] == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r += 2;
		} else {
			*w++ = *r++;
		}
	}
	*w = '\0';
}

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u8)tmp;
	return 0;
}

static const char *json_object_end(const char *start)
{
	int depth = 0;
	const char *p = start;

	if (*p != '{')
		return NULL;
	for (; *p; p++) {
		if (*p == '{')
			depth++;
		else if (*p == '}') {
			depth--;
			if (depth == 0)
				return p + 1;
		}
	}
	return NULL;
}

static int parse_tdlsinfo_object(const char *obj, size_t len, struct tdls_info *info)
{
	const char *start = host_json_find_key_in(obj, len, "tdlsinfo");
	const char *end;
	size_t sub_len;

	if (!start || *start != '{')
		return -1;
	end = json_object_end(start);
	if (!end)
		return -1;
	sub_len = (size_t)(end - start);

	memset(info, 0, sizeof(*info));
	if (parse_u8_field(start, sub_len, "driver_setup", &info->driver_setup))
		return -1;
	if (parse_u8_field(start, sub_len, "ap_prohibited", &info->ap_prohibited))
		return -1;
	if (parse_u8_field(start, sub_len, "ch_switch_prohibited",
			    &info->ch_switch_prohibited))
		return -1;
	if (parse_u8_field(start, sub_len, "link_established", &info->link_established))
		return -1;
	if (parse_u8_field(start, sub_len, "sta_cnt", &info->sta_cnt))
		return -1;
	if (parse_u8_field(start, sub_len, "sta_maximum", &info->sta_maximum))
		return -1;
	if (parse_u8_field(start, sub_len, "dev_discovered", &info->dev_discovered))
		return -1;
	return 0;
}

static int parse_mac(const char *s, u8 *mac)
{
	unsigned int a, b, c, d, e, f;

	if (sscanf(s, "%x:%x:%x:%x:%x:%x", &a, &b, &c, &d, &e, &f) != 6)
		return -1;
	mac[0] = (u8)a;
	mac[1] = (u8)b;
	mac[2] = (u8)c;
	mac[3] = (u8)d;
	mac[4] = (u8)e;
	mac[5] = (u8)f;
	return 0;
}

static int parse_sta_object(const char *obj, size_t len, struct sta_info *sta)
{
	const char *start = host_json_find_key_in(obj, len, "sta");
	const char *end;
	size_t sub_len;
	char mac_str[32];
	int tmp;

	if (!start || *start != '{')
		return -1;
	end = json_object_end(start);
	if (!end)
		return -1;
	sub_len = (size_t)(end - start);
	memset(sta, 0, sizeof(*sta));
	if (host_json_parse_string_in(start, sub_len, "mac", mac_str, sizeof(mac_str)))
		return -1;
	if (parse_mac(mac_str, sta->cmn.mac_addr))
		return -1;
	if (host_json_parse_int_in(start, sub_len, "tdls_sta_state", &tmp))
		return -1;
	sta->tdls_sta_state = (u32)tmp;
	if (parse_u8_field(start, sub_len, "wireless_mode", &sta->wireless_mode))
		return -1;
	if (parse_u8_field(start, sub_len, "bw_mode", &sta->cmn.bw_mode))
		return -1;
	if (host_json_parse_int_in(start, sub_len, "privacy", &tmp))
		return -1;
	sta->dot118021XPrivacy = (u32)tmp;
	if (host_json_parse_int_in(start, sub_len, "tpk_count", &tmp))
		return -1;
	sta->TPK_count = (u32)tmp;
	if (host_json_parse_int_in(start, sub_len, "tpk_lifetime", &tmp))
		return -1;
	sta->TDLS_PeerKey_Lifetime = (u32)tmp;
	if (host_json_parse_int_in(start, sub_len, "tx_pkts", &tmp))
		return -1;
	sta->sta_stats.tx_pkts = (u64)tmp;
	if (host_json_parse_int_in(start, sub_len, "rx_pkts", &tmp))
		return -1;
	sta->sta_stats.rx_data_pkts = (u64)tmp;
	return 0;
}

static void adapter_init_stapriv(struct _adapter *adapter)
{
	int i;

	for (i = 0; i < NUM_STA; i++)
		_rtw_init_listhead(&adapter->stapriv.sta_hash[i]);
}

static void adapter_link_tdls_sta(struct _adapter *adapter, struct sta_info *sta)
{
	_rtw_init_listhead(&sta->hash_list);
	rtw_list_insert_tail(&sta->hash_list, &adapter->stapriv.sta_hash[0]);
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_find_key_in(obj, len, "en_tdls") &&
	    parse_u8_field(obj, len, "en_tdls", &v->en_tdls))
		return -1;
	if (host_json_find_key_in(obj, len, "tdlsinfo")) {
		if (parse_tdlsinfo_object(obj, len, &v->tdlsinfo))
			return -1;
		v->has_tdlsinfo = 1;
	}
	if (host_json_find_key_in(obj, len, "fw_state")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "fw_state", &tmp))
			return -1;
		v->fw_state = (u32)tmp;
		v->has_network = 1;
	}
	if (host_json_find_key_in(obj, len, "ssid")) {
		if (host_json_parse_string_in(obj, len, "ssid", v->ssid, sizeof(v->ssid)))
			return -1;
		v->has_network = 1;
	}
	if (host_json_find_key_in(obj, len, "mac")) {
		char mac_str[32];

		if (host_json_parse_string_in(obj, len, "mac", mac_str, sizeof(mac_str)))
			return -1;
		if (parse_mac(mac_str, v->mac))
			return -1;
		v->has_network = 1;
	}
	if (host_json_find_key_in(obj, len, "wireless_mode") &&
	    parse_u8_field(obj, len, "wireless_mode", &v->wireless_mode))
		return -1;
	if (host_json_find_key_in(obj, len, "privacy")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "privacy", &tmp))
			return -1;
		v->privacy = (u32)tmp;
		v->has_network = 1;
	}
	if (host_json_find_key_in(obj, len, "channel") &&
	    parse_u8_field(obj, len, "channel", &v->channel))
		return -1;
	if (host_json_find_key_in(obj, len, "ch_offset") &&
	    parse_u8_field(obj, len, "ch_offset", &v->ch_offset))
		return -1;
	if (host_json_find_key_in(obj, len, "bwmode") &&
	    parse_u8_field(obj, len, "bwmode", &v->bwmode))
		return -1;
	if (host_json_find_key_in(obj, len, "sta")) {
		if (parse_sta_object(obj, len, &v->tdls_sta))
			return -1;
		v->has_tdls_sta = 1;
	}
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "get", v->get_field,
				      sizeof(v->get_field)))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	struct sta_info link_sta;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	adapter_init_stapriv(&adapter);
	adapter.registrypriv.en_tdls = v->en_tdls;
	if (v->has_tdlsinfo)
		adapter.tdlsinfo = v->tdlsinfo;
	if (v->has_network) {
		adapter.mlmepriv.fw_state = v->fw_state;
		if (v->ssid[0])
			strncpy((char *)adapter.mlmepriv.cur_network.network.Ssid.Ssid,
				v->ssid, sizeof(v->ssid) - 1);
		if (v->mac[0])
			memcpy(adapter.mlmepriv.cur_network.network.MacAddress, v->mac,
			       ETH_ALEN);
		adapter.mlmeextpriv.cur_wireless_mode = v->wireless_mode;
		adapter.securitypriv.dot11PrivacyAlgrthm = v->privacy;
		adapter.mlmeextpriv.cur_channel = v->channel;
		adapter.mlmeextpriv.cur_ch_offset = v->ch_offset;
		adapter.mlmeextpriv.cur_bwmode = v->bwmode;
	}
	if (v->has_tdls_sta) {
		link_sta = v->tdls_sta;
		adapter_link_tdls_sta(&adapter, &link_sta);
	}
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "tdls_enable")) {
			if (proc_set_tdls_enable(NULL, v->set_input,
						 strlen(v->set_input), NULL,
						 &dev) < 0) {
				fclose(capture);
				return -1;
			}
		} else {
			fclose(capture);
			return -1;
		}
	}

	if (!strcmp(v->get_field, "tdls_enable"))
		proc_get_tdls_enable(&seq, NULL);
	else if (!strcmp(v->get_field, "tdlsinfo"))
		proc_tdls_display_tdls_function_info(&seq);
#ifndef RUST_RTW_DEBUG_PROC_TDLS_ORACLE
	else if (!strcmp(v->get_field, "tdls_network"))
		proc_tdls_display_network_info(&seq);
	else if (!strcmp(v->get_field, "tdls_sta"))
		proc_tdls_display_tdls_sta_info(&seq);
#endif
	else {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n", v->name,
			got, v->expect);
		return -1;
	}
	return 0;
}

#ifdef RUST_RTW_DEBUG_PROC_TDLS_ORACLE
static int vector_rust_oracle_ready(const struct vector *v)
{
	/*
	 * W3-213 network/STA display procs land in later stack layers; the Rust
	 * L2 harness only differential-tests handlers present in this PR slice.
	 */
	if (!strcmp(v->get_field, "tdls_network") ||
	    !strcmp(v->get_field, "tdls_sta"))
		return 0;
	return 1;
}
#endif

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_proc_tdls_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	size_t ran = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
#ifdef RUST_RTW_DEBUG_PROC_TDLS_ORACLE
		if (!vector_rust_oracle_ready(&vectors[i]))
			continue;
#endif
		if (run_vector(&vectors[i]))
			return 1;
		ran++;
	}

	printf("ok: %zu proc TDLS vectors\n", ran);
	return 0;
}
