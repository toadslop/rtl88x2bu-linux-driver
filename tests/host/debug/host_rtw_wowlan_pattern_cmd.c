// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2: rtw_wowlan_parser_pattern_cmd + helpers matching the in-tree driver.
 *
 * Mirrors core/rtw_wlan_util.c (CONFIG_WOWLAN), os_dep/osdep_service.c (hexstr2bin),
 * and lib/string.c strsep() so glibc userland and kernel behavior stay aligned.
 */

#include <stddef.h>
#include <string.h>

#include "host_rtw_debug_proc_wow_types.h"

/*
 * Kernel strsep (tools/lib/string.c / lib/string.c) — empty tokens preserved.
 */
static char *host_kernel_strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}

static int hex2num_i(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int hex2byte_i(const char *hex)
{
	int a, b;

	a = hex2num_i(*hex++);
	if (a < 0)
		return -1;
	b = hex2num_i(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}

static int hexstr2bin(const char *hex, u8 *buf, size_t len)
{
	size_t i;
	int a;
	const char *ipos = hex;
	u8 *opos = buf;

	for (i = 0; i < len; i++) {
		a = hex2byte_i(ipos);
		if (a < 0)
			return -1;
		*opos++ = (u8)a;
		ipos += 2;
	}
	return 0;
}

int host_rtw_wowlan_parser_pattern_cmd(u8 *input_in, char *pattern,
				       int *pattern_len, char *bit_mask)
{
	char *cp;
	int pos = 0, mask_pos = 0;
	char *input = (char *)input_in;

	cp = strchr(input, '=');
	if (cp) {
		*cp = 0;
		cp++;
		input = cp;
	}

	cp = strchr(input, '\n');
	if (cp)
		*cp = 0;

	while (input) {
		cp = host_kernel_strsep(&input, ":");

		if (bit_mask && (strcmp(cp, "-") == 0 || strcmp(cp, "xx") == 0 ||
				 strcmp(cp, "--") == 0)) {
			/* skip this byte and leave mask bit unset */
		} else {
			u8 hex;

			if (strlen(cp) != 2)
				goto error;

			if (hexstr2bin(cp, &hex, 1) < 0)
				goto error;

			pattern[pos] = (char)hex;
			mask_pos = pos / 8;
			if (bit_mask)
				bit_mask[mask_pos] |= (char)(1 << (pos % 8));
		}

		pos++;
	}

	*pattern_len = pos;

	return _TRUE;
error:
	return _FALSE;
}
