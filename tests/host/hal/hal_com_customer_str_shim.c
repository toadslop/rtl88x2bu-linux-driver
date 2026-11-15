// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-86 customer string sync L2. */

#include "host_customer_str_test.h"
#include <stdio.h>
#include <string.h>

#define RTW_SCTX_SUBMITTED HOST_RTW_SCTX_SUBMITTED
#define RTW_SCTX_DONE_SUCCESS HOST_RTW_SCTX_DONE_SUCCESS

static inline void init_completion(struct host_completion *c)
{
	c->completed = 0;
}

#define RTW_CUSTOMER_STR_LEN 16
#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1

#define H2C_CUSTOMER_STR_W1 0xC6
#define H2C_CUSTOMER_STR_W2 0xC7
#define H2C_CUSTOMER_STR_W3 0xC8

struct host_customer_str_h2c {
	unsigned char cmd;
	unsigned char len;
	unsigned char parm[8];
};

struct host_customer_str_fix {
	struct submit_ctx sctx;
	struct submit_ctx *sctx_ptr;
	unsigned char customer_str[RTW_CUSTOMER_STR_LEN];
	int mutex;
	int h2c_count;
	struct host_customer_str_h2c h2c[8];
	unsigned char req_cmd_result;
	unsigned char write_cmd_result;
	int auto_complete_sctx;
};

static struct host_customer_str_fix g_fix;

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;

void host_customer_str_reset(void)
{
	memset(&g_fix, 0, sizeof(g_fix));
}

void host_customer_str_set_auto_complete(int on)
{
	g_fix.auto_complete_sctx = on;
}

void host_customer_str_set_req_cmd_result(unsigned char ret)
{
	g_fix.req_cmd_result = ret;
}

void host_customer_str_set_write_cmd_result(unsigned char ret)
{
	g_fix.write_cmd_result = ret;
}

void host_customer_str_fill_buf(const unsigned char *src)
{
	memcpy(g_fix.customer_str, src, RTW_CUSTOMER_STR_LEN);
}

int host_customer_str_h2c_count(void)
{
	return g_fix.h2c_count;
}

unsigned char host_customer_str_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_fix.h2c_count)
		return 0;
	return g_fix.h2c[idx].cmd;
}

unsigned char host_customer_str_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_fix.h2c_count)
		return 0;
	return g_fix.h2c[idx].len;
}

const unsigned char *host_customer_str_h2c_parm(int idx)
{
	if (idx < 0 || idx >= g_fix.h2c_count)
		return NULL;
	return g_fix.h2c[idx].parm;
}

struct _adapter *host_customer_str_adapter(void)
{
	return &g_adapter;
}

void *_adapter_to_dvobj(struct _adapter *adapter)
{
	(void)adapter;
	return &g_dvobj;
}

void *rtw_rust_hal_com_adapter_to_dvobj(void *adapter)
{
	return _adapter_to_dvobj(adapter);
}

void *rtw_rust_hal_com_customer_str_mutex(void *dvobj)
{
	(void)dvobj;
	return (void *)&g_fix.mutex;
}

void rtw_rust_hal_com_enter_critical_mutex(void *mutex)
{
	(void)mutex;
}

void rtw_rust_hal_com_exit_critical_mutex(void *mutex)
{
	(void)mutex;
}

void *rtw_rust_hal_com_customer_str_sctx(void *dvobj)
{
	(void)dvobj;
	return g_fix.sctx_ptr;
}

void rtw_rust_hal_com_customer_str_sctx_set(void *dvobj, struct submit_ctx *sctx)
{
	(void)dvobj;
	g_fix.sctx_ptr = sctx;
}

unsigned char *rtw_rust_hal_com_customer_str_buf(void *dvobj)
{
	(void)dvobj;
	return g_fix.customer_str;
}

int rtw_rust_hal_com_sctx_status(void *sctx)
{
	struct submit_ctx *sc = sctx;

	return sc ? sc->status : 0;
}

void rtw_rust_hal_com_warn_customer_str_sync_ongoing(const char *func)
{
	(void)func;
}

void rtw_rust_hal_com_warn_customer_str_cmd_fail(const char *func, const char *msg)
{
	(void)func;
	(void)msg;
}

void rtw_rust_hal_com_warn_customer_str_h2c_w_fail(const char *func, const char *part)
{
	(void)func;
	(void)part;
}

void rtw_rust_hal_com_warn_customer_str_write_check_fail(const char *func)
{
	(void)func;
}

void rtw_rust_hal_com_info_dump_customer_str(const char *title, const unsigned char *buf)
{
	(void)title;
	(void)buf;
}

unsigned char rtw_customer_str_req_cmd(struct _adapter *adapter)
{
	(void)adapter;
	if (g_fix.auto_complete_sctx && g_fix.sctx_ptr)
		g_fix.sctx_ptr->status = RTW_SCTX_DONE_SUCCESS;
	return g_fix.req_cmd_result ? _SUCCESS : 0;
}

unsigned char rtw_customer_str_write_cmd(struct _adapter *adapter, const unsigned char *cstr)
{
	(void)adapter;
	(void)cstr;
	return g_fix.write_cmd_result ? _SUCCESS : 0;
}

long rtw_hal_fill_h2c_cmd(struct _adapter *adapter, unsigned char cmd, unsigned int len,
			  unsigned char *parm)
{
	(void)adapter;
	if (g_fix.h2c_count >= 8)
		return _FAIL;
	g_fix.h2c[g_fix.h2c_count].cmd = cmd;
	g_fix.h2c[g_fix.h2c_count].len = (unsigned char)len;
	if (len > sizeof(g_fix.h2c[0].parm))
		len = sizeof(g_fix.h2c[0].parm);
	memcpy(g_fix.h2c[g_fix.h2c_count].parm, parm, len);
	g_fix.h2c_count++;
	return _SUCCESS;
}

int _rtw_memcmp(const void *a, const void *b, unsigned int sz)
{
	return memcmp(a, b, sz) == 0 ? _TRUE : 0;
}

void _rtw_memcpy(void *dst, const void *src, unsigned int sz)
{
	memcpy(dst, src, sz);
}

void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms)
{
	if (!sctx)
		return;
	sctx->timeout_ms = (unsigned int)timeout_ms;
	sctx->status = RTW_SCTX_SUBMITTED;
	init_completion(&sctx->done);
}

int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg)
{
	(void)msg;
	if (!sctx)
		return _FAIL;
	if (sctx->status == RTW_SCTX_DONE_SUCCESS)
		return _SUCCESS;
	return _FAIL;
}