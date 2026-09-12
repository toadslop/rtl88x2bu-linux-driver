// SPDX-License-Identifier: GPL-2.0
/* Host oracle for expire_timeout_chk auth_list — keep in sync with core/rtw_ap.c. */
#include "host_ap_expire_auth_types.h"
#include <stddef.h>
#include <string.h>

#define NUM_STA 2

struct _list {
	struct _list *next, *prev;
};

struct sta_info {
	u8 expire_to;
	struct _list auth_list;
};

struct sta_priv {
	struct _list auth_list;
	struct sta_info sta_pool[NUM_STA];
};

struct _adapter {
	struct sta_priv stapriv;
};

static u8 flush_count, sta_count;

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

static void list_init(struct _list *list)
{
	list->next = list->prev = list;
}

static void list_insert_tail(struct _list *n, struct _list *head)
{
	n->next = head;
	n->prev = head->prev;
	head->prev->next = n;
	head->prev = n;
}

void host_expire_auth_adapter_init(u8 *buf)
{
	struct _adapter *ad = (struct _adapter *)buf;

	memset(ad, 0, sizeof(*ad));
	list_init(&ad->stapriv.auth_list);
	sta_count = flush_count = 0;
}

void host_expire_auth_add_sta(u8 *buf, u8 expire_to)
{
	struct _adapter *ad = (struct _adapter *)buf;
	struct sta_info *psta;

	if (sta_count >= NUM_STA)
		return;
	psta = &ad->stapriv.sta_pool[sta_count++];
	psta->expire_to = expire_to;
	list_init(&psta->auth_list);
	list_insert_tail(&psta->auth_list, &ad->stapriv.auth_list);
}

void host_expire_auth_step(u8 *buf)
{
	struct _adapter *ad = (struct _adapter *)buf;
	struct _list *phead = &ad->stapriv.auth_list;
	struct _list *plist = phead->next;
	u8 flush_num = 0;

	flush_count = 0;
	while (plist != phead) {
		struct sta_info *psta = LIST_CONTAINOR(plist, struct sta_info, auth_list);

		plist = plist->next;
		if (psta->expire_to > 0) {
			psta->expire_to--;
			if (psta->expire_to == 0)
				flush_num++;
		}
	}
	flush_count = flush_num;
}

u8 host_expire_auth_sta_expire_to(const u8 *buf, u8 index)
{
	const struct _adapter *ad = (const struct _adapter *)buf;

	if (index >= NUM_STA)
		return 0;
	return ad->stapriv.sta_pool[index].expire_to;
}

u8 host_expire_auth_flush_count(void)
{
	return flush_count;
}
