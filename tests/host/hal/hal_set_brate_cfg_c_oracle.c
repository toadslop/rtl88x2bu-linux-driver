// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-13 HalSetBrateCfg (from hal/hal_com_rest.c). */

#include <stdint.h>

#define NDIS_802_11_LENGTH_RATES_EX 16

#define IEEE80211_CCK_RATE_1MB 0x02
#define IEEE80211_CCK_RATE_2MB 0x04
#define IEEE80211_CCK_RATE_5MB 0x0b
#define IEEE80211_CCK_RATE_11MB 0x16
#define IEEE80211_OFDM_RATE_6MB 0x0c
#define IEEE80211_OFDM_RATE_9MB 0x12
#define IEEE80211_OFDM_RATE_12MB 0x18
#define IEEE80211_OFDM_RATE_18MB 0x24
#define IEEE80211_OFDM_RATE_24MB 0x30
#define IEEE80211_OFDM_RATE_36MB 0x48
#define IEEE80211_OFDM_RATE_48MB 0x60
#define IEEE80211_OFDM_RATE_54MB 0x6c
#define IEEE80211_BASIC_RATE_MASK 0x80

#define RATE_1M (1u << 0)
#define RATE_2M (1u << 1)
#define RATE_5_5M (1u << 2)
#define RATE_11M (1u << 3)
#define RATE_6M (1u << 4)
#define RATE_9M (1u << 5)
#define RATE_12M (1u << 6)
#define RATE_18M (1u << 7)
#define RATE_24M (1u << 8)
#define RATE_36M (1u << 9)
#define RATE_48M (1u << 10)
#define RATE_54M (1u << 11)

void HalSetBrateCfg(void *adapter, uint8_t *mBratesOS, uint16_t *pBrateCfg)
{
	uint8_t i, is_brate, brate;

	(void)adapter;

	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		is_brate = mBratesOS[i] & IEEE80211_BASIC_RATE_MASK;
		brate = mBratesOS[i] & 0x7f;

		if (is_brate) {
			switch (brate) {
			case IEEE80211_CCK_RATE_1MB:
				*pBrateCfg |= RATE_1M;
				break;
			case IEEE80211_CCK_RATE_2MB:
				*pBrateCfg |= RATE_2M;
				break;
			case IEEE80211_CCK_RATE_5MB:
				*pBrateCfg |= RATE_5_5M;
				break;
			case IEEE80211_CCK_RATE_11MB:
				*pBrateCfg |= RATE_11M;
				break;
			case IEEE80211_OFDM_RATE_6MB:
				*pBrateCfg |= RATE_6M;
				break;
			case IEEE80211_OFDM_RATE_9MB:
				*pBrateCfg |= RATE_9M;
				break;
			case IEEE80211_OFDM_RATE_12MB:
				*pBrateCfg |= RATE_12M;
				break;
			case IEEE80211_OFDM_RATE_18MB:
				*pBrateCfg |= RATE_18M;
				break;
			case IEEE80211_OFDM_RATE_24MB:
				*pBrateCfg |= RATE_24M;
				break;
			case IEEE80211_OFDM_RATE_36MB:
				*pBrateCfg |= RATE_36M;
				break;
			case IEEE80211_OFDM_RATE_48MB:
				*pBrateCfg |= RATE_48M;
				break;
			case IEEE80211_OFDM_RATE_54MB:
				*pBrateCfg |= RATE_54M;
				break;
			default:
				break;
			}
		}
	}
}
