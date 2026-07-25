// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for ccmp.c (W2-09a).
 *
 * oracle: core/crypto/ccmp.c
 */

#include <stdio.h>

#include "host_ccmp_vector.h"
#include "host_vector_json.h"

int main(int argc, char **argv)
{
	const char *path = "ccmp_vectors.json";
	struct host_ccmp_vector vecs[HOST_CCMP_MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	size_t skipped = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), HOST_CCMP_MAX_VECTORS,
			      host_ccmp_parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
#ifdef RUST_CCMP_ORACLE
		if (vecs[i].fn != HOST_CCMP_FN_DECRYPT &&
		    vecs[i].fn != HOST_CCMP_FN_256_DECRYPT) {
			printf("skip %s (rust part1)\n", vecs[i].name);
			skipped++;
			continue;
		}
#endif
#ifndef RUST_CCMP_ORACLE
		if (vecs[i].rust_only) {
			printf("skip %s (rust-only)\n", vecs[i].name);
			skipped++;
			continue;
		}
#endif
		executed++;
		if (host_ccmp_run_vector(&vecs[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vecs[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	if (skipped)
		printf("all %zu ccmp vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/ccmp.c)\n",
		       executed, skipped);
	else
		printf("all %zu ccmp vectors passed (oracle: core/crypto/ccmp.c)\n",
		       executed);
	return 0;
}
