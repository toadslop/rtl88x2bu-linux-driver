/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RM_PARSE_TYPES_H
#define HOST_RM_PARSE_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1

#define RTW_INFO(fmt, ...) ((void)0)

enum ch_load_opt_sub_id { ch_load_rsvd, ch_load_rep_info };
enum noise_histo_opt_sub_id { noise_histo_rsvd, noise_histo_rep_info };

enum rm_cap_en {
	RM_CH_LOAD_CAP_EN = 8,
	RM_NOISE_HISTO_CAP_EN = 9,
};

struct opt_rep_info {
	u8 cond;
	u8 threshold;
};

struct meas_req_opt {
	struct opt_rep_info rep_cond;
};

struct rm_meas_req {
	int opt_s_elem_len;
	union {
		struct meas_req_opt clm;
		struct meas_req_opt nhm;
	} opt;
};

struct rm_obj {
	struct rm_meas_req q;
};

#endif /* HOST_RM_PARSE_TYPES_H */
