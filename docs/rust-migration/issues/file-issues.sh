#!/usr/bin/env bash
# File draft issues from docs/rust-migration/issues/*.md onto GitHub.
# Idempotent: skips draft IDs already present in ISSUE-MAP.md; appends new rows.
# Requires: Issues enabled on the repo; gh authenticated with issue write scope.
set -euo pipefail

ISSUE_DIR="$(cd "$(dirname "$0")" && pwd)"
# issues/ -> rust-migration/ -> docs/ -> repo root
ROOT="$(cd "$ISSUE_DIR/../../.." && pwd)"
MAP_FILE="$ISSUE_DIR/ISSUE-MAP.md"
REPO="${GITHUB_REPOSITORY:-$(gh repo view --json nameWithOwner -q .nameWithOwner)}"
REF="${FILE_ISSUES_REF:-$(git -C "$ROOT" rev-parse --abbrev-ref HEAD)}"

if ! gh api "repos/$REPO" --jq .has_issues | grep -q true; then
  echo "ERROR: Issues are disabled on $REPO." >&2
  echo "Enable them: GitHub → Settings → General → Features → Issues" >&2
  echo "Then re-run: $0" >&2
  exit 1
fi

labels=(
  rust-migration
  phase-1
  phase-2
  wave-0 wave-1 wave-2 wave-3 wave-4 wave-5 wave-6
  "size/~200"
)

echo "Ensuring labels on $REPO..."
for label in "${labels[@]}"; do
  color=0E8A16
  case "$label" in
    phase-2) color=5319E7 ;;
    wave-*) color=1D76DB ;;
    size/*) color=FBCA04 ;;
    rust-migration) color=B60205 ;;
  esac
  gh label create "$label" --repo "$REPO" --color "$color" --force >/dev/null
done

files=(
  epic-01-phase1.md
  epic-02-wave0.md
  epic-03-wave1.md
  epic-04-wave2.md
  epic-05-wave3.md
  epic-06-wave4.md
  epic-07-wave5.md
  epic-08-wave6.md
  epic-09-phase2.md
  epic-10-test-infra.md
  epic-11-architecture.md
  test-00-test-plan-doc.md
  test-01-symbol-check.md
  test-02-host-crypto-harness.md
  test-03-ci-host-tests.md
  arch-00-land-doc.md
  arch-01-domain-types-seed.md
  wave0-01-docs.md
  wave0-02-kbuild.md
  wave0-03-scaffold.md
  wave1-01-bindgen.md
  wave1-02-ffi-module.md
  wave1-03-pilot-aes-ctr.md
  wave1-04-pilot-makefile-smoke.md
  wave2-01-aes-omac1.md
  wave2-02-gcmp.md
  wave2-03-aes-siv.md
  wave2-04-aes-ccm.md
  wave2-05-sha256-internal.md
  wave2-06-sha256-prf-wrap.md
  wave2-07-aes-gcm-part1.md
  wave2-08-aes-gcm-part2.md
  wave2-09-ccmp-part1.md
  wave2-10-ccmp-part2.md
  wave2-11-aes-internal-part1.md
  wave2-12-aes-internal-part2.md
  wave2-13-aes-internal-part3.md
  wave2-14-aes-internal-part4.md
  wave2-15-aes-internal-enc.md
  wave2-16-sha256.md
)

# draft_id -> github issue number
declare -A ID_TO_NUM=()

load_map() {
  [[ -f "$MAP_FILE" ]] || return 0
  while IFS= read -r line; do
    if [[ "$line" =~ ^\|\ ([A-Za-z0-9-]+)\ \|\ #([0-9]+)\ \| ]]; then
      ID_TO_NUM["${BASH_REMATCH[1]}"]="${BASH_REMATCH[2]}"
    fi
  done <"$MAP_FILE"
}

ensure_map_header() {
  if [[ ! -f "$MAP_FILE" ]]; then
    {
      echo "# Issue map"
      echo ""
      echo "Generated/updated by \`docs/rust-migration/issues/file-issues.sh\`."
      echo ""
      echo "| Draft ID | GitHub | Title |"
      echo "|----------|--------|-------|"
    } >"$MAP_FILE"
  elif ! grep -q '^| Draft ID |' "$MAP_FILE"; then
    {
      echo "# Issue map"
      echo ""
      echo "| Draft ID | GitHub | Title |"
      echo "|----------|--------|-------|"
      cat "$MAP_FILE"
    } >"$MAP_FILE.tmp"
    mv "$MAP_FILE.tmp" "$MAP_FILE"
  fi
}

extract_field() {
  local file="$1" key="$2"
  sed -n "/^---$/,/^---$/p" "$file" | sed -n "s/^${key}:[[:space:]]*//p" | head -1
}

parse_id_list() {
  # "[A, B]" or "[]" -> newline-separated ids
  local raw="$1"
  raw="${raw#\[}"
  raw="${raw%\]}"
  raw="${raw//,/ }"
  for tok in $raw; do
    tok="${tok// /}"
    [[ -n "$tok" ]] && printf '%s\n' "$tok"
  done
}

body_without_frontmatter() {
  local file="$1"
  awk 'BEGIN{fm=0} /^---$/{fm++; next} fm>=2{print}' "$file"
}

