/* SPDX-License-Identifier: GPL-2.0 */
/* W3-41 rate-section helpers — MGN/IS_* macros for host L2 (rest.c oracle). */
#ifndef HOST_RATE_SECTION_TYPES_H
#define HOST_RATE_SECTION_TYPES_H

enum MGN_RATE {
	MGN_1M = 0x02,
	MGN_6M = 0x0C,
	MGN_11M = 0x16,
	MGN_54M = 0x6C,
	MGN_MCS0 = 0x80,
	MGN_MCS8 = 0x88,
	MGN_VHT1SS_MCS0 = 0xA0,
};

#define IS_CCK_RATE(r) \
	((r) == MGN_1M || (r) == 0x04 || (r) == 0x0B || (r) == MGN_11M)
#define IS_OFDM_RATE(r) ((r) >= MGN_6M && (r) <= MGN_54M && (r) != MGN_11M)
#define IS_HT1SS_RATE(r) ((r) >= MGN_MCS0 && (r) <= 0x87)
#define IS_HT2SS_RATE(r) ((r) >= MGN_MCS8 && (r) <= 0x8F)
#define IS_HT3SS_RATE(r) ((r) >= 0x90 && (r) <= 0x97)
#define IS_HT4SS_RATE(r) ((r) >= 0x98 && (r) <= 0x9F)
#define IS_VHT1SS_RATE(r) ((r) >= MGN_VHT1SS_MCS0 && (r) <= 0xA9)
#define IS_VHT2SS_RATE(r) ((r) >= 0xAA && (r) <= 0xB3)
#define IS_VHT3SS_RATE(r) ((r) >= 0xB4 && (r) <= 0xBD)
#define IS_VHT4SS_RATE(r) ((r) >= 0xBE && (r) <= 0xC7)

#ifndef HT_1SS
#define HT_1SS 2
#define HT_2SS 3
#define HT_3SS 4
#define HT_4SS 5
#define VHT_1SS 6
#define VHT_2SS 7
#define VHT_3SS 8
#define VHT_4SS 9
#define RATE_SECTION_NUM 10
#endif

RATE_SECTION mgn_rate_to_rs(enum MGN_RATE rate);
unsigned int rtw_get_cckrate_size(u8 *rate, unsigned int rate_length);
unsigned int rtw_is_cckrates_included(u8 *rate);
unsigned int rtw_is_cckratesonly_included(u8 *rate);
unsigned int rtw_get_rateset_len(u8 *rateset);
u8 secondary_ch_offset_to_hal_ch_offset(u8 ch_offset);
u8 hal_ch_offset_to_secondary_ch_offset(u8 ch_offset);

#endif /* HOST_RATE_SECTION_TYPES_H */
