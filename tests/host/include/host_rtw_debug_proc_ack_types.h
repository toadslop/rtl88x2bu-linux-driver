/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-227 proc ACK timeout. */
#ifndef HOST_RTW_DEBUG_PROC_ACK_TYPES_H
#define HOST_RTW_DEBUG_PROC_ACK_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif

#define REG_ACKTO 0x0640U
#define REG_ACKTO_CCK_8821C 0x0639U

struct _adapter {
	u8 _pad;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define FUNC_ADPT_FMT "%s"
#define FUNC_ADPT_ARG(p) "__adapter__"

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define RTW_INFO(fmt, ...) ((void)0)

static inline void rtw_warn_on(int cond)
{
	(void)cond;
}

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

struct host_ack_reg_log {
	unsigned read_ackto;
	unsigned read_cck;
	unsigned write_ackto;
	unsigned write_cck;
	u8 last_read_ackto;
	u8 last_read_cck;
	u8 last_write_ackto;
	u8 last_write_cck;
};

#ifndef HOST_ACK_L1_REF
extern struct host_ack_reg_log g_ack_reg_log;

void host_debug_proc_ack_reset_log(void);
const struct host_ack_reg_log *host_debug_proc_ack_get_log(void);

u8 rtw_read8(struct _adapter *padapter, u32 addr);
void rtw_write8(struct _adapter *padapter, u32 addr, u8 val);

void rtw_rust_debug_proc_emit_ack_timeout(void *sel,
					   struct _adapter *padapter);
ssize_t rtw_rust_debug_proc_set_ack_timeout(struct _adapter *padapter,
					    const void *buffer, size_t count);
#endif /* HOST_ACK_L1_REF */

#endif /* HOST_RTW_DEBUG_PROC_ACK_TYPES_H */
