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

void rtw_rust_debug_print_sel_cont(void *sel, const char *line)
{
	_RTW_PRINT_SEL(sel, "%s", line);
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

u32 rtw_rust_debug_read32(_adapter *adapter, u32 addr)
{
	return rtw_read32(adapter, addr);
}

u8 rtw_rust_debug_read8(_adapter *adapter, u32 addr)
{
	return rtw_read8(adapter, addr);
}

u16 rtw_rust_debug_read16(_adapter *adapter, u32 addr)
{
	return rtw_read16(adapter, addr);
}

void rtw_rust_debug_write8(_adapter *adapter, u32 addr, u8 val)
{
	rtw_write8(adapter, addr, val);
}

void rtw_rust_debug_write16(_adapter *adapter, u32 addr, u16 val)
{
	rtw_write16(adapter, addr, val);
}

void rtw_rust_debug_write32(_adapter *adapter, u32 addr, u32 val)
{
	rtw_write32(adapter, addr, val);
}

#ifdef CONFIG_PROC_DEBUG
_adapter *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	return (_adapter *)rtw_netdev_priv((struct net_device *)seq->private);
}

void rtw_rust_debug_proc_emit_fwstate(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "fwstate=0x%x\n", get_fwstate(&adapter->mlmepriv));
}

void rtw_rust_debug_proc_emit_mlmext_state(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "pmlmeinfo->state=0x%x\n",
		      adapter->mlmeextpriv.mlmext_info.state);
}

void rtw_rust_debug_proc_emit_qos_option(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "qos_option=%d\n", adapter->mlmepriv.qospriv.qos_option);
}

void rtw_rust_debug_proc_emit_ht_option(void *sel, _adapter *adapter)
{
#ifdef CONFIG_80211N_HT
	RTW_PRINT_SEL(sel, "ht_option=%d\n", adapter->mlmepriv.htpriv.ht_option);
#endif
}

void rtw_rust_debug_proc_emit_rf_info(void *sel, _adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(sel, "cur_ch=%d, cur_bw=%d, cur_ch_offet=%d\n",
		      pmlmeext->cur_channel, pmlmeext->cur_bwmode,
		      pmlmeext->cur_ch_offset);
	RTW_PRINT_SEL(sel, "oper_ch=%d, oper_bw=%d, oper_ch_offet=%d\n",
		      rtw_get_oper_ch(adapter), rtw_get_oper_bw(adapter),
		      rtw_get_oper_choffset(adapter));
}

_adapter *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return (_adapter *)rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_proc_emit_defs_param(void *sel, _adapter *adapter)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;

	RTW_PRINT_SEL(sel, "%s %15s\n", "lmt_sta", "lmt_time");
	RTW_PRINT_SEL(sel, "%-15u %-15u\n", mlme->defs_lmt_sta,
		      mlme->defs_lmt_time);
}

ssize_t rtw_rust_debug_proc_set_defs_param(_adapter *adapter,
					   const char __user *buffer,
					   size_t count)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;
	char tmp[32];
	u32 defs_lmt_sta;
	u32 defs_lmt_time;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u", &defs_lmt_sta, &defs_lmt_time);

		if (num >= 1)
			mlme->defs_lmt_sta = defs_lmt_sta;
		if (num >= 2)
			mlme->defs_lmt_time = defs_lmt_time;
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_write_reg(_adapter *adapter,
					  const char __user *buffer,
					  size_t count)
{
	char tmp[32];
	u32 addr, val, len;

	if (count < 3) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%x %x %x", &addr, &val, &len);

		if (num != 3) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}

		switch (len) {
		case 1:
			rtw_write8(adapter, addr, (u8)val);
			break;
		case 2:
			rtw_write16(adapter, addr, (u16)val);
			break;
		case 4:
			rtw_write32(adapter, addr, val);
			break;
		default:
			RTW_INFO("error write length=%d", len);
			break;
		}
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_read_reg(const char __user *buffer,
					 size_t count,
					 u32 *addr_out,
					 u32 *len_out,
					 int *parsed_ok)
{
	char tmp[16];
	u32 addr, len;

	if (parsed_ok)
		*parsed_ok = 0;

	if (count < 2) {
		RTW_INFO("argument size is less than 2\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%x %x", &addr, &len);

		if (num != 2) {
			RTW_INFO("invalid read_reg parameter!\n");
			return count;
		}

		*addr_out = addr;
		*len_out = len;
		if (parsed_ok)
			*parsed_ok = 1;
	}

	return count;
}

void rtw_rust_debug_proc_emit_read_reg_uninit(void *sel)
{
	RTW_PRINT_SEL(sel, "address not initialized\n");
}

