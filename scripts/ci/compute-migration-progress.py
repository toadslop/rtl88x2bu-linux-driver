#!/usr/bin/env python3
"""Compute Rust migration progress for Phase 1 (behavior parity).

Metrics:
  - module_loc_pct: share of baseline module C LOC now represented by Rust port objects
  - module_object_pct: share of linked translation units that are Rust ports (excl. infra)
  - migration_units_loc_pct: share of baseline LOC within actively ported core/crypto units

The 88x2bu module link set is derived from an L0 build (``88x2bu.mod``) and passed via
``--module-objects``. CI runs ``scripts/ci/run-migration-progress-ci.sh`` inside the L0
image; locally use ``scripts/ci/build-module-objects.sh`` or an existing ``88x2bu.mod``.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BASELINE_JSON = REPO_ROOT / "scripts/ci/migration-baseline.json"

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
    "rtw_vht": "core/rtw_vht.c",
    "rtw_sta_mgt": "core/rtw_sta_mgt.c",
    "rtw_sta_mgt_aid": "core/rtw_sta_mgt.c",
    "rtw_recv": "core/rtw_recv.c",
    "rtw_xmit": "core/rtw_xmit.c",
}

# Partial ports: estimate ported LOC as baseline(full parent) - current(split/rest C file).
PARTIAL_UNITS: dict[str, tuple[str, str]] = {
    "rtw_chplan_rest": ("core/rtw_chplan.c", "core/rtw_chplan_rest.c"),
    "rtw_io_rest": ("core/rtw_io.c", "core/rtw_io_rest.c"),
    "rtw_rf_rest": ("core/rtw_rf.c", "core/rtw_rf_rest.c"),
    "rtw_ieee80211_rest": ("core/rtw_ieee80211.c", "core/rtw_ieee80211_rest.c"),
    "rtw_security_rest": ("core/rtw_security.c", "core/rtw_security_rest.c"),
    "rtw_rm_util": ("core/rtw_rm_util.c", "core/rtw_rm_util_rest.c"),
    "rtw_vht": ("core/rtw_vht.c", "core/rtw_vht_rest.c"),
    "rtw_sta_mgt": ("core/rtw_sta_mgt.c", "core/rtw_sta_mgt_rest.c"),
    "rtw_sta_mgt_aid": ("core/rtw_sta_mgt.c", "core/rtw_sta_mgt_rest.c"),
    "rtw_recv": ("core/rtw_recv.c", "core/rtw_recv_rest.c"),
    "rtw_xmit": ("core/rtw_xmit.c", "core/rtw_xmit_rest.c"),
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
    "core/rtw_vht_rest.c": "core/rtw_vht.c",
    "core/rtw_sta_mgt_rest.c": "core/rtw_sta_mgt.c",
    "core/rtw_recv_rest.c": "core/rtw_recv.c",
    "core/rtw_xmit_rest.c": "core/rtw_xmit.c",
    "core/rtw_swcrypto_rest.c": "core/rtw_swcrypto.c",
}


SOURCE_SEARCH_DIRS = ("core", "hal", "os_dep", "core/crypto")


def canonical_baseline_c(c_path: str) -> str:
    if c_path in REST_C_TO_PARENT:
        return REST_C_TO_PARENT[c_path]
    name = Path(c_path).name
    if name.endswith("_rest.c"):
        stem = name[:-len("_rest.c")]
        for d in SOURCE_SEARCH_DIRS:
            parent = f"{d}/{stem}.c"
            if (REPO_ROOT / parent).is_file():
                return parent
    return c_path


def baseline_c_for_rust_stem(stem: str) -> str | None:
    if stem in RUST_TO_BASELINE_C:
        return RUST_TO_BASELINE_C[stem]
    if stem in PARTIAL_UNITS:
        return PARTIAL_UNITS[stem][0]
    for d in SOURCE_SEARCH_DIRS:
        direct = f"{d}/{stem}.c"
        if (REPO_ROOT / direct).is_file():
            return direct
    if stem.endswith("_rest"):
        base_stem = stem[: -len("_rest")]
        for d in SOURCE_SEARCH_DIRS:
            parent = f"{d}/{base_stem}.c"
            if (REPO_ROOT / parent).is_file():
                return parent
    parts = stem.split("_")
    for i in range(len(parts) - 1, 0, -1):
        candidate = "_".join(parts[:i])
        for d in SOURCE_SEARCH_DIRS:
            parent = f"{d}/{candidate}.c"
            if (REPO_ROOT / parent).is_file():
                return parent
    return None


def partial_unit_for_stem(stem: str) -> tuple[str, str] | None:
    if stem in PARTIAL_UNITS:
        return PARTIAL_UNITS[stem]
    parent = baseline_c_for_rust_stem(stem)
    if not parent:
        return None
    # Sibling `{parent}_rest.c` on disk implies a split partial port. Rust units
    # that fully replace C but keep a `_rest.c` stub must be listed in PARTIAL_UNITS.
    parent_stem = Path(parent).stem
    for d in SOURCE_SEARCH_DIRS:
        rest = f"{d}/{parent_stem}_rest.c"
        if (REPO_ROOT / rest).is_file() and rest != parent:
            return parent, rest
    if stem.endswith("_rest"):
        for d in SOURCE_SEARCH_DIRS:
            rest = f"{d}/{stem}.c"
            if (REPO_ROOT / rest).is_file():
                return parent, rest
    return None


def rust_port_stems_from_makefile(makefile_text: str) -> set[str]:
    stems: set[str] = set()
    for line in makefile_text.splitlines():
        if "$(MODULE_NAME)-y += rust/" not in line:
            continue
        part = line.split("+=", 1)[1].strip()
        if not part.startswith("rust/") or not part.endswith(".o"):
            continue
        stems.add(Path(part).stem)
    return stems


def makefile_text_at(tree_ref: str) -> str | None:
    data = git_file_bytes("Makefile", tree_ref)
    if data is None:
        return None
    return data.decode("utf-8", errors="replace")


def module_objects_for_ref(head_objs: list[str], tree_ref: str) -> list[str]:
    """Approximate the module link set at ``tree_ref`` without an L0 rebuild.

    C/HAL objects are taken from the HEAD L0 object list unchanged. Only
    ``rust/*.o`` membership is adjusted from the Makefile ``rust/`` list at
    ``tree_ref``. C TU add/drop at the base ref is **not** modeled — use a full
    base L0 list via ``--compare-module-objects`` when C link deltas matter.
    """
    mf = makefile_text_at(tree_ref)
    if mf is None:
        return list(head_objs)
    rust_stems = rust_port_stems_from_makefile(mf)
    out: list[str] = []
    seen: set[str] = set()

    for obj in head_objs:
        if obj.startswith("rust/"):
            stem = Path(obj).stem
            if stem in RUST_INFRA or stem in rust_stems:
                if obj not in seen:
                    out.append(obj)
                    seen.add(obj)
            continue
        if obj not in seen:
            out.append(obj)
            seen.add(obj)

    for stem in sorted(rust_stems):
        if stem in RUST_INFRA:
            continue
        ro = f"rust/{stem}.o"
        if ro not in seen:
            out.append(ro)
            seen.add(ro)

    return out


def git_rev_parse(ref: str) -> str:
    return subprocess.check_output(
        ["git", "rev-parse", ref],
        cwd=REPO_ROOT,
        stderr=subprocess.DEVNULL,
        text=True,
    ).strip()


def git_merge_base(a: str, b: str) -> str:
    return subprocess.check_output(
        ["git", "merge-base", a, b],
        cwd=REPO_ROOT,
        stderr=subprocess.DEVNULL,
        text=True,
    ).strip()


def git_diff_numstat(old_ref: str, new_ref: str, paths: list[str]) -> list[tuple[int, int, str]]:
    try:
        out = subprocess.check_output(
            ["git", "diff", "--numstat", f"{old_ref}..{new_ref}", "--", *paths],
            cwd=REPO_ROOT,
            stderr=subprocess.DEVNULL,
            text=True,
        )
    except subprocess.CalledProcessError:
        return []
    rows: list[tuple[int, int, str]] = []
    for line in out.splitlines():
        parts = line.split("\t", 2)
        if len(parts) != 3:
            continue
        add_s, del_s, path = parts
        if add_s == "-" or del_s == "-":
            continue
        rows.append((int(add_s), int(del_s), path))
    return rows


def compute_pr_scope_changes(base_ref: str, head_ref: str = "HEAD") -> dict:
    """Git-native deltas for the PR (merge-base..head), independent of L0 rebuilds."""
    try:
        old_ref = git_merge_base(base_ref, head_ref)
    except subprocess.CalledProcessError:
        old_ref = git_rev_parse(base_ref)

    paths = ["rust", "core", "hal", "os_dep", "Makefile"]
    rows = git_diff_numstat(old_ref, head_ref, paths)

    rust_add = rust_del = 0
    c_add = c_del = 0
    file_deltas: list[tuple[int, str]] = []

    for add, delete, path in rows:
        net = add - delete
        file_deltas.append((net, path))
        if path.startswith("rust/") and path.endswith(".rs"):
            rust_add += add
            rust_del += delete
        elif path.endswith(".c") and (
            path.startswith("core/")
            or path.startswith("hal/")
            or path.startswith("os_dep/")
        ):
            c_add += add
            c_del += delete

    mf_old = makefile_text_at(old_ref) or ""
    mf_new = makefile_text_at(head_ref) or (REPO_ROOT / "Makefile").read_text()
    stems_old = rust_port_stems_from_makefile(mf_old)
    stems_new = rust_port_stems_from_makefile(mf_new)
    added_stems = sorted(stems_new - stems_old - RUST_INFRA)
    removed_stems = sorted(stems_old - stems_new - RUST_INFRA)

    file_deltas.sort(key=lambda x: abs(x[0]), reverse=True)
    top_files = [(p, d) for d, p in file_deltas[:10] if d != 0]

    return {
        "diff_old_ref": old_ref,
        "diff_new_ref": head_ref,
        "rust_lines_added": rust_add,
        "rust_lines_removed": rust_del,
        "c_lines_added": c_add,
        "c_lines_removed": c_del,
        "makefile_rust_stems_added": added_stems,
        "makefile_rust_stems_removed": removed_stems,
        "top_file_deltas": top_files,
    }


def link_set_pr_deltas(
    head_objs: list[str],
    base_objs: list[str],
    *,
    rust_only_base: bool,
) -> dict[str, list[str]]:
    """Diff HEAD vs base module object lists for PR reporting bullets."""

    def _port_rust_obj(path: str) -> bool:
        if not path.startswith("rust/"):
            return False
        return Path(path).stem not in RUST_INFRA

    head_set = set(head_objs)
    base_set = set(base_objs)
    out: dict[str, list[str]] = {
        "link_rust_added": sorted(
            o for o in head_set - base_set if _port_rust_obj(o)
        ),
        "link_rust_removed": sorted(
            o for o in base_set - head_set if _port_rust_obj(o)
        ),
    }
    if not rust_only_base:
        out["link_c_dropped"] = sorted(
            o for o in base_set - head_set if not o.startswith("rust/")
        )
    return out


def format_pr_changes_section(pr: dict, compare_label: str) -> str:
    rust_net = pr["rust_lines_added"] - pr["rust_lines_removed"]
    c_net = pr["c_lines_added"] - pr["c_lines_removed"]
    lines = [
        f"\n### Changes in this PR (vs `{compare_label}`)\n",
        f"_Diff range: `{pr['diff_old_ref'][:12]}` → `{pr['diff_new_ref']}` "
        "(merge-base of PR branch and base)._ \n\n"
    ]
    if pr.get("compare_base_link_rust_only"):
        lines.append(
            "_Link-set bullets below use a **Rust-only** base snapshot (HEAD C "
            "objects + Makefile `rust/` at base). C TU membership at the base ref "
            "is not rebuilt unless CI passes `--compare-module-objects`._\n\n"
        )
    lines.extend(
        [
        "| Area | Lines added | Lines removed | Net |\n",
        "|------|-------------|---------------|-----|\n",
        f"| `rust/*.rs` | {pr['rust_lines_added']:,} | {pr['rust_lines_removed']:,} | {rust_net:+,} |\n",
        f"| Migration C (`core/`, `hal/`, `os_dep/`) | {pr['c_lines_added']:,} | "
        f"{pr['c_lines_removed']:,} | {c_net:+,} |\n",
        ]
    )
    if pr["makefile_rust_stems_added"]:
        lines.append(
            f"- **New Rust link units (Makefile):** "
            f"{', '.join(f'`{s}`' for s in pr['makefile_rust_stems_added'])}\n"
        )
    if pr["makefile_rust_stems_removed"]:
        lines.append(
            f"- **Removed Rust link units (Makefile):** "
            f"{', '.join(f'`{s}`' for s in pr['makefile_rust_stems_removed'])}\n"
        )
    if pr.get("link_rust_added"):
        lines.append(
            "- **New in `88x2bu` link (vs base snapshot):** "
            + ", ".join(f"`{o}`" for o in pr["link_rust_added"])
            + "\n"
        )
    if pr.get("link_rust_removed"):
        lines.append(
            "- **Removed from `88x2bu` link (vs base snapshot):** "
            + ", ".join(f"`{o}`" for o in pr["link_rust_removed"][:12])
            + (" …" if len(pr["link_rust_removed"]) > 12 else "")
            + "\n"
        )
    if pr.get("link_c_dropped"):
        lines.append(
            "- **C objects dropped from link (vs base snapshot):** "
            + ", ".join(f"`{o}`" for o in pr["link_c_dropped"][:12])
            + (" …" if len(pr["link_c_dropped"]) > 12 else "")
            + "\n"
        )
    if pr["top_file_deltas"]:
        lines.append("\n**Largest file deltas (net lines):**\n\n")
        for path, delta in pr["top_file_deltas"]:
            lines.append(f"- `{path}`: {delta:+,}\n")
    if (
        rust_net == 0
        and c_net == 0
        and not pr["makefile_rust_stems_added"]
        and not pr["makefile_rust_stems_removed"]
        and not pr.get("link_rust_added")
        and not pr.get("link_rust_removed")
        and not pr.get("link_c_dropped")
    ):
        lines.append(
            "\n_No migration-touched paths changed in this diff range "
            "(docs/tests-only PR, or changes outside `rust/` / migration C)._ \n"
        )
    return "".join(lines)


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


def read_objects_file(path: Path) -> str:
    if not path.is_file():
        sys.stderr.write(f"missing module object list: {path}\n")
        sys.exit(1)
    return path.read_text()


def load_module_objects(objects_text: str | None = None) -> list[str]:
    if objects_text is None:
        sys.stderr.write(
            "module object list required — pass --module-objects "
            "(from scripts/ci/build-module-objects.sh or CI)\n"
        )
        sys.exit(1)
    text = objects_text
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
        return baseline_c_for_rust_stem(stem), "rust"
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

    rust_loc = _rust_loc_for_objs(rust_objs, tree_ref)
    c_based = max(0, full_baseline - remaining_c)
    if rust_loc <= 0 and c_based <= 0:
        return 0
    # Credit whichever estimate is further along: C shrinkage or Rust growth.
    return min(full_baseline, max(c_based, rust_loc))


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
            if c_path in REST_C_TO_PARENT or Path(c_path).name.endswith("_rest.c"):
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
            partial = partial_unit_for_stem(stem)
            if partial:
                parent_c, rest_c = partial
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
        mapped = baseline_c_for_rust_stem(stem)
        partial = partial_unit_for_stem(stem)
        if mapped == parent_c or (partial and partial[0] == parent_c):
            objs.append(f"rust/{stem}.o")
    return objs


def rest_c_for_parent(parent_c: str, units: list[str]) -> str | None:
    for stem in units:
        partial = partial_unit_for_stem(stem)
        if partial:
            p, rest = partial
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

        partial = partial_unit_for_stem(stem)
        if partial:
            parent_c, rest_c = partial
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
            baseline_c = baseline_c_for_rust_stem(stem)
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


def snapshot_with_objects(
    tree_ref: str,
    baseline_ref: str,
    objects_text: str,
) -> dict:
    makefile_bytes = git_file_bytes("Makefile", tree_ref)
    makefile_text = (
        makefile_bytes.decode("utf-8", errors="replace") if makefile_bytes else None
    )
    module_metrics = compute_module_metrics(
        baseline_ref,
        tree_ref,
        objects_text=objects_text,
    )
    return {
        "module": module_metrics,
        "units": compute_migration_units_metrics(
            baseline_ref,
            tree_ref,
            makefile_text=makefile_text,
            objects_text=objects_text,
        ),
        "has_module_objects": True,
    }


def format_markdown(data: dict, baseline_label: str, baseline_ref: str) -> str:
    m = data["module"]
    u = data["units"]
    delta = data.get("delta")
    pr_changes = data.get("pr_changes")
    pr_section = ""
    if pr_changes:
        compare_label = data.get("compare_ref", "base")
        pr_section = format_pr_changes_section(pr_changes, compare_label)
    delta_section = ""
    if delta:
        delta_section = (
            "\n### Cumulative shift (HEAD vs base branch snapshot)\n\n"
            "_Estimated from link-set snapshots at each ref (one L0 build at HEAD). "
            "Use **Changes in this PR** above for per-PR diffs._\n\n"
        )
        if data.get("compare_base_link_rust_only"):
            delta_section += (
                "_Base snapshot adjusts **Rust** link membership only; module "
                "object % shifts do not reflect C TU add/drop at the base ref._\n\n"
            )
        delta_section += "| Metric | Δ |\n|--------|---|\n"
        if delta.get("module_loc_pct") is not None:
            delta_section += f"| Module LOC % | {delta['module_loc_pct']:+.1f} |\n"
            delta_section += (
                f"| Module objects % | {delta['module_object_pct']:+.1f} |\n"
            )
            ported_abs = delta.get("ported_baseline_c_loc")
            if ported_abs is not None:
                delta_section += (
                    f"| Ported baseline LOC | {ported_abs:+,} lines |\n"
                )
            rust_abs = delta.get("current_rust_loc")
            if rust_abs is not None:
                delta_section += (
                    f"| Rust migration source | {rust_abs:+,} lines |\n"
                )
            unit_count = delta.get("migration_unit_count")
            if unit_count is not None:
                delta_section += (
                    f"| Migration units (Makefile) | {unit_count:+d} |\n"
                )
        else:
            delta_section += (
                "| Module LOC % | _n/a (base module link set not built)_ |\n"
                "| Module objects % | _n/a_ |\n"
            )
        units_delta = delta.get("migration_units_loc_pct")
        if units_delta is not None:
            delta_section += (
                f"| Migration units LOC % | {units_delta:+.1f} |\n"
            )
            units_rust = delta.get("migration_units_rust_loc")
            if units_rust is not None:
                delta_section += (
                    f"| Migration-unit Rust source | {units_rust:+,} lines |\n"
                )
        else:
            delta_section += (
                "| Migration units LOC % | _n/a (base module link set not built)_ |\n"
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
        f"{pr_section}"
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
        "--module-objects",
        type=Path,
        required=True,
        help="Path to 88x2bu link object list (from build-module-objects.sh)",
    )
    parser.add_argument(
        "--compare-ref",
        help="Optional ref to compute deltas against (e.g. origin/master)",
    )
    parser.add_argument(
        "--compare-module-objects",
        type=Path,
        help="Optional base-ref link object list; if omitted, infer from HEAD list + git Makefile",
    )
    parser.add_argument("--json", action="store_true", help="Print JSON instead of markdown")
    args = parser.parse_args()

    meta = load_baseline_meta()
    baseline_ref = args.baseline_ref or meta["baseline_ref"]
    baseline_label = meta.get("baseline_label", baseline_ref)

    objects_text = read_objects_file(args.module_objects)
    makefile_text = (REPO_ROOT / "Makefile").read_text()
    module = compute_module_metrics(baseline_ref, objects_text=objects_text)
    units = compute_migration_units_metrics(
        baseline_ref, makefile_text=makefile_text, objects_text=objects_text
    )
    result: dict = {"baseline_ref": baseline_ref, "module": module, "units": units}

    if args.compare_ref:
        head_obj_list = load_module_objects(objects_text)
        rust_only_base = args.compare_module_objects is None
        if args.compare_module_objects is not None:
            base_obj_list = load_module_objects(
                read_objects_file(args.compare_module_objects)
            )
        else:
            base_obj_list = module_objects_for_ref(head_obj_list, args.compare_ref)
        base_objects_text = "\n".join(base_obj_list)
        base_snap = snapshot_with_objects(
            args.compare_ref, baseline_ref, base_objects_text
        )
        result["pr_changes"] = compute_pr_scope_changes(args.compare_ref, "HEAD")
        result["pr_changes"].update(
            link_set_pr_deltas(
                head_obj_list, base_obj_list, rust_only_base=rust_only_base
            )
        )
        if rust_only_base:
            result["compare_base_link_rust_only"] = True
            result["pr_changes"]["compare_base_link_rust_only"] = True
        delta: dict[str, float | int | None] = {
            "module_loc_pct": None,
            "module_object_pct": None,
            "migration_units_loc_pct": None,
            "ported_baseline_c_loc": None,
            "current_rust_loc": None,
            "migration_unit_count": None,
            "migration_units_rust_loc": None,
        }
        if base_snap["has_module_objects"]:
            delta["migration_units_loc_pct"] = round(
                units["migration_units_loc_pct"]
                - base_snap["units"]["migration_units_loc_pct"],
                1,
            )
            delta["migration_unit_count"] = (
                units["migration_unit_count"]
                - base_snap["units"]["migration_unit_count"]
            )
            delta["migration_units_rust_loc"] = (
                units["migration_units_rust_loc"]
                - base_snap["units"]["migration_units_rust_loc"]
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
            delta["ported_baseline_c_loc"] = (
                module["ported_baseline_c_loc"]
                - base_snap["module"]["ported_baseline_c_loc"]
            )
            delta["current_rust_loc"] = (
                module["current_rust_loc"] - base_snap["module"]["current_rust_loc"]
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
