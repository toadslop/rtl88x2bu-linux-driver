/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_RSVD_TYPES_H
#define HOST_HAL_COM_RSVD_TYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;

#define _TRUE 1
#define _FALSE 0

int _rtw_memcmp(const void *a, const void *b, u32 sz);

struct rsvd_page_cache_t {
	char *name;
	u8 loc;
	u8 page_num;
	u8 *data;
	u32 size;
};

#define PageNum(_Len, _Size) \
	((u32)(((_Len) / (_Size)) + (((_Len) & ((_Size) - 1)) ? 1 : 0)))

bool rsvd_page_cache_update_all(struct rsvd_page_cache_t *cache, u8 loc,
				u8 txdesc_len, u32 page_size, u8 *info,
				u32 info_len);
bool rsvd_page_cache_update_data(struct rsvd_page_cache_t *cache, u8 *info,
				 u32 info_len);
void rsvd_page_cache_free_data(struct rsvd_page_cache_t *cache);
void rsvd_page_cache_free(struct rsvd_page_cache_t *cache);

#endif /* HOST_HAL_COM_RSVD_TYPES_H */
