/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_FW_MEDIA_STATUS_TYPES_H
#define HOST_HAL_COM_FW_MEDIA_STATUS_TYPES_H

#define CONFIG_FW_MULTI_PORT_SUPPORT 1
#define CONFIG_TX_AC_LIFETIME 1

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef int bool;

#define _SUCCESS 1
#define _FAIL 0
#define CHANNEL_WIDTH_20 0

#define H2C_MEDIA_STATUS_RPT 0x01
#define H2C_MEDIA_STATUS_RPT_LEN 4

#define H2C_MSR_ROLE_STA 1
#define H2C_MSR_ROLE_MAX 8

#define MACID_NUM_SW_LIMIT 8

#define LE_BITS_TO_1BYTE(__ptr, __start, __len) \
	((u8)(((*(u8 *)(__ptr)) >> (__start)) & (((u8)0xff) >> (8 - (__len)))))

#define GET_H2CCMD_MSRRPT_PARM_OPMODE(__p) LE_BITS_TO_1BYTE((__p), 0, 1)
#define GET_H2CCMD_MSRRPT_PARM_ROLE(__p) LE_BITS_TO_1BYTE((__p), 4, 4)

struct macid_ctl_t {
	u8 num;
	u8 h2c_msr[MACID_NUM_SW_LIMIT];
	u8 bw[MACID_NUM_SW_LIMIT];
	u8 vht_en[MACID_NUM_SW_LIMIT];
	u32 rate_bmp0[MACID_NUM_SW_LIMIT];
	u32 rate_bmp1[MACID_NUM_SW_LIMIT];
	u8 op_num[H2C_MSR_ROLE_MAX];
};

struct dvobj_priv {
	struct macid_ctl_t macid_ctl;
};

struct _adapter {
	u8 hw_port;
	struct dvobj_priv dvobj;
};

struct host_fw_media_status_trace {
	u8 last_h2c_msr;
	unsigned int tx_aclt_calls;
	unsigned int update_tx_rate_bmp_calls;
};

extern struct host_fw_media_status_trace host_fw_media_status_trace;

struct dvobj_priv *adapter_to_dvobj(struct _adapter *a);
struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *d);
struct _adapter *host_fw_media_status_adapter(void);

void host_fw_media_status_reset(void);
int host_fw_media_status_h2c_count(void);
u8 host_fw_media_status_h2c_cmd(int idx);
unsigned int host_fw_media_status_h2c_len(int idx);
const u8 *host_fw_media_status_h2c_buf(int idx);
void host_fw_media_status_set_fill_ret(s32 v);

s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len,
			 u8 *buf);
u8 rtw_hal_get_port(struct _adapter *adapter);
u8 rtw_macid_ctl_set_h2c_msr(struct macid_ctl_t *ctl, u8 id, u8 h2c_msr);
void rtw_macid_ctl_set_bw(struct macid_ctl_t *ctl, u8 id, u8 bw);
void rtw_macid_ctl_set_vht_en(struct macid_ctl_t *ctl, u8 id, u8 en);
void rtw_macid_ctl_set_rate_bmp0(struct macid_ctl_t *ctl, u8 id, u32 bmp);
void rtw_macid_ctl_set_rate_bmp1(struct macid_ctl_t *ctl, u8 id, u32 bmp);
void rtw_hal_update_tx_aclt(struct _adapter *adapter);
void rtw_update_tx_rate_bmp(struct dvobj_priv *dvobj);

#endif