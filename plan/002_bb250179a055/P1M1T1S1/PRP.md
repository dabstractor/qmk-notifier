# PRP — P1.M1.T1.S1: Fix two core.rs firmware-commit comment references (qmk-notifier → qmk_notifier)

---

## Goal

**Feature Goal**: Correct two source-comment references in `src/core.rs` (of the
Rust transport crate `qmk-notifier`) that misattribute firmware commit `01a51935`
to the crate name `qmk-notifier` (hyphen) instead of the firmware module name
`qmk_notifier` (underscore). Commit `01a51935` is a **firmware** fix (RAW reply
size 30→32 bytes) that lives in the `dabstractor/qmk_notifier` C repo, so the
prose must use the underscore form.

**Deliverable**: Two single-word edits in `/home/dustin/projects/qmk-notifier/src/core.rs`
— line 129 and line 425 — changing `qmk-notifier commit \`01a51935\`` to
`qmk_notifier commit \`01a51935\``. Comment-only; no code, no signatures, no other
files, no other lines touched.

**Success Definition**:
- `grep -c "qmk-notifier" src/core.rs` → **0** (down from 2).
- `grep -c "qmk_notifier" src/core.rs` → **2** (up from 0), both being the
  firmware-commit refs on lines 129 and 425.
- `cargo build` compiles cleanly (comment-only change ⇒ identical to baseline).
- `cargo test` → **72 passed, 0 failed** (unchanged from baseline).
- No other file modified; `src/main.rs` and `src/lib.rs` untouched (those are
  sibling subtasks S2 / T2.S1, different direction/file).

## Why

- The ecosystem has a notorious naming hazard (PRD §1.2): `qmk-notifier` (hyphen)
  = this Rust **crate**; `qmk_notifier` (underscore) = the **firmware** C module.
  They are different repos (`dabstractor/qmk-notifier` vs `dabstractor/qmk_notifier`)
  that talk over a fixed wire protocol.
- A developer reading these two comments is being told a *firmware* fix landed in
  the *crate* repo — exactly the kind of cross-repo confusion the PRD calls out as
  a hazard. Correct attribution removes a real source of future bugs (a dev chasing
  commit `01a51935` in the wrong repo).
- This is part of milestone **P1.M1 "Consistent naming across source and
  documentation"**. S1 is the crate-source firmware-commit fix; S2 (main.rs) and
  T2.S1 (lib.rs) handle the inverse-direction and test-string fixes separately.

## What

Two edits in `/home/dustin/projects/qmk-notifier/src/core.rs` — replace the hyphen
with an underscore in the firmware-commit attribution only:

**Edit 1 — line 129** (doc comment on `const IN_DRAIN_MAX`):
```diff
-/// sends a 32-byte reply per report (fixed in qmk-notifier commit `01a51935`,
+/// sends a 32-byte reply per report (fixed in qmk_notifier commit `01a51935`,
```

**Edit 2 — line 425** (inline comment inside the drain loop of `burst_to_one`,
note the 4-space indent + `// `):
```diff
-    // valid 32-byte reply per report (qmk-notifier commit `01a51935`, which
+    // valid 32-byte reply per report (qmk_notifier commit `01a51935`, which
```

These are the **only** two `qmk-notifier` (hyphen) occurrences in `core.rs`
(verified: `grep -c "qmk-notifier" src/core.rs` == 2 pre-edit). Both are
firmware-commit references, so both are correctly underscored. There are **no**
crate-referential `qmk-notifier` comments and **no** `use qmk_notifier::` /
`mod qmk_notifier` lines in `core.rs` to worry about.

### Success Criteria

- [ ] Line 129 reads `qmk_notifier commit \`01a51935\`` (underscore).
- [ ] Line 425 reads `qmk_notifier commit \`01a51935\`` (underscore).
- [ ] `grep -c "qmk-notifier" src/core.rs` == **0**.
- [ ] `grep -c "qmk_notifier" src/core.rs` == **2**.
- [ ] `cargo build` → clean (0 warnings), identical to baseline.
- [ ] `cargo test` → **72 passed, 0 failed**.
- [ ] Only `src/core.rs` changed; `main.rs`, `lib.rs`, `README.md`, `PRD.md` untouched.

## All Needed Context

### Context Completeness Check

