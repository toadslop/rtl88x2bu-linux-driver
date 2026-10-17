// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: _chip_type_to_odm_ic_type table (W4-36). */

#include <stdint.h>

/* Values mirror hal/hal_intf.c and host_odm_phydm_types.h ODM IC flags. */
const uint32_t _chip_type_to_odm_ic_type[] = {
	0,
	1u << 0,
	1u << 3,
	1u << 1,
	1u << 2,
	1u << 4,
	1u << 5,
	1u << 8,
	1u << 10,
	1u << 10,
	1u << 7,
	1u << 11,
	1u << 13,
	1u << 16,
	1u << 17,
	1u << 18,
	1u << 14,
	1u << 24,
	0,
};
