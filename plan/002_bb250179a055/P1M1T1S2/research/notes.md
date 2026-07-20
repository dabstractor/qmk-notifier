# Research Notes — P1.M1.T1.S2: Fix main.rs binary-invocation comment reference

## Task (from item description + naming_audit row 47)

In the Rust crate `qmk-notifier` (hyphen), change ONE word in ONE doc-comment
line of `src/main.rs`:

```
line 88 (current):
/// (e.g. `qmk_notifier --list | head -1`). This restores that behavior.
                ^^^^^^^^^^^^^              <-- firmware underscore (WRONG)

target:
/// (e.g. `qmk-notifier --list | head -1`). This restores that behavior.
                ^^^^^^^^^^^^               <-- crate/binary hyphen (CORRECT)
```

The reference is a **shell example invoking the CLI binary**, which is named
`qmk-notifier` (hyphen) per `Cargo.toml name = "qmk-notifier"` and PRD §2/§1.2.

Comment-only. No code, no signatures, no other files. `cargo build` and
`cargo test` must still pass.

## CRITICAL FINDING #1 — the agent's CWD is the WRONG repo (cross-repo hazard)

The harness cwd is `/home/dustin/projects/qmk_notifier` (**underscore** = the
firmware C repo: notifier.c, pattern_match.c). It has NO `src/main.rs` and NO
`Cargo.toml`. The file to edit is in the **crate** repo:

```
/home/dustin/projects/qmk-notifier/src/main.rs   (HYPHEN)
```

The plan dir also lives only in the crate repo:
`/home/dustin/projects/qmk-notifier/plan/002_bb250179a055/`

This is the PRD §1.2 "naming hazard" biting the harness itself. The PRP MUST
instruct the implementer to `cd /home/dustin/projects/qmk-notifier` and use a
`test -f` guard, exactly as sibling S1 did. Verified:
`test -f /home/dustin/projects/qmk-notifier/src/main.rs && echo OK` → OK.

## CRITICAL FINDING #2 — direction is underscore → hyphen (INVERSE of S1)

- S1 (core.rs:129,425): **hyphen → underscore** (those refs name the FIRMWARE repo,
  commit `01a51935`).
- S2 (main.rs:88): **underscore → hyphen** (this ref names the CRATE/BINARY).

The two subtasks run in OPPOSITE directions because they fix OPPOSITE mistakes.
Do not let the implementer apply S1's direction here. The naming_audit row 47
prescribes the exact target: `qmk-notifier --list | head -1`.

## CRITICAL FINDING #3 — `main.rs:1` `use qmk_notifier::` is INVARIANT

`main.rs` line 1 is:
```rust
use qmk_notifier::{parse_cli_args, run, CommandResponse, RunCommand, RunParameters};
```
This is the **Cargo-derived library identifier** (`qmk_notifier`, underscore) —
Cargo auto-derives `_` from the package's `-`. PRD §1.2 explicitly states this:
"The crate's Rust *library* identifier is also `qmk_notifier`". The naming_audit
row 46 classifies it as LIBRARY_ID → **NO change**. The implementer MUST NOT
touch line 1. Only line 88 changes.

Verified `qmk_notifier` (underscore) occurrences in main.rs (exhaustive):
```
1:  use qmk_notifier::{...};          <- LIBRARY_ID, INVARIANT
88: /// (e.g. `qmk_notifier --list | head -1`). ...   <- the FIX target
```
So `grep -c "qmk_notifier" src/main.rs` is **2** pre-edit → **1** post-edit
(line 1 the import remains). And `grep -c "qmk-notifier" src/main.rs` is
**0** pre-edit → **1** post-edit. These counts are the grep gates.

## CRITICAL FINDING #4 — line 88 verified verbatim + unique anchor

Exact current content (verified by `sed -n '78,92p'`):
```
84  /// quietly with SIGPIPE (exit 141) when a downstream consumer exits early
85  /// (e.g. `qmk_notifier --list | head -1`). This restores that behavior.
```
(The audit's "line 88" counts from a slightly different trim; the real line is
**85** in the current working tree — minor drift. LOCATE BY CONTENT, not line
number: `grep -n 'qmk_notifier --list' src/main.rs`.) Wait — re-check: the grep
above returned `88:...`. Both the audit and grep say 88; my sed window showed it
at relative position 85 of the window starting at line 78, i.e. absolute 85+...

