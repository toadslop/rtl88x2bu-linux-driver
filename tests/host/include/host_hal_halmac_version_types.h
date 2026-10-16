/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_HALMAC_VERSION_TYPES_H
#define HOST_HAL_HALMAC_VERSION_TYPES_H

#include <stdint.h>

struct halmac_ver {
	uint8_t major_ver;
	uint8_t prototype_ver;
	uint8_t minor_ver;
};

enum halmac_ret_status {
	HALMAC_RET_SUCCESS = 0x00,
	HALMAC_RET_ADAPTER_INVALID = 0x05,
};

struct host_halmac_version_stub_state {
	enum halmac_ret_status status;
	struct halmac_ver ver;
};

extern struct host_halmac_version_stub_state host_halmac_version_stub;

void host_halmac_version_stub_reset(void);
enum halmac_ret_status halmac_get_version(struct halmac_ver *version);
void rtw_halmac_get_version(char *str, uint32_t len);

#endif /* HOST_HAL_HALMAC_VERSION_TYPES_H */
