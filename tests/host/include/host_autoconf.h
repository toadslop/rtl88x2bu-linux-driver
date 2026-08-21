/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal autoconf for host L2 tests (chplan / ie / swcrypto).
 */
#ifndef HOST_AUTOCONF_H
#define HOST_AUTOCONF_H

#define CONFIG_IEEE80211_BAND_5GHZ 1
#define CONFIG_80211N_HT
#define CONFIG_80211AC_VHT
#define CONFIG_DFS 1
#define CONFIG_AP_MODE
#define CONFIG_P2P
#define CONFIG_IEEE80211W 1
#define CONFIG_RTW_MESH_AEK 1
#define CONFIG_RTW_DEBUG 0
#define RTW_DEF_MODULE_REGULATORY_CERT 0
#define RTW_ERR(...) do { } while (0)

#endif /* HOST_AUTOCONF_H */
