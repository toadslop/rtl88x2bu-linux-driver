---
title: "[T7] CI: L1 symbol/ABI checks on C→Rust swaps"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T7
epic: E10
blocked_by: [T6]
estimate_loc: 150
---

## Goal

Automate the **L1 symbol gate** in CI so translation PRs cannot merge if exported `extern "C"` symbols disappear or change binding. Today `check-symbols.sh` and `make rust-check-symbols*` exist locally ([T1](../test-01-symbol-check.md)) but nothing runs them in GitHub Actions.

## Proposed approach

1. After the L0 job produces `rust/*.o` (depends on **T6**), run the existing Make targets, e.g.:

   ```bash
   make KDIR=/opt/linux LLVM=1 rust-check-symbols-selftest
   # For PRs that change a specific unit, run the matching target:
   # rust-check-symbols-rtw-security, rust-check-symbols-rtw-chplan, etc.
   ```

2. **Scope detection (pick one):**
   - **Simple (phase 1):** run `rust-check-symbols-selftest` plus a fixed list of aggregate targets on every `rust/**` change.
   - **Better (phase 2):** small script maps changed files → `rust-check-symbols-*` target(s); skip when only docs/tests change.

3. Fail the job on any symbol regression unless an allowlist file is added in the same PR (documented in PR template).

## Acceptance

- CI runs at least `rust-check-symbols-selftest` on `rust/**` changes
- CI runs the relevant per-unit `rust-check-symbols-*` target when that unit's Rust or C oracle sources change
- Failure message points to [`test-plan.md`](../test-plan.md#L1) and allowlist format
- No false greens from host-`gcc` reference objects where kbuild `OLD.o` is required (document exceptions)

## Out of scope

- Bit-exact `.ko` comparison
- Automatic allowlist generation