void rtw_rust_debug_proc_emit_read_reg(void *sel, u32 addr, u32 len, u32 val)
{
	switch (len) {
	case 1:
		RTW_PRINT_SEL(sel, "rtw_read8(0x%x)=0x%x\n", addr, val);
		break;
	case 2:
		RTW_PRINT_SEL(sel, "rtw_read16(0x%x)=0x%x\n", addr, val);
		break;
	case 4:
		RTW_PRINT_SEL(sel, "rtw_read32(0x%x)=0x%x\n", addr, val);
		break;
	default:
		RTW_PRINT_SEL(sel, "error read length=%d\n", len);
		break;
	}
}

void rtw_rust_debug_proc_emit_rx_stat(void *sel, _adapter *adapter)
{
	_irqL	 irqL;
	_list	*plist, *phead;
	struct sta_info *psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct sta_priv		*pstapriv = &(adapter->stapriv);
	u32 i, j;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			pstats = &psta->sta_stats;

			if (pstats == NULL)
				continue;
			if ((_rtw_memcmp(psta->cmn.mac_addr, bc_addr, ETH_ALEN) !=  _TRUE)
				&& (_rtw_memcmp(psta->cmn.mac_addr, null_addr, ETH_ALEN) != _TRUE)
				&& (_rtw_memcmp(psta->cmn.mac_addr, adapter_mac_addr(adapter), ETH_ALEN) != _TRUE)) {
				RTW_PRINT_SEL(sel, "MAC :\t\t"MAC_FMT "\n", MAC_ARG(psta->cmn.mac_addr));
				RTW_PRINT_SEL(sel, "data_rx_cnt :\t%llu\n", sta_rx_data_uc_pkts(psta) - pstats->last_rx_data_uc_pkts);
				pstats->last_rx_data_uc_pkts = sta_rx_data_uc_pkts(psta);
				RTW_PRINT_SEL(sel, "duplicate_cnt :\t%u\n", pstats->duplicate_cnt);
				pstats->duplicate_cnt = 0;
				RTW_PRINT_SEL(sel, "rx_per_rate_cnt :\n");

				for (j = 0; j < 0x60; j++) {
					RTW_PRINT_SEL(sel, "%08u  ", pstats->rxratecnt[j]);
					pstats->rxratecnt[j] = 0;
					if ((j%8) == 7)
						RTW_PRINT_SEL(sel, "\n");
				}
				RTW_PRINT_SEL(sel, "\n");
			}
		}
	}
	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
}

void rtw_rust_debug_proc_emit_tx_stat(void *sel, _adapter *adapter)
{
	_irqL	irqL;
	_list	*plist, *phead;
	struct sta_info *psta = NULL;
	u8 sta_mac[NUM_STA][ETH_ALEN] = {{0}};
	uint mac_id[NUM_STA];
	struct stainfo_stats	*pstats = NULL;
	struct sta_priv	*pstapriv = &(adapter->stapriv);
	struct sta_priv	*pstapriv_primary = &(GET_PRIMARY_ADAPTER(adapter))->stapriv;
	u32 i, macid_rec_idx = 0;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	struct submit_ctx gotc2h;

	if (!adapter->hal_func.reqtxrpt) {
		RTW_PRINT_SEL(sel, "Not support.\n");
		return;
	}

	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if ((_rtw_memcmp(psta->cmn.mac_addr, bc_addr, ETH_ALEN) !=  _TRUE)
				&& (_rtw_memcmp(psta->cmn.mac_addr, null_addr, ETH_ALEN) != _TRUE)
				&& (_rtw_memcmp(psta->cmn.mac_addr, adapter_mac_addr(adapter), ETH_ALEN) != _TRUE)) {
				_rtw_memcpy(&sta_mac[macid_rec_idx][0], psta->cmn.mac_addr, ETH_ALEN);
				mac_id[macid_rec_idx] = psta->cmn.mac_id;
				macid_rec_idx++;
			}
		}
	}
	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	for (i = 0; i < macid_rec_idx; i++) {
		_rtw_memcpy(pstapriv_primary->c2h_sta_mac, &sta_mac[i][0], ETH_ALEN);
		pstapriv_primary->c2h_adapter_id = adapter->iface_id;
		rtw_sctx_init(&gotc2h, 60);
		pstapriv_primary->gotc2h = &gotc2h;
		rtw_hal_reqtxrpt(adapter, mac_id[i]);
		if (rtw_sctx_wait(&gotc2h, __func__)) {
			psta = rtw_get_stainfo(pstapriv, &sta_mac[i][0]);
			if (psta) {
				pstats = &psta->sta_stats;
#if (!defined(ROKU_PRIVATE) && !defined(CONFIG_RTW_MULTI_AP))
				RTW_PRINT_SEL(sel, "data_sent_cnt :\t%u\n", pstats->tx_ok_cnt + pstats->tx_fail_cnt);
				RTW_PRINT_SEL(sel, "success_cnt :\t%u\n", pstats->tx_ok_cnt);
				RTW_PRINT_SEL(sel, "failure_cnt :\t%u\n", pstats->tx_fail_cnt);
				RTW_PRINT_SEL(sel, "retry_cnt :\t%u\n\n", pstats->tx_retry_cnt);
#else
				RTW_PRINT_SEL(sel, "MAC: " MAC_FMT " sent: %u fail: %u retry: %u\n",
					      MAC_ARG(&sta_mac[i][0]), pstats->tx_ok_cnt, pstats->tx_fail_cnt, pstats->tx_retry_cnt);
#endif /* ROKU_PRIVATE */
			} else
				RTW_PRINT_SEL(sel, "STA is gone\n");
		} else {
			pstapriv_primary->gotc2h = NULL;
			_rtw_memset(pstapriv_primary->c2h_sta_mac, 0, ETH_ALEN);
			pstapriv_primary->c2h_adapter_id = CONFIG_IFACE_NUMBER;
			RTW_PRINT_SEL(sel, "Warming : Query timeout, operation abort!!\n");
			break;
		}
		pstapriv_primary->gotc2h = NULL;
		_rtw_memset(pstapriv_primary->c2h_sta_mac, 0, ETH_ALEN);
		pstapriv_primary->c2h_adapter_id = CONFIG_IFACE_NUMBER;
	}
}

