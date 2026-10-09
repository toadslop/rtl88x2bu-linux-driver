// SPDX-License-Identifier: GPL-2.0
#include "host_rtw_debug_sec_cam_types.h"

#define _SEC_TYPE_256_ 0x10
#define _AES_ 0x04
#define _GCMP_ 0x07
#define _CCMP_256_ (_AES_ | _SEC_TYPE_256_)
#define _GCMP_256_ (_GCMP_ | _SEC_TYPE_256_)
#define _SEC_TYPE_MAX_ 8

static const char *_security_type_str[] = {
	"N/A",
	"WEP40",
	"TKIP",
	"TKIP_WM",
	"AES",
	"WEP104",
	"SMS4",
	"GCMP",
};

const char *security_type_str(u8 value)
{
	if (_CCMP_256_ == value)
		return "CCMP_256";
	if (_GCMP_256_ == value)
		return "GCMP_256";

	if (_SEC_TYPE_MAX_ > value)
		return _security_type_str[value];

	return NULL;
}
