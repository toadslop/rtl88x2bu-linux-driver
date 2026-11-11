// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-236 proc p2p_wowlan_info dump. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_wow_p2p_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_HEX 32
#define MAX_EXPECT 4096

struct vector {
	char name[MAX_NAME];
	u8 is_trigger;
	u8 wowlan_recv_frame_type;
	char peer_mac_hex[MAX_HEX];
	u16 wowlan_peer_wpsconfig;
	u8 wowlan_peer_is_persistent;
	u8 wowlan_peer_invitation_type;
	char expect[MAX_EXPECT];
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

static int parse_u16_field(const char *obj, size_t len, const char *key, u16 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u16)tmp;
	return 0;
}

static int hex_nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int fill_mac_from_hex(u8 *mac, const char *hex)
{
	size_t hlen = strlen(hex);
	size_t i;
	int hi, lo;

	if (hlen != ETH_ALEN * 2)
		return -1;
	for (i = 0; i < ETH_ALEN; i++) {
		hi = hex_nibble(hex[i * 2]);
		lo = hex_nibble(hex[i * 2 + 1]);
		if (hi < 0 || lo < 0)
			return -1;
		mac[i] = (u8)((hi << 4) | lo);
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "is_trigger", &v->is_trigger))
		return -1;
	if (host_json_find_key_in(obj, len, "wowlan_recv_frame_type")) {
		if (parse_u8_field(obj, len, "wowlan_recv_frame_type",
				   &v->wowlan_recv_frame_type))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "peer_mac")) {
		if (host_json_parse_string_in(obj, len, "peer_mac",
					    v->peer_mac_hex, sizeof(v->peer_mac_hex)))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "wowlan_peer_wpsconfig")) {
		if (parse_u16_field(obj, len, "wowlan_peer_wpsconfig",
				    &v->wowlan_peer_wpsconfig))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "wowlan_peer_is_persistent")) {
		if (parse_u8_field(obj, len, "wowlan_peer_is_persistent",
				   &v->wowlan_peer_is_persistent))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "wowlan_peer_invitation_type")) {
		if (parse_u8_field(obj, len, "wowlan_peer_invitation_type",
				   &v->wowlan_peer_invitation_type))
			return -1;
	}
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
	struct p2p_wowlan_info *peer;
	char got[MAX_EXPECT];
	size_t nread;
	FILE *capture;

	memset(&adapter, 0, sizeof(adapter));
	peer = &adapter.wdinfo.p2p_wow_info;
	peer->is_trigger = v->is_trigger;
	if (v->is_trigger) {
		peer->wowlan_recv_frame_type = v->wowlan_recv_frame_type;
		if (strlen(v->peer_mac_hex)) {
			if (fill_mac_from_hex(peer->wowlan_peer_addr,
					      v->peer_mac_hex))
				return -1;
		}
		peer->wowlan_peer_wpsconfig = v->wowlan_peer_wpsconfig;
		peer->wowlan_peer_is_persistent = v->wowlan_peer_is_persistent;
		peer->wowlan_peer_invitation_type =
			v->wowlan_peer_invitation_type;
	}

	dev.priv = &adapter;
	seq.private = &dev;
	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (proc_get_p2p_wowlan_info(&seq, NULL) != 0) {
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

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1]
				    : "debug_proc_wow_p2p_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("ok: %zu proc wow_p2p vectors\n", count);
	return 0;
}
