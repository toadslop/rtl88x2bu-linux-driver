// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for gcmp.c (W2-02b).
 *
 * oracle: core/crypto/gcmp.c
 */

#include <stdio.h>

#include "host_gcmp_vector.h"
#include "host_vector_json.h"

int main(int argc, char **argv)
{
	const char *path = "gcmp_vectors.json";
	struct host_gcmp_vector vecs[HOST_GCMP_MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	size_t skipped = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), HOST_GCMP_MAX_VECTORS,
			      host_gcmp_parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
#ifndef RUST_GCMP_ORACLE
		if (vecs[i].rust_only) {
			printf("skip %s (rust-only)\n", vecs[i].name);
			skipped++;
			continue;
		}
#endif
		executed++;
		if (host_gcmp_run_vector(&vecs[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vecs[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	if (skipped)
		printf("all %zu gcmp vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/gcmp.c)\n",
		       executed, skipped);
	else
		printf("all %zu gcmp vectors passed (oracle: core/crypto/gcmp.c)\n",
		       executed);
	return 0;
}
