# Host differential tests (L2)

Userspace parity tests for pure crypto units. No kernel headers or `KDIR` required — only `gcc` and `make`.

## Layout

```text
tests/host/
  include/           # userspace shims (types, crypto wrap)
  shim/              # allocator stubs for C oracle objects
  domain/            # A1 domain-type unit tests (rustc --test)
  crypto/
    host_vector_json.c   # shared hex/JSON helpers for vector fixtures
    aes_ctr_vectors.json
    test_aes_ctr.c   # oracle runner (C oracle + Rust staticlib)
    aes_omac1_vectors.json
    test_aes_omac1.c # oracle runner for OMAC1/CMAC (W2-01)
    host_gcmp_vector.c   # GCMP vector parse/run helpers (W2-02b)
    gcmp_vectors.json
    test_gcmp.c          # C-oracle runner for GCMP (W2-02c)
    test_aes_siv.c       # C-oracle runner for AES-SIV (W2-03a)
    aes_siv_vectors.json
    test_aes_ccm.c       # C-oracle runner for AES-CCM (W2-04a)
    aes_ccm_vectors.json
    test_sha256_internal.c   # C-oracle runner for SHA-256 (W2-05a)
    sha256_internal_vectors.json
    test_sha256_prf.c        # C-oracle runner for SHA256-PRF (W2-06a)
    sha256_prf_vectors.json
    Makefile
```

Driver crypto sources are compiled with `-DHOST_CRYPTO_TEST`, which makes
`core/crypto/rtw_crypto_wrap.h` include the host shim instead of `drv_types.h`.

## Run (domain types, A1)

```bash
make -C tests/host/domain test
```

## Run (aes-ctr parity)

```bash
make -C tests/host/crypto test
```

## Run (aes-omac1 parity, W2-01)

```bash
make -C tests/host/crypto test-omac1
```

Vectors characterize observable behavior of `core/crypto/aes-omac1.c` (CMAC
output, multi-fragment inputs, return codes). Success-path vectors run against
both the C oracle and Rust staticlib; `rust_only` vectors document stricter
shim validation (`num_elem == 0`, null `mac`, etc.) and run only on the Rust
path.

## aes-ctr details

This runs both paths against `aes_ctr_vectors.json`:

- **`test-c`** — links the in-tree C objects (`aes-internal.c`, `aes-internal-enc.c`, `aes-ctr.c`) and exercises the C oracle.
- **`test-rust`** — links `rust/aes_ctr.rs` as a userspace staticlib (no `aes-ctr.c`) and exercises the Rust `extern "C"` shims.

Vectors characterize observable behavior of `core/crypto/aes-ctr.c` (in-place
XOR, return codes, 128/192/256-bit keys, counter increment). They are **not**
NIST/OpenSSL oracles unless they happen to match.

`aes_ctr_vectors.json` is parsed by a tiny hand-rolled scanner in
`test_aes_ctr.c` (not a full JSON library). Keep vector fields at the **top
level** of each object — do not embed key names inside string values — or
extend the parser / switch to `jq` in CI when fixtures grow.

## W1-03 pilot (done)

The Rust port landed in `rust/aes_ctr.rs`:

1. `aes_ctr_vectors.json` is unchanged — both `test-c` and `test-rust` must pass.
2. Typed logic lives in `aes_ctr_encrypt_typed()`; `aes_ctr_encrypt` / `aes_128_ctr_encrypt` are thin C ABI shims.
3. `aes-ctr.o` was removed from the module link; `rust/aes_ctr.o` is linked instead (see `docs/rust-migration.md` swap recipe).

See also [`docs/rust-migration/test-plan.md`](../../docs/rust-migration/test-plan.md)
(L2 gate) and issue `wave1-03-pilot-aes-ctr.md`.

## Run (gcmp parity, W2-02c)

```bash
make -C tests/host/crypto test-gcmp-c
```

Compile-only gate (shim + `gcmp.c` / `aes-gcm.c` build under `HOST_CRYPTO_TEST`):

```bash
make -C tests/host/crypto test-gcmp-compile
```

`make -C tests/host/crypto test-gcmp` runs both C and Rust oracles (`test-gcmp-c` + `test-gcmp-rust`).

## Run (aes-siv parity, W2-03a)

```bash
make -C tests/host/crypto test-aes-siv-c
```

Compile-only gate (shim + `aes-siv.c` and dependencies build under `HOST_CRYPTO_TEST`):

```bash
make -C tests/host/crypto test-aes-siv-compile
```

`make -C tests/host/crypto test-aes-siv` runs both C and Rust oracles
(`test-aes-siv-c` + `test-aes-siv-rust`) and is included in the default `all` target
(`make -C tests/host/crypto`) alongside ctr, omac1, and gcmp.

## aes-siv details

W2-03a adds `aes_siv_vectors.json` and the C-oracle runner. W2-03b adds the Rust
oracle. W2-03c swaps `aes-siv.o` for `rust/aes_siv.o` in the module link.

- **`test-aes-siv-c`** — links the in-tree C objects (`aes-omac1.c`, `aes-ctr.c`, `aes-siv.c`)
  and exercises the C oracle against `aes_siv_vectors.json`.
- **`test-aes-siv-rust`** — links `rust/aes_siv.rs` as a userspace staticlib and
  exercises the Rust `extern "C"` shims against the same vectors.
- **`test-aes-siv-compile`** — compile-only gate; no link/run.

