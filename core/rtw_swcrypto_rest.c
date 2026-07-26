/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifdef HOST_SWCRYPTO_TEST
#include "host_autoconf.h"
#include "host_crypto_wrap.h"
#include "aes.h"
#include "wlancrypto_wrap.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#include <aes.h>
#include <sha256.h>
#include <wlancrypto_wrap.h>
#endif /* HOST_SWCRYPTO_TEST */

void rtw_swcrypto_log_err(const char *msg)
{
#ifdef HOST_SWCRYPTO_TEST
	(void)msg;
#else
	RTW_ERR("%s", msg);
#endif
}

#ifdef CONFIG_TDLS
#ifndef HOST_SWCRYPTO_WRAPPER_ONLY
void _tdls_generate_tpk(void *sta, const u8 *own_addr, const u8 *bssid)
{
	struct sta_info *psta = (struct sta_info *)sta;
	u8 *SNonce = psta->SNonce;
	u8 *ANonce = psta->ANonce;

	u8 key_input[SHA256_MAC_LEN];
	const u8 *nonce[2];
	size_t len[2];
	u8 data[3 * ETH_ALEN];

	/* IEEE Std 802.11z-2010 8.5.9.1:
	 * TPK-Key-Input = SHA-256(min(SNonce, ANonce) || max(SNonce, ANonce))
	 */
	len[0] = 32;
	len[1] = 32;
	if (_rtw_memcmp2(SNonce, ANonce, 32) < 0) {
		nonce[0] = SNonce;
		nonce[1] = ANonce;
	} else {
		nonce[0] = ANonce;
		nonce[1] = SNonce;
	}

	sha256_vector(2, nonce, len, key_input);

	/*
	 * TPK = KDF-Hash-Length(TPK-Key-Input, "TDLS PMK",
	 *	min(MAC_I, MAC_R) || max(MAC_I, MAC_R) || BSSID)
	 */

	if (_rtw_memcmp2(own_addr, psta->cmn.mac_addr, ETH_ALEN) < 0) {
		_rtw_memcpy(data, own_addr, ETH_ALEN);
		_rtw_memcpy(data + ETH_ALEN, psta->cmn.mac_addr, ETH_ALEN);
	} else {
		_rtw_memcpy(data, psta->cmn.mac_addr, ETH_ALEN);
		_rtw_memcpy(data + ETH_ALEN, own_addr, ETH_ALEN);
	}

	_rtw_memcpy(data + 2 * ETH_ALEN, bssid, ETH_ALEN);

	sha256_prf(key_input, SHA256_MAC_LEN, "TDLS PMK", data, sizeof(data),
		   (u8 *)&psta->tpk, sizeof(psta->tpk));
}
#endif /* !HOST_SWCRYPTO_WRAPPER_ONLY */
#endif /* CONFIG_TDLS */
