// SPDX-License-Identifier: GPL-2.0
/* FX-18 debug proc input validation (host L2). */

#ifndef HOST_RTW_DEBUG_PROC_INPUT_VALIDATION_TYPES_H
#define HOST_RTW_DEBUG_PROC_INPUT_VALIDATION_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define MAX_MDNS_SERVICE_NUM 10
#define MAX_MDNS_TXT_NUM 8
#define MAX_MDNS_TXT_SINGLE_LEN 255

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

int host_fx18_validate_war_ipv4_index(int32_t index);
int host_fx18_validate_war_ipv6_index(uint32_t index);
int host_fx18_validate_mdns_service_slot(int idx, int txt_idx);
int host_fx18_validate_mdns_txt_rsp_count(size_t count, size_t max_input_size);
size_t host_fx18_mdns_txt_single_len(const char *txt_str);

#endif
