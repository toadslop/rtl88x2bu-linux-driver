/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 regd_exc list tests (W3-51).
 */
#ifndef HOST_RF_REGD_EXC_TYPES_H
#define HOST_RF_REGD_EXC_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

typedef unsigned long _irqL;

struct _mutex {
	int dummy;
};

typedef struct _mutex _mutex;

struct _list {
	struct _list *next;
	struct _list *prev;
};

typedef struct _list _list;

struct regd_exc_ent {
	_list list;
	char country[2];
	u8 domain;
	char regd_name[0];
};

struct rf_ctl_t {
	_mutex txpwr_lmt_mutex;
	_list reg_exc_list;
	u8 regd_exc_num;
};

#define RTW_PRINT_SEL(sel, fmt, ...) ((void)0)
#define rtw_warn_on(cond) ((void)(cond))

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

static inline void rtw_list_delete(_list *plist)
{
	plist->next->prev = plist->prev;
	plist->prev->next = plist->next;
	plist->next = plist;
	plist->prev = plist;
}

static inline _list *get_next(_list *list)
{
	return list->next;
}

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

static inline u32 rtw_end_of_queue_search(_list *queue, _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

static inline void _enter_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
	(void)pmutex;
	(void)pirqL;
}

static inline void _exit_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
	(void)pmutex;
	(void)pirqL;
}

void _dump_regd_exc_list(void *sel, struct rf_ctl_t *rfctl);
void dump_regd_exc_list(void *sel, struct rf_ctl_t *rfctl);
void rtw_regd_exc_add_with_nlen(struct rf_ctl_t *rfctl, const char *country,
				u8 domain, const char *regd_name, u32 nlen);
void rtw_regd_exc_add(struct rf_ctl_t *rfctl, const char *country, u8 domain,
		      const char *regd_name);
struct regd_exc_ent *_rtw_regd_exc_search(struct rf_ctl_t *rfctl,
					  const char *country, u8 domain);
struct regd_exc_ent *rtw_regd_exc_search(struct rf_ctl_t *rfctl,
					 const char *country, u8 domain);
void rtw_regd_exc_list_free(struct rf_ctl_t *rfctl);

void host_rf_regd_exc_reset(struct rf_ctl_t *rfctl);

void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);

#endif /* HOST_RF_REGD_EXC_TYPES_H */