void rtw_rust_debug_proc_emit_scan_param(void *sel, _adapter *adapter)
{
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct ss_res *ss = &mlmeext->sitesurvey_res;

#define SCAN_PARAM_TITLE_FMT "%10s"
#define SCAN_PARAM_VALUE_FMT "%-10u"
#define SCAN_PARAM_TITLE_ARG , "scan_ch_ms"
#define SCAN_PARAM_VALUE_ARG , ss->scan_ch_ms
#ifdef CONFIG_80211N_HT
#define SCAN_PARAM_TITLE_FMT_HT " %15s %13s"
#define SCAN_PARAM_VALUE_FMT_HT " %-15u %-13u"
#define SCAN_PARAM_TITLE_ARG_HT , "rx_ampdu_accept", "rx_ampdu_size"
#define SCAN_PARAM_VALUE_ARG_HT , ss->rx_ampdu_accept, ss->rx_ampdu_size
#else
#define SCAN_PARAM_TITLE_FMT_HT ""
#define SCAN_PARAM_VALUE_FMT_HT ""
#define SCAN_PARAM_TITLE_ARG_HT
#define SCAN_PARAM_VALUE_ARG_HT
#endif
#ifdef CONFIG_SCAN_BACKOP
#define SCAN_PARAM_TITLE_FMT_BACKOP " %9s %12s"
#define SCAN_PARAM_VALUE_FMT_BACKOP " %-9u %-12u"
#define SCAN_PARAM_TITLE_ARG_BACKOP , "backop_ms", "scan_cnt_max"
#define SCAN_PARAM_VALUE_ARG_BACKOP , ss->backop_ms, ss->scan_cnt_max
#else
#define SCAN_PARAM_TITLE_FMT_BACKOP ""
#define SCAN_PARAM_VALUE_FMT_BACKOP ""
#define SCAN_PARAM_TITLE_ARG_BACKOP
#define SCAN_PARAM_VALUE_ARG_BACKOP
#endif

	RTW_PRINT_SEL(sel,
		      SCAN_PARAM_TITLE_FMT
		      SCAN_PARAM_TITLE_FMT_HT
		      SCAN_PARAM_TITLE_FMT_BACKOP
		      "\n"
		      SCAN_PARAM_TITLE_ARG
		      SCAN_PARAM_TITLE_ARG_HT
		      SCAN_PARAM_TITLE_ARG_BACKOP
	);

	RTW_PRINT_SEL(sel,
		      SCAN_PARAM_VALUE_FMT
		      SCAN_PARAM_VALUE_FMT_HT
		      SCAN_PARAM_VALUE_FMT_BACKOP
		      "\n"
		      SCAN_PARAM_VALUE_ARG
		      SCAN_PARAM_VALUE_ARG_HT
		      SCAN_PARAM_VALUE_ARG_BACKOP
	);
}

