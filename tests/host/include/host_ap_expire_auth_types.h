/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_EXPIRE_AUTH_TYPES_H
#define HOST_AP_EXPIRE_AUTH_TYPES_H

#include "host_types.h"
#include <stddef.h>

#define _TRUE 1
#define _FALSE 0
#define HOST_EXPIRE_AUTH_MAX_STA 2
#define HOST_EXPIRE_AUTH_ADAPTER_SZ 128

typedef unsigned long _irqL;
typedef int _lock;

struct _list {
	struct _list *next, *prev;
};

typedef struct _list _list;

struct sta_info {
	u8 expire_to;
	struct _list auth_list;
};

struct sta_priv {
	struct _list auth_list;
	_lock auth_list_lock;
	struct sta_info sta_pool[HOST_EXPIRE_AUTH_MAX_STA];
	u8 sta_count;
};

typedef struct _adapter {
	struct sta_priv stapriv;
} _adapter;

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

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

static inline struct _list *get_next(struct _list *list)
{
	return list->next;
}

static inline u8 rtw_end_of_queue_search(struct _list *queue, struct _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

void host_expire_auth_adapter_init(_adapter *padapter);
void host_expire_auth_add_sta(_adapter *padapter, u8 expire_to);
u8 host_expire_auth_flush_count(void);
void host_expire_auth_reset_flush_count(void);

void rtw_ap_expire_auth_list(_adapter *padapter);
void rtw_free_stainfo(_adapter *padapter, struct sta_info *psta);
int rtw_stainfo_offset(struct sta_priv *pstapriv, struct sta_info *psta);
u8 stainfo_offset_valid(int offset);
struct sta_info *rtw_get_stainfo_by_offset(struct sta_priv *pstapriv, int offset);

#endif /* HOST_AP_EXPIRE_AUTH_TYPES_H */
