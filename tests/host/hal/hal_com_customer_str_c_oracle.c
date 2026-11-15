// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-86 (pre-port hal/hal_com.c customer string sync). */

#include "host_customer_str_test.h"
#include <stdio.h>
#include <string.h>

#define RTW_CUSTOMER_STR_LEN 16
#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define RTW_SCTX_DONE_SUCCESS HOST_RTW_SCTX_DONE_SUCCESS

#define H2C_CUSTOMER_STR_W1 0xC6
#define H2C_CUSTOMER_STR_W2 0xC7
#define H2C_CUSTOMER_STR_W3 0xC8
#define H2C_CUSTOMER_STR_W1_LEN 7
#define H2C_CUSTOMER_STR_W2_LEN 7
#define H2C_CUSTOMER_STR_W3_LEN 5

extern void *_adapter_to_dvobj(struct _adapter *adapter);
extern void *rtw_rust_hal_com_customer_str_mutex(void *dvobj);
extern void rtw_rust_hal_com_enter_critical_mutex(void *mutex);
extern void rtw_rust_hal_com_exit_critical_mutex(void *mutex);
extern void *rtw_rust_hal_com_customer_str_sctx(void *dvobj);
extern void rtw_rust_hal_com_customer_str_sctx_set(void *dvobj, struct submit_ctx *sctx);
extern unsigned char *rtw_rust_hal_com_customer_str_buf(void *dvobj);
extern int rtw_rust_hal_com_sctx_status(void *sctx);
extern unsigned char rtw_customer_str_req_cmd(struct _adapter *adapter);
extern unsigned char rtw_customer_str_write_cmd(struct _adapter *adapter,
						const unsigned char *cstr);
extern long rtw_hal_fill_h2c_cmd(struct _adapter *adapter, unsigned char cmd, unsigned int len,
				 unsigned char *parm);
extern void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms);
extern int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg);

long rtw_hal_customer_str_read_c_oracle(struct _adapter *adapter, unsigned char *cs)
{
	void *dvobj = _adapter_to_dvobj(adapter);
	void *mutex = rtw_rust_hal_com_customer_str_mutex(dvobj);
	struct submit_ctx sctx;
	long ret = _SUCCESS;

	rtw_rust_hal_com_enter_critical_mutex(mutex);
	if (rtw_rust_hal_com_customer_str_sctx(dvobj) != NULL)
		ret = _FAIL;
	else {
		rtw_sctx_init(&sctx, 2 * 1000);
		rtw_rust_hal_com_customer_str_sctx_set(dvobj, &sctx);
	}
	rtw_rust_hal_com_exit_critical_mutex(mutex);

	if (ret == _FAIL)
		return ret;

	ret = rtw_customer_str_req_cmd(adapter);
	if (ret != _SUCCESS) {
		rtw_rust_hal_com_enter_critical_mutex(mutex);
		rtw_rust_hal_com_customer_str_sctx_set(dvobj, NULL);
		rtw_rust_hal_com_exit_critical_mutex(mutex);
		return ret;
	}

	rtw_sctx_wait(&sctx, "rtw_hal_customer_str_read");

	rtw_rust_hal_com_enter_critical_mutex(mutex);
	rtw_rust_hal_com_customer_str_sctx_set(dvobj, NULL);
	if (sctx.status == RTW_SCTX_DONE_SUCCESS)
		memcpy(cs, rtw_rust_hal_com_customer_str_buf(dvobj), RTW_CUSTOMER_STR_LEN);
	else
		ret = _FAIL;
	rtw_rust_hal_com_exit_critical_mutex(mutex);
	return ret;
}

long rtw_hal_h2c_customer_str_write_c_oracle(struct _adapter *adapter, const unsigned char *cs)
{
	unsigned char h2c_w1[H2C_CUSTOMER_STR_W1_LEN];
	unsigned char h2c_w2[H2C_CUSTOMER_STR_W2_LEN];
	unsigned char h2c_w3[H2C_CUSTOMER_STR_W3_LEN];
	long ret;

	memset(h2c_w1, 0, sizeof(h2c_w1));
	memset(h2c_w2, 0, sizeof(h2c_w2));
	memset(h2c_w3, 0, sizeof(h2c_w3));

	h2c_w1[0] = 1;
	memcpy(h2c_w1 + 1, cs, 6);
	h2c_w2[0] = 1;
	memcpy(h2c_w2 + 1, cs + 6, 6);
	h2c_w3[0] = 1;
	memcpy(h2c_w3 + 1, cs + 12, 4);

	ret = rtw_hal_fill_h2c_cmd(adapter, H2C_CUSTOMER_STR_W1, H2C_CUSTOMER_STR_W1_LEN, h2c_w1);
	if (ret != _SUCCESS)
		return ret;
	ret = rtw_hal_fill_h2c_cmd(adapter, H2C_CUSTOMER_STR_W2, H2C_CUSTOMER_STR_W2_LEN, h2c_w2);
	if (ret != _SUCCESS)
		return ret;
	return rtw_hal_fill_h2c_cmd(adapter, H2C_CUSTOMER_STR_W3, H2C_CUSTOMER_STR_W3_LEN, h2c_w3);
}

long rtw_hal_customer_str_write_c_oracle(struct _adapter *adapter, const unsigned char *cs)
{
	void *dvobj = _adapter_to_dvobj(adapter);
	void *mutex = rtw_rust_hal_com_customer_str_mutex(dvobj);
	struct submit_ctx sctx;
	long ret = _SUCCESS;

	rtw_rust_hal_com_enter_critical_mutex(mutex);
	if (rtw_rust_hal_com_customer_str_sctx(dvobj) != NULL)
		ret = _FAIL;
	else {
		rtw_sctx_init(&sctx, 2 * 1000);
		rtw_rust_hal_com_customer_str_sctx_set(dvobj, &sctx);
	}
	rtw_rust_hal_com_exit_critical_mutex(mutex);

	if (ret == _FAIL)
		return ret;

	ret = rtw_customer_str_write_cmd(adapter, cs);
	if (ret != _SUCCESS) {
		rtw_rust_hal_com_enter_critical_mutex(mutex);
		rtw_rust_hal_com_customer_str_sctx_set(dvobj, NULL);
		rtw_rust_hal_com_exit_critical_mutex(mutex);
		return ret;
	}

	ret = rtw_customer_str_req_cmd(adapter);
	if (ret != _SUCCESS) {
		rtw_rust_hal_com_enter_critical_mutex(mutex);
		rtw_rust_hal_com_customer_str_sctx_set(dvobj, NULL);
		rtw_rust_hal_com_exit_critical_mutex(mutex);
		return ret;
	}

	rtw_sctx_wait(&sctx, "rtw_hal_customer_str_write");

	rtw_rust_hal_com_enter_critical_mutex(mutex);
	rtw_rust_hal_com_customer_str_sctx_set(dvobj, NULL);
	if (sctx.status == RTW_SCTX_DONE_SUCCESS) {
		if (memcmp(cs, rtw_rust_hal_com_customer_str_buf(dvobj), RTW_CUSTOMER_STR_LEN) != 0)
			ret = _FAIL;
	} else {
		ret = _FAIL;
	}
	rtw_rust_hal_com_exit_critical_mutex(mutex);
	return ret;
}