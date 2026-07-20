# PRP — P1.M1.T1.S2: Fix main.rs binary-invocation comment reference (qmk_notifier → qmk-notifier)

---

## Goal

**Feature Goal**: Correct one source-comment reference in `src/main.rs` of the
Rust transport crate `qmk-notifier` that names the CLI **binary** using the
firmware underscore form (`qmk_notifier`) instead of the crate/binary hyphen
form (`qmk-notifier`). The binary IS `qmk-notifier` (per `Cargo.toml
name = "qmk-notifier"` and PRD §1.2/§2). A shell-invocation example must use the
hyphen form; today it uses the firmware module name, which is exactly the
cross-repo naming hazard PRD §1.2 warns about.

**Deliverable**: A single one-word edit on one doc-comment line of
`/home/dustin/projects/qmk-notifier/src/main.rs` — the SIGPIPE comment's
`(e.g. \`qmk_notifier --list | head -1\`)` becomes
`(e.g. \`qmk-notifier --list | head -1\`)`. Comment-only; no code, no signatures,
no other lines, no other files touched. The `use qmk_notifier::` library import
on line 1 is **invariant** and must NOT be changed.

**Success Definition**:
- `grep -c "qmk_notifier" src/main.rs` → **1** (down from 2; only the line-1
  library import remains).
- `grep -c "qmk-notifier" src/main.rs` → **1** (up from 0; the fixed line 88).
- `grep -n "qmk-notifier --list | head -1" src/main.rs` → exactly one hit (line ~88).
- `cargo build` → 0 warnings (identical to baseline; comment-only).
- `cargo test` → **72 passed, 0 failed** (unchanged from baseline).
- `cargo fmt --check` → exit 0.
- `git diff --name-only` → exactly `src/main.rs`.
- No other file modified; `src/core.rs` (sibling S1) and `src/lib.rs` (sibling
  T2.S1) untouched.

## User Persona (if applicable)

**Target User**: A developer (or downstream `qmkonnect` integrator) reading
`main.rs`'s SIGPIPE rationale and copying the shell example.

**Use Case**: Reproduce the SIGPIPE rationale locally by running
`qmk-notifier --list | head -1`.

**User Journey**: Read the `reset_sigpipe_to_default` doc comment → copy the
example → run `qmk-notifier --list | head -1` → observe exit 141 (SIGPIPE) on a
closed pipe.

**Pain Points Addressed**: Today the example says `qmk_notifier` (underscore) —
which is the firmware C module name, not the binary. A reader who runs it verbatim
gets "command not found"; worse, they may conflate the firmware module with the
crate. PRD §1.2 calls this conflation a documented hazard.

## Why

- PRD §1.2 names the hazard explicitly: `qmk-notifier` (hyphen) = this Rust
  **crate** (package + repo + binary); `qmk_notifier` (underscore) = the firmware
  **C module**. The comment in `main.rs` is a shell example that invokes the
  **binary**, so it must use the hyphen.
- This is part of milestone **P1.M1 "Consistent naming across source and
  documentation"**. S1 fixes two `core.rs` firmware-commit refs (hyphen→underscore,
  the *inverse* direction, because those name the firmware repo). S2 (this task)
  fixes the `main.rs` binary-invocation ref (underscore→hyphen). T2.S1 handles
  `lib.rs` test `argv[0]` strings. T3 sweeps README/PRD.
- Correct attribution removes a real source of "command not found" confusion and
  reinforces the hyphen/underscore rule at the exact code site a developer reads.

## What

One one-word edit on one doc-comment line in
`/home/dustin/projects/qmk-notifier/src/main.rs`:

```diff
-/// (e.g. `qmk_notifier --list | head -1`). This restores that behavior.
+/// (e.g. `qmk-notifier --list | head -1`). This restores that behavior.
```

This line is in the `///` doc comment on `reset_sigpipe_to_default`
(`#[cfg(unix)] fn reset_sigpipe_to_default()`), at approximately line 88 (locate
by content, not line number — see Gotchas). The surrounding block reads:

