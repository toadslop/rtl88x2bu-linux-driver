/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-221 11w TX mgmt inject test procs. */
#ifndef HOST_RTW_DEBUG_PROC_TX_MGMT_TYPES_H
#define HOST_RTW_DEBUG_PROC_TX_MGMT_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif

#define ETH_ALEN 6

#define WIFI_ASOC_STATE 0x00000001U
#define WIFI_STATION_STATE 0x00000008U
#define WIFI_AP_STATE 0x00000010U

#define SEC_IS_BIP_KEY_INSTALLED(sec) ((sec)->binstallBIPkey)

struct WLAN_BSSID_EX {
	u8 MacAddress[ETH_ALEN];
};

struct mlme_ext_info {
	struct WLAN_BSSID_EX network;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
};

struct mlme_priv {
	u32 fw_state;
};

struct security_priv {
	u8 binstallBIPkey;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct security_priv securitypriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define RTW_INFO(fmt, ...) ((void)0)

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x)                                                             \
	(x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

static inline int check_fwstate(struct mlme_priv *m, int s)
{
	return (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

static inline u8 *get_my_bssid(struct WLAN_BSSID_EX *pnetwork)
{
	return pnetwork->MacAddress;
}

static inline int _rtw_memcmp(const void *a, const void *b, size_t n)
{
	return memcmp(a, b, n);
}

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

struct host_tx_mgmt_issue_log {
	unsigned sa_query;
	unsigned deauth;
	unsigned auth;
	unsigned assocreq;
	u8 last_sa_key;
	u8 last_deauth_key;
	u8 last_auth_sel;
	u8 last_bssid[ETH_ALEN];
};

void host_debug_proc_tx_mgmt_reset_log(void);
const struct host_tx_mgmt_issue_log *host_debug_proc_tx_mgmt_get_log(void);

void issue_action_SA_Query(struct _adapter *padapter, u8 *raddr, u8 action,
			   unsigned short tid, u8 key_type);
int issue_deauth_11w(struct _adapter *padapter, u8 *da, unsigned short reason,
		     u8 key_type);
void issue_auth(struct _adapter *padapter, void *psta, unsigned short status);
void issue_assocreq(struct _adapter *padapter);

int proc_get_tx_sa_query(struct seq_file *m, void *v);
ssize_t proc_set_tx_sa_query(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
int proc_get_tx_deauth(struct seq_file *m, void *v);
ssize_t proc_set_tx_deauth(void *file, const void *buffer, size_t count,
			   long *pos, void *data);
int proc_get_tx_auth(struct seq_file *m, void *v);
ssize_t proc_set_tx_auth(void *file, const void *buffer, size_t count,
			 long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_TX_MGMT_TYPES_H */
