// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-06 rtw_dump_fw_info formatter (from hal/hal_com_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_fw_info_types.h"

int rtw_dump_fw_info_format(const host_hal_com_fw_fixture *fx, char *buf,
			    size_t buflen)
{
	int n;

	if (!fx || !buf || buflen == 0)
		return -1;

	if (fx->b_fw_ready)
		n = snprintf(buf, buflen, "FW VER -%u.%u\n", fx->firmware_version,
			     fx->firmware_sub_version);
	else
		n = snprintf(buf, buflen, "FW not ready\n");

	if (n < 0 || (size_t)n >= buflen)
		return -1;
	return n;
}
