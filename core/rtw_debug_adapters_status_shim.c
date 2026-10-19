// SPDX-License-Identifier: GPL-2.0
/* W3-144: C dump body for rust/rtw_debug_adapters_status.rs (CONFIG_RUST_RTW_DEBUG). */

#include <drv_types.h>

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_RTW_DEBUG)

void rtw_rust_debug_adapters_dump_body(void *sel, struct dvobj_priv *dvobj)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	int i;
	_adapter *iface;
	u8 u_ch, u_bw, u_offset;
#if (defined(CONFIG_SUPPORT_MULTI_BCN) && defined(CONFIG_FW_HANDLE_TXBCN)) || defined(CONFIG_CLIENT_PORT_CFG)
	char str_val[64] = {'\0'};
#endif

#if defined(CONFIG_SUPPORT_MULTI_BCN) && defined(CONFIG_FW_HANDLE_TXBCN)
	RTW_PRINT_SEL(sel, "[AP] LIMITED_AP_NUM:%d\n", CONFIG_LIMITED_AP_NUM);
	RTW_PRINT_SEL(sel, "[AP] vap_map:0x%02x\n", dvobj->vap_map);
#endif
#ifdef CONFIG_HW_P0_TSF_SYNC
	RTW_PRINT_SEL(sel, "[AP] p0 tsf sync port = %d\n", dvobj->p0_tsf.sync_port);
	RTW_PRINT_SEL(sel, "[AP] p0 tsf timer offset = %d\n", dvobj->p0_tsf.offset);
#endif
#ifdef CONFIG_CLIENT_PORT_CFG
	RTW_PRINT_SEL(sel, "[CLT] clt_num = %d\n", dvobj->clt_port.num);
	RTW_PRINT_SEL(sel, "[CLT] clt_map = 0x%02x\n", dvobj->clt_port.bmp);
#endif
#ifdef CONFIG_FW_MULTI_PORT_SUPPORT
	RTW_PRINT_SEL(sel, "[MI] default port id:%d\n\n", dvobj->dft.port_id);
#endif

	RTW_PRINT_SEL(sel, "dev status:%s%s\n\n"
		, dev_is_surprise_removed(dvobj) ? " SR" : ""
		, dev_is_drv_stopped(dvobj) ? " DS" : ""
	);

#ifdef CONFIG_P2P
#define P2P_INFO_TITLE_FMT	" %-3s %-4s"
#define P2P_INFO_TITLE_ARG	, "lch", "p2ps"
#ifdef CONFIG_IOCTL_CFG80211
#define P2P_INFO_VALUE_FMT	" %3u %c%3u"
#define P2P_INFO_VALUE_ARG	, iface->wdinfo.listen_channel, iface->wdev_data.p2p_enabled ? 'e' : ' ', rtw_p2p_state(&iface->wdinfo)
#else
#define P2P_INFO_VALUE_FMT	" %3u %4u"
#define P2P_INFO_VALUE_ARG	, iface->wdinfo.listen_channel, rtw_p2p_state(&iface->wdinfo)
#endif
#define P2P_INFO_DASH		"---------"
#else
#define P2P_INFO_TITLE_FMT	""
#define P2P_INFO_TITLE_ARG
#define P2P_INFO_VALUE_FMT	""
#define P2P_INFO_VALUE_ARG
#define P2P_INFO_DASH
#endif

