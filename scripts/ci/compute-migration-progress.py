#!/usr/bin/env python3
"""Compute Rust migration progress for Phase 1 (behavior parity).

Metrics:
  - module_loc_pct: share of baseline module C LOC now represented by Rust port objects
  - module_object_pct: share of linked translation units that are Rust ports (excl. infra)
  - migration_units_loc_pct: share of baseline LOC within actively ported core/crypto units

Reads scripts/ci/migration-module-objects.txt for the 88x2bu default link set
(CONFIG_RTL8822B + USB). Regenerate after Makefile object-list changes via
scripts/ci/update-migration-baseline.sh.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BASELINE_JSON = REPO_ROOT / "scripts/ci/migration-baseline.json"
MODULE_OBJECTS = REPO_ROOT / "scripts/ci/migration-module-objects.txt"

RUST_INFRA = frozenset({"kbuild_stub", "scaffold", "ffi", "domain_types"})

RUST_TO_BASELINE_C: dict[str, str] = {
    "aes_ctr": "core/crypto/aes-ctr.c",
    "aes_omac1": "core/crypto/aes-omac1.c",
    "gcmp": "core/crypto/gcmp.c",
    "aes_siv": "core/crypto/aes-siv.c",
    "aes_ccm": "core/crypto/aes-ccm.c",
    "aes_gcm": "core/crypto/aes-gcm.c",
    "ccmp": "core/crypto/ccmp.c",
    "aes_internal": "core/crypto/aes-internal.c",
    "aes_internal_enc": "core/crypto/aes-internal-enc.c",
    "sha256_internal": "core/crypto/sha256-internal.c",
    "sha256": "core/crypto/sha256.c",
    "sha256_prf": "core/crypto/sha256-prf.c",
    "rtw_crypto_wrap": "core/crypto/rtw_crypto_wrap.c",
    "rtw_chplan": "core/rtw_chplan.c",
    "rtw_chplan_rest": "core/rtw_chplan.c",
    "rtw_io_rest": "core/rtw_io.c",
    "rtw_rf_rest": "core/rtw_rf.c",
    "rtw_swcrypto": "core/rtw_swcrypto.c",
    "rtw_ieee80211": "core/rtw_ieee80211.c",
    "rtw_ieee80211_rest": "core/rtw_ieee80211.c",
    "rtw_security": "core/rtw_security.c",
    "rtw_security_rest": "core/rtw_security.c",
    "rtw_wlan_util": "core/rtw_wlan_util.c",
    "rtw_rm_util": "core/rtw_rm_util.c",
}

# Partial ports: estimate ported LOC as baseline(full parent) - current(split/rest C file).
PARTIAL_UNITS: dict[str, tuple[str, str]] = {
    "rtw_chplan_rest": ("core/rtw_chplan.c", "core/rtw_chplan_rest.c"),
    "rtw_io_rest": ("core/rtw_io.c", "core/rtw_io_rest.c"),
    "rtw_rf_rest": ("core/rtw_rf.c", "core/rtw_rf_rest.c"),
    "rtw_ieee80211_rest": ("core/rtw_ieee80211.c", "core/rtw_ieee80211_rest.c"),
    "rtw_security_rest": ("core/rtw_security.c", "core/rtw_security_rest.c"),
    "rtw_rm_util": ("core/rtw_rm_util.c", "core/rtw_rm_util_rest.c"),
    "rtw_wlan_util": ("core/rtw_wlan_util.c", "core/rtw_wlan_util.c"),
    "aes_internal": ("core/crypto/aes-internal.c", "core/crypto/aes-internal.c"),
    "rtw_security": ("core/rtw_security.c", "core/rtw_security_rest.c"),
    "rtw_chplan": ("core/rtw_chplan.c", "core/rtw_chplan_rest.c"),
    "rtw_ieee80211": ("core/rtw_ieee80211.c", "core/rtw_ieee80211_rest.c"),
    "rtw_swcrypto": ("core/rtw_swcrypto.c", "core/rtw_swcrypto_rest.c"),
}

# Linked C rest stubs fold into their parent baseline group (not separate TUs).
REST_C_TO_PARENT: dict[str, str] = {
    "core/rtw_chplan_rest.c": "core/rtw_chplan.c",
    "core/rtw_io_rest.c": "core/rtw_io.c",
    "core/rtw_rf_rest.c": "core/rtw_rf.c",
    "core/rtw_ieee80211_rest.c": "core/rtw_ieee80211.c",
    "core/rtw_security_rest.c": "core/rtw_security.c",
    "core/rtw_rm_util_rest.c": "core/rtw_rm_util.c",
    "core/rtw_swcrypto_rest.c": "core/rtw_swcrypto.c",
}


def canonical_baseline_c(c_path: str) -> str:
    return REST_C_TO_PARENT.get(c_path, c_path)


def load_baseline_meta() -> dict:
    with BASELINE_JSON.open() as f:
        return json.load(f)


def git_line_count(path: str, ref: str) -> int:
    try:
        out = subprocess.check_output(
            ["git", "show", f"{ref}:{path}"],
            cwd=REPO_ROOT,
            stderr=subprocess.DEVNULL,
        )
        return out.count(b"\n")
    except subprocess.CalledProcessError:
        return 0


def baseline_loc_for_path(path: str, baseline_ref: str) -> int:
    """Baseline C LOC for a source file at the configured import ref.

    Files added after ``baseline_ref`` (driver updates predating Rust) use LOC
    at the commit that first introduced the path so Rust ports receive fair
    credit and C fallbacks stay symmetric.
    """
    bl = git_line_count(path, baseline_ref)
    if bl > 0:
        return bl
    try:
        intro = subprocess.check_output(
            ["git", "log", "--diff-filter=A", "--format=%H", "-1", "--", path],
            cwd=REPO_ROOT,
            stderr=subprocess.DEVNULL,
            text=True,
        ).strip()
        if intro:
            return git_line_count(path, intro)
    except subprocess.CalledProcessError:
        pass
    return line_count_at(path, None)


def git_file_bytes(path: str, ref: str) -> bytes | None:
    try:
        return subprocess.check_output(
            ["git", "show", f"{ref}:{path}"],
            cwd=REPO_ROOT,
            stderr=subprocess.DEVNULL,
        )
    except subprocess.CalledProcessError:
        return None


def file_line_count(path: Path) -> int:
    if not path.exists():
        return 0
    return path.read_bytes().count(b"\n")


def line_count_at(path: str, tree_ref: str | None) -> int:
    if tree_ref:
        data = git_file_bytes(path, tree_ref)
        if data is not None:
            return data.count(b"\n")
    return file_line_count(REPO_ROOT / path)


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


def load_module_objects(objects_text: str | None = None) -> list[str]:
    text = objects_text
    if text is None:
        if not MODULE_OBJECTS.exists():
            sys.stderr.write(
                f"missing {MODULE_OBJECTS.relative_to(REPO_ROOT)} — "
                "run scripts/ci/update-migration-baseline.sh\n"
            )
            sys.exit(1)
        text = MODULE_OBJECTS.read_text()
    if text == "":
        return []
    objs = [
        normalize_object_path(line) for line in text.splitlines() if line.strip()
    ]
    return [o for o in objs if o]


def classify_object(obj_path: str) -> tuple[str | None, str]:
    if obj_path.startswith("rust/"):
        stem = Path(obj_path).stem
        if stem in RUST_INFRA:
            return None, "infra"
        return RUST_TO_BASELINE_C.get(stem), "rust"
    if obj_path.endswith(".o"):
        return obj_path[:-2] + ".c", "c"
    return None, "unknown"


def _rust_loc_for_objs(rust_objs: list[str], tree_ref: str | None) -> int:
    total = 0
    for obj in rust_objs:
        total += line_count_at(obj.replace(".o", ".rs"), tree_ref)
    return total


def _ported_loc_for_parent(
    parent_c: str,
    rest_c: str | None,
    rust_objs: list[str],
    linked_c: list[str],
    baseline_ref: str,
    tree_ref: str | None,
) -> int:
    full_baseline = baseline_loc_for_path(parent_c, baseline_ref)
    if not rust_objs:
        return 0

    remaining_c = 0
    if parent_c in linked_c:
        remaining_c += line_count_at(parent_c, tree_ref)
    if rest_c and rest_c in linked_c:
        remaining_c += line_count_at(rest_c, tree_ref)

    if remaining_c == 0:
        return full_baseline

    delta = full_baseline - remaining_c
    if delta > 0:
        return min(full_baseline, delta)

    rust_loc = _rust_loc_for_objs(rust_objs, tree_ref)
    return min(full_baseline, rust_loc) if rust_loc > 0 else 0


def compute_module_metrics(
    baseline_ref: str,
    tree_ref: str | None = None,
    objects_text: str | None = None,
) -> dict:
    objs = load_module_objects(objects_text)
    c_linked = 0
    rust_port = 0
    total_baseline_loc = 0
    ported_baseline_loc = 0
    current_c_loc = 0
    current_rust_loc = 0

    # Group linked objects by canonical baseline C path (each counted once).
    groups: dict[str, dict[str, list[str] | str | None]] = {}

    for obj in objs:
        c_path, kind = classify_object(obj)
        if kind == "infra":
            continue
        if kind == "rust":
            rust_port += 1
            rust_src = obj.replace(".o", ".rs")
            current_rust_loc += line_count_at(rust_src, tree_ref)
            canonical = c_path or f"__unmapped__:{Path(obj).stem}"
            groups.setdefault(
                canonical, {"rust": [], "rest_c": None, "linked_c": []}
            )
            groups[canonical]["rust"].append(obj)  # type: ignore[index]
        elif kind == "c" and c_path:
            c_linked += 1
            current_c_loc += line_count_at(c_path, tree_ref)
            canonical = canonical_baseline_c(c_path)
            grp = groups.setdefault(
                canonical, {"rust": [], "rest_c": None, "linked_c": []}
            )
            grp["linked_c"].append(c_path)  # type: ignore[index]
            if c_path in REST_C_TO_PARENT:
                grp["rest_c"] = c_path
        else:
            continue

    # Attach rest-file hints from partial-port stems.
    for canonical, grp in groups.items():
        if canonical.startswith("__unmapped__:"):
            continue
        rest_candidates: set[str] = set()
        for obj in grp["rust"]:  # type: ignore[union-attr]
            stem = Path(obj).stem
            if stem in PARTIAL_UNITS:
                parent_c, rest_c = PARTIAL_UNITS[stem]
                if parent_c == canonical and rest_c != parent_c:
                    rest_candidates.add(rest_c)
        if rest_candidates:
            grp["rest_c"] = sorted(rest_candidates)[0]

    for canonical, grp in groups.items():
        if canonical.startswith("__unmapped__:"):
            continue
        bl = baseline_loc_for_path(canonical, baseline_ref)
        total_baseline_loc += bl
        ported_baseline_loc += _ported_loc_for_parent(
            canonical,
            grp["rest_c"],  # type: ignore[arg-type]
            grp["rust"],  # type: ignore[arg-type]
            grp["linked_c"],  # type: ignore[arg-type]
            baseline_ref,
            tree_ref,
        )

    link_units = c_linked + rust_port
    module_loc_pct = (
        100.0 * ported_baseline_loc / total_baseline_loc
        if total_baseline_loc
        else 0.0
    )
    module_object_pct = 100.0 * rust_port / link_units if link_units else 0.0

    return {
        "module_objects_total": len(objs),
        "c_objects_linked": c_linked,
        "rust_port_objects": rust_port,
        "module_loc_pct": round(module_loc_pct, 1),
        "module_object_pct": round(module_object_pct, 1),
        "baseline_module_c_loc": total_baseline_loc,
        "ported_baseline_c_loc": ported_baseline_loc,
        "current_linked_c_loc": current_c_loc,
        "current_rust_loc": current_rust_loc,
    }


def discover_migration_units_from_makefile(makefile_text: str | None = None) -> list[str]:
    text = makefile_text
    if text is None:
        text = (REPO_ROOT / "Makefile").read_text()
    stems: list[str] = []
    for line in text.splitlines():
        line = line.strip()
        if "$(MODULE_NAME)-y += rust/" not in line:
            continue
        part = line.split("+=", 1)[1].strip()
        stem = Path(part.replace(".o", "")).stem
        if stem not in RUST_INFRA:
            stems.append(stem)
    return sorted(set(stems))


def linked_c_by_parent(objects_text: str | None) -> dict[str, list[str]]:
    linked: dict[str, list[str]] = {}
    for obj in load_module_objects(objects_text):
        c_path, kind = classify_object(obj)
        if kind != "c" or not c_path:
            continue
        canonical = canonical_baseline_c(c_path)
        linked.setdefault(canonical, []).append(c_path)
    return linked


def rust_objs_for_parent(parent_c: str, units: list[str]) -> list[str]:
    objs: list[str] = []
    for stem in units:
        mapped = RUST_TO_BASELINE_C.get(stem)
        partial = PARTIAL_UNITS.get(stem)
        if mapped == parent_c or (partial and partial[0] == parent_c):
            objs.append(f"rust/{stem}.o")
    return objs


def rest_c_for_parent(parent_c: str, units: list[str]) -> str | None:
    for stem in units:
        if stem in PARTIAL_UNITS:
            p, rest = PARTIAL_UNITS[stem]
            if p == parent_c and rest != parent_c:
                return rest
    return None


def compute_migration_units_metrics(
    baseline_ref: str,
    tree_ref: str | None = None,
    makefile_text: str | None = None,
    objects_text: str | None = None,
) -> dict:
    units = discover_migration_units_from_makefile(makefile_text)
    linked = linked_c_by_parent(objects_text)
    baseline_scope_loc = 0
    ported_loc_est = 0
    rust_loc = 0
    parent_ported: dict[str, int] = {}
    parent_baseline: dict[str, int] = {}

    for stem in units:
        rust_path = f"rust/{stem}.rs"
        rust_loc += line_count_at(rust_path, tree_ref)

        if stem in PARTIAL_UNITS:
            parent_c, rest_c = PARTIAL_UNITS[stem]
            if parent_c not in parent_baseline:
                full_baseline = baseline_loc_for_path(parent_c, baseline_ref)
                parent_baseline[parent_c] = full_baseline
                baseline_scope_loc += full_baseline
            if parent_c not in parent_ported:
                parent_ported[parent_c] = _ported_loc_for_parent(
                    parent_c,
                    rest_c_for_parent(parent_c, units),
                    rust_objs_for_parent(parent_c, units),
                    linked.get(parent_c, []),
                    baseline_ref,
                    tree_ref,
                )
        else:
            baseline_c = RUST_TO_BASELINE_C.get(stem)
            if not baseline_c:
                continue
            if baseline_c not in parent_baseline:
                unit_baseline = baseline_loc_for_path(baseline_c, baseline_ref)
                parent_baseline[baseline_c] = unit_baseline
                baseline_scope_loc += unit_baseline
            if baseline_c not in parent_ported:
                parent_ported[baseline_c] = _ported_loc_for_parent(
                    baseline_c,
                    None,
                    rust_objs_for_parent(baseline_c, units),
                    linked.get(baseline_c, []),
                    baseline_ref,
                    tree_ref,
                )

    ported_loc_est += sum(parent_ported.values())

    units_loc_pct = (
        100.0 * ported_loc_est / baseline_scope_loc if baseline_scope_loc else 0.0
    )
    return {
        "migration_unit_count": len(units),
        "migration_units_loc_pct": round(units_loc_pct, 1),
        "migration_units_baseline_loc": baseline_scope_loc,
        "migration_units_ported_loc_est": ported_loc_est,
        "migration_units_rust_loc": rust_loc,
    }


def snapshot_at_ref(tree_ref: str, baseline_ref: str) -> dict:
    objects_bytes = git_file_bytes(
        "scripts/ci/migration-module-objects.txt", tree_ref
    )
    makefile_bytes = git_file_bytes("Makefile", tree_ref)
    # Do not fall back to the worktree object list when the ref lacks the file.
    objects_text = (
        objects_bytes.decode("utf-8", errors="replace")
        if objects_bytes is not None
        else ""
    )
    makefile_text = (
        makefile_bytes.decode("utf-8", errors="replace") if makefile_bytes else None
    )
    module_metrics: dict | None
    if objects_bytes is None:
        module_metrics = None
    else:
        module_metrics = compute_module_metrics(
            baseline_ref, tree_ref, objects_text=objects_text
        )
    return {
        "module": module_metrics,
        "units": compute_migration_units_metrics(
            baseline_ref, tree_ref, makefile_text=makefile_text, objects_text=objects_text
        ),
        "has_module_objects": objects_bytes is not None,
    }


def format_markdown(data: dict, baseline_label: str, baseline_ref: str) -> str:
    m = data["module"]
    u = data["units"]
    delta = data.get("delta")
    delta_section = ""
    if delta:
        delta_section = (
            "\n### Change vs base branch\n\n"
            "| Metric | Δ |\n|--------|---|\n"
        )
        if delta.get("module_loc_pct") is not None:
            delta_section += f"| Module LOC % | {delta['module_loc_pct']:+.1f} |\n"
            delta_section += (
                f"| Module objects % | {delta['module_object_pct']:+.1f} |\n"
            )
        else:
            delta_section += (
                "| Module LOC % | _n/a (base ref lacks "
                "`migration-module-objects.txt`)_ |\n"
                "| Module objects % | _n/a_ |\n"
            )
        units_delta = delta.get("migration_units_loc_pct")
        if units_delta is not None:
            delta_section += (
                f"| Migration units LOC % | {units_delta:+.1f} |\n"
            )
        else:
            delta_section += (
                "| Migration units LOC % | _n/a (base ref lacks "
                "`migration-module-objects.txt`)_ |\n"
            )

    return (
        "## Rust migration progress (Phase 1)\n\n"
        f"_Baseline: `{baseline_ref}` — {baseline_label}. "
        "Phase 1 exit is zero C objects in the `88x2bu` link for the default "
        "8822B USB config ([`docs/rust-migration.md`](docs/rust-migration.md))._\n\n"
        "### Summary\n\n"
        "| Metric | Progress |\n|--------|----------|\n"
        f"| **Module link (LOC)** | **{m['module_loc_pct']}%** "
        f"({m['ported_baseline_c_loc']:,} / {m['baseline_module_c_loc']:,} baseline C lines "
        "now represented by Rust port objects) |\n"
        f"| **Module link (objects)** | **{m['module_object_pct']}%** "
        f"({m['rust_port_objects']} Rust port / {m['c_objects_linked']} C "
        "translation units, excl. infra `.rs`) |\n"
        f"| **Active migration units (LOC)** | **{u['migration_units_loc_pct']}%** "
        f"({u['migration_units_ported_loc_est']:,} / "
        f"{u['migration_units_baseline_loc']:,} baseline LOC across "
        f"{u['migration_unit_count']} `rust/*.rs` units in Makefile) |\n\n"
        "### Current tree\n\n"
        f"- Linked C source: **{m['current_linked_c_loc']:,}** lines "
        "(still compiled into `88x2bu.ko`)\n"
        f"- Rust migration source: **{m['current_rust_loc']:,}** lines "
        "(port objects; infra excluded from object %)\n"
        f"- Migration-unit Rust: **{u['migration_units_rust_loc']:,}** lines\n"
        f"{delta_section}\n"
        "<!-- migration-progress-report -->"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Compute Rust migration progress")
    parser.add_argument(
        "--baseline-ref",
        help="Git ref for baseline C LOC (default: migration-baseline.json)",
    )
    parser.add_argument(
        "--compare-ref",
        help="Optional ref to compute deltas against (e.g. origin/master)",
    )
    parser.add_argument("--json", action="store_true", help="Print JSON instead of markdown")
    args = parser.parse_args()

    meta = load_baseline_meta()
    baseline_ref = args.baseline_ref or meta["baseline_ref"]
    baseline_label = meta.get("baseline_label", baseline_ref)

    module = compute_module_metrics(baseline_ref)
    units = compute_migration_units_metrics(baseline_ref, objects_text=None)
    result: dict = {"baseline_ref": baseline_ref, "module": module, "units": units}

    if args.compare_ref:
        base_snap = snapshot_at_ref(args.compare_ref, baseline_ref)
        delta: dict[str, float | None] = {
            "module_loc_pct": None,
            "module_object_pct": None,
            "migration_units_loc_pct": None,
        }
        if base_snap["has_module_objects"]:
            delta["migration_units_loc_pct"] = round(
                units["migration_units_loc_pct"]
                - base_snap["units"]["migration_units_loc_pct"],
                1,
            )
        if base_snap["module"] is not None:
            delta["module_loc_pct"] = round(
                module["module_loc_pct"] - base_snap["module"]["module_loc_pct"],
                1,
            )
            delta["module_object_pct"] = round(
                module["module_object_pct"]
                - base_snap["module"]["module_object_pct"],
                1,
            )
        result["compare_ref"] = args.compare_ref
        result["compare_has_module_objects"] = base_snap["has_module_objects"]
        result["delta"] = delta

    if args.json:
        print(json.dumps(result, indent=2))
    else:
        print(format_markdown(result, baseline_label, baseline_ref))
    return 0


if __name__ == "__main__":
    sys.exit(main())