# Rewrite markdown links to absolute GitHub blob URLs so they work in Issues UI.
# Handles repo-root paths (core/foo.c) and relative paths from the draft file.
rewrite_links() {
  local src_file="$1"
  local body="$2"
  local src_dir
  src_dir="$(cd "$(dirname "$src_file")" && pwd)"
  REWRITE_BODY="$body" python3 - "$ROOT" "$REPO" "$REF" "$src_dir" <<'PY'
import os, re, sys

root, repo, ref, src_dir = sys.argv[1:5]
body = os.environ["REWRITE_BODY"]
blob = "https://github.com/{}/blob/{}/".format(repo, ref)

def to_repo_rel(url):
    if url.startswith(("http://", "https://", "mailto:", "#")):
        return None
    path = url.split()[0] if url else url
    if path.startswith("/"):
        return None
    abs_path = os.path.normpath(os.path.join(src_dir, path))
    try:
        rel = os.path.relpath(abs_path, root)
    except ValueError:
        return None
    if rel.startswith(".."):
        if path.startswith(("core/", "docs/", "hal/", "os_dep/", "include/", "rust/")):
            return path.lstrip("./")
        return None
    return rel.replace(os.sep, "/")

def repl(m):
    text, url = m.group(1), m.group(2)
    rel = to_repo_rel(url)
    if rel is None:
        return m.group(0)
    return "[{}]({}{})".format(text, blob, rel)

sys.stdout.write(re.sub(r"\[([^\]]+)\]\(([^)]+)\)", repl, body))
PY
}

append_dependency_footer() {
  local body="$1"
  local epic_id="$2"
  local blocked_csv="$3"
  local footer=""

  if [[ -n "$epic_id" && -n "${ID_TO_NUM[$epic_id]:-}" ]]; then
    footer+="- **Epic:** #${ID_TO_NUM[$epic_id]} (\`$epic_id\`)"$'\n'
  elif [[ -n "$epic_id" ]]; then
    footer+="- **Epic:** \`$epic_id\` (not filed yet)"$'\n'
  fi

  local blocked_line=""
  while IFS= read -r dep; do
    [[ -z "$dep" ]] && continue
    if [[ -n "${ID_TO_NUM[$dep]:-}" ]]; then
      blocked_line+="#${ID_TO_NUM[$dep]} (\`$dep\`), "
    else
      blocked_line+="\`$dep\` (not filed yet), "
    fi
  done < <(parse_id_list "$blocked_csv")

  if [[ -n "$blocked_line" ]]; then
    blocked_line="${blocked_line%, }"
    footer+="- **Blocked by:** $blocked_line"$'\n'
  fi

  if [[ -n "$footer" ]]; then
    printf '%s\n\n## Tracking\n\n%s' "$body" "$footer"
  else
    printf '%s' "$body"
  fi
}

map_has_id() {
  local id="$1"
  [[ -n "${ID_TO_NUM[$id]:-}" ]]
}

find_existing_issue_by_title() {
  local title="$1"
  gh issue list --repo "$REPO" --state all --search "in:title \"$title\"" --json number,title \
    --jq ".[] | select(.title == \"$title\") | .number" | head -1
}

load_map
ensure_map_header

created=0
skipped=0

for f in "${files[@]}"; do
  path="$ISSUE_DIR/$f"
  title="$(extract_field "$path" title | sed 's/^"//;s/"$//')"
  labels_csv="$(extract_field "$path" labels)"
  id="$(extract_field "$path" id)"
  epic_id="$(extract_field "$path" epic)"
  blocked_csv="$(extract_field "$path" blocked_by)"
  [[ -z "$blocked_csv" ]] && blocked_csv="[]"

  if map_has_id "$id"; then
    echo "Skipping $id (already in ISSUE-MAP.md as #${ID_TO_NUM[$id]})"
    skipped=$((skipped + 1))
    continue
  fi

  existing="$(find_existing_issue_by_title "$title" || true)"
  if [[ -n "$existing" ]]; then
    echo "Skipping $id (found existing #$existing by title); recording in map"
    ID_TO_NUM["$id"]="$existing"
    echo "| $id | #$existing | $title |" >>"$MAP_FILE"
    skipped=$((skipped + 1))
    continue
  fi

  label_args=()
  labels_csv="${labels_csv#\[}"
  labels_csv="${labels_csv%\]}"
  IFS=',' read -ra raw_labels <<<"$labels_csv"
  for lab in "${raw_labels[@]}"; do
    lab="$(echo "$lab" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')"
    [[ -n "$lab" ]] && label_args+=(--label "$lab")
  done

  body="$(body_without_frontmatter "$path")"
  body="$(rewrite_links "$path" "$body")"
  body="$(append_dependency_footer "$body" "$epic_id" "$blocked_csv")"

  echo "Creating: $title"
  url="$(gh issue create --repo "$REPO" --title "$title" --body "$body" "${label_args[@]}")"
  num="${url##*/}"
  ID_TO_NUM["$id"]="$num"
  echo "| $id | #$num | $title |" >>"$MAP_FILE"
  echo "  -> $url"
  created=$((created + 1))
done

# Second pass: refresh Tracking footers now that more IDs are resolved.
echo ""
echo "Refreshing dependency footers where epic/blocked_by targets exist..."
for f in "${files[@]}"; do
  path="$ISSUE_DIR/$f"
  id="$(extract_field "$path" id)"
  num="${ID_TO_NUM[$id]:-}"
  [[ -z "$num" ]] && continue
  epic_id="$(extract_field "$path" epic)"
  blocked_csv="$(extract_field "$path" blocked_by)"
  [[ -z "$blocked_csv" ]] && blocked_csv="[]"
  title="$(extract_field "$path" title | sed 's/^"//;s/"$//')"
  body="$(body_without_frontmatter "$path")"
  body="$(rewrite_links "$path" "$body")"
  body="$(append_dependency_footer "$body" "$epic_id" "$blocked_csv")"
  gh issue edit "$num" --repo "$REPO" --body "$body" >/dev/null
  echo "  updated #$num ($id)"
done

echo ""
echo "Done. created=$created skipped=$skipped map=$MAP_FILE"
