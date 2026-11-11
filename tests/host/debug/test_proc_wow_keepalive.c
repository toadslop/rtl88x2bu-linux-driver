// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-235 proc wow_keep_alive_info dump. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_wow_keepalive_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_HEX 512
#define MAX_EXPECT 4096

struct vector {
	char name[MAX_NAME];
	u8 wowlan_keep_alive_mode;
	u8 keep_alive_pattern_loc;
	u32 keep_alive_pattern_len;
	char pattern_hex[MAX_HEX];
	u16 wowlan_keep_alive_period;
	u8 wowlan_keep_alive_retry_counter;
	u8 wowlan_keep_alive_retry_interval;
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

static int parse_u32_field(const char *obj, size_t len, const char *key, u32 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u32)tmp;
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

static int fill_pattern_from_hex(struct pwrctrl_priv *pwr, const char *hex,
				 u32 len)
{
	size_t hlen = strlen(hex);
	size_t i;
	int hi, lo;

	if (len > WLAN_MAX_KEEP_ALIVE_IE_LEN)
		return -1;
	if (len == 0)
		return 0;
	if (hlen != (size_t)len * 2)
		return -1;

	for (i = 0; i < len; i++) {
		hi = hex_nibble(hex[i * 2]);
		lo = hex_nibble(hex[i * 2 + 1]);
		if (hi < 0 || lo < 0)
			return -1;
		pwr->keep_alive_pattern[i] = (u8)((hi << 4) | lo);
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_keep_alive_mode",
			   &v->wowlan_keep_alive_mode))
		return -1;
	if (parse_u8_field(obj, len, "keep_alive_pattern_loc",
			   &v->keep_alive_pattern_loc))
		return -1;
	if (parse_u32_field(obj, len, "keep_alive_pattern_len",
			    &v->keep_alive_pattern_len))
		return -1;
	if (host_json_parse_string_in(obj, len, "pattern_hex", v->pattern_hex,
				       sizeof(v->pattern_hex)))
		return -1;
	if (parse_u16_field(obj, len, "wowlan_keep_alive_period",
			    &v->wowlan_keep_alive_period))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_keep_alive_retry_counter",
			   &v->wowlan_keep_alive_retry_counter))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_keep_alive_retry_interval",
			   &v->wowlan_keep_alive_retry_interval))
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
	struct pwrctrl_priv *pwr;
	char got[MAX_EXPECT];
	size_t nread;
	FILE *capture;

	memset(&adapter, 0, sizeof(adapter));
	pwr = &adapter.pwrctrlpriv;
	pwr->wowlan_keep_alive_mode = v->wowlan_keep_alive_mode;
	pwr->keep_alive_pattern_loc = v->keep_alive_pattern_loc;
	pwr->keep_alive_pattern_len = v->keep_alive_pattern_len;
	pwr->wowlan_keep_alive_period = v->wowlan_keep_alive_period;
	pwr->wowlan_keep_alive_retry_counter = v->wowlan_keep_alive_retry_counter;
	pwr->wowlan_keep_alive_retry_interval = v->wowlan_keep_alive_retry_interval;

	if (fill_pattern_from_hex(pwr, v->pattern_hex, v->keep_alive_pattern_len))
		return -1;

	dev.priv = &adapter;
	seq.private = &dev;
	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (proc_dump_wow_keep_alive_info(&seq, NULL) != 0) {
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
				    : "debug_proc_wow_keepalive_vectors.json";
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

	printf("ok: %zu proc wow_keep_alive vectors\n", count);
	return 0;
}
