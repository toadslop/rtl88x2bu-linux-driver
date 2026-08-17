/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_XMIT_SCTX_TYPES_H
#define HOST_XMIT_SCTX_TYPES_H

#include "host_types.h"
#include <stdbool.h>

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define _FALSE 0
#define PLATFORM_LINUX 1
#define MAX_SCHEDULE_TIMEOUT ((unsigned long)-1)
#define RTW_INFO(fmt, ...) ((void)0)

typedef unsigned long systime;

enum {
	RTW_SCTX_SUBMITTED = -1,
	RTW_SCTX_DONE_SUCCESS = 0,
	RTW_SCTX_DONE_UNKNOWN,
	RTW_SCTX_DONE_TIMEOUT,
	RTW_SCTX_DONE_BUF_ALLOC,
	RTW_SCTX_DONE_BUF_FREE,
	RTW_SCTX_DONE_WRITE_PORT_ERR,
	RTW_SCTX_DONE_TX_DESC_NA,
	RTW_SCTX_DONE_TX_DENY,
	RTW_SCTX_DONE_CCX_PKT_FAIL,
	RTW_SCTX_DONE_DRV_STOP,
	RTW_SCTX_DONE_DEV_REMOVE,
};

struct host_completion { unsigned int completed; };

struct submit_ctx {
	systime submit_time;
	u32 timeout_ms;
	int status;
	struct host_completion done;
};

static inline systime rtw_get_current_time(void) { return 0; }
static inline unsigned long msecs_to_jiffies(int ms) { return (unsigned long)ms; }
static inline void init_completion(struct host_completion *c) { c->completed = 0; }
static inline void complete(struct host_completion *c) { c->completed = 1; }
static inline unsigned long wait_for_completion_timeout(struct host_completion *c,
							unsigned long expire)
{
	(void)expire;
	return c->completed ? 1UL : 0UL;
}

void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms);
int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg);
bool rtw_sctx_chk_waring_status(int status);
void rtw_sctx_done_err(struct submit_ctx **sctx, int status);
void rtw_sctx_done(struct submit_ctx **sctx);

#endif
