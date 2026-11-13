#!/usr/bin/env python3
"""List cfg names referenced under rust/**/*.rs (kernel build parity — T21)."""
from __future__ import annotations

import pathlib
import re
import sys

EXTRA_WANTS = (
    "config_80211d",
    "config_usb_hci",
    "CONFIG_USE_USB_BUFFER_ALLOC_RX",
    "config_find_best_channel",
    "config_p2p_ps",
    "dbg_config_error_detect",
)


def unwrap(expr: str) -> str:
    expr = expr.strip()
    for _ in range(32):
        m = re.match(r"^(all|any)\((.*)\)$", expr, re.DOTALL)
        if not m:
            break
        expr = m.group(2).strip()
    return expr


def cfg_attr_inners(text: str):
    i = 0
    needle = "#[cfg("
    while True:
        idx = text.find(needle, i)
        if idx < 0:
            break
        start = idx + len(needle)
        depth = 1
        j = start
        while j < len(text) and depth:
            ch = text[j]
            if ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
            j += 1
        if depth != 0:
            break
        yield text[start : j - 1]
        i = j


def collect_from_expr(expr: str, out: set[str], depth: int = 0) -> None:
    if depth > 32:
        return
    expr = unwrap(expr.strip())
    for part in re.split(r"\s*,\s*", expr):
        part = part.strip()
        if re.match(r"^(all|any)\(", part):
            collect_from_expr(part, out, depth + 1)
            continue
        part = unwrap(part)
        if part.startswith("not(") and part.endswith(")"):
            part = part[4:-1].strip()
        if part.startswith("not "):
            part = part[4:].strip()
        if part.startswith("host_") or part == "test":
            continue
        if re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", part):
            out.add(part)


def rust_cfg_allowlist(rust_root: pathlib.Path) -> list[str]:
    wants: set[str] = set()
    for path in rust_root.rglob("*.rs"):
        for inner in cfg_attr_inners(path.read_text()):
            collect_from_expr(inner, wants)
    wants.update(EXTRA_WANTS)
    return sorted(wants)


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: rust_cfg_allowlist.py RUST_SRC_DIR", file=sys.stderr)
        return 2
    root = pathlib.Path(sys.argv[1])
    for name in rust_cfg_allowlist(root):
        print(name)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
