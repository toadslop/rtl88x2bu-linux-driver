// SPDX-License-Identifier: GPL-2.0
#include "host_ap_expire_auth_types.h"
#include <string.h>

static u8 host_flush_count;

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

void host_expire_auth_reset_flush_count(void)
{
	host_flush_count = 0;
}

u8 host_expire_auth_flush_count(void)
{
	return host_flush_count;
}

void host_expire_auth_adapter_init(_adapter *padapter)
{
	memset(padapter, 0, sizeof(*padapter));
	list_init(&padapter->stapriv.auth_list);
	padapter->stapriv.sta_count = 0;
	host_expire_auth_reset_flush_count();
}

void host_expire_auth_add_sta(_adapter *padapter, u8 expire_to)
{
	struct sta_info *psta;

	if (padapter->stapriv.sta_count >= HOST_EXPIRE_AUTH_MAX_STA)
		return;
	psta = &padapter->stapriv.sta_pool[padapter->stapriv.sta_count++];
	psta->expire_to = expire_to;
	list_init(&psta->auth_list);
	list_insert_tail(&psta->auth_list, &padapter->stapriv.auth_list);
}

int rtw_stainfo_offset(struct sta_priv *pstapriv, struct sta_info *psta)
{
	return (int)(psta - pstapriv->sta_pool);
}

u8 stainfo_offset_valid(int offset)
{
	return (offset >= 0 && offset < HOST_EXPIRE_AUTH_MAX_STA) ? _TRUE : _FALSE;
}

struct sta_info *rtw_get_stainfo_by_offset(struct sta_priv *pstapriv, int offset)
{
	if (!stainfo_offset_valid(offset))
		return NULL;
	return &pstapriv->sta_pool[offset];
}

void rtw_free_stainfo(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	host_flush_count++;
}
