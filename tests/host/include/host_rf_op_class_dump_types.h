/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for op-class dump formatter tests (W3-57).
 */
#ifndef HOST_RF_OP_CLASS_DUMP_TYPES_H
#define HOST_RF_OP_CLASS_DUMP_TYPES_H

#include "host_rf_op_class_pref_types.h"

#include <stdio.h>

#undef CONFIG_RTW_DEBUG
#define CONFIG_RTW_DEBUG 1

struct host_sel_capture {
	char buf[8192];
	size_t len;
};

extern struct host_sel_capture host_sel_out;

static inline void host_sel_reset(void)
{
	host_sel_out.len = 0;
	host_sel_out.buf[0] = '\0';
}

#undef RTW_PRINT_SEL
#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		int _n = snprintf(host_sel_out.buf + host_sel_out.len, \
				  sizeof(host_sel_out.buf) - host_sel_out.len, \
				  fmt, ##__VA_ARGS__); \
		if (_n > 0) \
			host_sel_out.len += (size_t)_n; \
	} while (0)

bool dbg_global_op_class_validate(u8 gid);
void dump_global_op_class(void *sel);
void dump_cap_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);
void dump_reg_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);
void dump_cur_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);

void host_rf_op_class_dump_reset(struct rf_ctl_t *rfctl);
struct op_class_pref_t *host_rf_op_class_dump_add_pref(struct rf_ctl_t *rfctl,
						       u8 class_id, u8 op_ch_num,
						       u8 ir_ch_num);

#endif /* HOST_RF_OP_CLASS_DUMP_TYPES_H */
