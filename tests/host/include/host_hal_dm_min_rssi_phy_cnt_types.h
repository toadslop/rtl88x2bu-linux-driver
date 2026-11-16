// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_MIN_RSSI_PHY_CNT_TYPES_H
#define HOST_HAL_DM_MIN_RSSI_PHY_CNT_TYPES_H

#include <stdint.h>
#include <string.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint8_t boolean;

#define MACID_NUM_SW_LIMIT 32
#define _TRUE 1u
#define _FALSE 0u

#define PHYDM_INFO_RSSI_MIN 21u
#define PHYDM_INFO_EDCCA_FLAG 14u

#define LE_BITS_TO_1BYTE(__ptr, __start, __len) \
	((u8)(((*(u8 *)(__ptr)) >> (__start)) & (((u8)0xff) >> (8 - (__len)))))
#define GET_H2CCMD_MSRRPT_PARM_OPMODE(__pH2CCmd) LE_BITS_TO_1BYTE((__pH2CCmd), 0, 1)

#define is_broadcast_mac_addr(Addr) \
	((((Addr)[0] & 0xff) == 0xff) && (((Addr)[1] & 0xff) == 0xff) && \
	 (((Addr)[2] & 0xff) == 0xff) && (((Addr)[3] & 0xff) == 0xff) && \
	 (((Addr)[4] & 0xff) == 0xff) && (((Addr)[5] & 0xff) == 0xff))

enum phy_cnt {
	FA_OFDM,
	FA_CCK,
	FA_TOTAL,
	CCA_OFDM,
	CCA_CCK,
	CCA_ALL,
	CRC32_OK_VHT,
	CRC32_OK_HT,
	CRC32_OK_LEGACY,
	CRC32_OK_CCK,
	CRC32_ERROR_VHT,
	CRC32_ERROR_HT,
	CRC32_ERROR_LEGACY,
	CRC32_ERROR_CCK,
};

struct rssi_info {
	s8 rssi;
};

struct cmn_sta_info {
	u8 mac_addr[6];
	struct rssi_info rssi_stat;
};

struct sta_info {
	struct cmn_sta_info cmn;
};

struct macid_ctl_t {
	u8 h2c_msr[MACID_NUM_SW_LIMIT];
	struct sta_info *sta[MACID_NUM_SW_LIMIT];
};

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct macid_ctl_t macid_ctl;
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_macidctl(a) (&(a)->macid_ctl)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_min_rssi_phy_cnt_trace {
	u32 phydm_cmn_query_last;
	u32 phydm_cmn_query_ret;
};

extern struct host_hal_dm_min_rssi_phy_cnt_trace host_hal_dm_min_rssi_phy_cnt_trace;

void host_hal_dm_min_rssi_phy_cnt_reset(void);

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type);

s8 rtw_dm_get_min_rssi(_adapter *adapter);
s8 rtw_phydm_get_min_rssi(_adapter *adapter);
boolean rtw_phydm_get_edcca_flag(_adapter *adapter);
u32 rtw_phydm_get_phy_cnt(_adapter *adapter, enum phy_cnt cnt);

#endif