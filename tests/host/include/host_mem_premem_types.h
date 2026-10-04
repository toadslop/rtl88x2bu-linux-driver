/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MEM_PREMEM_TYPES_H
#define HOST_MEM_PREMEM_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

#define HOST_MEM_PREMEM_BUF_SZ 15360
#define HOST_MEM_PREMEM_NR_SKB 16
#define HOST_MEM_PREMEM_NR_BUF 8

struct host_skb {
	int tag;
};

void host_mem_premem_reset(void);
void host_mem_premem_seed_skb(int tag);
void host_mem_premem_set_buf(int index, u8 tag);
int host_mem_premem_queue_len(void);

u8 *rtw_get_buf_premem(int index);
u16 rtw_rtkm_get_buff_size(void);
u8 rtw_rtkm_get_nr_recv_skb(void);
struct host_skb *rtw_alloc_skb_premem(u16 in_size);
int rtw_free_skb_premem(struct host_skb *pskb);

#endif /* HOST_MEM_PREMEM_TYPES_H */
