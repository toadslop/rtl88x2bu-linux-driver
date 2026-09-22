/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RM_PARSE_TYPES_H
#define HOST_RM_PARSE_TYPES_H

#include "host_types.h"
#include <stddef.h>

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1

#define RTW_INFO(fmt, ...) ((void)0)

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define IW_ESSID_MAX_SIZE 32

typedef struct {
	u32 SsidLength;
	u8 Ssid[IW_ESSID_MAX_SIZE];
} NDIS_802_11_SSID;

#define MAX_CH_NUM_IN_OP_CLASS 11
typedef struct _RT_OPERATING_CLASS {
	int global_op_class;
	int Len;
	u8 Channel[MAX_CH_NUM_IN_OP_CLASS];
} RT_OPERATING_CLASS;

enum meas_type_of_req {
	basic_req,
	cca_req,
	rpi_histo_req,
	ch_load_req,
	noise_histo_req,
	bcn_req,
};

enum bcn_req_opt_sub_id {
	bcn_req_ssid = 0,
	bcn_req_rep_info = 1,
	bcn_req_rep_detail = 2,
	bcn_req_req = 10,
	bcn_req_ap_ch_rep = 51,
};

enum ch_load_opt_sub_id { ch_load_rsvd, ch_load_rep_info };
enum noise_histo_opt_sub_id { noise_histo_rsvd, noise_histo_rep_info };

struct opt_rep_info {
	u8 cond;
	u8 threshold;
};

#define BCN_REQ_OPT_MAX_NUM 16
#define BCN_REQ_OPT_AP_CH_RPT_MAX_NUM 12

struct bcn_req_opt {
	u8 opt_id[BCN_REQ_OPT_MAX_NUM];
	u8 opt_id_num;
	u8 rep_detail;
	NDIS_802_11_SSID ssid;
	struct opt_rep_info rep_cond;
	u8 ap_ch_rpt_num;
	struct _RT_OPERATING_CLASS *ap_ch_rpt[BCN_REQ_OPT_AP_CH_RPT_MAX_NUM];
	u8 *req_start;
	u8 req_len;
};

struct meas_req_opt {
	struct opt_rep_info rep_cond;
};

struct rm_meas_req {
	u8 m_type;
	u8 m_mode;
	u8 op_class;
	u8 ch_num;
	u16 rand_intvl;
	u16 meas_dur;
	u8 bssid[6];
	int opt_s_elem_len;
	union {
		struct bcn_req_opt bcn;
		struct meas_req_opt clm;
		struct meas_req_opt nhm;
	} opt;
};

struct rm_obj {
	struct rm_meas_req q;
};

enum rm_cap_en {
	RM_BCN_MEAS_REP_COND_CAP_EN = 7,
	RM_CH_LOAD_CAP_EN = 8,
	RM_NOISE_HISTO_CAP_EN = 9,
};

void *rtw_malloc(size_t sz);
void rtw_mfree(void *p, size_t sz);

static inline u16 le16_to_cpu(u16 val)
{
	return val;
}

#endif /* HOST_RM_PARSE_TYPES_H */