> _"If someone knew nothing about this codebase, would they have everything needed
> to implement this successfully?"_ — **Yes.** The exact two lines, their unique
> surrounding-text anchors, the before/after diff, the post-edit grep gates, and
> the cross-repo naming hazard are all included. The implementer needs no knowledge
> beyond what's here plus the ability to run `grep`/`cargo` in the crate repo.

### Documentation & References

```yaml
# MUST READ — the authoritative naming rule + ecosystem table
- file: PRD.md
  why: "§1.2 defines the naming hazard: qmk-notifier (hyphen) = this Rust crate;
        qmk_notifier (underscore) = firmware C module. §14 invariant 8: firmware
        PRD wins on disagreement. These are the rule this edit enforces."
  section: "1.2 The broader ecosystem (a dev must understand all three)"
  critical: "The crate's Rust LIBRARY identifier is ALSO qmk_notifier (Cargo
             derives _ from -), so `use qmk_notifier::` is correct and INVARIANT.
             This task only fixes PROSE/comments that name the firmware repo."

# MUST READ — the audit that classifies these two lines
- file: plan/002_bb250179a055/architecture/naming_audit.md
  why: "Rows 52-53 classify src/core.rs:129 and :425 as FIRMWARE references
        currently mislabeled as the crate; action items (lines 93-94) prescribe
        the exact `qmk_notifier commit \\`01a51935\\`` target."
  section: "rows 52, 53, and Action items 1-2"

# MUST READ / EDIT — the file being changed
- file: src/core.rs
  why: "Lines 129 and 425 are the two edit sites. Both are comments attributing
        firmware commit 01a51935 (RAW reply size 30->32 fix)."
  pattern: "Existing comment style: `///` for items, `// ` for inline; backticked
            commit hashes; 4-space indent for comments inside fn bodies (line 425)."
  gotcha: "Line 425 sits INSIDE burst_to_one's drain loop and is indented 4 spaces
           with a `// ` prefix — preserve the exact leading whitespace and the
           backtick-delimited hash. Line 129 is a `/// ` doc comment."

# REFERENCE — verified findings for this subtask
- docfile: plan/002_bb250179a055/P1M1T1S1/research/naming_fix_findings.md
  why: "Records the cross-repo path hazard, the exact verified line content, the
        'only 2 occurrences' finding, and the post-edit grep invariants."
```

### Current Codebase tree (verified, crate repo)

```bash
# Repo: /home/dustin/projects/qmk-notifier  (HYPHEN = Rust crate; the agent's cwd
# /home/dustin/projects/qmk_notifier UNDERSCORE is the firmware C repo — do NOT
# edit there for this task.)
.
├── Cargo.toml          # name="qmk-notifier", version="0.3.0"
├── Cargo.lock
├── README.md
├── PRD.md
└── src
    ├── main.rs         # (out of scope — sibling subtask S2)
    ├── lib.rs          # (out of scope — sibling subtask T2.S1)
    ├── core.rs         # <-- FILE TO EDIT: lines 129 and 425 ONLY
    └── error.rs
```

### Desired Codebase tree with files to be added/modified

```bash
src/
└── core.rs   # MODIFIED ONLY — 2 comment-word edits (line 129, line 425). No new files.
```

### Known Gotchas of our codebase & Library Quirks

```text
# CRITICAL — cross-repo naming hazard (you are likely standing in the WRONG repo):
#   The agent's cwd may be /home/dustin/projects/qmk_notifier (UNDERSCORE) = the
#   FIRMWARE C repo (notifier.c, pattern_match.c). It has NO src/core.rs.
#   The file to edit is in the CRATE repo:
#     /home/dustin/projects/qmk-notifier/src/core.rs   (HYPHEN)
#   Verify with: `test -f /home/dustin/projects/qmk-notifier/src/core.rs && echo OK`
#   The plan dir `plan/002_bb250179a055/` also lives in the CRATE repo only.

# CRITICAL — direction of this edit (do not invert):
#   HERE: qmk-notifier  ->  qmk_notifier   (hyphen -> underscore)
#   This is because the reference is to the FIRMWARE repo (commit 01a51935).
#   The OPPOSITE direction (underscore -> hyphen) is sibling subtask S2 (main.rs).
#   If you find yourself changing underscore to hyphen in core.rs, STOP — wrong task.

