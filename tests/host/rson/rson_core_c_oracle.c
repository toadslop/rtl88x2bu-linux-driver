// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"
#include <string.h>

unsigned char RTW_RSON_OUI[] = {0xFA, 0xFA, 0xFA};

void init_rtw_rson_data(struct dvobj_priv *dvobj)
{
	dvobj->rson_data.ver = RTW_RSON_VER;
	dvobj->rson_data.id = CONFIG_RTW_REPEATER_SON_ID;
	dvobj->rson_data.hopcnt = RTW_RSON_HC_NOTREADY;
	dvobj->rson_data.connectible = RTW_RSON_DENYCONNECT;
	dvobj->rson_data.loading = 0;
	memset(dvobj->rson_data.res, 0xAA, sizeof(dvobj->rson_data.res));
}

int str2hexbuf(char *str, u8 *hexbuf, int len)
{
	u8 *p;
	int i, slen, idx = 0;

	p = (unsigned char *)str;
	if ((*p != '0') || (*(p + 1) != 'x'))
		return _FALSE;
	slen = (int)strlen((char *)str);
	if (slen > (len * 2) + 2)
		return _FALSE;
	p += 2;
	for (i = 0; i < len; i++, idx = idx + 2) {
		hexbuf[i] = key_2char2num(p[idx], p[idx + 1]);
		if (slen <= idx + 2)
			break;
	}
	return _TRUE;
}

u8 rtw_rson_varify_ie(u8 *p)
{
	u8 ver;

	ver = *(p + 2 + sizeof(RTW_RSON_OUI));
	return ver == 1 ? _TRUE : _FALSE;
}
