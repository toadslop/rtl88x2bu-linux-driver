#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
"""T23: inventory panic relocations in kernel rust/*.o objects."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
RUST_DIR = REPO / "rust"
BASELINE = REPO / "scripts/ci/panic-sites-baseline.txt"

PANIC_RE = re.compile(
    r"panicking|panic_bounds_check|slice_.*_fail|panic_fmt|panic_cannot_unwind"
)
SYM_LINE = re.compile(
    r"^([0-9a-f]+)\s+([gl])\s+.*?F\s+\.text\s+([0-9a-f]+)\s+(.+)$"
)
# Trailing rustc PLT32 tweak (e.g. panic_const_add_overflow-0x4).
RELOC_SUFFIX = re.compile(r"-0x[0-9a-f]+$")
# Logical Rust item name at end of legacy v0 mangling (_RNv…_22ccmp_encrypt_pv1_inner).
# Rust legacy length-prefix item at EOL: …18ccmp_decrypt_inner
MANGLED_LEN_PREFIX = re.compile(r"(\d+)$")
RELOC_LINE = re.compile(r"^\s*([0-9a-f]+):\s+R_\S+\s+(\S+)")


def run_objdump(args: list[str], path: Path) -> str:
    return subprocess.check_output(
        ["llvm-objdump", *args, str(path)], text=True, errors="replace"
    )


PANIC_KIND_MARKERS = (
    "panic_const_add_overflow",
    "panic_const_sub_overflow",
    "panic_const_mul_overflow",
    "panic_const_shl_overflow",
    "panic_const_shr_overflow",
    "panic_const_div_by_zero",
    "panic_bounds_check",
    "slice_end_index_len_fail",
    "slice_start_index_len_fail",
    "slice_index_len_fail",
    "slice_index_order_fail",
    "panic_cannot_unwind",
)


def normalize_reloc_target(symbol: str) -> str:
    sym = symbol.lstrip("_")
    sym = RELOC_SUFFIX.sub("", sym)
    return sym


def panic_kind(symbol: str) -> str:
    sym = normalize_reloc_target(symbol)
    for marker in PANIC_KIND_MARKERS:
        if marker in sym:
            return marker
    if "panicking" in sym:
        tail = sym.split("panicking", 1)[-1]
        for part in reversed(tail.split("panic_")):
            if part.startswith("const_") or part.startswith("bounds"):
                return f"panic_{part.split('-')[0].split('::')[-1]}"[:64]
    return sym[:80]


def demangle_rust_item(symbol: str) -> str | None:
    for i in range(len(symbol) - 1, 0, -1):
        ch = symbol[i]
        if not ("a" <= ch <= "z"):
            continue
        name = symbol[i:]
        if not re.fullmatch(r"[a-z][a-z0-9_]*", name):
            continue
        prefix = symbol[:i]
        lm = MANGLED_LEN_PREFIX.search(prefix)
        if lm and int(lm.group(1)) == len(name):
            return name
    return None


def logical_rust_owner(symbol: str, kernel_exports: set[str]) -> str:
    """Map objdump function symbol to export-oriented owner name."""
    if not symbol.startswith("_R"):
        return symbol.lstrip("_")
    logical = demangle_rust_item(symbol)
    if not logical:
        return symbol[:80]
    if logical.endswith("_inner"):
        stem = logical[: -len("_inner")]
        if stem in kernel_exports:
            return stem
    if logical in kernel_exports:
        return logical
    return logical


def functions(path: Path) -> list[tuple[int, int, str, str]]:
    out: list[tuple[int, int, str, str]] = []
    for line in run_objdump(["-t"], path).splitlines():
        m = SYM_LINE.match(line.strip())
        if not m:
            continue
        addr = int(m.group(1), 16)
        bind = m.group(2)
        size = int(m.group(3), 16)
        name = m.group(4).strip()
        if size == 0:
            continue
        out.append((addr, addr + size, name, bind))
    out.sort(key=lambda t: t[0])
    return out


def kernel_export_names(funcs: list[tuple[int, int, str, str]]) -> set[str]:
    return {
        name
        for _s, _e, name, bind in funcs
        if bind == "g" and not name.startswith("_R")
    }


def enclosing(funcs: list[tuple[int, int, str, str]], offset: int) -> str:
    best = "?"
    for start, end, name, _bind in funcs:
        if start <= offset < end:
            best = name
        elif start > offset:
            break
    return best


def export_label(
    funcs: list[tuple[int, int, str, str]], offset: int, exports: set[str]
) -> str:
    """Prefer kernel export symbol for panics in rustc ``*_inner`` helpers."""
    chosen = enclosing(funcs, offset)
    for start, end, name, bind in reversed(funcs):
        if start <= offset < end and bind == "g" and not name.startswith("_R"):
            return name
    if chosen != "?":
        return logical_rust_owner(chosen, exports)
    return "?"


def collect_sites(obj: Path) -> set[str]:
    funcs = functions(obj)
    exports = kernel_export_names(funcs)
    sites: set[str] = set()
    stem = obj.stem
    for line in run_objdump(["-dr"], obj).splitlines():
        rm = RELOC_LINE.match(line)
        if not rm:
            continue
        off = int(rm.group(1), 16)
        target = rm.group(2)
        if not PANIC_RE.search(target):
            continue
        owner = export_label(funcs, off, exports)
        kind = panic_kind(target)
        sites.add(f"{stem}:{owner}:{kind}")
    return sites


def all_objects() -> list[Path]:
    return sorted(p for p in RUST_DIR.glob("*.o") if p.is_file())


def site_key(site: str) -> tuple[str, str, str]:
    stem, owner, kind = site.split(":", 2)
    return stem, owner, kind


def compact_encode(sites: set[str]) -> str:
    by_obj: dict[str, dict[str, set[str]]] = {}
    for site in sites:
        stem, owner, kind = site_key(site)
        by_obj.setdefault(stem, {}).setdefault(owner, set()).add(kind)
    lines: list[str] = []
    for stem in sorted(by_obj):
        parts: list[str] = []
        for owner in sorted(by_obj[stem]):
            kinds = ",".join(sorted(by_obj[stem][owner]))
            parts.append(f"{owner}={kinds}")
        lines.append(f"{stem}\t" + ";".join(parts))
    return "\n".join(lines) + ("\n" if lines else "")


def compact_decode(text: str) -> set[str]:
    sites: set[str] = set()
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        stem, _, rest = line.partition("\t")
        for chunk in rest.split(";"):
            if "=" not in chunk:
                continue
            owner, kinds = chunk.split("=", 1)
            for kind in kinds.split(","):
                if kind:
                    sites.add(f"{stem}:{owner}:{kind}")
    return sites


def write_baseline(sites: set[str]) -> None:
    BASELINE.write_text(compact_encode(sites), encoding="utf-8")


def load_baseline() -> set[str]:
    if not BASELINE.is_file():
        return set()
    return compact_decode(BASELINE.read_text(encoding="utf-8"))


def touched_stems(base_ref: str | None) -> set[str]:
    if not base_ref:
        return set()
    try:
        out = subprocess.check_output(
            ["git", "diff", "--name-only", base_ref, "--", "rust/"],
            text=True,
            cwd=REPO,
        )
    except subprocess.CalledProcessError:
        return set()
    stems: set[str] = set()
    for line in out.splitlines():
        p = Path(line.strip())
        if p.suffix == ".rs":
            stems.add(p.stem)
    return stems


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--write-baseline",
        action="store_true",
        help="Rewrite scripts/ci/panic-sites-baseline.txt from current rust/*.o",
    )
    ap.add_argument(
        "--base-ref",
        default="",
        help="Git ref for PR diff (e.g. origin/master); new sites only fail when object stem is touched",
    )
    ap.add_argument(
        "--strict",
        action="store_true",
        help="Fail on any new site vs baseline (default: only when touched object adds sites)",
    )
    args = ap.parse_args()

    objs = all_objects()
    if not objs:
        print("panic-sites: no rust/*.o — run `make KDIR=... LLVM=1` first", file=sys.stderr)
        return 2

    sites: set[str] = set()
    for obj in objs:
        sites |= collect_sites(obj)

    if args.write_baseline:
        write_baseline(sites)
        print(f"panic-sites: wrote {len(sites)} site(s) to {BASELINE.relative_to(REPO)}")
        return 0

    baseline = load_baseline()
    if not baseline:
        print(
            "panic-sites: missing baseline — run panic-sites.sh --write-baseline after L0 build",
            file=sys.stderr,
        )
        return 2

    new_sites = sites - baseline
    if not new_sites:
        print(f"panic-sites: OK ({len(sites)} site(s), no new panics vs baseline)")
        return 0

    touched = touched_stems(args.base_ref or None)
    blocking = sorted(new_sites)
    if not args.strict and touched:
        blocking = sorted(s for s in new_sites if s.split(":", 1)[0] in touched)

    if not blocking:
        print(
            f"panic-sites: advisory — {len(new_sites)} new site(s) outside PR rust/ scope"
        )
        for s in sorted(new_sites)[:20]:
            print(f"  + {s}")
        if len(new_sites) > 20:
            print(f"  … and {len(new_sites) - 20} more")
        return 0

    print("panic-sites: new panic site(s) in touched Rust objects:", file=sys.stderr)
    for s in blocking:
        print(f"  + {s}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
