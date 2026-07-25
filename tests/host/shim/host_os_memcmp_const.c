/* SPDX-License-Identifier: GPL-2.0 */
/* Linkable os_memcmp_const for Rust L2 oracles (host_wifi_types.h is static inline). */

#include <stddef.h>

int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const unsigned char *aa = a;
	const unsigned char *bb = b;
	unsigned char res = 0;
	size_t i;

	for (i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];
	return res;
}