# CRITICAL — these 2 are the ONLY hyphenated refs in core.rs:
#   Pre-edit:  grep -c "qmk-notifier" src/core.rs  == 2   (lines 129, 425)
#             grep -c "qmk_notifier" src/core.rs  == 0
#   Post-edit: grep -c "qmk-notifier" src/core.rs  == 0
#             grep -c "qmk_notifier" src/core.rs  == 2
#   Because both occurrences are firmware-commit refs, replacing BOTH is correct
#   and exhaustive. Do NOT hunt for more — there are none.

# NOTE — preserve exact formatting:
#   Line 129 is `/// ` (doc comment). Line 425 is `    // ` (4-space indent inside
#   burst_to_one's body). Keep the leading whitespace, the backticks around
#   `01a51935`, and everything after the commit hash unchanged. Only the one word
#   `qmk-notifier` -> `qmk_notifier` changes on each line.

# NOTE — do NOT touch Rust identifiers:
#   `use qmk_notifier::` / `mod qmk_notifier` (if present anywhere) are
#   Cargo-derived library identifiers and are INVARIANT. (None exist in core.rs;
#   it self-references via `crate::`. Listed only as a guardrail.)
```

## Implementation Blueprint

### Data models and structure

None. This is a pure comment edit — no types, no logic, no signatures, no tests added.

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: LOCATE & VERIFY (in the CRATE repo /home/dustin/projects/qmk-notifier)
  - RUN: `test -f /home/dustin/projects/qmk-notifier/src/core.rs && echo OK`
    (confirm you are editing the crate, not the firmware cwd).
  - RUN: `grep -n "qmk-notifier" src/core.rs`
    EXPECT: exactly two hits — line 129 and line 425. (If you see a different
    count, STOP: the file has drifted since this PRP; re-derive the lines.)
  - RUN: `grep -n "01a51935" src/core.rs`
    EXPECT: exactly two hits — line 129 and line 425 (the same two lines).

Task 2: EDIT line 129 (doc comment on IN_DRAIN_MAX)
  - OLD (unique anchor): `fixed in qmk-notifier commit \`01a51935\``
  - NEW:                  `fixed in qmk_notifier commit \`01a51935\``
  - PRESERVE: the `/// ` prefix and everything after the closing backtick.

Task 3: EDIT line 425 (inline comment in burst_to_one drain loop)
  - OLD (unique anchor): `valid 32-byte reply per report (qmk-notifier commit \`01a51935\`, which`
  - NEW:                  `valid 32-byte reply per report (qmk_notifier commit \`01a51935\``, which`
  - PRESERVE: the 4-space indent, the `// ` prefix, and the trailing `, which`.
  - NOTE: the two oldText anchors differ ("fixed in …" vs "valid … per report …"),
    so they are independently unique — safe to edit in one multi-edit call.

Task 4: VALIDATE (grep gates are the real signal; build/test prove no collateral)
  - RUN the Validation Loop below.
```

### Implementation Patterns & Key Details

```text
# Both edits are mechanical word swaps. The only "pattern" is precision:

# Anchor uniqueness (verified): the substrings
#   "fixed in qmk-notifier commit `01a51935`"        (line 129)
#   "valid 32-byte reply per report (qmk-notifier commit `01a51935`, which"  (line 425)
# each appear exactly once in core.rs. Use them verbatim as the search targets
# so the edit cannot accidentally hit any other line.

# Multi-edit: both swaps may be applied in a SINGLE edit call with two entries
# (the anchors do not overlap). Order within the call does not matter.
```

### Integration Points

```yaml
SOURCE FILES:
  - modify: "src/core.rs ONLY (crate repo /home/dustin/projects/qmk-notifier)"
  - change: "2 comment-word edits (line 129 doc comment; line 425 inline comment)"

BUILD / DEPS / PUBLIC API:
  - none affected. "Comment-only; Cargo.toml, signatures, re-exports unchanged."

DOCUMENTATION:
  - none external. "No README/PRD edits here (those are P1.M1.T3)."
```

## Validation Loop

### Level 1: Syntax & Style (Immediate Feedback)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE repo (hyphen), NOT the firmware cwd

cargo fmt            # rustfmt default style; comment edits are format-neutral
cargo fmt --check    # EXPECT: exit 0 (no formatting change from a comment-word swap)
cargo build          # EXPECT: clean, 0 warnings (identical to baseline)
```

### Level 2: Unit Tests (Component Validation)