```rust
/// Rust's runtime sets SIGPIPE to `SIG_IGN`, which turns the next `println!` to
/// a closed pipe into a panic (exit 101). Unix CLI tools are expected to die
/// quietly with SIGPIPE (exit 141) when a downstream consumer exits early
/// (e.g. `qmk_notifier --list | head -1`). This restores that behavior.
///                                                  ^^^^^^^^^^^^^  <-- FIX THIS TOKEN
```

Replace ONLY the token `qmk_notifier` → `qmk-notifier` inside the backticked
shell example. Preserve the leading `/// `, the backticks, the `(e.g. …)` wrapping,
the ` --list | head -1` arguments, and the trailing `. This restores that behavior.`
verbatim.

### Success Criteria

- [ ] Line ~88 reads `(e.g. \`qmk-notifier --list | head -1\`). This restores that behavior.`
- [ ] Line 1 `use qmk_notifier::{…};` is byte-for-byte **unchanged** (Cargo-derived
      library identifier — invariant).
- [ ] `grep -c "qmk_notifier" src/main.rs` == **1** (only the line-1 import).
- [ ] `grep -c "qmk-notifier" src/main.rs` == **1** (the fixed line 88).
- [ ] `cargo build` → 0 warnings; `cargo test` → 72 passed, 0 failed.
- [ ] `cargo fmt --check` → exit 0.
- [ ] Only `src/main.rs` changed; `core.rs`, `lib.rs`, `README.md`, `PRD.md` untouched.

## All Needed Context

### Context Completeness Check

> _"If someone knew nothing about this codebase, would they have everything needed
> to implement this successfully?"_ — **Yes.** The exact line content, the unique
> edit anchor, the before/after diff, the invariant line-1 guardrail, the post-edit
> grep gates, the cross-repo cwd hazard, and the verified build/test commands are
> all included. The implementer needs no knowledge beyond what is here plus the
> ability to run `grep`/`cargo` in the **crate** repo.

### Documentation & References

```yaml
# MUST READ — the authoritative naming rule + ecosystem table
- file: PRD.md
  why: "§1.2 defines the naming hazard: qmk-notifier (hyphen) = this Rust crate
        (package + repo + binary); qmk_notifier (underscore) = firmware C module.
        It also states the library IDENTIFIER is qmk_notifier (Cargo derives _
        from -), so `use qmk_notifier::` is correct and invariant."
  section: "1.2 The broader ecosystem (a dev must understand all three)"
  critical: "This task fixes a PROSE comment naming the BINARY, which must use the
             hyphen. It does NOT touch the library identifier `qmk_notifier` on
             line 1 — that is invariant."

# MUST READ — the audit that classifies this exact line
- file: plan/002_bb250179a055/architecture/naming_audit.md
  why: "Row 47 classifies src/main.rs:88 as a CRATE (binary) reference currently
        using the firmware underscore (WRONG), action YES; prescribes the target
        `qmk-notifier --list | head -1`. Row 46 separately classifies line 1
        (the use-import) as LIBRARY_ID, action NO. Action item 3 (line 95) states
        the exact before→after."
  section: "rows 46, 47 and Action item 3"

# MUST READ / EDIT — the file being changed
- file: src/main.rs
  why: "Line ~88 is the edit site (the SIGPIPE doc comment on
        reset_sigpipe_to_default). Line 1 is the invariant library import."
  pattern: "Comment style: `/// ` doc comments; backticked code/shell examples;
            prose punctuation preserved verbatim."
  gotcha: "LOCATE the line via `grep -n 'qmk_notifier --list' src/main.rs`, NOT by
           a hardcoded line number — minor drift is possible. The edit anchor
           `qmk_notifier --list | head -1` is unique in the file."

# REFERENCE — verified findings for this subtask
- docfile: plan/002_bb250179a055/P1M1T1S2/research/notes.md
  why: "Records the cross-repo cwd hazard, the underscore→hyphen direction
        (inverse of S1), the exhaustive occurrence map (qmk_notifier on lines 1
        and 88; no qmk-notifier yet), the binary-name confirmation from
        Cargo.toml, and the post-edit grep invariants."
