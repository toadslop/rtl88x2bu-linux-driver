/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_DEBUG_C_

#ifdef HOST_RTW_DEBUG_VERSION_LOG_TEST
#include "host_rtw_debug_version_log_types.h"
#else
#include <drv_types.h>
#include <rtw_version.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RTW_DEBUG_VERSION_LOG_TEST) || !defined(CONFIG_RUST_RTW_DEBUG)

#ifdef CONFIG_RTW_DEBUG
extern const char *rtw_log_level_str[];
#endif

void dump_drv_version(void *sel)
{
	RTW_PRINT_SEL(sel, "%s %s\n", DRV_NAME, DRIVERVERSION);
}

void dump_log_level(void *sel)
{
#ifdef CONFIG_RTW_DEBUG
	int i;

	RTW_PRINT_SEL(sel, "drv_log_level:%d\n", rtw_drv_log_level);
	for (i = 0; i <= _DRV_MAX_; i++) {
		if (rtw_log_level_str[i])
			RTW_PRINT_SEL(sel, "%c %s = %d\n",
				(rtw_drv_log_level == (uint)i) ? '+' : ' ', rtw_log_level_str[i], i);
	}
#else
	RTW_PRINT_SEL(sel, "CONFIG_RTW_DEBUG is disabled\n");
#endif
}

#endif /* !CONFIG_RUST || HOST_RTW_DEBUG_VERSION_LOG_TEST || !CONFIG_RUST_RTW_DEBUG */

#if defined(CONFIG_RUST) && !defined(HOST_RTW_DEBUG_VERSION_LOG_TEST) && defined(CONFIG_RUST_RTW_DEBUG)

#include <drv_types.h>

const char *rtw_rust_debug_drv_name(void)
{
	return DRV_NAME;
}

const char *rtw_rust_debug_driver_version(void)
{
	return DRIVERVERSION;
}

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	RTW_PRINT_SEL(sel, "%s", line);
}

void rtw_rust_debug_test_print(const char *msg)
{
	RTW_PRINT("%s", msg);
}

#ifdef CONFIG_RTW_DEBUG
extern const char *rtw_log_level_str[];

const char *rtw_rust_debug_log_level_str(int idx)
{
	if (idx < 0 || idx > _DRV_MAX_)
		return NULL;
	return rtw_log_level_str[idx];
}
#endif /* CONFIG_RTW_DEBUG */

#ifdef CONFIG_PROC_DEBUG
#include <linux/utsname.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
const char *rtw_rust_debug_kernel_release(void)
{
	return utsname()->release;
}
#else
const char *rtw_rust_debug_kernel_release(void)
{
	return NULL;
}
#endif

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
const char *rtw_rust_debug_realtek_config_path(void)
{
	return REALTEK_CONFIG_PATH;
}
#endif

void rtw_rust_debug_drv_cfg_num_values(int *dbg, unsigned int *reg_cert,
	int *txpwr_by_rate, int *txpwr_by_rate_en, int *txpwr_limit,
	int *txpwr_limit_en, int *adaptivity_en, int *adaptivity_mode,
	unsigned int *busy_deny_ms)
{
	if (dbg)
		*dbg = DBG;
	if (reg_cert)
		*reg_cert = RTW_DEF_MODULE_REGULATORY_CERT;
	if (txpwr_by_rate)
		*txpwr_by_rate = CONFIG_TXPWR_BY_RATE;
	if (txpwr_by_rate_en)
		*txpwr_by_rate_en = CONFIG_TXPWR_BY_RATE_EN;
	if (txpwr_limit)
		*txpwr_limit = CONFIG_TXPWR_LIMIT;
	if (txpwr_limit_en)
		*txpwr_limit_en = CONFIG_TXPWR_LIMIT_EN;
	if (adaptivity_en)
		*adaptivity_en = CONFIG_RTW_ADAPTIVITY_EN;
	if (adaptivity_mode)
		*adaptivity_mode = CONFIG_RTW_ADAPTIVITY_MODE;
#ifdef RTW_BUSY_DENY_SCAN
	if (busy_deny_ms)
		*busy_deny_ms = BUSY_TRAFFIC_SCAN_DENY_PERIOD;
#endif
}

#ifdef CONFIG_GPIO_WAKEUP
int rtw_rust_debug_wakeup_gpio_idx(void)
{
	return WAKEUP_GPIO_IDX;
}
#endif /* CONFIG_GPIO_WAKEUP */

