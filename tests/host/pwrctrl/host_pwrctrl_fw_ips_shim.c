/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-185 rtw_set_fw_in_ips_mode (CONFIG_FWLPS_IN_IPS). */

#include "host_pwrctrl_fw_ips.h"

#include <string.h>

#define RTW_INFO(fmt, ...) ((void)0)
#define BIT(x) (1U << (x))

#ifdef FW_IPS_KERN_REF
#define HOST_FW_IPS_API static
#else
#define HOST_FW_IPS_API
#endif

static struct host_pwrctrl_fw_ips_trace g_trace;
static u8 g_hmetfr_seq[128];
static int g_hmetfr_len;
static int g_hmetfr_idx;
static u8 g_reg_cr_seq[32];
static int g_reg_cr_len;
static int g_reg_cr_idx;
static u8 g_cpwm_orig;
static u8 g_cpwm_after;
static u32 g_cpwm_reads_before_toggle;
static u32 g_cpwm_read_count;
static u32 g_time_ms;

#ifndef FW_IPS_KERN_REF
HOST_FW_IPS_API void host_pwrctrl_fw_ips_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
	g_hmetfr_idx = 0;
	g_reg_cr_idx = 0;
	g_cpwm_read_count = 0;
}

HOST_FW_IPS_API struct host_pwrctrl_fw_ips_trace *host_pwrctrl_fw_ips_get_trace(void)
{
	return &g_trace;
}

HOST_FW_IPS_API void host_pwrctrl_fw_ips_set_hmetfr_sequence(const u8 *vals, int n)
{
	if (n > (int)sizeof(g_hmetfr_seq))
		n = (int)sizeof(g_hmetfr_seq);
	memcpy(g_hmetfr_seq, vals, n);
	g_hmetfr_len = n;
	g_hmetfr_idx = 0;
}

HOST_FW_IPS_API void host_pwrctrl_fw_ips_set_reg_cr_sequence(const u8 *vals, int n)
{
	if (n > (int)sizeof(g_reg_cr_seq))
		n = (int)sizeof(g_reg_cr_seq);
	memcpy(g_reg_cr_seq, vals, n);
	g_reg_cr_len = n;
	g_reg_cr_idx = 0;
}

HOST_FW_IPS_API void host_pwrctrl_fw_ips_set_cpwm_leave(u8 cpwm_orig, u8 cpwm_after, u32 reads_before_toggle)
{
	g_cpwm_orig = cpwm_orig;
	g_cpwm_after = cpwm_after;
	g_cpwm_reads_before_toggle = reads_before_toggle;
	g_cpwm_read_count = 0;
}

HOST_FW_IPS_API void host_pwrctrl_fw_ips_set_time_ms(u32 ms)
{
	g_time_ms = ms;
}
#endif /* !FW_IPS_KERN_REF */

static void record_h2c(u8 cmd, u8 len, const u8 *parm)
{
	if (g_trace.h2c_count >= 4)
		return;
	g_trace.h2c[g_trace.h2c_count].cmd = cmd;
	g_trace.h2c[g_trace.h2c_count].len = len;
	memcpy(g_trace.h2c[g_trace.h2c_count].parm, parm, len);
	g_trace.h2c_count++;
}

static void record_read8(u8 reg, u8 val)
{
	if (g_trace.read8_count >= 32)
		return;
	g_trace.read8_reg[g_trace.read8_count] = reg;
	g_trace.read8_val[g_trace.read8_count] = val;
	g_trace.read8_count++;
}

static void record_hwreg(u8 var, u8 val)
{
	if (g_trace.hwreg_count >= 16)
		return;
	g_trace.hwreg[g_trace.hwreg_count].var = var;
	g_trace.hwreg[g_trace.hwreg_count].val = val;
	g_trace.hwreg_count++;
}

static u8 read8_reg(u16 reg)
{
	u8 val = 0;

	if (reg == REG_HMETFR && g_hmetfr_idx < g_hmetfr_len) {
		val = g_hmetfr_seq[g_hmetfr_idx++];
	} else if (reg == REG_CR && g_reg_cr_idx < g_reg_cr_len) {
		val = g_reg_cr_seq[g_reg_cr_idx++];
	} else if (reg == REG_HMETFR) {
		val = 0;
	}
	record_read8((u8)(reg & 0xff), val);
	return val;
}

static void rtw_btcoex_IpsNotify(PADAPTER_FW_IPS padapter, u8 mode)
{
	(void)padapter;
	g_trace.btcoex_ips_notify_calls++;
	g_trace.last_btcoex_mode = mode;
}

static void rtw_hal_fill_h2c_cmd(PADAPTER_FW_IPS padapter, u8 cmd, u8 len, u8 *parm)
{
	(void)padapter;
	record_h2c(cmd, len, parm);
}

static u8 rtw_read8(PADAPTER_FW_IPS padapter, u16 reg)
{
	(void)padapter;
	return read8_reg(reg);
}