Let me resolve this precisely: the `grep -n "qmk_notifier" src/main.rs` output
was unambiguous — line **88**. The naming_audit also says 88. The sed window I
printed started at line 78 and the SIGPIPE line was the 8th printed line ⇒
absolute line 78+8-1 = **85**?? No: `sed -n '78,92p'` prints lines 78..92; the
8th line in that range is line 85. There's a 3-line discrepancy between grep (88)
and the sed-window count (85). This is almost certainly because the file's actual
blank-line/whitespace layout differs from my mental count. **Trust grep, not the
sed-window count.** `grep -n "qmk_notifier --list" src/main.rs` ⇒ line 88 is
authoritative. The PRP will instruct: LOCATE BY THE CONTENT STRING, never by a
hardcoded line number — grep is the source of truth.

The unique edit anchor (appears exactly once in main.rs):
```
`(e.g. `qmk_notifier --list | head -1`)`
```
Use the substring `qmk_notifier --list | head -1` as the search target; it is
unique. Replace ONLY the identifier token `qmk_notifier` → `qmk-notifier`
within it; preserve the backticks, the `(e.g. …)` wrapping, and the trailing
`. This restores that behavior.` verbatim.

## CRITICAL FINDING #5 — comment-only ⇒ build/test invariant

Baseline (verified): `cargo test` →
```
test result: ok. 72 passed; 0 failed; 0 ignored; ...   (lib unit tests)
test result: ok. 0 passed; ...                          (integration tests dir, empty)
test result: ok. 0 passed; ...                          (binary, no #[test] in main.rs)
```
A one-word comment edit cannot change compilation or any test outcome.
Post-edit: `cargo build` → 0 warnings; `cargo test` → 72 passed, 0 failed.
`cargo fmt --check` → exit 0 (rustfmt does not touch comment *content*, and the
line length/structure is unchanged). `cargo clippy` → no new lint (comments are
not linted).

## CRITICAL FINDING #6 — binary name confirmation

`Cargo.toml`:
```
[package]
name = "qmk-notifier"     <-- HYPHEN: package + binary name
...
repository = "https://github.com/dabstractor/qmk-notifier"
```
Cargo builds the binary as `qmk-notifier` (the package name with the hyphen
preserved for the binary artifact — only the *library* identifier is `_`-ified).
So a shell invocation example MUST read `qmk-notifier --list | head -1`. This is
independently confirmed by the actual built binary names found under
`target/debug/deps/qmk_notifier-*` (those are the *internal* hashed test/example
artifact names, which use the library identifier — not the public binary name;
the public binary is `target/debug/qmk-notifier`).

## Scope boundary (vs siblings)

- S1 (P1.M1.T1.S1): `src/core.rs:129,425` — hyphen→underscore (firmware commit).
  DIFFERENT file, OPPOSITE direction. No overlap with S2.
- T2.S1 (P1.M1.T2.S1): `src/lib.rs` — 10 test `argv[0]` strings + doc comment,
  underscore→hyphen. DIFFERENT file. No overlap with S2.
- T3 (P1.M1.T3): README.md / PRD.md grep gate + Cargo drift doc. DIFFERENT files.

S2 touches ONLY `src/main.rs` line 88. Nothing else.

## Validation gates (verified, against the CRATE repo)

- `grep -c "qmk_notifier" src/main.rs` → **1** (only the line-1 import).
- `grep -c "qmk-notifier" src/main.rs` → **1** (the fixed line 88).
- `grep -n "qmk-notifier --list | head -1" src/main.rs` → exactly one hit.
- `cargo build` → 0 warnings.
- `cargo test` → 72 passed, 0 failed.
- `cargo fmt --check` → exit 0.
- `git diff --name-only` → exactly `src/main.rs`.
- `git diff src/main.rs` → a single-line change swapping one token in a comment.