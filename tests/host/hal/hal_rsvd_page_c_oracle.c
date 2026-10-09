// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-02 rsvd_page_cache helpers (from hal/hal_com.c). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_rsvd_types.h"

void *rtw_malloc(u32 sz);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
int _rtw_memcmp(const void *a, const void *b, u32 sz);
void rtw_warn_on(int cond);

#define RTW_INFO(fmt, ...) ((void)0)
#define RTW_ERR(fmt, ...) ((void)0)
#define RTW_WARN(fmt, ...) ((void)0)

bool rsvd_page_cache_update_all(struct rsvd_page_cache_t *cache, u8 loc,
				u8 txdesc_len, u32 page_size, u8 *info,
				u32 info_len)
{
	u8 page_num;
	bool modified = 0;
	bool loc_mod = 0, size_mod = 0, page_num_mod = 0;

	page_num = info_len ? (u8)PageNum(txdesc_len + info_len, page_size) : 0;
	if (!info_len)
		loc = 0;

	if (cache->loc != loc) {
		RTW_INFO("%s %s loc change (%u -> %u)\n", __func__, cache->name,
			 cache->loc, loc);
		loc_mod = 1;
	}
	if (cache->size != info_len) {
		RTW_INFO("%s %s size change (%u -> %u)\n", __func__, cache->name,
			 cache->size, info_len);
		size_mod = 1;
	}
	if (cache->page_num != page_num) {
		RTW_INFO("%s %s page_num change (%u -> %u)\n", __func__,
			 cache->name, cache->page_num, page_num);
		page_num_mod = 1;
	}

	if (info && info_len) {
		if (cache->data) {
			if (cache->size == info_len) {
				if (_rtw_memcmp(cache->data, info, info_len) != _TRUE) {
					RTW_INFO("%s %s data change\n", __func__,
						 cache->name);
					modified = 1;
				}
			} else
				rsvd_page_cache_free_data(cache);
		}

		if (!cache->data) {
			cache->data = rtw_malloc(info_len);
			if (!cache->data) {
				RTW_ERR("%s %s alloc data with size(%u) fail\n",
					__func__, cache->name, info_len);
				rtw_warn_on(1);
			} else {
				RTW_INFO("%s %s alloc data with size(%u)\n",
					 __func__, cache->name, info_len);
			}
			modified = 1;
		}

		if (cache->data && modified)
			memcpy(cache->data, info, info_len);
	} else {
		if (cache->data && size_mod)
			rsvd_page_cache_free_data(cache);
	}

	cache->loc = loc;
	cache->page_num = page_num;
	cache->size = info_len;

	return modified | loc_mod | size_mod | page_num_mod;
}

bool rsvd_page_cache_update_data(struct rsvd_page_cache_t *cache, u8 *info,
				 u32 info_len)
{
	bool modified = 0;

	if (!info || !info_len) {
		RTW_WARN("%s %s invalid input(info:%p, info_len:%u)\n", __func__,
			 cache->name, info, info_len);
		goto exit;
	}

	if (!cache->loc || !cache->page_num || !cache->size) {
		RTW_ERR("%s %s layout not ready(loc:%u, page_num:%u, size:%u)\n",
			__func__, cache->name, cache->loc, cache->page_num,
			cache->size);
		rtw_warn_on(1);
		goto exit;
	}

	if (cache->size != info_len) {
		RTW_ERR("%s %s size(%u) differ with info_len(%u)\n", __func__,
			cache->name, cache->size, info_len);
		rtw_warn_on(1);
		goto exit;
	}

	if (!cache->data) {
		cache->data = rtw_zmalloc(cache->size);
		if (!cache->data) {
			RTW_ERR("%s %s alloc data with size(%u) fail\n", __func__,
				cache->name, cache->size);
			rtw_warn_on(1);
			goto exit;
		}
		RTW_INFO("%s %s alloc data with size(%u)\n", __func__, cache->name,
			 info_len);
		modified = 1;
	}

	if (_rtw_memcmp(cache->data, info, cache->size) == _FALSE) {
		RTW_INFO("%s %s data change\n", __func__, cache->name);
		memcpy(cache->data, info, cache->size);
		modified = 1;
	}

exit:
	return modified;
}

void rsvd_page_cache_free_data(struct rsvd_page_cache_t *cache)
{
	if (cache->data) {
		rtw_mfree(cache->data, cache->size);
		cache->data = NULL;
	}
}

void rsvd_page_cache_free(struct rsvd_page_cache_t *cache)
{
	cache->loc = 0;
	cache->page_num = 0;
	rsvd_page_cache_free_data(cache);
	cache->size = 0;
}
