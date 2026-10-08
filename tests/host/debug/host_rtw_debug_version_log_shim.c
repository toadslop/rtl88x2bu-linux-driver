// SPDX-License-Identifier: GPL-2.0
#include "host_rtw_debug_version_log_types.h"

uint rtw_drv_log_level = _DRV_INFO_;

const char *rtw_log_level_str[] = {
	"_DRV_NONE_ = 0",
	"_DRV_ALWAYS_ = 1",
	"_DRV_ERR_ = 2",
	"_DRV_WARNING_ = 3",
	"_DRV_INFO_ = 4",
	"_DRV_DEBUG_ = 5",
	"_DRV_MAX_ = 6",
};