```

### Current Codebase tree (verified, CRATE repo)

```bash
# Repo: /home/dustin/projects/qmk-notifier   (HYPHEN = Rust crate; the agent's cwd
# /home/dustin/projects/qmk_notifier UNDERSCORE is the firmware C repo — do NOT
# edit there for this task. The plan dir also lives only in the crate repo.)
.
├── Cargo.toml          # name="qmk-notifier" (HYPHEN); libc gated under cfg(unix)
├── Cargo.lock
├── README.md
├── PRD.md
└── src
    ├── main.rs         # <-- FILE TO EDIT: line ~88 only (line 1 invariant)
    ├── lib.rs          # (out of scope — sibling subtask T2.S1)
    ├── core.rs         # (out of scope — sibling subtask S1)
    └── error.rs
```

### Desired Codebase tree with files to be added/modified

```bash
src/
└── main.rs   # MODIFIED ONLY — 1 comment-token edit (line ~88). No new files.
```

### Known Gotchas of our codebase & Library Quirks

```text
# CRITICAL — cross-repo cwd hazard (you are likely standing in the WRONG repo):
#   The agent's cwd may be /home/dustin/projects/qmk_notifier (UNDERSCORE) = the
#   FIRMWARE C repo (notifier.c, pattern_match.c). It has NO src/main.rs.
#   The file to edit is in the CRATE repo:
#     /home/dustin/projects/qmk-notifier/src/main.rs   (HYPHEN)
#   Verify: `cd /home/dustin/projects/qmk-notifier && test -f src/main.rs && echo OK`
#   The plan dir plan/002_bb250179a055/ also lives ONLY in the crate repo.

# CRITICAL — direction of THIS edit (do not invert):
#   HERE: qmk_notifier  ->  qmk-notifier   (underscore -> hyphen)
#   This is because the reference names the BINARY (the crate package name).
#   The OPPOSITE direction (hyphen -> underscore) is sibling subtask S1 (core.rs),
#   which names the FIRMWARE repo. If you find yourself changing hyphen to
#   underscore in main.rs, STOP — wrong task/direction.

# CRITICAL — line 1 `use qmk_notifier::` is INVARIANT (do NOT touch):
#   main.rs:1  `use qmk_notifier::{parse_cli_args, run, CommandResponse, ...};`
#   This is the Cargo-derived LIBRARY identifier (PRD §1.2: Cargo auto-derives _
#   from -). naming_audit row 46 = LIBRARY_ID, action NO. Only line ~88 changes.

# NOTE — locate by content, not line number:
#   `grep -n 'qmk_notifier --list' src/main.rs` is the source of truth. The audit
#   cites line 88; minor drift is possible. The unique edit anchor is the
#   substring `qmk_notifier --list | head -1` (appears exactly once in main.rs).

# NOTE — preserve exact formatting:
#   The line is a `/// ` doc comment. Keep the leading `/// `, the backticks
#   around the shell command, the `(e.g. …)` wrapping, the arguments
#   `--list | head -1`, and the trailing `. This restores that behavior.`.
#   Only the single token `qmk_notifier` -> `qmk-notifier` changes.

