# Research Notes — P1.M1.T1.S1 (Fix two core.rs firmware-commit comment refs)

## CRITICAL: cross-repo naming hazard (path resolution)
- There are TWO sibling repos that look almost identical by name:
  - `/home/dustin/projects/qmk_notifier` (UNDERSCORE) = the **firmware C** repo
    (notifier.c, pattern_match.c). This is the agent's CURRENT WORKING DIRECTORY.
  - `/home/dustin/projects/qmk-notifier` (HYPHEN) = the **Rust transport crate**
    repo. THIS is the repo this task edits (`src/core.rs` is Rust).
- The plan `002_bb250179a055` and `architecture/naming_audit.md` live ONLY in the
  CRATE repo: `/home/dustin/projects/qmk-notifier/plan/002_bb250179a055/`.
- The firmware cwd's `plan/` contains DIFFERENT plans
  (`001_e329fbe4ae4d`, `002_c243e735980a`, `003_…`, `004_…`) — NOT this one.
- => The PRP and research files MUST be written with ABSOLUTE paths under
  `/home/dustin/projects/qmk-notifier/plan/002_bb250179a055/...`. A relative
  `plan/002_bb250179a055/...` would land in the wrong (firmware) repo.

## Naming rule (the whole point of this task)
- `qmk-notifier` (HYPHEN) = the Rust crate (package + repo).
- `qmk_notifier` (UNDERSCORE) = the firmware C module.
- Commit `01a51935` is a **firmware** fix (RAW reply size 30→32 bytes) that lives
  in `dabstractor/qmk_notifier` (firmware). So prose attributing that commit must
  use the UNDERSCORE form.
- The crate's Rust *library* identifier is also `qmk_notifier` (Cargo auto-derives
  `_` from `-`), so `use qmk_notifier::` / `mod qmk_notifier` are INVARIANT and
  must NEVER be edited for naming reasons. (None exist in core.rs anyway — see below.)

## Verified target lines (exact, current core.rs @ crate repo, 1689 lines, v0.3.0)
- **Line 129** (doc comment on `const IN_DRAIN_MAX`):
  `/// sends a 32-byte reply per report (fixed in qmk-notifier commit \`01a51935\`,`
  Unique edit anchor: `fixed in qmk-notifier commit \`01a51935\``
  → `fixed in qmk_notifier commit \`01a51935\``

- **Line 425** (inline comment inside the drain loop of `burst_to_one`, indented 4 spaces):
  `    // valid 32-byte reply per report (qmk-notifier commit \`01a51935\`, which`
  Unique edit anchor: `valid 32-byte reply per report (qmk-notifier commit \`01a51935\`, which`
  → `valid 32-byte reply per report (qmk_notifier commit \`01a51935\``, which`

## CRITICAL: these are the ONLY two hyphenated refs in core.rs
- `grep -c "qmk-notifier" src/core.rs` == **2** (lines 129 and 425 — both firmware commits).
- `grep -c "qmk_notifier" src/core.rs` == **0** (currently zero underscored refs in core.rs).
- There are NO crate-referential `qmk-notifier` comments in core.rs, and NO
  `use qmk_notifier::` / `mod qmk_notifier` lines in core.rs (it refers to itself
  via `crate::`).
- => Both occurrences are legitimately firmware-commit refs and BOTH must be
  underscored. The contract's "do not change other qmk-notifier refs" warning is
  vacuously satisfied (there are none). The edit is unambiguous.
- Post-edit invariants: `grep -c "qmk-notifier" src/core.rs` == **0**;
  `grep -c "qmk_notifier" src/core.rs` == **2**.

## naming_audit.md confirmation
- Row 52: `src/core.rs:129` classified FIRMWARE, current `qmk-notifier`, target
  `qmk_notifier`, "Fix → `qmk_notifier commit \`01a51935\``".
- Row 53: `src/core.rs:425` identical.
- Audit "Action items" lines 93–94 list exactly these two fixes.

## Scope boundary (do NOT touch)
- Sibling P1.M1.T1.S2 edits `src/main.rs` (binary-invocation ref, OPPOSITE
  direction: `qmk_notifier → qmk-notifier`). Different file, different direction.
- Sibling P1.M1.T2.S1 edits 10 `src/lib.rs` test `argv[0]` strings + a doc
  comment. Different file.
- This PRP touches ONLY `src/core.rs` lines 129 and 425.
- README.md / PRD.md naming references are handled by P1.M1.T3 (out of scope here).

## Baseline (verified in the crate repo)
- `grep '^version' Cargo.toml` → `version = "0.3.0"`.
- `cargo test` → **72 passed, 0 failed** (matches contract).
- `cargo build` → clean (0 warnings, baseline).
- This subtask is COMMENT-ONLY → `cargo build`/`cargo test` results are unchanged
  (72 passed). The grep deltas above are the real acceptance signal.