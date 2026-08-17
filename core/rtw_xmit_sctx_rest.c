/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_XMIT_SCTX_REST_C_

#ifdef HOST_XMIT_SCTX_TEST
#include "host_xmit_sctx_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_XMIT_SCTX_TEST)

void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms)
{
	sctx->timeout_ms = timeout_ms;
	sctx->submit_time = rtw_get_current_time();
#ifdef PLATFORM_LINUX
	init_completion(&sctx->done);
#endif
	sctx->status = RTW_SCTX_SUBMITTED;
}

int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg)
{
	int ret = _FAIL;
	unsigned long expire;
	int status = 0;

#ifdef PLATFORM_LINUX
	expire = sctx->timeout_ms ? msecs_to_jiffies(sctx->timeout_ms) : MAX_SCHEDULE_TIMEOUT;
	if (!wait_for_completion_timeout(&sctx->done, expire)) {
		status = RTW_SCTX_DONE_TIMEOUT;
		RTW_INFO("%s timeout: %s\n", __func__, msg);
	} else
		status = sctx->status;
#endif

	if (status == RTW_SCTX_DONE_SUCCESS)
		ret = _SUCCESS;

	return ret;
}

bool rtw_sctx_chk_waring_status(int status)
{
	switch (status) {
	case RTW_SCTX_DONE_UNKNOWN:
	case RTW_SCTX_DONE_BUF_ALLOC:
	case RTW_SCTX_DONE_BUF_FREE:
	case RTW_SCTX_DONE_DRV_STOP:
	case RTW_SCTX_DONE_DEV_REMOVE:
		return _TRUE;
	default:
		return _FALSE;
	}
}

void rtw_sctx_done_err(struct submit_ctx **sctx, int status)
{
	if (*sctx) {
		if (rtw_sctx_chk_waring_status(status))
			RTW_INFO("%s status:%d\n", __func__, status);
		(*sctx)->status = status;
#ifdef PLATFORM_LINUX
		complete(&((*sctx)->done));
#endif
		*sctx = NULL;
	}
}

void rtw_sctx_done(struct submit_ctx **sctx)
{
	rtw_sctx_done_err(sctx, RTW_SCTX_DONE_SUCCESS);
}

#endif /* !CONFIG_RUST || HOST_XMIT_SCTX_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_XMIT_SCTX_TEST)

systime rtw_rust_sctx_get_current_time(void)
{
	return rtw_get_current_time();
}

void rtw_rust_sctx_field_init(struct submit_ctx *sctx, int timeout_ms, systime submit_time)
{
	sctx->timeout_ms = timeout_ms;
	sctx->submit_time = submit_time;
#ifdef PLATFORM_LINUX
	init_completion(&sctx->done);
#endif
	sctx->status = RTW_SCTX_SUBMITTED;
}

void rtw_rust_sctx_field_set_status(struct submit_ctx *sctx, int status)
{
	sctx->status = status;
}

int rtw_rust_sctx_field_get_status(struct submit_ctx *sctx)
{
	return sctx->status;
}

u32 rtw_rust_sctx_field_get_timeout_ms(struct submit_ctx *sctx)
{
	return sctx->timeout_ms;
}

unsigned long rtw_rust_sctx_msecs_to_jiffies(int ms)
{
	return msecs_to_jiffies(ms);
}

unsigned long rtw_rust_sctx_max_schedule_timeout(void)
{
	return MAX_SCHEDULE_TIMEOUT;
}

unsigned long rtw_rust_sctx_wait_done(struct submit_ctx *sctx, unsigned long expire)
{
#ifdef PLATFORM_LINUX
	return wait_for_completion_timeout(&sctx->done, expire);
#else
	(void)sctx;
	(void)expire;
	return 0;
#endif
}

void rtw_rust_sctx_complete_done(struct submit_ctx *sctx)
{
#ifdef PLATFORM_LINUX
	complete(&sctx->done);
#endif
}

void rtw_rust_sctx_log_timeout(const char *msg)
{
	RTW_INFO("%s timeout: %s\n", "rtw_sctx_wait", msg);
}

void rtw_rust_sctx_log_warning_status(int status)
{
	RTW_INFO("%s status:%d\n", "rtw_sctx_done_err", status);
}

#endif /* CONFIG_RUST && !HOST_XMIT_SCTX_TEST */
