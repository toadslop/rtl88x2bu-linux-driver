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
#include <hal_data.h>
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

/* W3-138: HCI cfg banners for dump_drv_cfg tail (Rust calls this; legacy C path too). */
void rtw_rust_debug_dump_drv_cfg_hci_banners(void *sel)
{
#ifdef CONFIG_USB_HCI
#ifdef CONFIG_SUPPORT_USB_INT
	RTW_PRINT_SEL(sel, "CONFIG_SUPPORT_USB_INT\n");
#endif
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	RTW_PRINT_SEL(sel, "CONFIG_USB_INTERRUPT_IN_PIPE\n");
#endif
#ifdef CONFIG_USB_TX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_USB_TX_AGGREGATION\n");
#endif
#ifdef CONFIG_USB_RX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_USB_RX_AGGREGATION\n");
#endif
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_TX
	RTW_PRINT_SEL(sel, "CONFIG_USE_USB_BUFFER_ALLOC_TX\n");
#endif
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_RX
	RTW_PRINT_SEL(sel, "CONFIG_USE_USB_BUFFER_ALLOC_RX\n");
#endif
#ifdef CONFIG_PREALLOC_RECV_SKB
	RTW_PRINT_SEL(sel, "CONFIG_PREALLOC_RECV_SKB\n");
#endif
#ifdef CONFIG_FIX_NR_BULKIN_BUFFER
	RTW_PRINT_SEL(sel, "CONFIG_FIX_NR_BULKIN_BUFFER\n");
#endif
#endif /* CONFIG_USB_HCI */

#ifdef CONFIG_SDIO_HCI
#ifdef CONFIG_TX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_TX_AGGREGATION\n");
#endif
#ifdef CONFIG_RX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_RX_AGGREGATION\n");
#endif
#ifdef RTW_XMIT_THREAD_HIGH_PRIORITY
	RTW_PRINT_SEL(sel, "RTW_XMIT_THREAD_HIGH_PRIORITY\n");
#endif
#ifdef RTW_XMIT_THREAD_HIGH_PRIORITY_AGG
	RTW_PRINT_SEL(sel, "RTW_XMIT_THREAD_HIGH_PRIORITY_AGG\n");
#endif
#ifdef DBG_SDIO
	RTW_PRINT_SEL(sel, "DBG_SDIO = %d\n", DBG_SDIO);
#endif
#endif /* CONFIG_SDIO_HCI */
}

#define RTW_RUST_DRV_CFG_TAIL_MI_MBSSID		BIT(0)
#define RTW_RUST_DRV_CFG_TAIL_SWTIMER_TXBCN	BIT(1)
#define RTW_RUST_DRV_CFG_TAIL_FW_HANDLE_TXBCN	BIT(2)
#define RTW_RUST_DRV_CFG_TAIL_CLIENT_PORT	BIT(3)
#define RTW_RUST_DRV_CFG_TAIL_PCI_TX_POLL	BIT(4)

void rtw_rust_debug_drv_cfg_tail_values(unsigned int *flags, int *iface_number,
	int *limited_ap_num, int *up_mapping_rule, int *nr_xmitframe,
	int *nr_xmitbuff, int *max_xmitbuf_sz, int *nr_xmit_extbuff,
	int *max_xmit_extbuf_sz, int *max_cmdbuf_sz, int *nr_recvframe,
	int *nr_recvbuff, unsigned int *rtw_recvbuf_nr_out, int *max_recvbuf_sz)
{
	unsigned int f = 0;

#ifdef CONFIG_MI_WITH_MBSSID_CAM
	f |= RTW_RUST_DRV_CFG_TAIL_MI_MBSSID;
#endif
#ifdef CONFIG_SWTIMER_BASED_TXBCN
	f |= RTW_RUST_DRV_CFG_TAIL_SWTIMER_TXBCN;
#endif
#ifdef CONFIG_FW_HANDLE_TXBCN
	f |= RTW_RUST_DRV_CFG_TAIL_FW_HANDLE_TXBCN;
#endif
#ifdef CONFIG_CLIENT_PORT_CFG
	f |= RTW_RUST_DRV_CFG_TAIL_CLIENT_PORT;
#endif
#ifdef CONFIG_PCI_TX_POLLING
	f |= RTW_RUST_DRV_CFG_TAIL_PCI_TX_POLL;
#endif

	if (flags)
		*flags = f;
	if (iface_number)
		*iface_number = CONFIG_IFACE_NUMBER;
#ifdef CONFIG_FW_HANDLE_TXBCN
	if (limited_ap_num)
		*limited_ap_num = CONFIG_LIMITED_AP_NUM;
#endif
	if (up_mapping_rule)
		*up_mapping_rule = CONFIG_RTW_UP_MAPPING_RULE;
	if (nr_xmitframe)
		*nr_xmitframe = NR_XMITFRAME;
	if (nr_xmitbuff)
		*nr_xmitbuff = NR_XMITBUFF;
	if (max_xmitbuf_sz)
		*max_xmitbuf_sz = MAX_XMITBUF_SZ;
	if (nr_xmit_extbuff)
		*nr_xmit_extbuff = NR_XMIT_EXTBUFF;
	if (max_xmit_extbuf_sz)
		*max_xmit_extbuf_sz = MAX_XMIT_EXTBUF_SZ;
	if (max_cmdbuf_sz)
		*max_cmdbuf_sz = MAX_CMDBUF_SZ;
	if (nr_recvframe)
		*nr_recvframe = NR_RECVFRAME;
	if (nr_recvbuff)
		*nr_recvbuff = NR_RECVBUFF;
	if (max_recvbuf_sz)
		*max_recvbuf_sz = MAX_RECVBUF_SZ;
	if (rtw_recvbuf_nr_out) {
		extern uint rtw_recvbuf_nr;

		*rtw_recvbuf_nr_out = rtw_recvbuf_nr;
	}
}

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