```bash
cargo test
# EXPECT: "test result: ok. 72 passed; 0 failed; 0 ignored; ..." (unchanged baseline).
# Comment-only edits cannot change test outcomes; this gate guards against an
# accidental non-comment edit (e.g. a typo'd code token).
```

### Level 3: Integration Testing (System Validation)

```text
NOT APPLICABLE. This is a comment-only change to a transport crate with no HID
device required. The grep gates (Level 4) and the build/test gates (Levels 1-2)
fully cover correctness. No service to start, no endpoint to hit, no hardware path.
```

### Level 4: Naming-Specific Verification (the real acceptance gate)

```bash
cd /home/dustin/projects/qmk-notifier

# Before/after counts (the definitive signal):
echo "hyphen (must be 0):  $(grep -c 'qmk-notifier' src/core.rs)"
echo "underscore (must be 2): $(grep -c 'qmk_notifier' src/core.rs)"

# Show the two fixed lines with context to confirm they still attribute 01a51935:
grep -n "qmk_notifier commit \`01a51935\`" src/core.rs
# EXPECT: two hits — line ~129 (/// doc comment) and line ~425 (    // inline).

# Negative gate: no surviving hyphenated firmware-commit attribution:
grep -n "qmk-notifier commit" src/core.rs || echo "OK: no stale hyphenated commit refs"
# EXPECT: the "OK: ..." branch (grep exits 1 ⇒ "|| echo" fires).

# Confirm scope: only core.rs changed (no main.rs / lib.rs drift):
git -C /home/dustin/projects/qmk-notifier diff --name-only
# EXPECT: exactly `src/core.rs` (and nothing else).
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1 passed: `cargo build` → 0 warnings; `cargo fmt --check` → exit 0.
- [ ] Level 2 passed: `cargo test` → 72 passed, 0 failed.
- [ ] Level 4 passed: `grep -c "qmk-notifier" src/core.rs` == 0.
- [ ] Level 4 passed: `grep -c "qmk_notifier" src/core.rs` == 2 (lines 129 & 425).
- [ ] `git diff --name-only` == `src/core.rs` only.

### Feature Validation

- [ ] Line 129 attributes commit `01a51935` to `qmk_notifier` (firmware).
- [ ] Line 425 attributes commit `01a51935` to `qmk_notifier` (firmware).
- [ ] No other `qmk-notifier` reference in core.rs was created or left behind.
- [ ] Commit hash `01a51935` backticks and surrounding prose preserved verbatim.

### Code Quality Validation

- [ ] Comment style preserved (`/// ` on 129; `    // ` indent on 425).
- [ ] No Rust identifier (`use`/`mod`) touched.
- [ ] No files outside `src/core.rs` modified.

### Documentation & Deployment

- [ ] No README/PRD changes (out of scope — P1.M1.T3).
- [ ] No env vars, config, or public API surface affected (comment-only).

---

## Anti-Patterns to Avoid

- ❌ Don't edit in the firmware cwd (`/home/dustin/projects/qmk_notifier`, underscore).
      The target is the **crate** repo `/home/dustin/projects/qmk-notifier` (hyphen).
- ❌ Don't invert the direction. HERE it is hyphen→underscore (firmware commit).
      The underscore→hyphen direction is a DIFFERENT subtask (S2, main.rs).
- ❌ Don't "fix all" `qmk-notifier` across the repo — only the two core.rs lines.
      main.rs / lib.rs / README / PRD are separate subtasks with different rules.
- ❌ Don't touch `use qmk_notifier::` / `mod qmk_notifier` identifiers — those are
      Cargo-derived library names and are invariant. (None exist in core.rs anyway.)
- ❌ Don't reformat the surrounding comment block — preserve leading whitespace,
      backticks, and trailing prose. Only the single word changes per line.
- ❌ Don't add/alter tests or code — this is comment-only; `cargo test` must stay
      at exactly 72 passed with no source-token change.

---

**Confidence Score: 10/10** for one-pass implementation success. It is a
deterministic, two-word comment edit with verified exact line numbers, unique
edit anchors, an exhaustive occurrence map (these are the only two hyphenated
refs in core.rs), and machine-checkable grep gates (2→0 hyphen, 0→2 underscore).
The only residual risk — editing in the wrong repo — is called out explicitly with
a `test -f` guard and absolute paths.