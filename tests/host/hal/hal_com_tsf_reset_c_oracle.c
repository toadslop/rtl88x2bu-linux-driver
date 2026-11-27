// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-143 TSF reset H2C offload (hal_com_rest.c). */

#include "host_hal_com_tsf_reset_types.h"

struct _adapter;

extern u8 rtw_read8(void *adapter, u32 addr);
extern s32 rtw_hal_fill_h2c_cmd(void *adapter, u8 id, u32 len, u8 *buf);
extern void rtw_msleep_os(unsigned int ms);
extern void rtw_mi_buddy_scan_abort(void *adapter, u8 bwait);

static s32 rtw_hal_h2c_reset_tsf_c_oracle(struct _adapter *adapter, u8 reset_port)
{
	u8 buf[2];
	s32 ret;

	if (reset_port == HW_PORT0) {
		buf[0] = 0x1;
		buf[1] = 0;
	} else {
		buf[0] = 0x0;
		buf[1] = 0x1;
	}

	ret = rtw_hal_fill_h2c_cmd(adapter, H2C_RESET_TSF, 2, buf);
	return ret;
}

s32 rtw_hal_reset_tsf_c_oracle(struct _adapter *adapter, u8 reset_port)
{
	u8 reset_cnt_before = 0, reset_cnt_after = 0, loop_cnt = 0;
	u32 reg_reset_tsf_cnt = (reset_port == HW_PORT0) ?
				REG_FW_RESET_TSF_CNT_0 : REG_FW_RESET_TSF_CNT_1;
	s32 ret;

	rtw_mi_buddy_scan_abort(adapter, _FALSE);
	reset_cnt_after = reset_cnt_before = rtw_read8(adapter, reg_reset_tsf_cnt);
	ret = rtw_hal_h2c_reset_tsf_c_oracle(adapter, reset_port);
	if (ret != _SUCCESS)
		return ret;

	while ((reset_cnt_after == reset_cnt_before) && (loop_cnt < 10)) {
		rtw_msleep_os(100);
		loop_cnt++;
		reset_cnt_after = rtw_read8(adapter, reg_reset_tsf_cnt);
	}

	return (loop_cnt >= 10) ? _FAIL : _SUCCESS;
}