# Host differential tests (L2)

Userspace parity tests for pure crypto units. No kernel headers or `KDIR` required — only `gcc` and `make`.

## Layout

```text
tests/host/
  include/           # userspace shims (types, crypto wrap)
  shim/              # allocator stubs for C oracle objects
  domain/            # A1 domain-type unit tests (rustc --test)
  crypto/
    aes_ctr_vectors.json
    test_aes_ctr.c   # oracle runner (C today; Rust in W1-03)
    Makefile
```

Driver crypto sources are compiled with `-DHOST_CRYPTO_TEST`, which makes
`core/crypto/rtw_crypto_wrap.h` include the host shim instead of `drv_types.h`.

## Run (domain types, A1)

```bash
make -C tests/host/domain test
```

## Run (aes-ctr oracle)

```bash
make -C tests/host/crypto test
```

This builds `test_aes_ctr`, links the in-tree C objects (`aes-internal.c`,
`aes-internal-enc.c`, `aes-ctr.c`), and runs every vector in
`aes_ctr_vectors.json` against the **current C implementation**.

Vectors characterize observable behavior of `core/crypto/aes-ctr.c` (in-place
XOR, return codes, 128/192/256-bit keys, counter increment). They are **not**
NIST/OpenSSL oracles unless they happen to match.

`aes_ctr_vectors.json` is parsed by a tiny hand-rolled scanner in
`test_aes_ctr.c` (not a full JSON library). Keep vector fields at the **top
level** of each object — do not embed key names inside string values — or
extend the parser / switch to `jq` in CI when fixtures grow.

## W1-03 (Rust pilot)

When `aes-ctr.c` is ported to Rust:

1. Keep `aes_ctr_vectors.json` unchanged.
2. Implement typed Rust logic + thin `extern "C"` shims for `aes_ctr_encrypt` /
   `aes_128_ctr_encrypt`.
3. Extend the harness (or add a sibling binary) to run the **same JSON** against
   the Rust objects/userspace build and require byte/return parity with these
   fixtures.
4. Only after L0–L2 are green, drop `aes-ctr.o` from the module link (W1-04).

See also [`docs/rust-migration/test-plan.md`](../../docs/rust-migration/test-plan.md)
(L2 gate) and issue `wave1-03-pilot-aes-ctr.md`.