ssize_t rtw_rust_debug_proc_set_scan_param(_adapter *adapter,
					   const char __user *buffer,
					   size_t count)
{
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct ss_res *ss = &mlmeext->sitesurvey_res;

	char tmp[32] = {0};

	u16 scan_ch_ms;
#define SCAN_PARAM_INPUT_FMT "%hu"
#define SCAN_PARAM_INPUT_ARG , &scan_ch_ms
#ifdef CONFIG_80211N_HT
	u8 rx_ampdu_accept;
	u8 rx_ampdu_size;
#define SCAN_PARAM_INPUT_FMT_HT " %hhu %hhu"
#define SCAN_PARAM_INPUT_ARG_HT , &rx_ampdu_accept, &rx_ampdu_size
#else
#define SCAN_PARAM_INPUT_FMT_HT ""
#define SCAN_PARAM_INPUT_ARG_HT
#endif
#ifdef CONFIG_SCAN_BACKOP
	u16 backop_ms;
	u8 scan_cnt_max;
#define SCAN_PARAM_INPUT_FMT_BACKOP " %hu %hhu"
#define SCAN_PARAM_INPUT_ARG_BACKOP , &backop_ms, &scan_cnt_max
#else
#define SCAN_PARAM_INPUT_FMT_BACKOP ""
#define SCAN_PARAM_INPUT_ARG_BACKOP
#endif

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp,
				 SCAN_PARAM_INPUT_FMT
				 SCAN_PARAM_INPUT_FMT_HT
				 SCAN_PARAM_INPUT_FMT_BACKOP
				 SCAN_PARAM_INPUT_ARG
				 SCAN_PARAM_INPUT_ARG_HT
				 SCAN_PARAM_INPUT_ARG_BACKOP
		);

		if (num-- > 0)
			ss->scan_ch_ms = scan_ch_ms;
#ifdef CONFIG_80211N_HT
		if (num-- > 0)
			ss->rx_ampdu_accept = rx_ampdu_accept;
		if (num-- > 0)
			ss->rx_ampdu_size = rx_ampdu_size;
#endif
#ifdef CONFIG_SCAN_BACKOP
		if (num-- > 0)
			ss->backop_ms = backop_ms;
		if (num-- > 0)
			ss->scan_cnt_max = scan_cnt_max;
#endif
	}

	return count;
}

void rtw_rust_debug_proc_emit_scan_abort(void *sel, _adapter *adapter)
{
	u32 pass_ms;

	pass_ms = rtw_scan_abort_timeout(adapter, 10000);

	RTW_PRINT_SEL(sel, "%u\n", pass_ms);
}

#ifdef CONFIG_LAYER2_ROAMING
void rtw_rust_debug_proc_emit_roam_flags(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "0x%02x\n", rtw_roam_flags(adapter));
}

void rtw_rust_debug_proc_emit_roam_param(void *sel, _adapter *adapter)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;

	RTW_PRINT_SEL(sel, "%12s %15s %26s %16s\n", "rssi_diff_th", "scanr_exp_ms",
		      "scan_interval(unit:2 sec)", "rssi_threshold");
	RTW_PRINT_SEL(sel, "%-15u %-13u %-27u %-11u\n",
		      mlme->roam_rssi_diff_th, mlme->roam_scanr_exp_ms,
		      mlme->roam_scan_int, mlme->roam_rssi_threshold);
}

ssize_t rtw_rust_debug_proc_set_roam_flags(_adapter *adapter,
					   const char __user *buffer,
					   size_t count)
{
	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &flags);

		if (num == 1)
			rtw_assign_roam_flags(adapter, flags);
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_roam_param(_adapter *adapter,
					   const char __user *buffer,
					   size_t count)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;
	char tmp[32];
	u8 rssi_diff_th;
	u32 scanr_exp_ms;
	u32 scan_int;
	u8 rssi_threshold;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhu %u %u %hhu", &rssi_diff_th,
				 &scanr_exp_ms, &scan_int, &rssi_threshold);

		if (num >= 1)
			mlme->roam_rssi_diff_th = rssi_diff_th;
		if (num >= 2)
			mlme->roam_scanr_exp_ms = scanr_exp_ms;
		if (num >= 3)
			mlme->roam_scan_int = scan_int;
		if (num >= 4)
			mlme->roam_rssi_threshold = rssi_threshold;
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_roam_tgt_addr(_adapter *adapter,
					      const char __user *buffer,
					      size_t count)
{
	char tmp[32];
	u8 addr[ETH_ALEN];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", addr,
				 addr + 1, addr + 2, addr + 3, addr + 4,
				 addr + 5);

		if (num == 6)
			_rtw_memcpy(adapter->mlmepriv.roam_tgt_addr, addr,
				    ETH_ALEN);

		RTW_INFO("set roam_tgt_addr to "MAC_FMT"\n",
			 MAC_ARG(adapter->mlmepriv.roam_tgt_addr));
	}

	return count;
}
#endif /* CONFIG_LAYER2_ROAMING */

