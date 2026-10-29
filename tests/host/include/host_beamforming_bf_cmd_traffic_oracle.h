/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_CMD_TRAFFIC_ORACLE_H
#define HOST_BEAMFORMING_BF_CMD_TRAFFIC_ORACLE_H

#include <stddef.h>
#include <string.h>

#include "host_beamforming_bf_init_cmd_oracle.h"
#include "host_beamforming_bf_types.h"
#include "host_types.h"

#define BF_HOST_MAX_BFEE_ENTRY 8
#define BF_HOST_BEAMFORMING_WK_CID 17
#define BF_HOST_CMD_SET_DRV_EXTRA 12
#define BF_HOST_CMD_ID_C2H_SND_TXBF 0x02
#define BF_HOST_SOUNDING_STATE_NONE 0
#define BF_HOST_SOUNDING_STATE_INIT 1

struct bf_host_bfee_entry {
	u8 used;
	u16 mac_id;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u32 tx_timestamp;
	u64 tx_bytes;
	u8 bApplySounding;
	u8 bDeleteSounding;
	u8 SoundCnt;
};

struct bf_host_cmd_traffic_info {
	struct bf_host_bfee_entry bfee_entry[BF_HOST_MAX_BFEE_ENTRY];
	u8 beamformee_su_cnt;
	u8 beamformee_mu_cnt;
	struct bf_host_sounding_info sounding_info;
	u8 sounding_timer_armed;
};

struct bf_host_sta_cmn {
	u16 p_aid;
	u8 g_id;
};

struct bf_host_sta_stats {
	u64 tx_bytes;
};

struct bf_host_sta {
	u8 used;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	struct bf_host_sta_cmn cmn;
	struct bf_host_sta_stats sta_stats;
};

struct bf_host_stapriv {
	struct bf_host_sta stas[4];
};

struct bf_host_pkt_attrib {
	u16 txbf_p_aid;
	u16 txbf_g_id;
};

struct bf_host_cmd_traffic_adpt {
	struct bf_host_adpt base;
	struct bf_host_stapriv stapriv;
	struct bf_host_cmd_traffic_info traffic;
};

typedef struct bf_host_cmd_traffic_adpt *bf_host_cmd_traffic_padpt;

struct bf_host_cmd_enqueue_tr {
	u32 count;
	s32 last_type;
	s32 last_size;
	u8 last_enqueue;
	u8 last_ec_id;
	u8 sync_cmd_hdl;
};

struct bf_host_c2h_tr {
	u32 cancel_timer;
	u32 end_period_cmd;
	u8 end_period_res;
};

struct bf_host_traffic_tr {
	u32 timer_set;
	u8 bfee0_apply;
	u8 bfee0_delete;
	u8 bfee0_sound_cnt;
};

extern struct bf_host_cmd_enqueue_tr bf_host_cmd_enqueue_tr;
extern struct bf_host_c2h_tr bf_host_c2h_tr;
extern struct bf_host_traffic_tr bf_host_traffic_tr;
extern u8 bf_host_phydm_sounding_idx;

static inline struct bf_host_cmd_traffic_info *bf_host_traffic_info(bf_host_cmd_traffic_padpt a)
{
	return &a->traffic;
}

static inline u8 bf_host_c2h_snd_result(u8 *buf)
{
	/* C2H_SND_TXBF_GET_SND_RESULT: LE bit 16 of the C2H payload (halmac). */
	if (!buf)
		return 0;
	return (buf[2] & 1) ? 1 : 0;
}

void bf_host_cancel_sounding_timeout_timer(bf_host_cmd_traffic_padpt a);
void bf_host_set_sounding_timer(bf_host_cmd_traffic_padpt a, u32 ms);
struct bf_host_sta *bf_host_get_stainfo(bf_host_cmd_traffic_padpt a, u8 *mac);
u8 bf_host_phydm_get_beamforming_sounding_info(u16 *tp, u8 num, u8 *tx_rate);
u32 bf_host_get_current_time(void);
u32 bf_host_get_time_interval_ms(u32 a, u32 b);
u8 bf_host_get_current_tx_rate(bf_host_cmd_traffic_padpt a, struct bf_host_sta *sta);

#ifndef HOST_BF_CMD_TRAFFIC_RUST
static inline u8 o_bf_cmd(bf_host_cmd_traffic_padpt adapter, s32 type, u8 *pbuf, s32 size, u8 enqueue)
{
	u8 res = 1;

	if (!enqueue) {
		o_bf_cmd_hdl(&adapter->base, (u8)type, pbuf);
		bf_host_cmd_enqueue_tr.sync_cmd_hdl++;
		return res;
	}

	bf_host_cmd_enqueue_tr.count++;
	bf_host_cmd_enqueue_tr.last_type = type;
	bf_host_cmd_enqueue_tr.last_size = size;
	bf_host_cmd_enqueue_tr.last_enqueue = enqueue;
	bf_host_cmd_enqueue_tr.last_ec_id = BF_HOST_BEAMFORMING_WK_CID;
	(void)pbuf;
	return res;
}