Vectors characterize observable behavior of `core/crypto/aes-siv.c` (encrypt/decrypt,
AES-128/192/256 keys, zero/multi/long associated data, tampered decrypt, bad key length,
`num_elem` overflow). A missing or non-array `elements` key is parsed as zero associated
data (valid for SIV). `rust_only` vectors are skipped by the C runner and exercised on the
Rust path.

## Run (aes-ccm parity, W2-04a)

```bash
make -C tests/host/crypto test-aes-ccm-c
```

Compile-only gate (shim + `aes-ccm.c` build under `HOST_CRYPTO_TEST`):

```bash
make -C tests/host/crypto test-aes-ccm-compile
```

`make -C tests/host/crypto test-aes-ccm` runs both C and Rust oracles
(`test-aes-ccm-c` + `test-aes-ccm-rust`) and is included in the default `all` target.

## aes-ccm details

W2-04a adds `aes_ccm_vectors.json`, the C-oracle runner, and `WPA_PUT_BE16` in the
host Wi-Fi shim (required by `aes-ccm.c` under `HOST_CRYPTO_TEST`). W2-04b adds the
Rust oracle (`rust/aes_ccm.rs`). W2-04c swaps `aes-ccm.o` for `rust/aes_ccm.o` in
the module link.

- **`test-aes-ccm-c`** — links in-tree C objects (`aes-internal*.c`, `aes-ccm.c`) and
  exercises the C oracle against `aes_ccm_vectors.json`.
- **`test-aes-ccm-rust`** — links `rust/aes_ccm.rs` as a userspace staticlib and
  exercises the Rust `extern "C"` shims against the same vectors.
- **`test-aes-ccm-compile`** — compile-only gate; no link/run.

Vectors characterize observable behavior of `core/crypto/aes-ccm.c` (fixed `L=2`,
`aad_len <= 30`): encrypt/decrypt, empty/partial/multi-block plaintext, zero and
long AAD (including the `aad_len == 30` accept boundary), AES-128/192/256 keys,
`M=8`/`M=16`, `M=0` accept quirk, auth mismatch (including C's decrypt-before-MAC
plaintext side-effect), bad key length, and reject paths.

## sha256-internal details (W2-05a)

```bash
make -C tests/host/crypto test-sha256-internal-c
```

Compile-only gate (shim + `sha256-internal.c` build under `HOST_CRYPTO_TEST`):

```bash
make -C tests/host/crypto test-sha256-internal-compile
```

`make -C tests/host/crypto test-sha256-internal` runs both C and Rust oracles
(`test-sha256-internal-c` + `test-sha256-internal-rust`) and is included in the
default `all` target. W2-05b adds the Rust oracle (`rust/sha256_internal.rs`).
W2-05c swaps `sha256-internal.o` for `rust/sha256_internal.o` in the module link.

Vectors characterize observable behavior of `core/crypto/sha256-internal.c`
(`sha256_vector`): empty/single/multi-block messages, multi-fragment inputs, and
empty-element edge cases.

## sha256-prf details (W2-06a)

```bash
make -C tests/host/crypto test-sha256-prf-c
```

Compile-only gate (shim + `sha256-prf.c` dependency chain under `HOST_CRYPTO_TEST`):

```bash
make -C tests/host/crypto test-sha256-prf-compile
```

`make -C tests/host/crypto test-sha256-prf` runs both C and Rust oracles
(`test-sha256-prf-c` + `test-sha256-prf-rust`) and is included in the default
`all` target. W2-06b adds the Rust oracle (`rust/sha256_prf.rs`). W2-06c swaps
`sha256-prf.o` for `rust/sha256_prf.o` in the module link.

Vectors characterize observable behavior of `core/crypto/sha256-prf.c`
(`sha256_prf` / `sha256_prf_bits`): FT derivation, non-byte-aligned bit
lengths, short keys, and long data bindings.

## rtw-crypto-wrap details (W2-06d)

```bash
make -C tests/host/crypto test-rtw-crypto-wrap-c
```

Compile-only gate (`rtw_crypto_wrap.c` built without `HOST_CRYPTO_TEST` via
`tests/host/include/drv_types.h`):

```bash
make -C tests/host/crypto test-rtw-crypto-wrap-compile
```

`make -C tests/host/crypto test-rtw-crypto-wrap` runs both C and Rust oracles
(`test-rtw-crypto-wrap-c` + `test-rtw-crypto-wrap-rust`) and is included in the
default `all` target. W2-06e adds the Rust oracle (`rust/rtw_crypto_wrap.rs`).
W2-06f swaps `rtw_crypto_wrap.o` for `rust/rtw_crypto_wrap.o` in the module link.

Vectors characterize observable behavior of `core/crypto/rtw_crypto_wrap.c`
(`os_memcmp_const`, `os_strlen`, `os_memdup`, `forced_memzero`,
`rtw_registrypriv_amsdu_mode`).

## gcmp details

W2-02b added `host_gcmp_vector.c` (shared parse/run helpers). W2-02c adds the
C-oracle runner and `gcmp_vectors.json` fixture. W2-02e adds the Rust oracle.

- **`test-gcmp-c`** — links the in-tree C objects (`aes-gcm.c`, `gcmp.c`) and
  exercises the C oracle against `gcmp_vectors.json`.
- **`test-gcmp-rust`** — links `rust/gcmp.rs` as a userspace staticlib and
  exercises the Rust `extern "C"` shims against the same vectors.
- **`test-gcmp-compile`** — compile-only gate; no link/run.

Vectors characterize observable behavior of `core/crypto/gcmp.c` (encrypt/decrypt,
QoS/SPP A-MSDU mode, embedded PN, return codes). `rust_only` vectors are skipped
by the C runner and exercised on the Rust path.
