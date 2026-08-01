/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 sta_mgt tests (W3-37).
 */
#ifndef HOST_STA_MGT_TYPES_H
#define HOST_STA_MGT_TYPES_H

#include <stdbool.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6

#define CONFIG_RTW_MACADDR_ACL 1
#define NUM_ACL 16
#define RTW_ACL_PERIOD_DEV 0
#define RTW_ACL_PERIOD_BSS 1
#define RTW_ACL_PERIOD_NUM 2
#define RTW_ACL_MODE_DISABLED 0
#define RTW_ACL_MODE_ACCEPT_UNLESS_LISTED 1
#define RTW_ACL_MODE_DENY_UNLESS_LISTED 2

typedef unsigned long _irqL;
typedef int _lock;

struct _list {
	struct _list *next;
	struct _list *prev;
};

typedef struct _list _list;

struct __queue {
	_lock lock;
	struct _list queue;
};

typedef struct __queue _queue;

struct rtw_wlan_acl_node {
	struct _list list;
	u8 addr[ETH_ALEN];
	u8 valid;
};

struct wlan_acl_pool {
	int mode;
	int num;
	struct rtw_wlan_acl_node aclnode[NUM_ACL];
	_queue acl_node_q;
};

struct sta_priv {
	struct wlan_acl_pool acl_list[RTW_ACL_PERIOD_NUM];
};

struct _adapter {
	struct sta_priv stapriv;
};

typedef struct _adapter _adapter;

#define RTW_INFO(fmt, ...) ((void)0)
#define rtw_warn_on(cond) ((void)0)

static inline void _enter_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline void _exit_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline void _rtw_init_listhead(_list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void rtw_list_insert_tail(_list *n, _list *head)
{
	_list *prev = head->prev;

	n->next = head;
	n->prev = prev;
	prev->next = n;
	head->prev = n;
}

static inline struct _list *get_next(_list *list)
{
	return list->next;
}

static inline _list *get_list_head(_queue *queue)
{
	return &(queue->queue);
}

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

static inline u32 rtw_end_of_queue_search(_list *queue, _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

static inline int _rtw_memcmp(const void *dst, const void *src, u32 sz)
{
	return memcmp(dst, src, sz) ? _FALSE : _TRUE;
}

static inline u16 ntohs(u16 val)
{
	return (u16)(((val & 0xff) << 8) | ((val >> 8) & 0xff));
}

void host_sta_mgt_acl_reset(_adapter *adapter);
void host_sta_mgt_acl_set_mode(_adapter *adapter, u8 period, int mode);
int host_sta_mgt_acl_add(_adapter *adapter, u8 period, const u8 *addr);

#endif /* HOST_STA_MGT_TYPES_H */
