#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
"""T24: when a Makefile pairs test-foo and test-foo-rust, default test must run both."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
HOST = REPO / "tests" / "host"

TARGET = re.compile(r"^(test[-a-z0-9]+)\s*:")
PREREQ = re.compile(r"^\s*test\s*:")


def tokens_from_test_recipe(lines: list[str]) -> set[str]:
    body_parts: list[str] = []
    in_test = False
    for line in lines:
        if PREREQ.match(line):
            in_test = True
            body_parts.append(line.split(":", 1)[-1])
            if not line.rstrip().endswith("\\"):
                break
            continue
        if in_test:
            body_parts.append(line)
            if not line.rstrip().endswith("\\"):
                break
    if not body_parts:
        return set()
    body = " ".join(body_parts)
    return set(re.findall(r"\b(test[-a-z0-9]+)\b", body))


def declared_targets(lines: list[str]) -> set[str]:
    out: set[str] = set()
    for line in lines:
        m = TARGET.match(line.strip())
        if m:
            out.add(m.group(1))
    return out


def main() -> int:
    errors: list[str] = []
    checked = 0
    for makefile in sorted(HOST.rglob("Makefile")):
        lines = makefile.read_text(encoding="utf-8", errors="replace").splitlines()
        declared = declared_targets(lines)
        in_test = tokens_from_test_recipe(lines)
        for name in sorted(declared):
            if not name.endswith("-rust"):
                continue
            base = name[: -len("-rust")]
            if base not in declared:
                continue
            checked += 1
            if base in in_test and name not in in_test:
                errors.append(
                    f"{makefile.relative_to(REPO)}: `test:` runs `{base}` but omits `{name}`"
                )
    if errors:
        for e in errors:
            print(e, file=sys.stderr)
        return 1
    print(f"check-l2-rust-test-targets: OK ({checked} paired rust targets)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