void rtw_rust_debug_proc_emit_sec_info(void *sel, _adapter *adapter)
{
	struct security_priv *sec = &adapter->securitypriv;

	RTW_PRINT_SEL(sel, "auth_alg=0x%x, enc_alg=0x%x, auth_type=0x%x, enc_type=0x%x\n",
		      sec->dot11AuthAlgrthm, sec->dot11PrivacyAlgrthm,
		      sec->ndisauthtype, sec->ndisencryptstatus);
	RTW_PRINT_SEL(sel, "hw_decrypted=%d\n", sec->hw_decrypted);

#ifdef DBG_SW_SEC_CNT
	RTW_PRINT_SEL(sel, "wep_sw_enc_cnt=%llu, %llu, %llu\n",
		      sec->wep_sw_enc_cnt_bc, sec->wep_sw_enc_cnt_mc,
		      sec->wep_sw_enc_cnt_uc);
	RTW_PRINT_SEL(sel, "wep_sw_dec_cnt=%llu, %llu, %llu\n",
		      sec->wep_sw_dec_cnt_bc, sec->wep_sw_dec_cnt_mc,
		      sec->wep_sw_dec_cnt_uc);
	RTW_PRINT_SEL(sel, "tkip_sw_enc_cnt=%llu, %llu, %llu\n",
		      sec->tkip_sw_enc_cnt_bc, sec->tkip_sw_enc_cnt_mc,
		      sec->tkip_sw_enc_cnt_uc);
	RTW_PRINT_SEL(sel, "tkip_sw_dec_cnt=%llu, %llu, %llu\n",
		      sec->tkip_sw_dec_cnt_bc, sec->tkip_sw_dec_cnt_mc,
		      sec->tkip_sw_dec_cnt_uc);
	RTW_PRINT_SEL(sel, "aes_sw_enc_cnt=%llu, %llu, %llu\n",
		      sec->aes_sw_enc_cnt_bc, sec->aes_sw_enc_cnt_mc,
		      sec->aes_sw_enc_cnt_uc);
	RTW_PRINT_SEL(sel, "aes_sw_dec_cnt=%llu, %llu, %llu\n",
		      sec->aes_sw_dec_cnt_bc, sec->aes_sw_dec_cnt_mc,
		      sec->aes_sw_dec_cnt_uc);
	RTW_PRINT_SEL(sel, "gcmp_sw_enc_cnt=%llu, %llu, %llu\n",
		      sec->gcmp_sw_enc_cnt_bc, sec->gcmp_sw_enc_cnt_mc,
		      sec->gcmp_sw_enc_cnt_uc);
	RTW_PRINT_SEL(sel, "gcmp_sw_dec_cnt=%llu, %llu, %llu\n",
		      sec->gcmp_sw_dec_cnt_bc, sec->gcmp_sw_dec_cnt_mc,
		      sec->gcmp_sw_dec_cnt_uc);
#endif /* DBG_SW_SEC_CNT */
}

