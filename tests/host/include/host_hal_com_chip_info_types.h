/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_CHIP_INFO_TYPES_H
#define HOST_HAL_COM_CHIP_INFO_TYPES_H

#include <stddef.h>

#include "host_types.h"

#ifndef _TRUE
#define _TRUE 1
#define _FALSE 0
#endif

#include "HalVerDef.h"

int dump_chip_info_format(HAL_VERSION chip_version, char *buf, size_t buflen);
void dump_chip_info(HAL_VERSION chip_version);

#endif /* HOST_HAL_COM_CHIP_INFO_TYPES_H */
