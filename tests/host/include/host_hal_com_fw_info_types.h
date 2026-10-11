/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_FW_INFO_TYPES_H
#define HOST_HAL_COM_FW_INFO_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct host_hal_com_fw_fixture {
	uint8_t b_fw_ready;
	uint16_t firmware_version;
	uint16_t firmware_sub_version;
} host_hal_com_fw_fixture;

int rtw_dump_fw_info_format(const host_hal_com_fw_fixture *fx, char *buf,
			    size_t buflen);

#endif /* HOST_HAL_COM_FW_INFO_TYPES_H */
