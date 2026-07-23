# Rust driver architecture

This document is normative for the Rust rewrite. Phase 1 still preserves **observable behavior** of the C driver, but Rust code must not become an untyped mirror of C (`u8`/`u32`/`*mut` soup). Behavior parity is enforced by tests; structure is enforced by types.

## Goals

1. **Behavior parity first** — characterization tests lock current C behavior; ports must match.
2. **Strong domain types** — validated newtypes/enums at Rust APIs; reject illegal values at boundaries.
3. **Unsafe at the edges** — `unsafe` and raw pointers live in thin FFI / OS / HAL shims, not in protocol logic.
4. **Incremental** — each ~200 LOC PR can add types + tests for just the surface it touches.

## Layering

```text
┌─────────────────────────────────────────────────────────┐
│  os / usb / netdev / cfg80211  (RfL + thin FFI shims)   │  ← unsafe OK, small
├─────────────────────────────────────────────────────────┤
│  domain services  (mlme, xmit, recv, security, cmd…)     │  ← safe, typed
├─────────────────────────────────────────────────────────┤
│  domain types  (MacAddr, IeBuf, KeyMaterial, Channel…) │  ← validated
├─────────────────────────────────────────────────────────┤
│  abi / ffi  (bindgen #[repr(C)], extern "C" for C mix)  │  ← raw, temporary
└─────────────────────────────────────────────────────────┘
```

During the mixed C/Rust era, **abi/ffi** exists so remaining C can call Rust (and vice versa). As C disappears, call sites move to domain services and abi shrinks.

| Layer | May use | Must not |
|-------|---------|----------|
| `abi` / `ffi` | `#[repr(C)]`, bindgen, raw pointers, `extern "C"` | Business logic beyond glue |
| `domain` types | Newtypes, enums, `Result`, checked constructors | Silent truncation; public raw `*mut` |
| `domain` services | Domain types, RfL safe sync where available | Sprinkling magic integers; unchecked casts |
| `os` / HAL shims | Kernel/USB FFI, mapping to domain types at entry | Leaking raw kernel pointers into services |

## Domain typing rules

1. **No “stringly” or “intly” public APIs** in Rust modules we own. Prefer:
   - `MacAddr([u8; 6])` instead of `[u8]` / `*mut u8` for addresses
   - `Channel`, `Bandwidth`, `SecurityMode`, `KeyId` instead of bare `u8`/`u32` flags where meaning exists
   - `IeeeFrame` / `IeView` instead of untyped buffers when parsing
2. **Parse, don’t validate later.** Constructors return `Result<T, E>` (or RfL equivalents). Illegal values never construct a domain value.
3. **Bitflags and wire enums** become typed bitflags/enums with explicit `from_raw` / `to_raw` at the ABI edge only.
4. **Pointers** from C become borrows or owned wrappers as soon as control enters Rust (`&Adapter`, `Pin<&mut _>`, etc. — exact form evolves with RfL). Do not pass `*mut u8` through three Rust functions “because C did.”
5. **Transparency at the edge.** Newtypes may be `#[repr(transparent)]` so conversion to C ABI is zero-cost and obvious.
6. **Magic numbers** from headers become named constants or typed enums in `domain`, not copied literals in services.

### Phase 1 vs Phase 2 (types)

| Concern | Phase 1 (parity) | Phase 2 (idiomatic) |
|---------|------------------|---------------------|
| Observable behavior | Must match C (tests) | May change only with test updates |
| Internal Rust APIs | **Domain types required** for new/ported code | Tighten invariants; remove escape hatches |
| C ABI / remaining C | Raw `extern "C"` wrappers OK | Delete as C goes away |
| `unsafe` | Allowed in abi/os shims | Minimize; RfL abstractions |
| Control flow | Prefer faithful port inside services | Refactor once tests hold |

**Important:** “Exact translation” means **same behavior**, not “same untyped shape forever.” A ported function may accept `KeyMaterial` instead of `*const u8` + `len` **if** tests prove identical outputs for all characterized inputs, and an `extern "C"` shim preserves the old symbol for C callers.

## Characterization → unit tests (required per port)

There is no legacy suite. For each chunk we port:

```text
1. Identify behavior  →  inputs/outputs, error codes, in-place mutation
2. Capture oracle     →  run C (host build) or record vectors from C
3. Freeze as tests    →  Rust unit/host tests assert byte/return parity
4. Port implementation →  domain-typed Rust + thin ABI shim if needed
5. Keep tests green   →  later behavior changes update tests deliberately
```

### What to characterize

- **Pure functions (crypto, IE parse, chplan):** full host vectors + differential vs C (L2).
- **Stateful but isolatable logic:** table-driven cases for state transitions; mock only at OS boundary.
- **Kernel-tied code:** document observed contracts in tests where host execution is impossible; cover with L1 symbols + L3 load; add KUnit later if needed.

### Test ownership

- Tests live next to the Rust module (`#[cfg(test)]` / host `tests/`) and name the C oracle they freeze (`// oracle: core/crypto/aes-ctr.c`).
- Initially tests mean **parity with C**. When we intentionally change behavior, update tests in the same PR and note “spec change” in the PR body.

## Module layout (target)

```text
rust/
  abi/           # bindgen + extern C shims (shrinks over time)
  domain/
    types/       # MacAddr, Channel, keys, frames, …
    crypto/      # typed crypto services
    mlme/        # …
  os/            # USB, netdev, cfg80211 shims → domain
  scaffold.rs    # Wave 0 only / temporary
```

Exact paths can adjust; the **layer dependencies** must not invert (`domain` must not depend on USB details).

## PR checklist (architecture)

```markdown
## Architecture
- [ ] Characterization tests added/updated (parity with C unless spec change)
- [ ] New Rust API uses domain types (no new public raw pointers / mystery integers)
- [ ] `unsafe` confined to abi/os shim
- [ ] `extern "C"` shim present only if C still calls this symbol
- [ ] L0/L1/(L2) gates from test-plan.md
```

## Multi-part file ports (Kbuild)

Splitting a large `.c` across PRs is allowed only with an explicit link-safe mechanism:

1. Move a function group to Rust (`extern "C"` names preserved).
2. **Remove those definitions from C** (extract remainder to `foo_rest.c` or edit the original TU).
3. Makefile links the Rust object **and** the remaining C object — never the original full TU plus Rust if both define the same symbols.
4. Final part deletes the remaining C object.

Do not describe this as “mixed ownership of one `.c` file” without the extract/remove step.

## Anti-goals

- Translating C to Rust line-for-line while keeping `*mut u8` and `u32` flags as the primary API
- “We’ll add types later” for ported modules (types land with the port, even if small)
- Wide `unsafe` blocks around entire files “for convenience”
- Partial ports that leave duplicate `extern "C"` symbols in C and Rust