static void rtw_hal_get_hwreg(PADAPTER_FW_IPS padapter, u8 var, u8 *pval)
{
	(void)padapter;
	if (var == HOST_HW_VAR_RPWM_TOG) {
		*pval = 0x10;
	} else if (var == HOST_HW_VAR_CPWM) {
		g_cpwm_read_count++;
		if (g_cpwm_read_count > g_cpwm_reads_before_toggle)
			*pval = g_cpwm_after;
		else
			*pval = g_cpwm_orig;
	} else {
		*pval = 0;
	}
	record_hwreg(var, *pval);
}

static void rtw_hal_set_hwreg(PADAPTER_FW_IPS padapter, u8 var, u8 *pval)
{
	(void)padapter;
	record_hwreg(var, *pval);
	if (var == HOST_HW_VAR_SET_RPWM)
		adapter_to_pwrctl_fw_ips(padapter)->tog = (u8)((*pval + 0x80) & 0x80);
}

static void rtw_mdelay_os(u32 ms)
{
	g_time_ms += ms;
}

static systime rtw_get_current_time(void)
{
	return g_time_ms;
}

static u32 rtw_get_passing_time_ms(systime start)
{
	if (g_time_ms < start)
		return 0;
	return g_time_ms - start;
}

void rtw_set_fw_in_ips_mode(PADAPTER_FW_IPS padapter, u8 enable)
{
	struct pwrctrl_priv_fw_ips *pwrpriv = adapter_to_pwrctl_fw_ips(padapter);
	int cnt = 0;
	systime start_time;
	u8 val8 = 0;
	u8 cpwm_orig = 0, cpwm_now = 0;
	u8 parm[H2C_INACTIVE_PS_LEN] = {0};

	if (padapter->netif_up == _FALSE) {
		RTW_INFO("%s: ERROR, netif is down\n", __func__);
		return;
	}

	if (enable) {
		rtw_btcoex_IpsNotify(padapter, pwrpriv->ips_mode_req);
		RTW_INFO("%s: issue H2C to FW when entering IPS\n", __func__);

		parm[0] = 0x1;
		rtw_hal_fill_h2c_cmd(padapter, H2C_INACTIVE_PS_, H2C_INACTIVE_PS_LEN, parm);
		do {
			val8 = rtw_read8(padapter, REG_HMETFR);
			cnt++;
			RTW_INFO("%s  polling REG_HMETFR=0x%x, cnt=%d\n", __func__, val8, cnt);
			rtw_mdelay_os(10);
		} while (cnt < 100 && (val8 != 0));

#ifdef CONFIG_LPS_LCLK
		if (val8 == 0) {
			rtw_hal_get_hwreg(padapter, HOST_HW_VAR_RPWM_TOG, &val8);
			val8 += 0x80;
			val8 |= BIT(0);
			rtw_hal_set_hwreg(padapter, HOST_HW_VAR_SET_RPWM, &val8);
			adapter_to_pwrctl_fw_ips(padapter)->tog = (u8)((val8 + 0x80) & 0x80);
			cnt = 0;
			val8 = 0;
			if (parm[1] == 0 || parm[2] == 0) {
				do {
					val8 = rtw_read8(padapter, REG_CR);
					cnt++;
					rtw_mdelay_os(10);
				} while (cnt < 20 && (val8 != 0xEA));
			}
		}
#endif
	} else {
		RTW_INFO("%s: Leaving IPS in FWLPS state\n", __func__);

#ifdef CONFIG_LPS_LCLK
		cpwm_orig = 0;
		rtw_hal_get_hwreg(padapter, HOST_HW_VAR_CPWM, &cpwm_orig);

		rtw_hal_get_hwreg(padapter, HOST_HW_VAR_RPWM_TOG, &val8);
		val8 += 0x80;
		val8 |= BIT(6);
		rtw_hal_set_hwreg(padapter, HOST_HW_VAR_SET_RPWM, &val8);
		adapter_to_pwrctl_fw_ips(padapter)->tog = (u8)((val8 + 0x80) & 0x80);

		start_time = rtw_get_current_time();
		do {
			rtw_mdelay_os(1);
			rtw_hal_get_hwreg(padapter, HOST_HW_VAR_CPWM, &cpwm_now);
			if ((cpwm_orig ^ cpwm_now) & 0x80)
				break;
			if (rtw_get_passing_time_ms(start_time) > 100)
				break;
		} while (1);
#endif
		parm[0] = 0x0;
		parm[1] = 0x0;
		parm[2] = 0x0;
		rtw_hal_fill_h2c_cmd(padapter, H2C_INACTIVE_PS_, H2C_INACTIVE_PS_LEN, parm);
		rtw_btcoex_IpsNotify(padapter, IPS_NONE);
	}
	g_trace.final_tog = pwrpriv->tog;
}
