// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-221 11w TX mgmt inject test procs (host L2, STA paths). */

#include "host_rtw_debug_proc_tx_mgmt_types.h"

static struct host_tx_mgmt_issue_log g_issue_log;

void host_debug_proc_tx_mgmt_reset_log(void)
{
	memset(&g_issue_log, 0, sizeof(g_issue_log));
}

const struct host_tx_mgmt_issue_log *host_debug_proc_tx_mgmt_get_log(void)
{
	return &g_issue_log;
}

void issue_action_SA_Query(struct _adapter *padapter, u8 *raddr, u8 action,
			   unsigned short tid, u8 key_type)
{
	(void)padapter;
	(void)action;
	(void)tid;
	g_issue_log.sa_query++;
	g_issue_log.last_sa_key = key_type;
	memcpy(g_issue_log.last_bssid, raddr, ETH_ALEN);
}

int issue_deauth_11w(struct _adapter *padapter, u8 *da, unsigned short reason,
		     u8 key_type)
{
	(void)padapter;
	(void)reason;
	g_issue_log.deauth++;
	g_issue_log.last_deauth_key = key_type;
	memcpy(g_issue_log.last_bssid, da, ETH_ALEN);
	return 0;
}

void issue_auth(struct _adapter *padapter, void *psta, unsigned short status)
{
	(void)padapter;
	(void)psta;
	(void)status;
	g_issue_log.auth++;
}

void issue_assocreq(struct _adapter *padapter)
{
	(void)padapter;
	g_issue_log.assocreq++;
}

ssize_t proc_set_tx_sa_query(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	char tmp[16];
	u32 key_type;

	(void)file;
	(void)pos;

	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -1;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {
		int num = sscanf(tmp, "%x", &key_type);

		if (num != 1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return (ssize_t)count;
		}
		RTW_INFO("0: set sa query request , key_type=%d\n", key_type);
	}

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE &&
	    check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE &&
	    SEC_IS_BIP_KEY_INSTALLED(&padapter->securitypriv) == _TRUE) {
		RTW_INFO("STA:" MAC_FMT "\n",
			 MAC_ARG(get_my_bssid(&pmlmeinfo->network)));
		issue_action_SA_Query(padapter,
				      get_my_bssid(&pmlmeinfo->network), 0, 0,
				      (u8)key_type);
	}

	return (ssize_t)count;
}

int proc_get_tx_sa_query(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_deauth(void *file, const void *buffer, size_t count,
			   long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	char tmp[16];
	u32 key_type = 0;

	(void)file;
	(void)pos;

	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -1;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {
		int num = sscanf(tmp, "%x", &key_type);

		if (num != 1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return (ssize_t)count;
		}
		RTW_INFO("key_type=%d\n", key_type);
	}
	if (key_type > 4)
		return (ssize_t)count;

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE &&
	    check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		if (key_type == 3)
			return (ssize_t)count;
		issue_deauth_11w(padapter, get_my_bssid(&pmlmeinfo->network), 0,
				 (u8)key_type);
	}

	return (ssize_t)count;
}

int proc_get_tx_deauth(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_auth(void *file, const void *buffer, size_t count,
			 long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	char tmp[16];
	u32 tx_auth;

	(void)file;
	(void)pos;

	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -1;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {
		int num = sscanf(tmp, "%x", &tx_auth);

		if (num != 1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return (ssize_t)count;
		}
		RTW_INFO("1: setnd auth, 2: send assoc request. tx_auth=%d\n",
			 tx_auth);
	}

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE &&
	    check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		if (tx_auth == 1)
			issue_auth(padapter, NULL, 0);
		else if (tx_auth == 2)
			issue_assocreq(padapter);
		g_issue_log.last_auth_sel = (u8)tx_auth;
	}

	return (ssize_t)count;
}

int proc_get_tx_auth(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}
