// SPDX-License-Identifier: GPL-2.0
/* Shared core for W3-227 ACK timeout proc L2 (C oracle + kernel helpers). */

#include "host_rtw_debug_proc_ack_types.h"

#include <stdio.h>

#ifdef HOST_ACK_L1_REF
#define HOST_ACK_SCOPE static
#else
#define HOST_ACK_SCOPE
#endif
HOST_ACK_SCOPE struct host_ack_reg_log g_ack_reg_log;

HOST_ACK_SCOPE void host_debug_proc_ack_reset_log(void)
{
	memset(&g_ack_reg_log, 0, sizeof(g_ack_reg_log));
}

HOST_ACK_SCOPE const struct host_ack_reg_log *host_debug_proc_ack_get_log(void)
{
	return &g_ack_reg_log;
}

HOST_ACK_SCOPE u8 host_ack_reg_mem[2];

HOST_ACK_SCOPE void host_debug_proc_ack_seed_regs(u8 ackto, u8 cck)
{
	host_ack_reg_mem[0] = ackto;
	host_ack_reg_mem[1] = cck;
}

HOST_ACK_SCOPE u8 rtw_read8(struct _adapter *padapter, u32 addr)
{
	(void)padapter;

	if (addr == REG_ACKTO) {
		g_ack_reg_log.read_ackto++;
		g_ack_reg_log.last_read_ackto = host_ack_reg_mem[0];
		return host_ack_reg_mem[0];
	}
#ifdef CONFIG_RTL8821C
	if (addr == REG_ACKTO_CCK_8821C) {
		g_ack_reg_log.read_cck++;
		g_ack_reg_log.last_read_cck = host_ack_reg_mem[1];
		return host_ack_reg_mem[1];
	}
#endif
	return 0;
}

HOST_ACK_SCOPE void rtw_write8(struct _adapter *padapter, u32 addr, u8 val)
{
	(void)padapter;

	if (addr == REG_ACKTO) {
		g_ack_reg_log.write_ackto++;
		g_ack_reg_log.last_write_ackto = val;
		host_ack_reg_mem[0] = val;
	}
#ifdef CONFIG_RTL8821C
	if (addr == REG_ACKTO_CCK_8821C) {
		g_ack_reg_log.write_cck++;
		g_ack_reg_log.last_write_cck = val;
		host_ack_reg_mem[1] = val;
	}
#endif
}

HOST_ACK_SCOPE void rtw_rust_debug_proc_emit_ack_timeout(void *sel,
							struct _adapter *padapter)
{
	u8 ack_timeout_val;
#ifdef CONFIG_RTL8821C
	u8 ack_timeout_val_cck;
#endif

	ack_timeout_val = rtw_read8(padapter, REG_ACKTO);

#ifdef CONFIG_RTL8821C
	ack_timeout_val_cck = rtw_read8(padapter, REG_ACKTO_CCK_8821C);
	RTW_PRINT_SEL(sel,
		      "Current CCK packet ACK Timeout = %d us (0x%x).\n",
		      (int)ack_timeout_val_cck, (int)ack_timeout_val_cck);
	RTW_PRINT_SEL(sel,
		      "Current non-CCK packet ACK Timeout = %d us (0x%x).\n",
		      (int)ack_timeout_val, (int)ack_timeout_val);
#else
	RTW_PRINT_SEL(sel, "Current ACK Timeout = %d us (0x%x).\n",
		      (int)ack_timeout_val, (int)ack_timeout_val);
#endif
}

HOST_ACK_SCOPE ssize_t rtw_rust_debug_proc_set_ack_timeout(
	struct _adapter *padapter, const void *buffer, size_t count)
{
	char tmp[32];
	u32 ack_timeout_ms, ack_timeout_ms_cck;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num;

		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%u %u", &ack_timeout_ms, &ack_timeout_ms_cck);

#ifdef CONFIG_RTL8821C
		if (num < 2) {
			RTW_INFO(FUNC_ADPT_FMT ": input parameters < 2\n",
				 FUNC_ADPT_ARG(padapter));
			return -EINVAL;
		}
#else
		if (num < 1) {
			RTW_INFO(FUNC_ADPT_FMT ": input parameters < 1\n",
				 FUNC_ADPT_ARG(padapter));
			return -EINVAL;
		}
#endif
		rtw_write8(padapter, REG_ACKTO, (u8)ack_timeout_ms);

#ifdef CONFIG_RTL8821C
		rtw_write8(padapter, REG_ACKTO_CCK_8821C,
			   (u8)ack_timeout_ms_cck);
		RTW_INFO("Set CCK packet ACK Timeout to %d us.\n",
			 (int)ack_timeout_ms_cck);
		RTW_INFO("Set non-CCK packet ACK Timeout to %d us.\n",
			 (int)ack_timeout_ms);
#else
		RTW_INFO("Set ACK Timeout to %d us.\n", (int)ack_timeout_ms);
#endif
	}

	return (ssize_t)count;
}
