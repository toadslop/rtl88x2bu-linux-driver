/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-210 PHY cap debug dumps. */
#ifndef HOST_RTW_DEBUG_PHY_CAP_TYPES_H
#define HOST_RTW_DEBUG_PHY_CAP_TYPES_H

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20

#define TEST_FLAG(__flag, __test) (((__flag) & (__test)) != 0)

#define LDPC_HT_ENABLE_RX BIT0
#define LDPC_HT_ENABLE_TX BIT1
#define STBC_HT_ENABLE_RX BIT0
#define STBC_HT_ENABLE_TX BIT1

#define LDPC_VHT_ENABLE_RX BIT0
#define LDPC_VHT_ENABLE_TX BIT1
#define STBC_VHT_ENABLE_RX BIT0
#define STBC_VHT_ENABLE_TX BIT1

#define BEAMFORMING_HT_BEAMFORMER_ENABLE BIT0
#define BEAMFORMING_HT_BEAMFORMEE_ENABLE BIT1
#define BEAMFORMING_VHT_BEAMFORMER_ENABLE BIT0
#define BEAMFORMING_VHT_BEAMFORMEE_ENABLE BIT1
#define BEAMFORMING_VHT_MU_MIMO_AP_ENABLE BIT2
#define BEAMFORMING_VHT_MU_MIMO_STA_ENABLE BIT3

struct ht_priv {
	u8 stbc_cap;
	u8 ldpc_cap;
	u8 beamform_cap;
};

struct vht_priv {
	u8 stbc_cap;
	u8 ldpc_cap;
	u16 beamform_cap;
};

struct mlme_priv {
	struct ht_priv htpriv;
	struct vht_priv vhtpriv;
};

struct registry_priv {
	u8 stbc_cap;
	u8 ldpc_cap;
	u8 beamform_cap;
	u8 beamformer_rf_num;
	u8 beamformee_rf_num;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct registry_priv registrypriv;
};

typedef struct _adapter _adapter;

#define RTW_PRINT_SEL(sel, fmt, ...) fprintf((FILE *)(sel), fmt, ##__VA_ARGS__)

void rtw_ht_use_default_setting(_adapter *padapter);
void rtw_vht_use_default_setting(_adapter *padapter);
void rtw_dump_dft_phy_cap(void *sel, _adapter *adapter);
void rtw_get_dft_phy_cap(void *sel, _adapter *adapter);
void rtw_dump_drv_phy_cap(void *sel, _adapter *adapter);

#endif /* HOST_RTW_DEBUG_PHY_CAP_TYPES_H */