#if defined(CONFIG_RTW_DEBUG) || defined(CONFIG_PROC_DEBUG)
u8 rtw_rust_debug_sec_cam_num(_adapter *adapter)
{
	return adapter_to_dvobj(adapter)->cam_ctl.num;
}

void rtw_rust_debug_sec_cam_read(_adapter *adapter, u8 idx, u16 *ctrl, u8 *mac, u8 *key)
{
	struct sec_cam_ent ent;

	rtw_sec_read_cam_ent(adapter, idx, (u8 *)(&ent.ctrl), ent.mac, ent.key);
	if (ctrl)
		*ctrl = ent.ctrl;
	if (mac)
		_rtw_memcpy(mac, ent.mac, ETH_ALEN);
	if (key)
		_rtw_memcpy(key, ent.key, 16);
}

struct sec_cam_ent *rtw_rust_debug_sec_cam_cache_ent(_adapter *adapter, u8 idx)
{
	return &adapter_to_dvobj(adapter)->cam_cache[idx];
}
#endif

void rtw_rust_debug_log_info(const char *line)
{
	RTW_INFO("%s", line);
}

u16 rtw_rust_debug_recv_sink_udpport(_adapter *adapter)
{
	return adapter->recvpriv.sink_udpport;
}

u16 rtw_rust_debug_recv_pre_rtp_rxseq(_adapter *adapter)
{
	return adapter->recvpriv.pre_rtp_rxseq;
}

u16 rtw_rust_debug_recv_cur_rtp_rxseq(_adapter *adapter)
{
	return adapter->recvpriv.cur_rtp_rxseq;
}

void rtw_rust_debug_recv_set_pre_rtp_rxseq(_adapter *adapter, u16 seq)
{
	adapter->recvpriv.pre_rtp_rxseq = seq;
}

void rtw_rust_debug_recv_set_cur_rtp_rxseq(_adapter *adapter, u16 seq)
{
	adapter->recvpriv.cur_rtp_rxseq = seq;
}

void rtw_rust_debug_sta_reorder_get(struct sta_info *sta, int tid, u8 *enable,
				    u8 *ampdu_size, u16 *indicate_seq)
{
	struct recv_reorder_ctrl *rc = &sta->recvreorder_ctrl[tid];

	if (enable)
		*enable = rc->enable;
	if (ampdu_size)
		*ampdu_size = rc->ampdu_size;
	if (indicate_seq)
		*indicate_seq = rc->indicate_seq;
}

_adapter *rtw_rust_debug_dvobj_primary_adapter(struct dvobj_priv *dvobj)
{
	return dvobj_get_primary_adapter(dvobj);
}

struct rf_ctl_t *rtw_rust_debug_dvobj_rfctl(struct dvobj_priv *dvobj)
{
	return dvobj_to_rfctl(dvobj);
}

u8 rtw_rust_debug_hal_chk_proto_cap(_adapter *adapter, u8 cap)
{
	return hal_chk_proto_cap(adapter, cap) ? 1 : 0;
}

u8 rtw_rust_debug_hal_is_bw_support(_adapter *adapter, u8 bw)
{
	return hal_is_bw_support(adapter, bw) ? 1 : 0;
}

const char *rtw_rust_debug_ch_width_str(u8 bw)
{
	return ch_width_str(bw);
}

u32 rtw_rust_debug_rfctl_rate_bmp_ht(struct rf_ctl_t *rfctl, u8 bw)
{
	return rfctl->rate_bmp_ht_by_bw[bw];
}

u64 rtw_rust_debug_rfctl_rate_bmp_vht(struct rf_ctl_t *rfctl, u8 bw)
{
	return rfctl->rate_bmp_vht_by_bw[bw];
}

u16 rtw_rust_debug_rfctl_rate_bmp_cck_ofdm(struct rf_ctl_t *rfctl)
{
	return rfctl->rate_bmp_cck_ofdm;
}

#endif /* CONFIG_RUST && CONFIG_RUST_RTW_DEBUG */
