// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle for rtw_chip_rftype_to_hal_rftype (W4-64). */

#include <stdint.h>

typedef uint8_t u8;

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
	RF_TYPE_MAX = 16,
};

enum hal_rf_type_e {
	RF_TYPE_1T1R = 0,
	RF_TYPE_1T2R = 1,
	RF_TYPE_2T2R = 2,
	RF_TYPE_2T3R = 3,
	RF_TYPE_2T4R = 4,
	RF_TYPE_3T3R = 5,
	RF_TYPE_3T4R = 6,
	RF_TYPE_4T4R = 7,
};

#define rtw_min(a, b) ((a) > (b) ? (b) : (a))

static const u8 _trx_num_to_rf_type[RF_PATH_MAX][RF_PATH_MAX] = {
	{RF_1T1R, RF_1T2R, RF_1T3R, RF_1T4R},
	{RF_2T1R, RF_2T2R, RF_2T3R, RF_2T4R},
	{RF_3T1R, RF_3T2R, RF_3T3R, RF_3T4R},
	{RF_4T1R, RF_4T2R, RF_4T3R, RF_4T4R},
};

enum rf_type trx_num_to_rf_type(u8 tx_num, u8 rx_num)
{
	if (tx_num > 0 && tx_num <= RF_PATH_MAX && rx_num > 0 && rx_num <= RF_PATH_MAX)
		return _trx_num_to_rf_type[tx_num - 1][rx_num - 1];
	return RF_TYPE_MAX;
}

enum rf_type chip_rftype_to_hal_rftype_oracle(u8 cv_rf_type, u8 limit)
{
	u8 tx_num = 0;
	u8 rx_num = 0;

	if (cv_rf_type == RF_TYPE_1T1R) {
		tx_num = 1;
		rx_num = 1;
	} else if (cv_rf_type == RF_TYPE_1T2R) {
		tx_num = 1;
		rx_num = 2;
	} else if (cv_rf_type == RF_TYPE_2T2R) {
		tx_num = 2;
		rx_num = 2;
	} else if (cv_rf_type == RF_TYPE_2T3R) {
		tx_num = 2;
		rx_num = 3;
	} else if (cv_rf_type == RF_TYPE_2T4R) {
		tx_num = 2;
		rx_num = 4;
	} else if (cv_rf_type == RF_TYPE_3T3R) {
		tx_num = 3;
		rx_num = 3;
	} else if (cv_rf_type == RF_TYPE_3T4R) {
		tx_num = 3;
		rx_num = 4;
	} else if (cv_rf_type == RF_TYPE_4T4R) {
		tx_num = 4;
		rx_num = 4;
	}

	if (limit) {
		tx_num = rtw_min(tx_num, limit);
		rx_num = rtw_min(rx_num, limit);
	}

	return trx_num_to_rf_type(tx_num, rx_num);
}
