// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-217 WoWLAN registry / GPIO proc get/set. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_wow_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64
#define MAX_INPUT 128
#define MAX_FIELD 48
#define MAX_EXPECT 4096

struct vector {
	char name[MAX_NAME];
	u8 wowlan_enable;
	u8 wakeup_event;
	u8 wowlan_last_wake_reason;
	u8 default_patterns_en;
	u8 wowlan_pattern_idx;
	u8 wowlan_gpio_index;
	u8 wowlan_gpio_output_state;
	u8 is_high_active;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	long expect_ret;
	int check_ret;
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

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp_ret;

	memset(v, 0, sizeof(*v));
	v->check_ret = 0;
	v->expect_ret = 0;

	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_enable", &v->wowlan_enable))
		return -1;
	if (parse_u8_field(obj, len, "wakeup_event", &v->wakeup_event))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_last_wake_reason",
			   &v->wowlan_last_wake_reason))
		return -1;
	if (parse_u8_field(obj, len, "default_patterns_en",
			   &v->default_patterns_en))
		return -1;
	if (host_json_find_key_in(obj, len, "wowlan_pattern_idx")) {
		if (parse_u8_field(obj, len, "wowlan_pattern_idx",
				    &v->wowlan_pattern_idx))
			return -1;
	} else {
		v->wowlan_pattern_idx = 0;
	}
	if (parse_u8_field(obj, len, "wowlan_gpio_index", &v->wowlan_gpio_index))
		return -1;
	if (parse_u8_field(obj, len, "wowlan_gpio_output_state",
			   &v->wowlan_gpio_output_state))
		return -1;
	if (parse_u8_field(obj, len, "is_high_active", &v->is_high_active))
		return -1;
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
	if (host_json_find_key_in(obj, len, "check_ret")) {
		if (host_json_parse_int_in(obj, len, "check_ret", &tmp_ret))
			return -1;
		v->check_ret = tmp_ret;
	}
	if (!host_json_find_key_in(obj, len, "expect_ret"))
		return 0;
	if (host_json_parse_int_in(obj, len, "expect_ret", &tmp_ret))
		return -1;
	v->expect_ret = (long)tmp_ret;
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;
	long ret;

	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.wowlan_enable = v->wowlan_enable;
	adapter.registrypriv.wakeup_event = v->wakeup_event;
	adapter.pwrctrlpriv.wowlan_last_wake_reason =
		v->wowlan_last_wake_reason;
	adapter.pwrctrlpriv.default_patterns_en = v->default_patterns_en;
	adapter.pwrctrlpriv.wowlan_pattern_idx = v->wowlan_pattern_idx;
	adapter.pwrctrlpriv.wowlan_gpio_index = v->wowlan_gpio_index;
	adapter.pwrctrlpriv.wowlan_gpio_output_state =
		v->wowlan_gpio_output_state;
	adapter.pwrctrlpriv.is_high_active = v->is_high_active;
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "wow_enable")) {
			ret = proc_set_wow_enable(NULL, v->set_input,
						  strlen(v->set_input), NULL,
						  &dev);
		} else if (!strcmp(v->set_field, "wakeup_event")) {
			ret = proc_set_wakeup_event(
				NULL, v->set_input, strlen(v->set_input),
				NULL, &dev);
		} else if (!strcmp(v->set_field, "wowlan_gpio_info")) {
			ret = proc_set_wowlan_gpio_info(
				NULL, v->set_input, strlen(v->set_input),
				NULL, &dev);
		} else if (!strcmp(v->set_field, "pattern_info")) {
			ret = proc_set_pattern_info(
				NULL, v->set_input, strlen(v->set_input),
				NULL, &dev);
		} else {
			fclose(capture);
			return -1;
		}

		if (v->check_ret) {
			if (ret != v->expect_ret) {
				fprintf(stderr,
					"FAIL %s ret: got %ld expect %ld\n",
					v->name, ret, v->expect_ret);
				fclose(capture);
				return -1;
			}
			fclose(capture);
			return 0;
		}
		if (ret < 0) {
			fprintf(stderr, "FAIL %s: set returned %ld\n", v->name,
				ret);
			fclose(capture);
			return -1;
		}
	}

	if (!strcmp(v->get_field, "wow_enable"))
		proc_get_wow_enable(&seq, NULL);
	else if (!strcmp(v->get_field, "wakeup_event"))
		proc_get_wakeup_event(&seq, NULL);
	else if (!strcmp(v->get_field, "wakeup_reason"))
		proc_get_wakeup_reason(&seq, NULL);
	else if (!strcmp(v->get_field, "wowlan_gpio_info"))
		proc_get_wowlan_gpio_info(&seq, NULL);
	else if (!strcmp(v->get_field, "pattern_info"))
		proc_get_pattern_info(&seq, NULL);
	else {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n",
			v->name, got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_proc_wow_vectors.json";
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

	printf("ok: %zu proc wow registry + pattern vectors\n", count);
	return 0;
}
