/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-133 security CAM dump formatters. */
#ifndef HOST_RTW_DEBUG_SEC_CAM_TYPES_H
#define HOST_RTW_DEBUG_SEC_CAM_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define CONFIG_RTW_DEBUG 1
#define CONFIG_PROC_DEBUG 1
#define ETH_ALEN 6

typedef uint8_t u8;
typedef uint16_t u16;

struct sec_cam_ent {
	u16 ctrl;
	u8 mac[ETH_ALEN];
	u8 key[16];
};

#define RTW_DBGDUMP ((void *)1)

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		if (sel != RTW_DBGDUMP) \
			fprintf((FILE *)sel, fmt, ##__VA_ARGS__); \
	} while (0)

const char *security_type_str(u8 value);

int dump_sec_cam_ent_format(struct sec_cam_ent *ent, int id, char *buf, size_t buflen);
int dump_sec_cam_ent_title_format(u8 has_id, char *buf, size_t buflen);

#endif /* HOST_RTW_DEBUG_SEC_CAM_TYPES_H */
