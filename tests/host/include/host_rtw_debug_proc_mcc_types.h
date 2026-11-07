/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-222 MCC proc registry setters. */
#ifndef HOST_RTW_DEBUG_PROC_MCC_TYPES_H
#define HOST_RTW_DEBUG_PROC_MCC_TYPES_H

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

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif

#define MAX_IFACE 4

struct registry_priv {
	u8 en_mcc;
	u32 rtw_mcc_single_tx_cri;
};

struct mcc_obj_priv {
	u8 enable_runtime_duration;
};

struct dvobj_priv {
	u8 iface_nums;
	struct _adapter *padapters[MAX_IFACE];
	struct mcc_obj_priv mcc_objpriv;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct dvobj_priv *dvobj;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define adapter_to_dvobj(adapter) ((adapter)->dvobj)

#define SET_MCC_RUNTIME_DURATION(adapter, flag)                                \
	do {                                                                   \
		adapter_to_dvobj(adapter)->mcc_objpriv.enable_runtime_duration = \
			(flag);                                                \
	} while (0)

#define RTW_INFO(fmt, ...) ((void)0)
#define rtw_warn_on(x) ((void)(x))

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

struct host_mcc_cmd_log {
	unsigned duration_cmd;
	u8 last_type;
	u8 last_val;
	unsigned phydm_cmd;
	u8 last_phydm_enable;
};

extern struct host_mcc_cmd_log g_mcc_cmd_log;

void host_debug_proc_mcc_reset_log(void);
const struct host_mcc_cmd_log *host_debug_proc_mcc_get_log(void);

u8 host_rtw_set_mcc_duration_cmd(struct _adapter *adapter, u8 type, u8 val);
u8 host_rtw_set_mcc_phydm_offload_enable_cmd(struct _adapter *adapter,
					     u8 enable, u8 enqueue);

ssize_t host_proc_set_mcc_enable(struct _adapter *padapter, const void *buffer,
				 size_t count);
ssize_t host_proc_set_mcc_duration(struct _adapter *padapter, const void *buffer,
				   size_t count);
ssize_t host_proc_set_mcc_phydm_offload_enable(struct _adapter *padapter,
					       const void *buffer, size_t count);
ssize_t host_proc_set_mcc_single_tx_criteria(struct _adapter *padapter,
					   const void *buffer, size_t count);

#endif /* HOST_RTW_DEBUG_PROC_MCC_TYPES_H */
