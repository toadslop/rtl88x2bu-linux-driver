// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for core/rtw_swcrypto.c CCMP/GCMP wrappers (W3-01).
 *
 * oracle: core/rtw_swcrypto.c
 */

#include <stdio.h>

#include "host_swcrypto_vector.h"
#include "host_vector_json.h"

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

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu swcrypto wrapper vectors passed (oracle: core/rtw_swcrypto.c)\n",
	       nvec);
	return 0;
}