void rtw_rust_debug_dump_drv_cfg_pre_num_banners(void *sel)
{
#ifdef CONFIG_IOCTL_CFG80211
	RTW_PRINT_SEL(sel, "CFG80211\n");
#ifdef RTW_USE_CFG80211_STA_EVENT
	RTW_PRINT_SEL(sel, "RTW_USE_CFG80211_STA_EVENT\n");
#endif
	#ifdef CONFIG_RADIO_WORK
	RTW_PRINT_SEL(sel, "CONFIG_RADIO_WORK\n");
	#endif
#else
	RTW_PRINT_SEL(sel, "WEXT\n");
#endif
	RTW_PRINT_SEL(sel, "DBG:%d\n", DBG);
#ifdef CONFIG_RTW_DEBUG
	RTW_PRINT_SEL(sel, "CONFIG_RTW_DEBUG\n");
#endif
#ifdef CONFIG_CONCURRENT_MODE
	RTW_PRINT_SEL(sel, "CONFIG_CONCURRENT_MODE\n");
#endif
#ifdef CONFIG_POWER_SAVING
	RTW_PRINT_SEL(sel, "CONFIG_POWER_SAVING\n");
	#ifdef CONFIG_IPS
	RTW_PRINT_SEL(sel, "CONFIG_IPS\n");
	#endif
	#ifdef CONFIG_LPS
	RTW_PRINT_SEL(sel, "CONFIG_LPS\n");
		#ifdef CONFIG_LPS_LCLK
		RTW_PRINT_SEL(sel, "CONFIG_LPS_LCLK\n");
		#ifdef CONFIG_DETECT_CPWM_BY_POLLING
		RTW_PRINT_SEL(sel, "CONFIG_DETECT_CPWM_BY_POLLING\n");
		#endif
		#endif
		#ifdef CONFIG_LPS_CHK_BY_TP
		RTW_PRINT_SEL(sel, "CONFIG_LPS_CHK_BY_TP\n");
		#endif
		#ifdef CONFIG_LPS_ACK
		RTW_PRINT_SEL(sel, "CONFIG_LPS_ACK\n");
		#endif
	#endif
#endif
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	RTW_PRINT_SEL(sel, "LOAD_PHY_PARA_FROM_FILE - REALTEK_CONFIG_PATH=%s\n", REALTEK_CONFIG_PATH);
	#if defined(CONFIG_MULTIDRV) || defined(REALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER)
	RTW_PRINT_SEL(sel, "LOAD_PHY_PARA_FROM_FILE - REALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER\n");
	#endif
#ifdef CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY
	RTW_PRINT_SEL(sel, "CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY\n");
#endif
#ifdef CONFIG_CALIBRATE_TX_POWER_TO_MAX
	RTW_PRINT_SEL(sel, "CONFIG_CALIBRATE_TX_POWER_TO_MAX\n");
#endif
#endif
}

void rtw_rust_debug_dump_drv_cfg_odm_minimal_banners(void *sel)
{
#ifdef CONFIG_DISABLE_ODM
	RTW_PRINT_SEL(sel, "CONFIG_DISABLE_ODM\n");
#endif
#ifdef CONFIG_MINIMAL_MEMORY_USAGE
	RTW_PRINT_SEL(sel, "CONFIG_MINIMAL_MEMORY_USAGE\n");
#endif
}

void rtw_rust_debug_dump_drv_cfg_post_num_banners(void *sel)
{
#ifdef CONFIG_WOWLAN
	RTW_PRINT_SEL(sel, "CONFIG_WOWLAN - ");
#ifdef CONFIG_GPIO_WAKEUP
	RTW_PRINT_SEL(sel, "CONFIG_GPIO_WAKEUP - WAKEUP_GPIO_IDX:%d\n", WAKEUP_GPIO_IDX);
#endif
#endif
#ifdef CONFIG_TDLS
	RTW_PRINT_SEL(sel, "CONFIG_TDLS\n");
#endif
#ifdef CONFIG_RTW_80211R
	RTW_PRINT_SEL(sel, "CONFIG_RTW_80211R\n");
#endif
#ifdef CONFIG_RTW_NETIF_SG
	RTW_PRINT_SEL(sel, "CONFIG_RTW_NETIF_SG\n");
#endif
#ifdef CONFIG_RTW_WIFI_HAL
	RTW_PRINT_SEL(sel, "CONFIG_RTW_WIFI_HAL\n");
#endif
#ifdef RTW_BUSY_DENY_SCAN
	RTW_PRINT_SEL(sel, "RTW_BUSY_DENY_SCAN\n");
	RTW_PRINT_SEL(sel, "BUSY_TRAFFIC_SCAN_DENY_PERIOD = %u ms\n", \
		      BUSY_TRAFFIC_SCAN_DENY_PERIOD);
#endif
#ifdef CONFIG_RTW_TPT_MODE
	RTW_PRINT_SEL(sel, "CONFIG_RTW_TPT_MODE\n");
#endif
}


#endif /* CONFIG_PROC_DEBUG */

#endif /* CONFIG_RUST && CONFIG_RUST_RTW_DEBUG */
