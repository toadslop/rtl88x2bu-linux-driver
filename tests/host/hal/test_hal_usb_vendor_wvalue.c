// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for usb_vendor_wvalue_fixup (W4-28). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 48

typedef uint16_t u16;

struct vector {
	char name[MAX_NAME];
	unsigned wvalue;
	unsigned expect;
};

u16 usb_vendor_wvalue_fixup(u16 wvalue);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "wvalue", &tmp))
		return -1;
	v->wvalue = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				     : "hal_usb_vendor_wvalue_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		u16 got = usb_vendor_wvalue_fixup((u16)vectors[i].wvalue);

		if (got != (u16)vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: wvalue=0x%04x got=0x%04x expect=0x%04x\n",
				vectors[i].name, vectors[i].wvalue, got,
				vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
