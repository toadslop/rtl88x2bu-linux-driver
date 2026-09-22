// SPDX-License-Identifier: GPL-2.0
#include <stddef.h>
#include "host_mem_premem_types.h"

static struct host_skb skb_pool[HOST_MEM_PREMEM_NR_SKB];
static struct host_skb *q[HOST_MEM_PREMEM_NR_SKB];
static int q_len;
static int pool_used;
static u8 *rtk_buf_mem[HOST_MEM_PREMEM_NR_BUF];
static u8 buf_slots[HOST_MEM_PREMEM_NR_BUF];

void host_mem_premem_reset(void)
{
	q_len = 0;
	pool_used = 0;
	for (int i = 0; i < HOST_MEM_PREMEM_NR_BUF; i++) {
		buf_slots[i] = 0;
		rtk_buf_mem[i] = NULL;
	}
}

void host_mem_premem_set_buf(int index, u8 tag)
{
	if (index < 0 || index >= HOST_MEM_PREMEM_NR_BUF)
		return;
	buf_slots[index] = tag;
	rtk_buf_mem[index] = &buf_slots[index];
}

void host_mem_premem_seed_skb(int tag)
{
	if (pool_used >= HOST_MEM_PREMEM_NR_SKB || q_len >= HOST_MEM_PREMEM_NR_SKB)
		return;
	skb_pool[pool_used].tag = tag;
	q[q_len++] = &skb_pool[pool_used++];
}

int host_mem_premem_queue_len(void)
{
	return q_len;
}

u8 *rtw_get_buf_premem(int index)
{
	if (index < 0 || index >= HOST_MEM_PREMEM_NR_BUF)
		return NULL;
	return rtk_buf_mem[index];
}

u16 rtw_rtkm_get_buff_size(void)
{
	return HOST_MEM_PREMEM_BUF_SZ;
}

u8 rtw_rtkm_get_nr_recv_skb(void)
{
	return HOST_MEM_PREMEM_NR_SKB;
}

struct host_skb *rtw_alloc_skb_premem(u16 in_size)
{
	struct host_skb *s;
	int i;

	if (in_size > HOST_MEM_PREMEM_BUF_SZ || q_len == 0)
		return NULL;
	s = q[0];
	for (i = 1; i < q_len; i++)
		q[i - 1] = q[i];
	q_len--;
	return s;
}

int rtw_free_skb_premem(struct host_skb *pskb)
{
	if (!pskb || q_len >= HOST_MEM_PREMEM_NR_SKB)
		return -1;
	q[q_len++] = pskb;
	return 0;
}
