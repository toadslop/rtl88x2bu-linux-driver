// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_hal_com_get_rsvd_page_types.h"
#include "host_vector_json.h"

void rtw_dump_rsvd_page_c_oracle(void *sel, struct _adapter *a, u8 po, u8 pn);
#ifdef RUST_HAL_COM_DUMP_RSVD_FIFO_ORACLE
void rtw_dump_rsvd_page(void *sel, struct _adapter *a, u8 po, u8 pn);
#else
#define rtw_dump_rsvd_page rtw_dump_rsvd_page_c_oracle
#endif

struct vector {
	char name[48];
	int page_offset;
	int page_num;
	int page_size;
	int zvmalloc_fail;
	char expect[2048];
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "page_offset", &tmp))
		v->page_offset = tmp;
	if (!host_json_parse_int_in(obj, len, "page_num", &tmp))
		v->page_num = tmp;
	if (!host_json_parse_int_in(obj, len, "page_size", &tmp))
		v->page_size = tmp;
	else
		v->page_size = -1;
	if (!host_json_parse_int_in(obj, len, "zvmalloc_fail", &tmp))
		v->zvmalloc_fail = tmp;
	else
		v->zvmalloc_fail = 0;
	return host_json_parse_string_in(obj, len, "expect", v->expect,
				       sizeof(v->expect));
}

static void unescape(char *s)
{
	char *w = s;

	for (char *r = s; *r; r++) {
		if (*r == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r++;
		} else {
			*w++ = *r;
		}
	}
	*w = '\0';
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				  : "hal_com_dump_rsvd_fifo_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "load %s failed\n", path), 1;

	for (size_t i = 0; i < count; i++) {
		struct host_sel_capture cap;
		struct _adapter *adap;
		char got[2048];

		unescape(vectors[i].expect);
		host_get_rsvd_page_reset();
		adap = host_get_rsvd_page_adapter();
		if (vectors[i].page_size >= 0)
			host_get_rsvd_page_set_page_size((u32)vectors[i].page_size);
		host_get_rsvd_page_set_zvmalloc_fail(vectors[i].zvmalloc_fail);
		host_sel_reset(&cap);
		rtw_dump_rsvd_page(&cap, adap, (u8)vectors[i].page_offset,
				 (u8)vectors[i].page_num);
		if (host_sel_string(&cap, got, sizeof(got)) < 0 ||
		    strcmp(got, vectors[i].expect)) {
			fprintf(stderr, "FAIL %s\n", vectors[i].name);
			return 1;
		}
		printf("PASS %s\n", vectors[i].name);
	}
	return 0;
}