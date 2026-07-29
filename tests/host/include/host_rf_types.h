/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_rf_rest tests (W3-19).
 * Channel-width enums match include/cmn_info/rtw_sta_info.h (kernel build).
 */
#ifndef HOST_RF_TYPES_H
#define HOST_RF_TYPES_H

#include <stdbool.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_40 1
#define CHANNEL_WIDTH_80 2
#define CHANNEL_WIDTH_160 3
#define CHANNEL_WIDTH_80_80 4
#define CHANNEL_WIDTH_5 5
#define CHANNEL_WIDTH_10 6
#define CHANNEL_WIDTH_MAX 7

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define CENTER_CH_2G_40M_NUM 9
#define CENTER_CH_2G_NUM 14
#define CENTER_CH_5G_20M_NUM 28
#define CENTER_CH_5G_40M_NUM 14
#define CENTER_CH_5G_80M_NUM 7
#define CENTER_CH_5G_160M_NUM 3

#define RTW_PRINT(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

typedef enum _BAND_TYPE {
	BAND_ON_2_4G = 0,
	BAND_ON_5G = 1,
	BAND_MAX,
} BAND_TYPE;

#define BIT0 (1 << 0)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT(x) (1 << (x))

#define BAND_CAP_2G BIT0
#define BAND_CAP_5G BIT1

#define BW_CAP_5M BIT0
#define BW_CAP_10M BIT1
#define BW_CAP_20M BIT2
#define BW_CAP_40M BIT3
#define BW_CAP_80M BIT4
#define BW_CAP_160M BIT5
#define BW_CAP_80_80M BIT6

enum opc_bw {
	OPC_BW20 = 0,
	OPC_BW40PLUS = 1,
	OPC_BW40MINUS = 2,
	OPC_BW80 = 3,
	OPC_BW160 = 4,
	OPC_BW80P80 = 5,
	OPC_BW_NUM,
};

extern const char *const _ch_width_str[];
#define ch_width_str(bw) (((bw) < CHANNEL_WIDTH_MAX) ? _ch_width_str[(bw)] : "CHANNEL_WIDTH_MAX")

extern const u8 _ch_width_to_bw_cap[];
#define ch_width_to_bw_cap(bw) (((bw) < CHANNEL_WIDTH_MAX) ? _ch_width_to_bw_cap[(bw)] : 0)

extern const char *const _band_str[];
#define band_str(band) (((band) >= BAND_MAX) ? _band_str[BAND_MAX] : _band_str[(band)])

extern const u8 _band_to_band_cap[];
#define band_to_band_cap(band) (((band) >= BAND_MAX) ? _band_to_band_cap[BAND_MAX] : _band_to_band_cap[(band)])

extern const char *const _opc_bw_str[OPC_BW_NUM];
#define opc_bw_str(bw) (((bw) < OPC_BW_NUM) ? _opc_bw_str[(bw)] : "N/A")

extern const u8 _opc_bw_to_ch_width[OPC_BW_NUM];
#define opc_bw_to_ch_width(bw) (((bw) < OPC_BW_NUM) ? _opc_bw_to_ch_width[(bw)] : CHANNEL_WIDTH_MAX)

#define rtw_is_2g_ch(ch) ((ch) >= 1 && (ch) <= 14)
#define rtw_is_5g_ch(ch) ((ch) >= 36 && (ch) <= 177)

struct op_class_t {
	u8 class_id;
	BAND_TYPE band;
	enum opc_bw bw;
	u8 *len_ch_attr;
};

#define OPC_CH_LIST_LEN(_opc) (_opc.len_ch_attr[0])
#define OPC_CH_LIST_CH(_opc, _i) (_opc.len_ch_attr[_i + 1])

extern const struct op_class_t global_op_class[];
extern const int global_op_class_num;

bool is_valid_global_op_class_id(u8 gid);
s16 get_sub_op_class(u8 gid, u8 ch);
u8 rtw_get_op_class_by_chbw(u8 ch, u8 bw, u8 offset);
u8 rtw_get_bw_offset_by_op_class_ch(u8 gid, u8 ch, u8 *bw, u8 *offset);

u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);

#define RF_PATH_MAX 4

enum rf_type {
	RF_1T1R = 0,
	RF_1T2R = 1,
	RF_2T2R = 2,
	RF_2T3R = 3,
	RF_2T4R = 4,
	RF_3T3R = 5,
	RF_3T4R = 6,
	RF_4T4R = 7,
	RF_4T3R = 8,
	RF_4T2R = 9,
	RF_4T1R = 10,
	RF_3T2R = 11,
	RF_3T1R = 12,
	RF_2T1R = 13,
	RF_1T4R = 14,
	RF_1T3R = 15,
	RF_TYPE_MAX,
};

enum bb_path {
	BB_PATH_A = 0x00000001,
	BB_PATH_B = 0x00000002,
	BB_PATH_C = 0x00000004,
	BB_PATH_D = 0x00000008,
};

#define RF_TYPE_VALID(rf_type) ((rf_type) < RF_TYPE_MAX)

extern const u8 _rf_type_to_rf_tx_cnt[];
#define rf_type_to_rf_tx_cnt(rf_type) \
	(RF_TYPE_VALID(rf_type) ? _rf_type_to_rf_tx_cnt[(rf_type)] : 0)

extern const u8 _rf_type_to_rf_rx_cnt[];
#define rf_type_to_rf_rx_cnt(rf_type) \
	(RF_TYPE_VALID(rf_type) ? _rf_type_to_rf_rx_cnt[(rf_type)] : 0)

void rf_type_to_default_trx_bmp(enum rf_type rf, enum bb_path *tx, enum bb_path *rx);
enum rf_type trx_num_to_rf_type(u8 tx_num, u8 rx_num);
enum rf_type trx_bmp_to_rf_type(u8 tx_bmp, u8 rx_bmp);
bool rf_type_is_a_in_b(enum rf_type a, enum rf_type b);
u8 rtw_restrict_trx_path_bmp_by_trx_num_lmt(u8 trx_path_bmp, u8 tx_num_lmt,
					    u8 rx_num_lmt, u8 *tx_num, u8 *rx_num);
u8 rtw_restrict_trx_path_bmp_by_rftype(u8 trx_path_bmp, enum rf_type type,
				       u8 *tx_num, u8 *rx_num);

#endif /* HOST_RF_TYPES_H */
