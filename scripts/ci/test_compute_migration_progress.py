#!/usr/bin/env python3
"""Unit tests for migration link-mix (no L0 build)."""

import importlib.util
import sys
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
_spec = importlib.util.spec_from_file_location(
    "compute_migration_progress",
    REPO_ROOT / "scripts/ci/compute-migration-progress.py",
)
assert _spec and _spec.loader
cmp = importlib.util.module_from_spec(_spec)
sys.modules["compute_migration_progress"] = cmp
_spec.loader.exec_module(cmp)


class ComputeLinkMixTests(unittest.TestCase):
    def test_counts_rust_vs_other(self) -> None:
        objects = [
            "rust/aes_ctr.o",
            "rust/hal_com.o",
            "core/rtw_debug.o",
            "hal/hal_intf.o",
            "os_dep/linux/usb_intf.o",
        ]
        mix = cmp.compute_link_mix(objects)
        self.assertEqual(mix["total_objects"], 5)
        self.assertEqual(mix["rust_objects"], 2)
        self.assertEqual(mix["c_objects"], 3)
        self.assertEqual(mix["rust_pct"], 40.0)
        self.assertEqual(mix["c_pct"], 60.0)

    def test_empty(self) -> None:
        mix = cmp.compute_link_mix([])
        self.assertEqual(mix["rust_pct"], 0.0)
        self.assertEqual(mix["c_pct"], 0.0)

    def test_format_is_single_metric(self) -> None:
        md = cmp.format_markdown(
            {
                "rust_pct": 36.2,
                "c_pct": 63.8,
                "rust_objects": 140,
                "total_objects": 387,
            }
        )
        self.assertIn("36.2% Rust", md)
        self.assertIn("63.8% C", md)
        self.assertIn("140 / 387", md)
        self.assertNotIn("LOC", md)


if __name__ == "__main__":
    unittest.main()