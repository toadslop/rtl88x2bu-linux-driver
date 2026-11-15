/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CUSTOMER_STR_TEST_H
#define HOST_CUSTOMER_STR_TEST_H

struct _adapter { int _pad; };
struct dvobj_priv { int _pad; };

enum {
	HOST_RTW_SCTX_SUBMITTED = -1,
	HOST_RTW_SCTX_DONE_SUCCESS = 0,
};

typedef unsigned int host_u32;

struct host_completion { unsigned int completed; };

struct submit_ctx {
	unsigned long submit_time;
	host_u32 timeout_ms;
	int status;
	struct host_completion done;
};

#endif