#if (defined(CONFIG_SUPPORT_MULTI_BCN) && defined(CONFIG_FW_HANDLE_TXBCN)) || defined(CONFIG_CLIENT_PORT_CFG)
#define INFO_FMT	" %-4s"
#define INFO_ARG	, "info"
#define INFO_CNT_FMT	" %-20s"
#define INFO_CNT_ARG	, str_val
#else
#define INFO_FMT	""
#define INFO_ARG
#define INFO_CNT_FMT	""
#define INFO_CNT_ARG
#endif

	RTW_PRINT_SEL(sel, "%-2s %-15s %c %-3s %-3s %-3s %-17s %-4s %-7s %-5s"
		P2P_INFO_TITLE_FMT
		" %s"INFO_FMT"\n"
		, "id", "ifname", ' ', "bup", "nup", "ncd", "macaddr", "port", "ch", "class"
		P2P_INFO_TITLE_ARG
		, "status"INFO_ARG);

	RTW_PRINT_SEL(sel, "---------------------------------------------------------------"
		P2P_INFO_DASH
		"-------\n");

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			#if (defined(CONFIG_SUPPORT_MULTI_BCN) && defined(CONFIG_FW_HANDLE_TXBCN)) || defined(CONFIG_CLIENT_PORT_CFG)
			_rtw_memset(&str_val, '\0', sizeof(str_val));
			#endif
			#if defined(CONFIG_AP_MODE) && defined(CONFIG_SUPPORT_MULTI_BCN) && defined(CONFIG_FW_HANDLE_TXBCN)
			if (MLME_IS_AP(iface) || MLME_IS_MESH(iface)) {
				u8 len;
				char *p = str_val;
				char tmp_str[10] = {'\0'};

				len = snprintf(tmp_str, sizeof(tmp_str), "%s", "ap_id:");
				strncpy(p, tmp_str, len);
				p += len;
				_rtw_memset(&tmp_str, '\0', sizeof(tmp_str));
				#ifdef DBG_HW_PORT
				len = snprintf(tmp_str, sizeof(tmp_str), "%d (%d,%d)", iface->vap_id, iface->hw_port, iface->client_port);
				#else
				len = snprintf(tmp_str, sizeof(tmp_str), "%d", iface->vap_id);
				#endif
				strncpy(p, tmp_str, len);
			}
			#endif
			#ifdef CONFIG_CLIENT_PORT_CFG
			if (MLME_IS_STA(iface)) {
				u8 len;
				char *p = str_val;
				char tmp_str[10] = {'\0'};

				len = snprintf(tmp_str, sizeof(tmp_str), "%s", "c_pid:");
				strncpy(p, tmp_str, len);
				p += len;
				_rtw_memset(&tmp_str, '\0', sizeof(tmp_str));
				#ifdef DBG_HW_PORT
				len = snprintf(tmp_str, sizeof(tmp_str), "%d (%d,%d)", iface->client_port, iface->hw_port, iface->client_port);
				#else
				len = snprintf(tmp_str, sizeof(tmp_str), "%d", iface->client_port);
				#endif
				strncpy(p, tmp_str, len);
			}
			#endif

			RTW_PRINT_SEL(sel, "%2d %-15s %c %3u %3u %3u "MAC_FMT" %4hhu %3u,%u,%u %5u"
				P2P_INFO_VALUE_FMT
				" "MLME_STATE_FMT" " INFO_CNT_FMT"\n"
				, i, iface->registered ? ADPT_ARG(iface) : NULL
				, iface->registered ? 'R' : ' '
				, iface->bup
				, iface->netif_up
				, iface->net_closed
				, MAC_ARG(adapter_mac_addr(iface))
				, rtw_hal_get_port(iface)
				, iface->mlmeextpriv.cur_channel
				, iface->mlmeextpriv.cur_bwmode
				, iface->mlmeextpriv.cur_ch_offset
				, rtw_get_op_class_by_chbw(iface->mlmeextpriv.cur_channel
					, iface->mlmeextpriv.cur_bwmode
					, iface->mlmeextpriv.cur_ch_offset)
				P2P_INFO_VALUE_ARG
				, MLME_STATE_ARG(iface)
				INFO_CNT_ARG
			);
		}
	}

	RTW_PRINT_SEL(sel, "---------------------------------------------------------------"
		P2P_INFO_DASH
		"-------\n");

	if (rtw_mi_get_ch_setting_union(dvobj_get_primary_adapter(dvobj), &u_ch, &u_bw, &u_offset))
		RTW_PRINT_SEL(sel, "%55s %3u,%u,%u %5u\n"
			, "union:"
			, u_ch, u_bw, u_offset, rtw_get_op_class_by_chbw(u_ch, u_bw, u_offset));

	RTW_PRINT_SEL(sel, "%55s %3u,%u,%u offch_state:%d\n"
		, "oper:"
		, dvobj->oper_channel
		, dvobj->oper_bwmode
		, dvobj->oper_ch_offset
		, rfctl->offch_state
	);

#ifdef CONFIG_DFS_MASTER
	if (rfctl->radar_detect_ch != 0) {
		RTW_PRINT_SEL(sel, "%55s %3u,%u,%u"
			, "radar_detect:"
			, rfctl->radar_detect_ch
			, rfctl->radar_detect_bw
			, rfctl->radar_detect_offset
		);

		if (rfctl->radar_detect_by_others)
			_RTW_PRINT_SEL(sel, ", by AP of STA link");
		else {
			u32 non_ocp_ms;
			u32 cac_ms;
			u8 dfs_domain = rtw_rfctl_get_dfs_domain(rfctl);

			_RTW_PRINT_SEL(sel, ", domain:%s(%u)", rtw_dfs_regd_str(dfs_domain), dfs_domain);

			rtw_get_ch_waiting_ms(rfctl
				, rfctl->radar_detect_ch
				, rfctl->radar_detect_bw
				, rfctl->radar_detect_offset
				, &non_ocp_ms
				, &cac_ms
			);

			if (non_ocp_ms)
				_RTW_PRINT_SEL(sel, ", non_ocp:%d", non_ocp_ms);
			if (cac_ms)
				_RTW_PRINT_SEL(sel, ", cac:%d", cac_ms);
		}

		_RTW_PRINT_SEL(sel, "\n");
	}
#endif
}

#endif /* CONFIG_RUST && CONFIG_RUST_RTW_DEBUG */
