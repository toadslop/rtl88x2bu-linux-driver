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
RELOC_LINE = re.compile(r"^\s*([0-9a-f]+):\s+R_\S+\s+(\S+)")


def run_objdump(args: list[str], path: Path) -> str:
    return subprocess.check_output(
        ["llvm-objdump", *args, str(path)], text=True, errors="replace"
    )


def panic_kind(symbol: str) -> str:
    sym = symbol.lstrip("_")
    for marker in (
        "panic_const_add_overflow",
        "panic_const_sub_overflow",
        "panic_const_mul_overflow",
        "panic_const_shl_overflow",
        "panic_const_shr_overflow",
        "panic_bounds_check",
        "slice_end_index_len_fail",
        "slice_start_index_len_fail",
        "slice_index_len_fail",
    ):
        if marker in sym:
            return marker
    if "panicking" in sym:
        return sym.split("panicking")[-1].split("::")[-1][:64]
    return sym[:80]


def functions(path: Path) -> list[tuple[int, int, str]]:
    out: list[tuple[int, int, str]] = []
    for line in run_objdump(["-t"], path).splitlines():
        m = SYM_LINE.match(line.strip())
        if not m:
            continue
        addr = int(m.group(1), 16)
        size = int(m.group(3), 16)
        name = m.group(4).strip()
        if size == 0:
            continue
        out.append((addr, addr + size, name))
    out.sort(key=lambda t: t[0])
    return out


def enclosing(funcs: list[tuple[int, int, str]], offset: int) -> str:
    best = "?"
    for start, end, name in funcs:
        if start <= offset < end:
            best = name
        elif start > offset:
            break
    return best


def export_label(funcs: list[tuple[int, int, str]], offset: int) -> str:
    """Prefer the nearest global export (unmangled C name) enclosing offset."""
    chosen = enclosing(funcs, offset)
    globals = [(s, e, n) for s, e, n in funcs if not n.startswith("_")]
    for start, end, name in reversed(globals):
        if start <= offset < end:
            return name
    if chosen != "?":
        return chosen
    for start, end, name in reversed(funcs):
        if start <= offset < end:
            return name
    return "?"


def collect_sites(obj: Path) -> set[str]:
    funcs = functions(obj)
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
        owner = export_label(funcs, off)
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
