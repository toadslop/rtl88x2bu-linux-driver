// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rsvd_page_cache helpers (W4-02). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_rsvd_types.h"

static struct rsvd_page_cache_t cache;
static char cache_name[] = "l2";

static void reset_cache(void)
{
	memset(&cache, 0, sizeof(cache));
	cache.name = cache_name;
}

static int expect_data(const u8 *want, u32 want_len)
{
	if (!cache.data || cache.size != want_len)
		return -1;
	return _rtw_memcmp(cache.data, want, want_len) == _TRUE ? 0 : -1;
}

static int test_update_all_alloc(void)
{
	u8 info[] = {0xaa, 0xbb, 0xcc};
	bool mod;

	reset_cache();
	mod = rsvd_page_cache_update_all(&cache, 2, 40, 128, info, 3);
	if (!mod || cache.loc != 2 || cache.page_num != 1 || cache.size != 3)
		return -1;
	return expect_data(info, 3);
}

static int test_update_all_idempotent(void)
{
	u8 info[] = {0xaa, 0xbb, 0xcc};

	reset_cache();
	if (!rsvd_page_cache_update_all(&cache, 2, 40, 128, info, 3))
		return -1;
	if (rsvd_page_cache_update_all(&cache, 2, 40, 128, info, 3))
		return -1;
	return expect_data(info, 3);
}

static int test_update_all_clear(void)
{
	u8 info[] = {0xaa, 0xbb, 0xcc};

	reset_cache();
	rsvd_page_cache_update_all(&cache, 2, 40, 128, info, 3);
	if (!rsvd_page_cache_update_all(&cache, 0, 40, 128, NULL, 0))
		return -1;
	if (cache.loc || cache.page_num || cache.size || cache.data)
		return -1;
	return 0;
}

static int test_update_data(void)
{
	u8 setup[] = {1, 2, 3, 4};
	u8 info[] = {0x0a, 0x0b, 0x0c, 0x0d};

	reset_cache();
	rsvd_page_cache_update_all(&cache, 1, 32, 64, setup, 4);
	if (!rsvd_page_cache_update_data(&cache, info, 4))
		return -1;
	if (rsvd_page_cache_update_data(&cache, info, 4))
		return -1;
	return expect_data(info, 4);
}

static int test_free_data(void)
{
	u8 info[] = {0x11, 0x22, 0x33};

	reset_cache();
	rsvd_page_cache_update_all(&cache, 3, 24, 128, info, 3);
	rsvd_page_cache_free_data(&cache);
	if (!cache.data && cache.loc == 3 && cache.size == 3)
		return 0;
	return -1;
}

static int test_free(void)
{
	u8 info[] = {0x44, 0x55, 0x66};

	reset_cache();
	rsvd_page_cache_update_all(&cache, 4, 24, 128, info, 3);
	rsvd_page_cache_free(&cache);
	if (cache.loc || cache.page_num || cache.size || cache.data)
		return -1;
	return 0;
}

int main(void)
{
	struct {
		const char *name;
		int (*fn)(void);
	} cases[] = {
		{"update_all_alloc", test_update_all_alloc},
		{"update_all_idempotent", test_update_all_idempotent},
		{"update_all_clear", test_update_all_clear},
		{"update_data", test_update_data},
		{"free_data", test_free_data},
		{"free", test_free},
	};
	int bad = 0;

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
		if (cases[i].fn()) {
			fprintf(stderr, "FAIL %s\n", cases[i].name);
			bad = 1;
		} else {
			printf("PASS %s\n", cases[i].name);
		}
	}
	if (!bad)
		printf("PASS %zu cases\n", sizeof(cases) / sizeof(cases[0]));
	return bad;
}
