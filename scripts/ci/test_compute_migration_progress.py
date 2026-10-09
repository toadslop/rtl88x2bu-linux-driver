#!/usr/bin/env python3
"""Unit tests for migration progress helpers (no L0 build)."""

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest import mock

REPO_ROOT = Path(__file__).resolve().parents[2]
_spec = importlib.util.spec_from_file_location(
    "compute_migration_progress",
    REPO_ROOT / "scripts/ci/compute-migration-progress.py",
)
assert _spec and _spec.loader
cmp = importlib.util.module_from_spec(_spec)
sys.modules["compute_migration_progress"] = cmp
_spec.loader.exec_module(cmp)


class BaselineDiscoveryTests(unittest.TestCase):
    def test_hal_com_maps_to_hal_c(self) -> None:
        self.assertEqual(cmp.baseline_c_for_rust_stem("hal_com"), "hal/hal_com.c")

    def test_partial_hal_com_rest(self) -> None:
        partial = cmp.partial_unit_for_stem("hal_com")
        self.assertIsNotNone(partial)
        self.assertEqual(partial[0], "hal/hal_com.c")
        self.assertEqual(partial[1], "hal/hal_com_rest.c")

    def test_canonical_rest_hal(self) -> None:
        self.assertEqual(
            cmp.canonical_baseline_c("hal/hal_com_rest.c"),
            "hal/hal_com.c",
        )


class MakefileRustStemTests(unittest.TestCase):
    def test_parses_rust_y_lines(self) -> None:
        text = "$(MODULE_NAME)-y += rust/hal_com.o\n$(MODULE_NAME)-y += rust/aes_ctr.o\n"
        self.assertEqual(cmp.rust_port_stems_from_makefile(text), {"hal_com", "aes_ctr"})


class ModuleObjectsForRefTests(unittest.TestCase):
    def test_drops_rust_not_in_base_makefile(self) -> None:
        head = ["rust/hal_com.o", "rust/aes_ctr.o", "core/rtw_debug.o"]
        mf = "$(MODULE_NAME)-y += rust/aes_ctr.o\n"
        with mock.patch.object(cmp, "makefile_text_at", return_value=mf):
            out = cmp.module_objects_for_ref(head, "base-ref")
        self.assertIn("rust/aes_ctr.o", out)
        self.assertNotIn("rust/hal_com.o", out)
        self.assertIn("core/rtw_debug.o", out)


if __name__ == "__main__":
    unittest.main()
