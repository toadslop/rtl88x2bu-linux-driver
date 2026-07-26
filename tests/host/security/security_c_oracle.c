// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle slice: core/rtw_security.c type-string helpers (T5 / W3-04).
 */
#include "host_security_types.h"

static const char *_security_type_str[] = {
	"N/A",
	"WEP40",
	"TKIP",
	"TKIP_WM",
	"AES",
	"WEP104",
	"SMS4",
	"GCMP",
};

static const char *_security_type_bip_str[] = {
	"BIP_CMAC_128",
	"BIP_GMAC_128",
	"BIP_GMAC_256",
	"BIP_CMAC_256",
};

const char *security_type_str(u8 value)
{
#ifdef CONFIG_IEEE80211W
	if ((_BIP_MAX_ > value) && (value >= _BIP_CMAC_128_))
		return _security_type_bip_str[value & ~_SEC_TYPE_BIT_];
#endif

	if (_CCMP_256_ == value)
		return "CCMP_256";
	if (_GCMP_256_ == value)
		return "GCMP_256";

	if (_SEC_TYPE_MAX_ > value)
		return _security_type_str[value];

	return NULL;
}

#ifdef CONFIG_IEEE80211W
u32 security_type_bip_to_gmcs(enum security_type type)
{
	switch (type) {
	case _BIP_CMAC_128_:
		return WPA_CIPHER_BIP_CMAC_128;
	case _BIP_GMAC_128_:
		return WPA_CIPHER_BIP_GMAC_128;
	case _BIP_GMAC_256_:
		return WPA_CIPHER_BIP_GMAC_256;
	case _BIP_CMAC_256_:
		return WPA_CIPHER_BIP_CMAC_256;
	default:
		return 0;
	}
}
#endif
