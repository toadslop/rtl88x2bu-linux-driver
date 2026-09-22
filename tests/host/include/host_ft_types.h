/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_FT_TYPES_H
#define HOST_FT_TYPES_H

#include <stdbool.h>
#include "host_types.h"

#define _FALSE 0
#define RTW_FT_EN 0x01
#define RTW_FT_BTM_ROAM 0x10

struct ft_roam_info {
	u8 ft_flags;
	bool ft_updated_bcn;
};

void host_ft_info_init(struct ft_roam_info *pft);

#endif /* HOST_FT_TYPES_H */
