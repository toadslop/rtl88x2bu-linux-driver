// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mlme_ext_band_ie_types.h"

static country_ent_t ce;
static _adapter ad;

static void hx(WLAN_BSSID_EX *n, const char *h)
{
	size_t l = strlen(h) / 2;
	unsigned v;
	for (size_t i = 0; i < l; i++) {
		sscanf(h + i * 2, "%2x", &v);
		n->IEs[i] = (u8)v;
	}
	n->IELength = (u32)l;
}

static int go(const char *tag, u8 ch, u32 wm, u8 ht, const char *in,
	      const char *ie, const char *sr, u32 len)
{
	WLAN_BSSID_EX n;
	char g[512];
	size_t i;
	unsigned v;

	memset(&n, 0, sizeof(n));
	memset(&ad, 0, sizeof(ad));
	hx(&n, in);
	ad.registrypriv.wireless_mode = wm;
	ad.mlmepriv.htpriv.ht_option = ht;
	ad.rfctl.country_ent = &ce;
	change_band_update_ie(&ad, &n, ch);
	for (i = 0; i < n.IELength; i++)
		sprintf(g + i * 2, "%02x", n.IEs[i]);
	g[n.IELength * 2] = '\0';
	if (strcmp(g, ie)) {
		fprintf(stderr, "%s ie: got %s want %s\n", tag, g, ie);
		return -1;
	}
	for (i = 0; i < strlen(sr) / 2; i++) {
		sscanf(sr + i * 2, "%2x", &v);
		if (n.SupportedRates[i] != (u8)v) {
			fprintf(stderr, "%s rates mismatch\n", tag);
			return -1;
		}
	}
	if (n.Length != len) {
		fprintf(stderr, "%s len %u != %u\n", tag, n.Length, len);
		return -1;
	}
	printf("PASS: %s\n", tag);
	return 0;
}

int main(void)
{
	if (go("24g_bg_add_erp", 6, 3, 0,
	     "0000000000000000640031000000030106010882848b960c121824",
	     "0000000000000000640031000000030106010882848b960c1218242a01043204b048606c",
	     "82848b968c129824b048606c", 180))
		return 1;
	if (go("5g_11a_strip_erp", 36, 68, 1,
	     "0000000000000000640031000000030106010882848b960c121824",
	     "0000000000000000640031000000030106010882848b960c121824",
	     "8c129824b048606c", 171))
		return 1;
	puts("All band_ie vectors passed.");
	return 0;
}
