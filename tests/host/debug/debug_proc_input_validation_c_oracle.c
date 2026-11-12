// SPDX-License-Identifier: GPL-2.0
/* FX-18: host oracle for debug proc input validation (kernel parity). */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "host_rtw_debug_proc_input_validation_types.h"

#define EFAULT_NEG ((ssize_t)-EFAULT)

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return EFAULT_NEG;

	if (count > tmp_size) {
		return EFAULT_NEG;
	}

	if (buffer)
		memcpy(tmp, buffer, count);

	if (count < tmp_size)
		tmp[count] = '\0';

	return (ssize_t)count;
}

int host_fx18_validate_war_ipv4_index(s32 index)
{
	if (index < 1 || index > 4)
		return -EINVAL;
	return 0;
}

int host_fx18_validate_war_ipv6_index(u32 index)
{
	if (index < 1 || index > 8)
		return -EINVAL;
	return 0;
}

int host_fx18_validate_mdns_service_slot(int idx, int txt_idx)
{
	if (idx >= MAX_MDNS_SERVICE_NUM)
		return -EINVAL;
	if (txt_idx < 0 || txt_idx >= MAX_MDNS_TXT_NUM)
		return -EINVAL;
	return 0;
}

int host_fx18_validate_mdns_txt_rsp_count(size_t count, size_t max_input_size)
{
	if (count > max_input_size)
		return -EINVAL;
	return 0;
}

size_t host_fx18_mdns_txt_single_len(const char *txt_str)
{
	size_t txt_len = strlen(txt_str);

	if (txt_len > MAX_MDNS_TXT_SINGLE_LEN)
		txt_len = MAX_MDNS_TXT_SINGLE_LEN;
	return txt_len;
}
