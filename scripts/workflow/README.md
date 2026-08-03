# Agent workflow scripts

Deterministic helpers for rust-migration agent skills. Prefer these over
re-deriving GitHub queries and dependency rules in chat.

## `find-work.sh` / `find_work.py`

Encodes:

- **Path selection** ([`pick-up-work-item`](../../.cursor/skills/pick-up-work-item/SKILL.md))
- **PR classification** ([`prepare-all-prs-for-merge`](../../.cursor/skills/prepare-all-prs-for-merge/SKILL.md) Phase 1)
- **Ready issue selection** ([`select-ready-issue`](../../.cursor/skills/select-ready-issue/SKILL.md))

### Usage

```bash
# Full pick-up decision (JSON) — run this first on "pick up work"
./scripts/workflow/find-work.sh path

# Human-readable summary
./scripts/workflow/find-work.sh path --human

# PR queue only
./scripts/workflow/find-work.sh prs

# Issue graph / selection only
./scripts/workflow/find-work.sh issues

# User override (prioritize a draft ID when ready)
./scripts/workflow/find-work.sh path --issue W3-40
```

### Requirements

- `gh` authenticated with read access to issues and pull requests
- `git` with `origin` remote (for optional merge-base checks on stacked PR bases)

### Output

`path` emits JSON with three top-level keys:

| Key | Contents |
|-----|----------|
| `prs` | `total`, `eligible`, `skipped`, `needs_prep`, `merge_ready` |
| `issues` | `selected`, `readyCandidates`, `chainHeadBlocked`, `chainHeadInFlight`, `saturation`, `wholeWaveSaturated`, `pathCGap`, `overrideWarning` |
| `pathDecision` | `path` (`A`/`B`/`C`/`stop`), `reason`, `action` |

Agents should **parse this JSON** and follow the chosen path skill — not re-list
or re-classify PRs/issues manually unless the script fails.
