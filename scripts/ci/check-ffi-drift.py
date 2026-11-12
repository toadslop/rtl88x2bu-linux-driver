#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
"""T23: FFI drift checks (FX-17 shims + duplicate extern \"C\" declarations)."""

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
EXTERN_BLOCK = re.compile(r'extern\s+"C"\s*\{([^}]*)\}', re.DOTALL)
EXPORT_FN = re.compile(
    r"#\[no_mangle\]\s+pub\s+extern\s+\"C\"\s+fn\s+([a-zA-Z0-9_]+)\s*\(([^)]*)\)"
    r"\s*(?:->\s*([^{]+))?\s*\{",
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

SKIP_FILES = {
    "bindings/generated.rs",
    "kbuild_stub.rs",
    "scaffold.rs",
    "ffi.rs",
}


def norm_ty(t: str) -> str:
    t = t.strip().replace("*const ", "*").replace("*mut ", "*")
    t = t.replace("core::ffi::c_void", "c_void")
    t = re.sub(r"\s+", "", t)
    for a, b in (
        ("U8", "u8"),
        ("U32", "u32"),
        ("S32", "i32"),
        ("Systime", "c_ulong"),
        ("bool", "bool"),
        ("c_ulong", "c_ulong"),
        ("c_int", "i32"),
    ):
        t = t.replace(a, b)
    return t


def sig(name: str, args: str, ret: str | None) -> str:
    parts = []
    for p in [x.strip() for x in args.split(",") if x.strip()]:
        parts.append(norm_ty(p.split(":")[-1]))
    r = norm_ty(ret.strip()) if ret and ret.strip() else "()"
    return f"{name}({','.join(parts)})->{r}"


def decls_in_text(text: str) -> dict[str, str]:
    out: dict[str, str] = {}
    for m in FN.finditer(text):
        out[m.group(1)] = sig(m.group(1), m.group(2) or "", m.group(3))
    return out


def kernel_rust_files() -> list[Path]:
    files: list[Path] = []
    for path in sorted(RUST.glob("*.rs")):
        if path.name.endswith("_host.rs"):
            continue
        if path.name in SKIP_FILES or path.name.startswith("bindings"):
            continue
        files.append(path)
    return files


def import_decls(path: Path, text: str) -> dict[str, str]:
    out: dict[str, str] = {}
    for block in EXTERN_BLOCK.finditer(text):
        out.update(decls_in_text(block.group(1)))
    return out


def export_decls(text: str) -> dict[str, str]:
    out: dict[str, str] = {}
    for m in EXPORT_FN.finditer(text):
        out[m.group(1)] = sig(m.group(1), m.group(2) or "", m.group(3))
    return out


def check_duplicates(
    by_name: dict[str, list[tuple[str, str]]],
    label: str,
) -> list[str]:
    errors: list[str] = []
    for name, entries in sorted(by_name.items()):
        sigs = {s for _, s in entries}
        if len(sigs) <= 1:
            continue
        detail = "; ".join(f"{f}:{s}" for f, s in entries)
        errors.append(f"{label} `{name}` has inconsistent signatures: {detail}")
    return errors


def main() -> int:
    import os

    report_dupes = os.environ.get("FFI_DRIFT_REPORT_DUPES") == "1"
    errors: list[str] = []

    imports: dict[str, list[tuple[str, str]]] = {}
    exports: dict[str, list[tuple[str, str]]] = {}

    for path in kernel_rust_files():
        text = path.read_text(encoding="utf-8", errors="replace")
        for name, s in import_decls(path, text).items():
            imports.setdefault(name, []).append((path.name, s))
        for name, s in export_decls(text).items():
            exports.setdefault(name, []).append((path.name, s))

        for name, s in decls_in_text(text).items():
            if name not in CANONICAL:
                continue
            if ONLY_FILE.get(name) and path.name != ONLY_FILE[name]:
                errors.append(
                    f"{name}: must be declared only in {ONLY_FILE[name]}, also in {path.name}"
                )
            if s != CANONICAL[name]:
                errors.append(f"{name} in {path.name}: got `{s}`, want `{CANONICAL[name]}`")

    if report_dupes:
        rust_imports = {k: v for k, v in imports.items() if k.startswith("rtw_rust_")}
        rust_exports = {k: v for k, v in exports.items() if k.startswith("rtw_rust_")}
        errors.extend(check_duplicates(rust_imports, "rtw_rust import"))
        errors.extend(check_duplicates(rust_exports, "rtw_rust export"))

    if errors:
        for e in errors:
            print(e, file=sys.stderr)
        return 1
    print(
        "check-ffi-drift: OK "
        f"({len(imports)} import(s), {len(exports)} export(s), FX-17 shims)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
