#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
"""FX-17: verify consolidated kernel FFI shims (T23 will replace with bindgen)."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
RUST = REPO / "rust"

FN = re.compile(
    r"^\s*(?:pub\s+)?fn\s+([a-zA-Z0-9_]+)\s*\(([^)]*)\)\s*(?:->\s*([^{;]+))?\s*;",
    re.MULTILINE,
)

CANONICAL = {
    "rtw_rust_hal_com_warn_c2h_short_len": "rtw_rust_hal_com_warn_c2h_short_len(*u8,u32,i32)->()",
    "rtw_rust_hal_com_get_current_time": "rtw_rust_hal_com_get_current_time()->c_ulong",
    "rtw_rust_hal_com_get_passing_time_ms": "rtw_rust_hal_com_get_passing_time_ms(c_ulong)->u32",
    "rtw_rust_recv_sta_get_current_time": "rtw_rust_recv_sta_get_current_time()->c_ulong",
    "rtw_rust_recv_sta_get_passing_time_ms": "rtw_rust_recv_sta_get_passing_time_ms(c_ulong)->i32",
}

ONLY_FILE = {
    "rtw_rust_hal_com_warn_c2h_short_len": "ffi_hal_com_c2h.rs",
    "rtw_rust_hal_com_get_current_time": "ffi_hal_com_systime.rs",
    "rtw_rust_hal_com_get_passing_time_ms": "ffi_hal_com_systime.rs",
}


def norm_ty(t: str) -> str:
    t = t.strip().replace("*const ", "*").replace("*mut ", "*")
    t = t.replace("core::ffi::c_void", "c_void")
    t = re.sub(r"\s+", "", t)
    for a, b in (("U8", "u8"), ("U32", "u32"), ("S32", "i32"), ("Systime", "c_ulong"), ("bool", "bool")):
        t = t.replace(a, b)
    return t


def sig(name: str, args: str, ret: str | None) -> str:
    parts = []
    for p in [x.strip() for x in args.split(",") if x.strip()]:
        parts.append(norm_ty(p.split(":")[-1]))
    r = norm_ty(ret or "()")
    return f"{name}({','.join(parts)})->{r}"


def decls_in(path: Path) -> dict[str, str]:
    text = path.read_text(encoding="utf-8", errors="replace")
    out: dict[str, str] = {}
    for m in FN.finditer(text):
        out[m.group(1)] = sig(m.group(1), m.group(2) or "", m.group(3))
    return out


def main() -> int:
    errors: list[str] = []
    for path in sorted(RUST.glob("*.rs")):
        for name, s in decls_in(path).items():
            if name not in CANONICAL:
                continue
            if ONLY_FILE.get(name) and path.name != ONLY_FILE[name]:
                errors.append(f"{name}: must be declared only in {ONLY_FILE[name]}, also in {path.name}")
            if s != CANONICAL[name]:
                errors.append(f"{name} in {path.name}: got `{s}`, want `{CANONICAL[name]}`")
    if errors:
        for e in errors:
            print(e, file=sys.stderr)
        return 1
    print("check-ffi-drift: OK (FX-17 consolidated shims)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
