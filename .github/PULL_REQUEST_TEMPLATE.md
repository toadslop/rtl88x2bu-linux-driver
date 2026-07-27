Before opening a translation PR, read [`docs/rust-migration/test-plan.md`](docs/rust-migration/test-plan.md) and [`docs/rust-migration/architecture.md`](docs/rust-migration/architecture.md).

Check only the items that apply to this PR's scope. L1, L2, and L3 are required when the change touches a C→Rust swap, pure/leaf code, or init/USB registration respectively (see the merge rule in the test plan).

## Verification

- [ ] Characterization: C behavior identified; vectors/tests added before or with the port
- [ ] L0: builds with pinned KDIR + LLVM=1
- [ ] L1: symbol check vs previous .o (attached or CI log)
- [ ] L2: host/unit parity tests passing (if pure chunk)
- [ ] L3: VM insmod/rmmod (if init/USB registration touched)
- [ ] L4: hardware smoke (only if this closes a wave/milestone)

## Architecture

- [ ] Domain types at Rust API (no new public raw pointers / mystery integers)
- [ ] `unsafe` confined to abi/os shim
- [ ] `extern "C"` shim only if C still calls this symbol
- [ ] Behavior change vs C? If yes, tests updated and called out as spec change
