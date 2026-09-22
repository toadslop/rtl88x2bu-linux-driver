// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include "host_rm_parse_types.h"

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *p, size_t sz)
{
	(void)sz;
	free(p);
}

int rm_en_cap_chk_and_set(struct rm_obj *prm, enum rm_cap_en en)
{
	(void)prm;
	(void)en;
	return _SUCCESS;
}
