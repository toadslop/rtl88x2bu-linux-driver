// SPDX-License-Identifier: GPL-2.0
/* W3-191: C implementations for rust/rtw_beamforming_cmd.rs kernel exports. */

#include <drv_types.h>
#include <hal_data.h>

#ifdef CONFIG_RUST

static void _rust_c2h_snd_txbf(PADAPTER adapter, u8 *buf, u8 buf_len)
{
	struct beamforming_info *info;
	u8 res;

	(void)buf_len;
	info = GET_BEAMFORM_INFO(adapter);
	_cancel_timer_ex(&info->sounding_timeout_timer);
	res = C2H_SND_TXBF_GET_SND_RESULT(buf) ? _TRUE : _FALSE;
	rtw_bf_cmd(adapter, BEAMFORMING_CTRL_END_PERIOD, &res, 1, 1);
}

u8 rtw_rust_bf_cmd_impl(PADAPTER adapter, s32 type, u8 *pbuf, s32 size, u8 enqueue)
{
	struct cmd_obj *ph2c;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &adapter->cmdpriv;
	u8 *wk_buf;
	u8 res = _SUCCESS;

	if (!enqueue) {
		rtw_bf_cmd_hdl(adapter, type, pbuf);
		goto exit;
	}

	ph2c = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (ph2c == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)ph2c, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (pbuf != NULL) {
		wk_buf = rtw_zmalloc(size);
		if (wk_buf == NULL) {
			rtw_mfree((u8 *)ph2c, sizeof(struct cmd_obj));
			rtw_mfree((u8 *)pdrvextra_cmd_parm, sizeof(struct drvextra_cmd_parm));
			res = _FAIL;
			goto exit;
		}
		_rtw_memcpy(wk_buf, pbuf, size);
	} else {
		wk_buf = NULL;
		size = 0;
	}

	pdrvextra_cmd_parm->ec_id = BEAMFORMING_WK_CID;
	pdrvextra_cmd_parm->type = type;
	pdrvextra_cmd_parm->size = size;
	pdrvextra_cmd_parm->pbuf = wk_buf;

	init_h2fwcmd_w_parm_no_rsp(ph2c, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);
	res = rtw_enqueue_cmd(pcmdpriv, ph2c);

exit:
	return res;
}

void rtw_rust_bf_update_attrib_impl(PADAPTER adapter, struct pkt_attrib *attrib, struct sta_info *sta)
{
	(void)adapter;
	if (sta) {
		attrib->txbf_g_id = sta->cmn.bf_info.g_id;
		attrib->txbf_p_aid = sta->cmn.bf_info.p_aid;
	}
}

void rtw_rust_bf_c2h_handler_impl(PADAPTER adapter, u8 id, u8 *buf, u8 buf_len)
{
	switch (id) {
	case CMD_ID_C2H_SND_TXBF:
		_rust_c2h_snd_txbf(adapter, buf, buf_len);
		break;
	}
}

#define toMbps(bytes, secs) (rtw_division64(bytes >> 17, secs))

void rtw_rust_bf_update_traffic_impl(PADAPTER adapter)
{
	struct beamforming_info *info;
	struct beamformee_entry *bfee;
	struct sta_info *sta;
	u8 bfee_cnt, sounding_idx, i;
	u16 tp[MAX_BEAMFORMEE_ENTRY_NUM] = {0};
	u8 tx_rate[MAX_BEAMFORMEE_ENTRY_NUM] = {0};
	u64 tx_bytes, last_bytes;
	u32 time;
	systime last_timestamp;
	u8 set_timer = _FALSE;

	info = GET_BEAMFORM_INFO(adapter);
	bfee_cnt = info->beamformee_su_cnt + info->beamformee_mu_cnt;
	if (bfee_cnt == 0)
		return;

	for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
		bfee = &info->bfee_entry[i];
		if (_FALSE == bfee->used)
			continue;

		sta = rtw_get_stainfo(&adapter->stapriv, bfee->mac_addr);
		if (!sta)
			continue;

		last_timestamp = bfee->tx_timestamp;
		last_bytes = bfee->tx_bytes;
		bfee->tx_timestamp = rtw_get_current_time();
		bfee->tx_bytes = sta->sta_stats.tx_bytes;
		if (last_timestamp) {
			if (bfee->tx_bytes >= last_bytes)
				tx_bytes = bfee->tx_bytes - last_bytes;
			else
				tx_bytes = bfee->tx_bytes + (~last_bytes);
			time = rtw_get_time_interval_ms(last_timestamp, bfee->tx_timestamp);
			time = (time > 1000) ? time / 1000 : 1;
			tp[i] = toMbps(tx_bytes, time);
			tx_rate[i] = rtw_get_current_tx_rate(adapter, sta);
		}
	}

	sounding_idx = phydm_get_beamforming_sounding_info(GET_PDM_ODM(adapter), tp,
							   MAX_BEAMFORMEE_ENTRY_NUM, tx_rate);

	for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
		bfee = &info->bfee_entry[i];
		if (_FALSE == bfee->used)
			continue;

		if (sounding_idx & BIT(i)) {
			if (_FALSE == bfee->bApplySounding) {
				bfee->bApplySounding = _TRUE;
				bfee->SoundCnt = 0;
				set_timer = _TRUE;
			}
		} else if (_TRUE == bfee->bApplySounding) {
			bfee->bApplySounding = _FALSE;
			bfee->bDeleteSounding = _TRUE;
			bfee->SoundCnt = 0;
			set_timer = _TRUE;
		}
	}

	if (_TRUE == set_timer &&
	    SOUNDING_STATE_NONE == info->sounding_info.state) {
		info->sounding_info.state = SOUNDING_STATE_INIT;
		_set_timer(&info->sounding_timer, 0);
	}
}

#endif /* CONFIG_RUST */
