/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_FT_TYPES_H
#define HOST_FT_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define RTW_FT_MAX_IE_SZ 256
#define RTW_FT_EN 0x01
#define RTW_FT_BTM_ROAM 0x10
#define EID_WPA2 48

typedef int sint;
typedef unsigned int uint;

struct ft_roam_info {
	u8 ft_flags;
	bool ft_updated_bcn;
	u8 updated_ft_ies[RTW_FT_MAX_IE_SZ];
	u16 updated_ft_ies_len;
};

struct mlme_priv {
	struct ft_roam_info ft_roam;
};

struct pkt_attrib {
	uint pktlen;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen);

void host_ft_info_init(struct ft_roam_info *pft);
u8 host_ft_update_rsnie(_adapter *padapter, u8 bwrite, struct pkt_attrib *pattrib,
			u8 **pframe);

#endif /* HOST_FT_TYPES_H */