static inline void o_bf_update_attrib(bf_host_cmd_traffic_padpt adapter, struct bf_host_pkt_attrib *attrib,
				      struct bf_host_sta *sta)
{
	if (sta) {
		attrib->txbf_g_id = sta->cmn.g_id;
		attrib->txbf_p_aid = sta->cmn.p_aid;
	}
	(void)adapter;
}

static inline void o_bf_c2h_handler(bf_host_cmd_traffic_padpt adapter, u8 id, u8 *buf, u8 buf_len)
{
	u8 res;

	(void)buf_len;
	if (id != BF_HOST_CMD_ID_C2H_SND_TXBF)
		return;

	bf_host_cancel_sounding_timeout_timer(adapter);
	res = bf_host_c2h_snd_result(buf) ? 1 : 0;
	bf_host_c2h_tr.cancel_timer++;
	bf_host_c2h_tr.end_period_res = res;
	o_bf_cmd(adapter, BF_HOST_CMD_END_PERIOD, &res, 1, 1);
	bf_host_c2h_tr.end_period_cmd++;
}

static inline u64 bf_host_to_mbps(u64 bytes, u32 secs)
{
	return (bytes >> 17) / (secs ? secs : 1);
}

static inline void o_bf_update_traffic(bf_host_cmd_traffic_padpt adapter)
{
	struct bf_host_cmd_traffic_info *info;
	struct bf_host_bfee_entry *bfee;
	struct bf_host_sta *sta;
	u8 bfee_cnt, sounding_idx, i;
	u16 tp[BF_HOST_MAX_BFEE_ENTRY];
	u8 tx_rate[BF_HOST_MAX_BFEE_ENTRY];
	u64 tx_bytes, last_bytes;
	u32 time;
	u32 last_timestamp;
	u8 set_timer = 0;

	memset(tp, 0, sizeof(tp));
	memset(tx_rate, 0, sizeof(tx_rate));
	info = bf_host_traffic_info(adapter);
	bfee_cnt = info->beamformee_su_cnt + info->beamformee_mu_cnt;
	if (bfee_cnt == 0)
		return;

	for (i = 0; i < BF_HOST_MAX_BFEE_ENTRY; i++) {
		bfee = &info->bfee_entry[i];
		if (!bfee->used)
			continue;

		sta = bf_host_get_stainfo(adapter, bfee->mac_addr);
		if (!sta)
			continue;

		last_timestamp = bfee->tx_timestamp;
		last_bytes = bfee->tx_bytes;
		bfee->tx_timestamp = bf_host_get_current_time();
		bfee->tx_bytes = sta->sta_stats.tx_bytes;
		if (last_timestamp) {
			if (bfee->tx_bytes >= last_bytes)
				tx_bytes = bfee->tx_bytes - last_bytes;
			else
				tx_bytes = bfee->tx_bytes + (~last_bytes);
			time = bf_host_get_time_interval_ms(last_timestamp, bfee->tx_timestamp);
			time = (time > 1000) ? time / 1000 : 1;
			tp[i] = (u16)bf_host_to_mbps(tx_bytes, time);
			tx_rate[i] = bf_host_get_current_tx_rate(adapter, sta);
		}
	}

	sounding_idx = bf_host_phydm_get_beamforming_sounding_info(tp, BF_HOST_MAX_BFEE_ENTRY, tx_rate);

	for (i = 0; i < BF_HOST_MAX_BFEE_ENTRY; i++) {
		bfee = &info->bfee_entry[i];
		if (!bfee->used)
			continue;

		if (sounding_idx & (1u << i)) {
			if (!bfee->bApplySounding) {
				bfee->bApplySounding = 1;
				bfee->SoundCnt = 0;
				set_timer = 1;
			}
		} else if (bfee->bApplySounding) {
			bfee->bApplySounding = 0;
			bfee->bDeleteSounding = 1;
			bfee->SoundCnt = 0;
			set_timer = 1;
		}
	}

	if (set_timer && info->sounding_info.state == BF_HOST_SOUNDING_STATE_NONE) {
		info->sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;
		bf_host_set_sounding_timer(adapter, 0);
		bf_host_traffic_tr.timer_set++;
	}

	if (info->bfee_entry[0].used) {
		bf_host_traffic_tr.bfee0_apply = info->bfee_entry[0].bApplySounding;
		bf_host_traffic_tr.bfee0_delete = info->bfee_entry[0].bDeleteSounding;
		bf_host_traffic_tr.bfee0_sound_cnt = info->bfee_entry[0].SoundCnt;
	}
}
#else
u8 o_bf_cmd(bf_host_cmd_traffic_padpt adapter, s32 type, u8 *pbuf, s32 size, u8 enqueue);
void o_bf_update_attrib(bf_host_cmd_traffic_padpt adapter, struct bf_host_pkt_attrib *attrib,
			struct bf_host_sta *sta);
void o_bf_c2h_handler(bf_host_cmd_traffic_padpt adapter, u8 id, u8 *buf, u8 buf_len);
void o_bf_update_traffic(bf_host_cmd_traffic_padpt adapter);
#endif

#endif
