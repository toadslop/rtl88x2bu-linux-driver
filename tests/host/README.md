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
    test_aes_ctr.c   # oracle runner (C oracle + Rust staticlib)
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
