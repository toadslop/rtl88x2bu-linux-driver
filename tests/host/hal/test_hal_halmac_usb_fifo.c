// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for USB mode + FIFO sel mappers (W4-23). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	unsigned a;
	unsigned b;
};

unsigned fifo_sel_drv2halmac(unsigned fifo_sel);
unsigned usb_mode_drv2halmac(unsigned usb_mode);

static int parse_fifo(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "fifo_sel", &tmp))
		return -1;
	v->a = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->b = (unsigned)tmp;
	return 0;
}

static int parse_usb(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "usb_mode", &tmp))
		return -1;
	v->a = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->b = (unsigned)tmp;
	return 0;
}

static int run_file(const char *path, int usb_mode)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      usb_mode ? parse_usb : parse_fifo, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = usb_mode ?
			usb_mode_drv2halmac(vectors[i].a) :
			fifo_sel_drv2halmac(vectors[i].a);

		if (got != vectors[i].b) {
			fprintf(stderr, "FAIL %s %s: got=%u expect=%u\n",
				usb_mode ? "usb_mode" : "fifo_sel",
				vectors[i].name, got, vectors[i].b);
			return -1;
		}
	}

	printf("PASS %s: %zu vectors from %s\n",
	       usb_mode ? "usb_mode" : "fifo_sel", count, path);
	return 0;
}

int main(int argc, char **argv)
{
	const char *fifo = argc > 1 ? argv[1] :
				 "hal_halmac_fifo_sel_drv2halmac_vectors.json";
	const char *usb = argc > 2 ? argv[2] :
				"hal_halmac_usb_mode_drv2halmac_vectors.json";

	if (run_file(fifo, 0) || run_file(usb, 1))
		return 1;
	return 0;
}
