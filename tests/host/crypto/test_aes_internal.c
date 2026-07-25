// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-internal.c (W2-11a).
 *
 * oracle: core/crypto/aes-internal.c + aes-internal-enc.c
 */

#include <stdio.h>

#include "host_aes_internal_vector.h"
#include "host_vector_json.h"

int main(int argc, char **argv)
{
	const char *path = "aes_internal_vectors.json";
	struct host_aes_internal_vector vecs[HOST_AES_INTERNAL_MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	size_t skipped = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]),
			      HOST_AES_INTERNAL_MAX_VECTORS,
			      host_aes_internal_parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
#ifndef RUST_AES_INTERNAL_ORACLE
		if (vecs[i].rust_only) {
			printf("skip %s (rust-only)\n", vecs[i].name);
			skipped++;
			continue;
		}
#endif
		executed++;
		if (host_aes_internal_run_vector(&vecs[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vecs[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	if (skipped)
		printf("all %zu aes-internal vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/aes-internal*.c)\n",
		       executed, skipped);
	else
		printf("all %zu aes-internal vectors passed "
		       "(oracle: core/crypto/aes-internal*.c)\n",
		       executed);
	return 0;
}
