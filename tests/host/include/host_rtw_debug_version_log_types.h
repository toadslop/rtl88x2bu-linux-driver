/* SPDX-License-Identifier: GPL-2.0 */
/* Host L1 types for W3-130 debug version/log dumps. */
#ifndef HOST_RTW_DEBUG_VERSION_LOG_TYPES_H
#define HOST_RTW_DEBUG_VERSION_LOG_TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DRV_NAME "rtl88x2bu"
#define DRIVERVERSION "v5.13.1-host-test"
#define CONFIG_RTW_DEBUG 1

typedef unsigned int uint;

enum {
	_DRV_NONE_ = 0,
	_DRV_ALWAYS_ = 1,
	_DRV_ERR_ = 2,
	_DRV_WARNING_ = 3,
	_DRV_INFO_ = 4,
	_DRV_DEBUG_ = 5,
	_DRV_MAX_ = 6,
};

#define RTW_DBGDUMP ((void *)1)

extern uint rtw_drv_log_level;
extern const char *rtw_log_level_str[];

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		if (sel != RTW_DBGDUMP) \
			fprintf((FILE *)sel, fmt, ##__VA_ARGS__); \
	} while (0)

#endif /* HOST_RTW_DEBUG_VERSION_LOG_TYPES_H */
