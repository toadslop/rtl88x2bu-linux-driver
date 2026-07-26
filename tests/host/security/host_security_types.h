/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal security types for host L2 differential tests (T5).
 */
#ifndef HOST_SECURITY_TYPES_H
#define HOST_SECURITY_TYPES_H

#include "host_types.h"

#define CONFIG_IEEE80211W 1

enum security_type {
	_NO_PRIVACY_	= 0x00,
	_WEP40_		= 0x01,
	_TKIP_		= 0x02,
	_TKIP_WTMIC_	= 0x03,
	_AES_		= 0x04,
	_WEP104_	= 0x05,
	_SMS4_		= 0x06,
	_GCMP_		= 0x07,
	_SEC_TYPE_MAX_,

	_SEC_TYPE_256_	= 0x10,
	_CCMP_256_	= (_AES_ | _SEC_TYPE_256_),
	_GCMP_256_	= (_GCMP_ | _SEC_TYPE_256_),

	_SEC_TYPE_BIT_	= 0x20,
	_BIP_CMAC_128_	= (_SEC_TYPE_BIT_),
	_BIP_GMAC_128_	= (_SEC_TYPE_BIT_ + 1),
	_BIP_GMAC_256_	= (_SEC_TYPE_BIT_ + 2),
	_BIP_CMAC_256_	= (_SEC_TYPE_BIT_ + 3),
	_BIP_MAX_,
};

#define WPA_CIPHER_BIP_CMAC_128	BIT(8)
#define WPA_CIPHER_BIP_GMAC_128	BIT(9)
#define WPA_CIPHER_BIP_GMAC_256	BIT(10)
#define WPA_CIPHER_BIP_CMAC_256	BIT(11)

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#endif /* HOST_SECURITY_TYPES_H */