#ifdef CONFIG_WAR_OFFLOAD
void rtw_rust_debug_proc_emit_war_offload_enable(void *sel, _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);

	if (_TRUE == pwrpriv->wowlan_war_offload_mode) {
		RTW_PRINT_SEL(sel, "\n[ Offload Feature Enabled ]\n");

		if (WAR_ARP_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n ARP Reponse offload enabled\n");
		}
#ifdef CONFIG_OFFLOAD_MDNS_V4
		if (WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n MDNS v4 Reponse offload enabled\n");
		}
		if (WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n MDNS v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_MDNS_v4 */
#ifdef CONFIG_OFFLOAD_MDNS_V6
		if (WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n MDNS v6 Reponse offload enabled\n");
		}
		if (WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n MDNS v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_MDNS_V6 */

		if (WAR_ARP_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) {
			RTW_PRINT_SEL(sel, "\n ARP Request wakeup enabled\n");
		}

	} else {
		RTW_PRINT_SEL(sel, "\n[ Offload Feature Disabled ]\n");
	}
}

ssize_t rtw_rust_debug_proc_set_war_offload_enable(_adapter *adapter,
						   const char __user *buffer,
						   size_t count)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	char tmp[32];
	u32 offload_cfg = 0;

	if (NULL == buffer) {
		RTW_INFO(FUNC_ADPT_FMT ": input buffer is NULL!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is 0!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is too large\n",
			 FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%x", &offload_cfg);

		if (num == 1) {
			RTW_INFO(FUNC_ADPT_FMT ": Set war offload cfg = %x\n",
				 FUNC_ADPT_ARG(adapter), offload_cfg);
			pwrpriv->wowlan_war_offload_ctrl = offload_cfg;
			pwrpriv->wowlan_war_offload_mode = offload_cfg ? _TRUE : _FALSE;

		}
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_war_offload_ipv4_addr(_adapter *adapter,
						      const char __user *buffer,
						      size_t count)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	char tmp[128];
	u32 ip_addr = 0, ip_subnet = 0, ip_gateway = 0, index = 0;
	struct war_ipv4_fmt *pip_info = &pwrpriv->wowlan_war_offload_ipv4;

	if (NULL == buffer) {
		RTW_INFO(FUNC_ADPT_FMT ": input buffer is NULL!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is 0!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is too large\n",
			 FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d %x %x %x", &index, &ip_addr,
				 &ip_subnet, &ip_gateway);

		if (num == 4) {
			pip_info->ip_addr[index - 1] = ip_addr;
			pip_info->ip_subnet[index - 1] = ip_subnet;
			pip_info->ip_gateway[index - 1] = ip_gateway;
			RTW_INFO(FUNC_ADPT_FMT "Setup IPv4 address:\n",
				 FUNC_ADPT_ARG(adapter));
			RTW_INFO("Index(%d) IP=%d.%d.%d.%d\n", index,
				 (ip_addr & 0xff), ((ip_addr & 0xff00) >> 8),
				 ((ip_addr & 0xff0000) >> 16),
				 ((ip_addr & 0xff000000) >> 24));
		} else {
			RTW_INFO("Wrong input buffer count (%d)\n", num);
			return -EFAULT;
		}
	}

	return count;
}

ssize_t rtw_rust_debug_proc_set_war_offload_ipv6_addr(_adapter *adapter,
						      const char __user *buffer,
						      size_t count)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	char tmp[255];
	struct war_ipv6_fmt *pip_info = &pwrpriv->wowlan_war_offload_ipv6;

	if (NULL == buffer) {
		RTW_INFO(FUNC_ADPT_FMT ": input buffer is NULL!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is 0!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is too large\n",
			 FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num;
		int i;
		u32 index;
		u16 val[8];
		u16 big_endian_val[8];

		num = sscanf(tmp, "%d %hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx",
			     &index, &val[0], &val[1], &val[2], &val[3],
			     &val[4], &val[5], &val[6], &val[7]);
		for (i = 0; i < 8; i++)
			big_endian_val[i] = htons(val[i]);
		_rtw_memcpy(pip_info->ipv6_addr[index - 1], big_endian_val,
			     RTW_IPv6_ADDR_LEN);

		if (num == 9) {
			RTW_INFO(FUNC_ADPT_FMT "Setup IPv6 address\n",
				 FUNC_ADPT_ARG(adapter));
		} else {
			RTW_INFO("Wrong input count (%d)\n", num);
			return -EFAULT;
		}
	}

	return count;
}

#if defined(CONFIG_OFFLOAD_MDNS_V4) || defined(CONFIG_OFFLOAD_MDNS_V6)
void rtw_rust_debug_proc_emit_war_offload_mdns_domain_name(void *sel,
							   _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);

	if (_TRUE == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			RTW_PRINT_SEL(sel, "\nDomain Name:[%s](%d)\n\n",
				      pwrpriv->wowlan_war_offload_mdns_domain_name,
				      pwrpriv->wowlan_war_offload_mdns_domain_name_len);
		} else {
			RTW_PRINT_SEL(sel, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		RTW_PRINT_SEL(sel, "\nOffload Not enabled\n\n");
	}
}

ssize_t rtw_rust_debug_proc_set_war_offload_mdns_domain_name(
	_adapter *adapter, const char __user *buffer, size_t count)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	char tmp[MAX_MDNS_DOMAIN_NAME_LEN + 1];
	char domain_name[MAX_MDNS_DOMAIN_NAME_LEN + 1];

	if (NULL == buffer) {
		RTW_INFO(FUNC_ADPT_FMT ": input buffer is NULL!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is 0!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO(FUNC_ADPT_FMT
			 ": input length is large than MAX_MDNS_DOMAIN_NAME_LEN(%d)\n",
			 FUNC_ADPT_ARG(adapter), MAX_MDNS_DOMAIN_NAME_LEN);
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%s", domain_name);

		if (1 == num) {
			pwrpriv->wowlan_war_offload_mdns_domain_name_len =
				strlen(domain_name);
			_rtw_memset(pwrpriv->wowlan_war_offload_mdns_domain_name,
				    0x00, MAX_MDNS_DOMAIN_NAME_LEN);
			_rtw_memcpy(pwrpriv->wowlan_war_offload_mdns_domain_name,
				    domain_name, strlen(domain_name));
		}
	}

	return count;
}

void rtw_rust_debug_proc_emit_war_offload_mdns_machine_name(void *sel,
							    _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	int i = 0;

	if (_TRUE == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
		    (WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			for (i = 0; i < pwrpriv->wowlan_war_offload_mdns_mnane_num;
			     i++) {
				RTW_PRINT_SEL(sel, "[%d]", i);
				rtw_wow_war_mdns_dump_buf(
					sel, (u8 *)"Machine Name",
					pwrpriv->wowlan_war_offload_mdns_mnane[i]
						.name,
					pwrpriv->wowlan_war_offload_mdns_mnane[i]
						.name_len);
			}
			RTW_PRINT_SEL(sel, "\n");
		} else {
			RTW_PRINT_SEL(sel, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		RTW_PRINT_SEL(sel, "\nOffload Not enabled\n\n");
	}
}

ssize_t rtw_rust_debug_proc_set_war_offload_mdns_machine_name(
	_adapter *adapter, const char __user *buffer, size_t count)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
	char tmp[MAX_MDNS_MACHINE_NAME_LEN * 3 - 1 + 1];

	if (NULL == buffer) {
		RTW_INFO(FUNC_ADPT_FMT ": input buffer is NULL!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO(FUNC_ADPT_FMT ": input length is 0!\n",
			 FUNC_ADPT_ARG(adapter));
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO(FUNC_ADPT_FMT
			 ": input length, %lu, is large than MAX_MDNS_MACHINE_NAME_LEN(%d)\n",
			 FUNC_ADPT_ARG(adapter), (count + 1) / 3,
			 MAX_MDNS_MACHINE_NAME_LEN);
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (strncmp(tmp, "clean", 5) == 0) {
			_rtw_memset(pwrpriv->wowlan_war_offload_mdns_mnane, 0,
				    sizeof(pwrpriv->wowlan_war_offload_mdns_mnane));
			pwrpriv->wowlan_war_offload_mdns_mnane_num = 0;
		} else {
			int idx = pwrpriv->wowlan_war_offload_mdns_mnane_num;

			if (idx == MAX_MDNS_MACHINE_NAME_NUM) {
				RTW_INFO(FUNC_ADPT_FMT
					 ": the num of machine name is already %d(MAX_MDNS_MACHINE_NAME_NUM)!\n",
					 FUNC_ADPT_ARG(adapter),
					 MAX_MDNS_MACHINE_NAME_NUM);
				return -EFAULT;
			}
			if (rtw_wow_war_mdns_parser_pattern(
				    tmp,
				    pwrpriv->wowlan_war_offload_mdns_mnane[idx]
					    .name,
				    (u32 *)&pwrpriv
					    ->wowlan_war_offload_mdns_mnane[idx]
					    .name_len,
				    MAX_MDNS_MACHINE_NAME_LEN))
				pwrpriv->wowlan_war_offload_mdns_mnane_num++;
		}
	}

	return count;
}
#endif /* CONFIG_OFFLOAD_MDNS_V4 || CONFIG_OFFLOAD_MDNS_V6 */
#endif /* CONFIG_WAR_OFFLOAD */

#ifdef CONFIG_RTW_REPEATER_SON
void rtw_rust_debug_proc_emit_rson_data(void *sel, _adapter *adapter)
{
	char rson_data_str[256];

	rtw_rson_get_property_str(adapter, rson_data_str);
	RTW_PRINT_SEL(sel, "%s\n", rson_data_str);
}

ssize_t rtw_rust_debug_proc_set_rson_data(_adapter *adapter,
					  const char __user *buffer,
					  size_t count)
{
	char tmp[64] = {0};
	int num;
	u8 field[10], value[64];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num = sscanf(tmp, "%s %s", field, value);
		if (num != 2) {
			RTW_INFO("Invalid format : echo <field> <value> > son_data\n");
			return count;
		}
		RTW_INFO("field=%s  value=%s\n", field, value);
		num = rtw_rson_set_property(adapter, field, value);
		if (num != 1) {
			RTW_INFO("Invalid field(%s) or value(%s)\n", field,
				 value);
			return count;
		}
	}
	return count;
}
#endif /* CONFIG_RTW_REPEATER_SON */

void rtw_rust_debug_proc_emit_survey_info(void *sel, _adapter *padapter)
{
	_irqL irqL;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	_queue *queue = &(pmlmepriv->scanned_queue);
	struct wlan_network *pnetwork = NULL;
	_list *plist, *phead;
	s32 notify_signal;
	s16 notify_noise = 0;
	u16 index = 0, ie_cap = 0;
	unsigned char *ie_wpa = NULL, *ie_wpa2 = NULL, *ie_wps = NULL;
	unsigned char *ie_p2p = NULL;
	char flag_str[64];
	int ielen = 0;
	u32 wpsielen = 0;
#ifdef CONFIG_RTW_MESH
	const char *ssid_title_str = "ssid/mesh_id";
#else
	const char *ssid_title_str = "ssid";
#endif
	u8 rsni = 255;

	_enter_critical_bh(&(pmlmepriv->scanned_queue.lock), &irqL);
	phead = get_list_head(queue);
	if (!phead)
		goto survey_exit;
	plist = get_next(phead);
	if (!plist)
		goto survey_exit;

#ifdef CONFIG_RTW_REPEATER_SON
	rtw_rson_show_survey_info(sel, plist, phead);
#else

	RTW_PRINT_SEL(sel, "%5s  %-17s  %3s  %-3s  %-4s  %-4s  %4s  %5s  %32s  %32s \n",
		      "index", "bssid", "ch", "RSSI", "SdBm", "Noise", "rsni", "age",
		      "flag", ssid_title_str);
	while (1) {
		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			break;

		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);
		if (!pnetwork)
			break;

		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE &&
		    is_same_network(&pmlmepriv->cur_network.network,
				    &pnetwork->network, 0)) {
			notify_signal = translate_percentage_to_dbm(
				padapter->recvpriv.signal_strength);
		} else {
			notify_signal = translate_percentage_to_dbm(
				pnetwork->network.PhyInfo.SignalStrength);
		}

#ifdef CONFIG_BACKGROUND_NOISE_MONITOR
		if (IS_NM_ENABLE(padapter))
			notify_noise = rtw_noise_query_by_chan_num(
				padapter, pnetwork->network.Configuration.DSConfig);
#endif
#ifdef CONFIG_RTW_ACS
		rsni = rtw_acs_get_rsni(padapter, (int)(pnetwork->network.Rssi),
					pnetwork->network.Configuration.DSConfig);
#endif

		ie_wpa = rtw_get_wpa_ie(&pnetwork->network.IEs[12], &ielen,
					pnetwork->network.IELength - 12);
		ie_wpa2 = rtw_get_wpa2_ie(&pnetwork->network.IEs[12], &ielen,
					  pnetwork->network.IELength - 12);
		ie_cap = rtw_get_capability(&pnetwork->network);
		ie_wps = rtw_get_wps_ie(&pnetwork->network.IEs[12],
					pnetwork->network.IELength - 12, NULL,
					&wpsielen);
#ifdef CONFIG_P2P
		ie_p2p = rtw_get_p2p_ie(&pnetwork->network.IEs[12],
					pnetwork->network.IELength - 12, NULL,
					&ielen);
#endif
		sprintf(flag_str, "%s%s%s%s%s%s%s",
			(ie_wpa) ? "[WPA]" : "",
			(ie_wpa2) ? "[WPA2]" : "",
			(!ie_wpa && !ie_wpa && ie_cap & BIT(4)) ? "[WEP]" : "",
			(ie_wps) ? "[WPS]" : "",
			(pnetwork->network.InfrastructureMode == Ndis802_11IBSS) ?
				"[IBSS]" :
			(pnetwork->network.InfrastructureMode == Ndis802_11_mesh) ?
				"[MESH]" : "",
			(ie_cap & BIT(0)) ? "[ESS]" : "",
			(ie_p2p) ? "[P2P]" : "");
		RTW_PRINT_SEL(sel, "%5d  " MAC_FMT "  %3d  %4d  %4d  %4d  %4d   %5d  %32s  %32s\n",
			      ++index,
			      MAC_ARG(pnetwork->network.MacAddress),
			      pnetwork->network.Configuration.DSConfig,
			      (int)pnetwork->network.Rssi,
			      notify_signal,
			      notify_noise,
			      rsni,
			      rtw_get_passing_time_ms(pnetwork->last_scanned),
			      flag_str,
			      pnetwork->network.InfrastructureMode == Ndis802_11_mesh ?
				      pnetwork->network.mesh_id.Ssid :
				      pnetwork->network.Ssid.Ssid);
		plist = get_next(plist);
	}
