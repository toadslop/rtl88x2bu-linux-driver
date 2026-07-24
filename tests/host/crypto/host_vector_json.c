/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Shared hex + hand-rolled JSON helpers for host L2 crypto vector fixtures.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

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

int host_hex_decode(const char *hex, unsigned char *out, size_t out_cap,
		    size_t *out_len)
{
	size_t n = 0;
	int hi = -1;

	if (!hex)
		return -1;

	for (; *hex; hex++) {
		int v;

		if (*hex == ' ' || *hex == '\n' || *hex == '\r' || *hex == '\t')
			continue;
		v = hex_nibble(*hex);
		if (v < 0)
			return -1;
		if (hi < 0) {
			hi = v;
			continue;
		}
		if (n >= out_cap)
			return -1;
		out[n++] = (unsigned char)((hi << 4) | v);
		hi = -1;
	}
	if (hi >= 0)
		return -1;
	*out_len = n;
	return 0;
}

const char *host_json_skip_ws(const char *p)
{
	while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
		p++;
	return p;
}

const char *host_json_find_key_in(const char *obj, size_t obj_len, const char *key)
{
	char pattern[64];
	const char *end = obj + obj_len;
	const char *p;
	size_t key_len = strlen(key);

	snprintf(pattern, sizeof(pattern), "\"%s\"", key);
	for (p = obj; p < end; p++) {
		if (strncmp(p, pattern, key_len + 2) != 0)
			continue;
		p += key_len + 2;
		p = host_json_skip_ws(p);
		if (p >= end || *p != ':')
			return NULL;
		p++;
		return host_json_skip_ws(p);
	}
	return NULL;
}

int host_json_parse_string_in(const char *obj, size_t obj_len, const char *key,
			      char *out, size_t out_cap)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t i = 0;

	if (!p || p >= obj + obj_len || *p != '"')
		return -1;
	p++;
	while (p < obj + obj_len && *p && *p != '"') {
		if (i + 1 >= out_cap)
			return -1;
		out[i++] = *p++;
	}
	if (p >= obj + obj_len || *p != '"')
		return -1;
	out[i] = '\0';
	return 0;
}

int host_json_parse_int_in(const char *obj, size_t obj_len, const char *key,
			   int *out)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	char *end = NULL;
	long v;

	if (!p || p >= obj + obj_len)
		return -1;
	v = strtol(p, &end, 10);
	if (end == p)
		return -1;
	*out = (int)v;
	return 0;
}

int host_json_parse_bool_in(const char *obj, size_t obj_len, const char *key,
			    int *out)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);

	if (!p || p >= obj + obj_len)
		return -1;
	if (strncmp(p, "true", 4) == 0) {
		*out = 1;
		return 0;
	}
	if (strncmp(p, "false", 5) == 0) {
		*out = 0;
		return 0;
	}
	return -1;
}

int host_load_vectors(const char *path, void *vecs, size_t vec_size, size_t cap,
		      host_vector_parse_fn parse_fn, size_t *count_out)
{
	char *json = NULL;
	long fsize;
	size_t count = 0;
	const char *p;
	FILE *f = fopen(path, "rb");

	if (!f)
		return -1;
	if (fseek(f, 0, SEEK_END))
		goto fail;
	fsize = ftell(f);
	if (fsize < 0)
		goto fail;
	rewind(f);
	json = malloc((size_t)fsize + 1);
	if (!json)
		goto fail;
	if (fread(json, 1, (size_t)fsize, f) != (size_t)fsize)
		goto fail;
	json[fsize] = '\0';
	fclose(f);
	f = NULL;

	p = strstr(json, "\"vectors\"");
	if (!p)
		goto fail;
	p = strchr(p, '[');
	if (!p)
		goto fail;
	p++;
	while (count < cap) {
		p = host_json_skip_ws(p);
		if (*p == ']')
			break;
		if (*p != '{')
			goto fail;
		{
			const char *obj = p;
			const char *obj_end = NULL;
			int depth = 0;

			while (*p) {
				if (*p == '{')
					depth++;
				else if (*p == '}') {
					depth--;
					if (depth == 0) {
						obj_end = p + 1;
						p++;
						break;
					}
				}
				p++;
			}
			if (!obj_end)
				goto fail;
			if (parse_fn(obj, (size_t)(obj_end - obj),
				     (char *)vecs + count * vec_size))
				goto fail;
			count++;
		}
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	free(json);
	*count_out = count;
	return 0;

fail:
	free(json);
	if (f)
		fclose(f);
	return -1;
}
