// SPDX-License-Identifier: GPL-2.0
/* C oracle + halmac_get_version stub for W4-27 host L2. */

#include <stdio.h>

#include "host_hal_halmac_version_types.h"

struct host_halmac_version_stub_state host_halmac_version_stub;

void host_halmac_version_stub_reset(void)
{
	host_halmac_version_stub.status = HALMAC_RET_SUCCESS;
	host_halmac_version_stub.ver.major_ver = 0;
	host_halmac_version_stub.ver.prototype_ver = 0;
	host_halmac_version_stub.ver.minor_ver = 0;
}

enum halmac_ret_status halmac_get_version(struct halmac_ver *version)
{
	if (!version)
		return HALMAC_RET_ADAPTER_INVALID;
	if (host_halmac_version_stub.status != HALMAC_RET_SUCCESS)
		return host_halmac_version_stub.status;
	*version = host_halmac_version_stub.ver;
	return HALMAC_RET_SUCCESS;
}

#ifndef RUST_HAL_HALMAC_ORACLE
void rtw_halmac_get_version(char *str, uint32_t len)
{
	enum halmac_ret_status status;
	struct halmac_ver ver;

	if (!str || len == 0)
		return;

	status = halmac_get_version(&ver);
	if (status != HALMAC_RET_SUCCESS)
		return;

	snprintf(str, len, "V%d_%02d_%02d", ver.major_ver, ver.prototype_ver,
		 ver.minor_ver);
}
#endif /* !RUST_HAL_HALMAC_ORACLE */
