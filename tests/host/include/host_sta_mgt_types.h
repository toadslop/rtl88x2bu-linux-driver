/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 sta_mgt tests (W3-37, W3-38).
 */
#ifndef HOST_STA_MGT_TYPES_H
#define HOST_STA_MGT_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6

#define CONFIG_RTW_MACADDR_ACL 1
#define CONFIG_RTW_PRE_LINK_STA 1
#define NUM_ACL 16
#define RTW_ACL_PERIOD_DEV 0
#define RTW_ACL_PERIOD_BSS 1
#define RTW_ACL_PERIOD_NUM 2
#define RTW_ACL_MODE_DISABLED 0
#define RTW_ACL_MODE_ACCEPT_UNLESS_LISTED 1
#define RTW_ACL_MODE_DENY_UNLESS_LISTED 2
#define RTW_PRE_LINK_STA_NUM 8
#define WIFI_FW_PRE_LINK 0x00000800
#define HOST_STA_MGT_MAX_STA 32
#define HOST_STA_MGT_NUM_STA 4
#define SESSION_TRACKER_REG_ID_NUM 1

typedef unsigned long _irqL;
typedef int _lock;
typedef unsigned int uint;

struct _list {
	struct _list *next;
	struct _list *prev;
};

typedef struct _list _list;

struct __queue {
	struct _list queue;
	_lock lock;
};

typedef struct __queue _queue;

struct _adapter;

typedef bool (*st_match_rule)(struct _adapter *adapter, u8 *local_naddr, u8 *local_port,
			       u8 *remote_naddr, u8 *remote_port);

struct st_register {
	u8 s_proto;
	st_match_rule rule;
};

struct session_tracker {
	_list list;
	u32 local_naddr;
	u16 local_port;
	u32 remote_naddr;
	u16 remote_port;
	unsigned long set_time;
	u8 status;
};

struct st_ctl_t {
	struct st_register reg[SESSION_TRACKER_REG_ID_NUM];
	_queue tracker_q;
};

#define NUM_STA HOST_STA_MGT_NUM_STA
#define stainfo_offset_valid(offset) ((offset) >= 0 && (offset) < NUM_STA)

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

struct pre_link_sta_node_t {
	u8 valid;
	u8 addr[ETH_ALEN];
};

struct pre_link_sta_ctl_t {
	_lock lock;
	u8 num;
	struct pre_link_sta_node_t node[RTW_PRE_LINK_STA_NUM];
};

struct cmn_sta_info {
	u16 aid;
	u8 mac_addr[ETH_ALEN];
};

struct sta_info {
	struct cmn_sta_info cmn;
	uint state;
	struct st_ctl_t st_ctl;
};

struct sta_priv {
	struct wlan_acl_pool acl_list[RTW_ACL_PERIOD_NUM];
	struct _adapter *padapter;
	struct sta_info **sta_aid;
	u16 max_aid;
	u16 started_aid;
	u8 rr_aid;
	u16 max_num_sta;
	struct pre_link_sta_ctl_t pre_link_sta_ctl;
	u8 *pstainfo_buf;
};

struct _adapter {
	struct sta_priv stapriv;
};

typedef struct _adapter _adapter;

#define RTW_INFO(fmt, ...) ((void)0)
#define rtw_warn_on(cond) ((void)0)

static inline void _rtw_init_listhead(_list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void _rtw_spinlock_init(_lock *lock)
{
	(void)lock;
}

static inline void _rtw_spinlock_free(_lock *lock)
{
	(void)lock;
}

static inline void _rtw_init_queue(_queue *queue)
{
	_rtw_init_listhead(&queue->queue);
	_rtw_spinlock_init(&queue->lock);
}

static inline void _rtw_deinit_queue(_queue *queue)
{
	_rtw_spinlock_free(&queue->lock);
}

static inline void rtw_list_delete(_list *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	entry->next = entry;
	entry->prev = entry;
}

void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *pbuf, u32 sz);

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

static inline void rtw_list_insert_tail(_list *n, _list *head)
{
	_list *prev = head->prev;

	n->next = head;
	n->prev = prev;
	prev->next = n;
	head->prev = n;
}

static inline int _rtw_memcmp(const void *dst, const void *src, u32 sz)
{
	return memcmp(dst, src, sz) ? _FALSE : _TRUE;
}

static inline u16 ntohs(u16 val)
{
	return (u16)(((val & 0xff) << 8) | ((val >> 8) & 0xff));
}

int rtw_check_invalid_mac_address(const u8 *mac, u8 check_local_bit);
struct sta_info *rtw_get_stainfo(struct sta_priv *stapriv, const u8 *hwaddr);
void rtw_free_stainfo(_adapter *padapter, struct sta_info *psta);

void host_sta_mgt_acl_reset(_adapter *adapter);
void host_sta_mgt_acl_set_mode(_adapter *adapter, u8 period, int mode);
int host_sta_mgt_acl_add(_adapter *adapter, u8 period, const u8 *addr);
void host_sta_mgt_reset(_adapter *adapter);
int host_sta_mgt_aid_setup(_adapter *adapter, u16 max_aid, u16 max_num_sta, u8 rr_aid);
struct sta_info *host_sta_mgt_sta_add(_adapter *adapter, const u8 *mac, uint state);
int host_sta_mgt_pre_link_add(_adapter *adapter, const u8 *mac);
void host_sta_mgt_pre_link_init(_adapter *adapter);
int host_sta_mgt_pre_link_count(_adapter *adapter);
void host_sta_mgt_stctl_reset(struct st_ctl_t *st_ctl);
int host_sta_mgt_stctl_tracker_count(struct st_ctl_t *st_ctl);
void host_sta_mgt_stctl_tracker_add(struct st_ctl_t *st_ctl);
int host_sta_mgt_offset_setup(_adapter *adapter, u8 sta_index, struct sta_info **out_sta);

void rtw_st_ctl_init(struct st_ctl_t *st_ctl);
void rtw_st_ctl_deinit(struct st_ctl_t *st_ctl);
void rtw_st_ctl_register(struct st_ctl_t *st_ctl, u8 st_reg_id, struct st_register *reg);
void rtw_st_ctl_unregister(struct st_ctl_t *st_ctl, u8 st_reg_id);
bool rtw_st_ctl_chk_reg_s_proto(struct st_ctl_t *st_ctl, u8 s_proto);
bool rtw_st_ctl_chk_reg_rule(struct st_ctl_t *st_ctl, _adapter *adapter, u8 *local_naddr,
			     u8 *local_port, u8 *remote_naddr, u8 *remote_port);
int rtw_stainfo_offset(struct sta_priv *stapriv, struct sta_info *sta);
extern struct st_register test_st_reg;
bool test_st_match_rule(_adapter *adapter, u8 *local_naddr, u8 *local_port,
			u8 *remote_naddr, u8 *remote_port);

#endif /* HOST_STA_MGT_TYPES_H */