#endif
survey_exit:
	_exit_critical_bh(&(pmlmepriv->scanned_queue.lock), &irqL);
}

ssize_t rtw_rust_debug_proc_set_survey_info(_adapter *padapter,
					    const char __user *buffer,
					    size_t count)
{
	u8 _status = _FALSE;
	u8 ssc_chk;
	char tmp[32] = {0};
	char cmd[8] = {0};
	bool acs = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%s", cmd);

		if (num < 1)
			return count;

		if (strcmp("acs", cmd) == 0)
			acs = 1;
	}

	ssc_chk = rtw_sitesurvey_condition_check(padapter, _FALSE);
	if (ssc_chk != SS_ALLOW)
		goto survey_set_exit;

	rtw_ps_deny(padapter, PS_DENY_SCAN);
	if (_FAIL == rtw_pwr_wakeup(padapter))
		goto survey_cancel_ps_deny;
	if (!rtw_is_adapter_up(padapter)) {
		RTW_INFO("scan abort!! adapter cannot use\n");
		goto survey_cancel_ps_deny;
	}

	if (acs) {
#ifdef CONFIG_RTW_ACS
		_status = rtw_set_acs_sitesurvey(padapter);
#endif
	} else
		_status = rtw_set_802_11_bssid_list_scan(padapter, NULL);

survey_cancel_ps_deny:
	rtw_ps_deny_cancel(padapter, PS_DENY_SCAN);
survey_set_exit:
	return count;
}

#endif /* CONFIG_PROC_DEBUG */

#endif /* CONFIG_RUST && CONFIG_RUST_RTW_DEBUG */