# NOTE — exhaustive occurrence map (verified):
#   Pre-edit:  grep -c "qmk_notifier"  src/main.rs == 2   (line 1 import + line ~88 comment)
#             grep -c "qmk-notifier" src/main.rs == 0   (none yet)
#   Post-edit: grep -c "qmk_notifier"  src/main.rs == 1   (line 1 import only)
#             grep -c "qmk-notifier" src/main.rs == 1   (the fixed line ~88)
#   There are exactly these occurrences — do not hunt for more.
```

## Implementation Blueprint

### Data models and structure

None. This is a pure comment-token edit — no types, no logic, no signatures, no tests.

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: LOCATE & VERIFY (in the CRATE repo /home/dustin/projects/qmk-notifier)
  - RUN: `cd /home/dustin/projects/qmk-notifier && test -f src/main.rs && echo OK`
    (confirm you are in the crate repo, NOT the firmware cwd).
  - RUN: `grep -n 'qmk_notifier --list' src/main.rs`
    EXPECT: exactly one hit (the SIGPIPE doc-comment line ~88). If zero hits,
    STOP: the file has drifted since this PRP; re-derive the line.
  - RUN: `grep -n 'qmk_notifier' src/main.rs`
    EXPECT: exactly two hits — line 1 (the `use` import) and the SIGPIPE comment.
    Only the SECOND (the `--list` one) is the edit target.
  - RUN: `grep -c 'qmk-notifier' src/main.rs`
    EXPECT: 0 (pre-edit baseline).

Task 2: EDIT the SIGPIPE doc-comment line
  - OLD (unique anchor): `(e.g. \`qmk_notifier --list | head -1\`). This restores that behavior.`
  - NEW:                  `(e.g. \`qmk-notifier --list | head -1\`). This restores that behavior.`
  - PRESERVE: leading `/// `, backticks, `(e.g. …)`, `--list | head -1`, trailing prose.
  - CHANGE: only the token `qmk_notifier` -> `qmk-notifier` inside the backticks.

Task 3: VALIDATE (the grep gates are the real signal; build/test prove no collateral)
  - RUN the Validation Loop below.
```

### Implementation Patterns & Key Details

```text
# The edit is a mechanical single-token swap. The only "pattern" is precision
# and respecting the two hard guardrails (right repo; line-1 import untouched).

# Anchor uniqueness (verified): the substring
#   `(e.g. `qmk_notifier --list | head -1`)`
# appears exactly once in main.rs. Use it (or the shorter unique fragment
# `qmk_notifier --list | head -1`) as the search target so the edit cannot
# accidentally hit line 1's `use qmk_notifier::` import.
```

### Integration Points

```yaml
SOURCE FILES:
  - modify: "src/main.rs ONLY (crate repo /home/dustin/projects/qmk-notifier)"
  - change: "1 comment-token edit on the SIGPIPE doc-comment line (~88)"

BUILD / DEPS / PUBLIC API:
  - none affected. "Comment-only; Cargo.toml, signatures, re-exports unchanged."

DOCUMENTATION:
  - none external. "No README/PRD edits here (those are P1.M1.T3)."
```

## Validation Loop

### Level 1: Syntax & Style (Immediate Feedback)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE repo (hyphen), NOT the firmware cwd

cargo fmt            # rustfmt default style; comment-content edits are format-neutral
cargo fmt --check    # EXPECT: exit 0 (a comment-word swap changes no formatting)
cargo build          # EXPECT: clean, 0 warnings (identical to baseline)
cargo clippy         # EXPECT: no new lint (comments are not linted)
```

### Level 2: Unit Tests (Component Validation)

```bash
cd /home/dustin/projects/qmk-notifier
cargo test
# EXPECT: "test result: ok. 72 passed; 0 failed; 0 ignored; ..." (unchanged baseline).
# Comment-only edits cannot change test outcomes; this gate guards against an
# accidental non-comment edit (e.g. a typo'd code token on line 1 or the fn).
```

### Level 3: Integration Testing (System Validation)

```text
NOT APPLICABLE. This is a comment-only change to a transport crate's binary entry
point; no HID device and no running service are required. The grep gates (Level 4)
and the build/test gates (Levels 1-2) fully cover correctness. (Optionally, after
the edit, `cargo run -- --list | head -1` would demonstrate the SIGPIPE path — but
that exercises pre-existing behavior, not this comment, and needs hardware-free
device enumeration which still succeeds without a keyboard.)
```

### Level 4: Naming-Specific Verification (the real acceptance gate)

```bash
cd /home/dustin/projects/qmk-notifier

# Definitive before/after counts:
echo "underscore (must be 1 — only the line-1 import): $(grep -c 'qmk_notifier' src/main.rs)"
echo "hyphen     (must be 1 — the fixed line 88):     $(grep -c 'qmk-notifier' src/main.rs)"

# Show the fixed line to confirm it now names the binary with the hyphen:
grep -n 'qmk-notifier --list | head -1' src/main.rs
# EXPECT: exactly one hit (~line 88).

# Confirm the line-1 library import is UNCHANGED (still underscore):
grep -n 'use qmk_notifier::' src/main.rs
# EXPECT: exactly one hit, line 1, still `qmk_notifier` (underscore).

# Negative gate: no surviving underscored binary-invocation example:
grep -n '`qmk_notifier --list' src/main.rs || echo "OK: no stale underscored binary example"
# EXPECT: the "OK: ..." branch (grep exits 1 ⇒ "|| echo" fires).

# Confirm scope: only main.rs changed (no core.rs/lib.rs drift):
git -C /home/dustin/projects/qmk-notifier diff --name-only
# EXPECT: exactly `src/main.rs` (and nothing else).
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1 passed: `cargo build` → 0 warnings; `cargo fmt --check` → exit 0.
- [ ] Level 2 passed: `cargo test` → 72 passed, 0 failed.
- [ ] Level 4 passed: `grep -c "qmk_notifier" src/main.rs` == 1 (line-1 import only).
- [ ] Level 4 passed: `grep -c "qmk-notifier" src/main.rs` == 1 (the fixed line).
- [ ] `git diff --name-only` == `src/main.rs` only.

### Feature Validation

- [ ] Line ~88 names the binary `qmk-notifier` (hyphen) in the shell example.
- [ ] Line 1 `use qmk_notifier::{…};` is byte-for-byte unchanged (library id, invariant).
- [ ] No other `qmk_notifier` comment reference to the binary survives in main.rs.
- [ ] Backticks, `(e.g. …)` wrapping, `--list | head -1` args, and trailing prose preserved.

### Code Quality Validation

- [ ] Comment style preserved (`/// ` doc comment).
- [ ] No Rust identifier (`use qmk_notifier::`) touched.
- [ ] No files outside `src/main.rs` modified.

### Documentation & Deployment

- [ ] No README/PRD changes (out of scope — P1.M1.T3).
- [ ] No env vars, config, deps, or public API surface affected (comment-only).

---

## Anti-Patterns to Avoid

- ❌ Don't edit in the firmware cwd (`/home/dustin/projects/qmk_notifier`, underscore).
      The target is the **crate** repo `/home/dustin/projects/qmk-notifier` (hyphen).
      `cd` there and `test -f src/main.rs` before editing.
- ❌ Don't invert the direction. HERE it is underscore→hyphen (binary reference).
      The hyphen→underscore direction is a DIFFERENT subtask (S1, core.rs, which
      names the firmware repo). If you are changing hyphen to underscore in
      main.rs, you are doing the wrong task.
- ❌ Don't touch line 1's `use qmk_notifier::{…}` import — that is the
      Cargo-derived library identifier and is invariant. naming_audit row 46 = NO.
- ❌ Don't "fix all" `qmk_notifier` across main.rs — only the ONE `--list`
      example line. The line-1 import must stay underscore.
- ❌ Don't trust a hardcoded line number — locate via
      `grep -n 'qmk_notifier --list' src/main.rs`. Minor drift is possible.
- ❌ Don't reformat the surrounding comment block — preserve leading `/// `,
      backticks, the `(e.g. …)` wrapping, and the trailing prose. Only the single
      token changes.
- ❌ Don't add/alter tests or code — this is comment-only; `cargo test` must stay
      at exactly 72 passed with no source-token change.
- ❌ Don't conflate the binary artifact name with the internal hashed test/example
      artifact names under `target/debug/deps/qmk_notifier-*` (those use the
      library identifier). The public binary IS `qmk-notifier` (Cargo.toml name).

---

**Confidence Score: 10/10** for one-pass implementation success. It is a
deterministic, single-token comment edit with a verified unique edit anchor, an
exhaustive occurrence map (qmk_notifier on lines 1 and ~88; zero qmk-notifier
pre-edit), machine-checkable grep gates (2→1 underscore, 0→1 hyphen), a hard
guardrail on the invariant line-1 import, and the cross-repo cwd hazard called
out explicitly with a `test -f` guard and absolute paths. The two residual risks
— editing in the wrong repo and inverting the direction — are both flagged with
explicit STOP conditions.