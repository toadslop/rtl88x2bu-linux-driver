#!/usr/bin/env python3
"""Lightweight tests for find_work helpers (no gh network)."""

import sys
import unittest
from pathlib import Path
from unittest import mock

REPO_ROOT = Path(__file__).resolve().parents[2]

import importlib.util

_spec = importlib.util.spec_from_file_location(
    "find_work", REPO_ROOT / "scripts/workflow/find_work.py"
)
assert _spec and _spec.loader
fw = importlib.util.module_from_spec(_spec)
fw.__name__ = "find_work"
sys.modules["find_work"] = fw
_spec.loader.exec_module(fw)


class PrefetchGitRefsTests(unittest.TestCase):
    def setUp(self) -> None:
        fw._GIT_FETCHED_REFS.clear()

    @mock.patch.object(fw, "subprocess")
    def test_batches_large_ref_sets(self, subprocess_mod: mock.Mock) -> None:
        run_mock = subprocess_mod.run
        branches = [f"cursor/branch-{i}" for i in range(95)]
        fw.prefetch_git_refs(branches)
        self.assertGreaterEqual(run_mock.call_count, 2)
        first_args = run_mock.call_args_list[0][0][0]
        self.assertEqual(first_args[0], "git")
        self.assertEqual(first_args[1], "fetch")
        self.assertIn(fw.DEFAULT_BRANCH, first_args)

    @mock.patch.object(fw, "subprocess")
    def test_skips_repeat_prefetch(self, subprocess_mod: mock.Mock) -> None:
        run_mock = subprocess_mod.run
        fw.prefetch_git_refs(["cursor/a"])
        fw.prefetch_git_refs(["cursor/a", "cursor/b"])
        self.assertEqual(run_mock.call_count, 2)


class ClassifyPrepTests(unittest.TestCase):
    def test_changes_requested_needs_prep(self) -> None:
        pr = fw.PullRequest(
            number=1,
            title="[W1-1] test",
            url="http://example",
            is_draft=False,
            base_ref=fw.DEFAULT_BRANCH,
            head_ref="cursor/test",
        )
        pr.enrichment_ok = True
        pr.mergeable = "MERGEABLE"
        pr.merge_state = "CLEAN"
        pr.review_decision = "CHANGES_REQUESTED"
        self.assertEqual(pr.classify_prep(), "needs_prep")


class ImplementsIssueTests(unittest.TestCase):
    def test_docs_filing_pr_does_not_implement_body_issue_refs(self) -> None:
        pr = fw.PullRequest(
            number=1267,
            title="docs: Wave 4 tranche 4 issue specs (W4-14..W4-19)",
            url="http://example",
            is_draft=False,
            base_ref=fw.DEFAULT_BRANCH,
            head_ref="cursor/next-work-item-skill-968c",
        )
        pr.enrich({}, "Filing #1261 and #1265 for W4-14 / W4-18.")
        self.assertFalse(pr.implements_issue(1261, "W4-14"))
        self.assertFalse(pr.implements_issue(1265, "W4-18"))

    def test_docs_filing_pr_matches_title_bracket_id(self) -> None:
        pr = fw.PullRequest(
            number=99,
            title="docs: [W4-14] spec only",
            url="http://example",
            is_draft=False,
            base_ref=fw.DEFAULT_BRANCH,
            head_ref="cursor/spec",
        )
        pr.enrich({}, "")
        self.assertTrue(pr.implements_issue(1261, "W4-14"))


class DecidePathTests(unittest.TestCase):
    def test_path_c_when_no_open_children(self) -> None:
        decision = fw.decide_path(
            {"needs_prep": []},
            {
                "pathCGap": True,
                "pathCReason": "no_open_children",
            },
        )
        self.assertEqual(decision["path"], "C")
        self.assertEqual(decision["reason"], "no_open_children")


if __name__ == "__main__":
    unittest.main()
