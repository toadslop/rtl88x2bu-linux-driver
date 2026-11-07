/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-220 monitor / simple_config / xmit_block proc handlers. */
#ifndef HOST_RTW_DEBUG_PROC_MONITOR_TYPES_H
#define HOST_RTW_DEBUG_PROC_MONITOR_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif

#define BIT(x) (1U << (x))
#define BIT0 BIT(0)
#define BIT1 BIT(1)

#define WIFI_MONITOR_STATE 0x80000000U

#define ARPHRD_IEEE80211 801U
#define ARPHRD_IEEE80211_RADIOTAP 803U

enum XMIT_BLOCK_REASON {
	XMIT_BLOCK_NONE = 0,
	XMIT_BLOCK_REDLMEM = BIT0,
	XMIT_BLOCK_SUSPEND = BIT1,
	XMIT_BLOCK_MAX = 0xFF,
};

struct mlme_priv {
	u32 fw_state;
};

struct mlme_ext_priv {
	u8 cur_channel;
	u8 cur_ch_offset;
	u8 cur_bwmode;
};

struct dvobj_priv {
	u8 xmit_block;
	u8 oper_channel;
	u8 oper_bwmode;
	u8 oper_ch_offset;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct dvobj_priv *dvobj;
	u8 rtw_simple_config;
};

struct net_device {
	void *priv;
	u16 type;
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

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

static inline struct dvobj_priv *adapter_to_dvobj(struct _adapter *a)
{
	return a ? a->dvobj : NULL;
}

static inline int check_fwstate(struct mlme_priv *m, int s)
{
	if (!s && !m->fw_state)
		return _TRUE;
	return (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

#define MLME_IS_MONITOR(adapter)                                               \
	check_fwstate(&((adapter)->mlmepriv), WIFI_MONITOR_STATE)

static inline u8 rtw_get_oper_ch(struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	return dvobj ? dvobj->oper_channel : 0;
}

static inline u8 rtw_get_oper_bw(struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	return dvobj ? dvobj->oper_bwmode : 0;
}

static inline u8 rtw_get_oper_choffset(struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	return dvobj ? dvobj->oper_ch_offset : 0;
}

static inline void set_channel_bwmode(struct _adapter *padapter, u8 channel,
				    u8 channel_offset, u16 bwmode)
{
	if (!padapter)
		return;
	padapter->mlmeextpriv.cur_channel = channel;
	padapter->mlmeextpriv.cur_ch_offset = channel_offset;
	padapter->mlmeextpriv.cur_bwmode = (u8)bwmode;
	if (padapter->dvobj) {
		padapter->dvobj->oper_channel = channel;
		padapter->dvobj->oper_ch_offset = channel_offset;
		padapter->dvobj->oper_bwmode = (u8)bwmode;
	}
}

static inline void dump_xmit_block(void *sel, struct _adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 block;

	if (!dvobj)
		return;
	block = dvobj->xmit_block;

	RTW_PRINT_SEL(sel, "[XMIT-BLOCK] xmit_block :0x%02x\n", block);
	if (block & XMIT_BLOCK_REDLMEM)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_REDLMEM");
	if (block & XMIT_BLOCK_SUSPEND)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_SUSPEND");
	if (block == XMIT_BLOCK_NONE)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_NONE");
}

static inline void rtw_set_xmit_block(struct _adapter *padapter,
				      enum XMIT_BLOCK_REASON reason)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (!dvobj)
		return;
	dvobj->xmit_block |= (u8)reason;
}

static inline void rtw_clr_xmit_block(struct _adapter *padapter,
				      enum XMIT_BLOCK_REASON reason)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (!dvobj)
		return;
	dvobj->xmit_block &= (u8)~reason;
}

int proc_get_monitor(struct seq_file *m, void *v);
ssize_t proc_set_monitor(void *file, const void *buffer, size_t count,
			 long *pos, void *data);
int proc_get_simple_config(struct seq_file *m, void *v);
ssize_t proc_set_simple_config(void *file, const void *buffer, size_t count,
			       long *pos, void *data);
int proc_get_xmit_block(struct seq_file *m, void *v);
ssize_t proc_set_xmit_block(void *file, const void *buffer, size_t count,
			    long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_MONITOR_TYPES_H */
