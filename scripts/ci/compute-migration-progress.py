#!/usr/bin/env python3
"""Rust vs C share of objects linked into ``88x2bu.ko``.

Counts lines in an L0-built ``88x2bu.mod`` (one repo-relative ``.o`` path per line):
paths under ``rust/`` are Rust; all other linked objects are treated as C (``core/``,
``hal/``, ``os_dep/``, ``platform/``, etc.). Infra crates (``kbuild_stub``, ``scaffold``,
``ffi``, ``domain_types``) are included in the Rust count — same as a raw mod walk.

CI: ``scripts/ci/run-migration-progress-ci.sh`` / ``build-module-objects.sh``.
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]


def normalize_object_path(line: str) -> str:
    line = line.strip()
    if not line:
        return ""
    if line.startswith("/"):
        try:
            return str(Path(line).resolve().relative_to(REPO_ROOT.resolve()))
        except ValueError:
            return line.lstrip("/")
    return line


def read_objects_file(path: Path) -> str:
    if not path.is_file():
        sys.stderr.write(f"missing module object list: {path}\n")
        sys.exit(1)
    return path.read_text()


def load_module_objects(objects_text: str) -> list[str]:
    if objects_text == "":
        return []
    return [
        o
        for o in (normalize_object_path(line) for line in objects_text.splitlines())
        if o
    ]


def compute_link_mix(objects: list[str]) -> dict[str, int | float]:
    rust = sum(1 for o in objects if o.startswith("rust/"))
    total = len(objects)
    c = total - rust
    rust_pct = round(100.0 * rust / total, 1) if total else 0.0
    c_pct = round(100.0 * c / total, 1) if total else 0.0
    return {
        "total_objects": total,
        "rust_objects": rust,
        "c_objects": c,
        "rust_pct": rust_pct,
        "c_pct": c_pct,
    }


def format_markdown(mix: dict[str, int | float]) -> str:
    return (
        "## Migration link mix\n\n"
        f"**{mix['rust_pct']}% Rust** · **{mix['c_pct']}% C** "
        f"({mix['rust_objects']} / {mix['total_objects']} linked objects in `88x2bu.ko`)\n\n"
        "<!-- migration-progress-report -->"
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Rust vs C linked-object percentage for 88x2bu.ko"
    )
    parser.add_argument(
        "--module-objects",
        type=Path,
        required=True,
        help="Path to 88x2bu link object list (from build-module-objects.sh)",
    )
    parser.add_argument("--json", action="store_true", help="Print JSON instead of markdown")
    args = parser.parse_args()

    objects = load_module_objects(read_objects_file(args.module_objects))
    mix = compute_link_mix(objects)

    if args.json:
        print(json.dumps(mix, indent=2))
    else:
        print(format_markdown(mix))
    return 0


if __name__ == "__main__":
    sys.exit(main())