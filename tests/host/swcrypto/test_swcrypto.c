// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for core/rtw_swcrypto.c CCMP/GCMP wrappers (W3-01).
 *
 * oracle: core/rtw_swcrypto.c
 */

#include <stdio.h>
#include <string.h>

#include "host_swcrypto_vector.h"
#include "host_vector_json.h"
#include "host_wifi_types.h"

#ifdef RUST_SWCRYPTO_ORACLE
int _rtw_ccmp_decrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);
int _rtw_gcmp_decrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);

/*
 * A received frame shorter than its 802.11 header must be rejected, not
 * underflow `plen - hdrlen` (which panics under overflow checks). The C
 * oracle is skipped: it wraps into a ~4 GiB allocation request.
 */
static int run_runt_frame_checks(void)
{
	u8 key[32];
	u8 frame[64];
	unsigned int hdrlen = 26, plen;
	int failed = 0;

	memset(key, 0x11, sizeof(key));
	memset(frame, 0, sizeof(frame));

	for (plen = 0; plen < hdrlen; plen++) {
		if (_rtw_ccmp_decrypt(NULL, key, 16, hdrlen, frame, plen) != 0 ||
		    _rtw_ccmp_decrypt(NULL, key, 32, hdrlen, frame, plen) != 0 ||
		    _rtw_gcmp_decrypt(NULL, key, 16, hdrlen, frame, plen) != 0 ||
		    _rtw_gcmp_decrypt(NULL, key, 32, hdrlen, frame, plen) != 0) {
			fprintf(stderr, "runt frame plen=%u not rejected\n", plen);
			failed++;
		}
	}
	if (!failed)
		printf("ok ccmp/gcmp decrypt reject runt frames (plen < hdrlen)\n");
	return failed;
}
#endif

int main(int argc, char **argv)
{
	const char *path = "swcrypto_vectors.json";
	struct host_swcrypto_vector vecs[HOST_SWCRYPTO_MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), HOST_SWCRYPTO_MAX_VECTORS,
			      host_swcrypto_parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (host_swcrypto_run_vector(&vecs[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vecs[i].name);
	}

#ifdef RUST_SWCRYPTO_ORACLE
	failed += run_runt_frame_checks();
#endif

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu swcrypto wrapper vectors passed (oracle: core/rtw_swcrypto.c)\n",
	       nvec);
	return 0;
}
