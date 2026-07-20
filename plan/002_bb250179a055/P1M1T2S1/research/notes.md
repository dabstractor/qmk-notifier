# Research Notes — P1.M1.T2.S1: lib.rs argv[0] + doc-comment naming alignment

## Task recap
Change 10 clap argv[0] string literals in `src/lib.rs` test code from
`"qmk_notifier"` (firmware/library-identifier form) to `"qmk-notifier"` (the real
binary, per `Cargo.toml name = "qmk-notifier"`), and align one ambiguous doc
comment. Comment/cosmetic only — no parsing change, no API change.

## REPO-LOCATION GOTCHA (verified, important)
The plan/task paths are relative to the **Rust crate** root
`/home/dustin/projects/qmk-notifier` (HYPHEN). The harness session cwd is
`/home/dustin/projects/qmk_notifier` (UNDERSCORE) — that is a DIFFERENT repo: the
qmk **firmware** C project (notifier.c, pattern_match.c, rules.mk). All work for
this task happens in the crate:
`cd /home/dustin/projects/qmk-notifier`. The firmware repo is irrelevant here.

## Authority: naming_audit.md (architecture/)
Source of truth for this whole milestone. Key classes:
- **CRATE**  `qmk-notifier` (hyphen) = package / repo / binary. Target = hyphen.
- **FIRMWARE** `qmk_notifier` (underscore) = C module / repo / commit `01a51935`. Target = underscore.
- **LIBRARY_ID** `qmk_notifier` (underscore) = Rust `use`/`mod`/dep key — Cargo auto-derives
  the underscore library id from the hyphen package name. **INVARIANT (underscore).**
- **ARGV0** `qmk-notifier` (hyphen) = clap argv[0] / program name. Cosmetic.

My task = the ARGV0 (10 sites) + 1 ambiguous doc comment. None of the LIBRARY_ID
or FIRMWARE sites in lib.rs (there are none of either in lib.rs — verified).

## Verified current state of src/lib.rs (line numbers EXACT, no drift)
`grep -nE '"qmk_notifier"' src/lib.rs` → exactly 10 hits, matching the audit:
```
876:   let params = cli_for(&["qmk_notifier", "--query-info"]);
887:   let params = cli_for(&["qmk_notifier", "--list-callbacks"]);
899:           "qmk_notifier",                      (multi-line array element)
912:   let params = cli_for(&["qmk_notifier", "hello"]);
915:   let params = cli_for(&["qmk_notifier", "--list"]);
924:           &["qmk_notifier", "--query-info", "msg"],
925:           &["qmk_notifier", "--query-info", "--list-callbacks"],
926:           &["qmk_notifier", "--list-callbacks", "msg"],
927:           &["qmk_notifier", "--list", "--query-info"],
944:           .try_get_matches_from(["qmk_notifier", "--verbose"])
```
Plus the doc comment:
```
154: /// Build the clap `Command` for `qmk_notifier` (PRD §11 *CLI*). Pure: it only
```
There are NO `use qmk_notifier::` / `mod qmk_notifier` tokens in lib.rs (it IS the
crate root; main.rs line 1 has the `use qmk_notifier::` import, but main.rs is NOT
this task's file — sibling P1.M1.T1.S2 owns main.rs).

## WHY the change is safe (cosmetic, zero parsing impact)
- `cli_for(args)` (lib.rs:867) = `build_cli_command().try_get_matches_from(args)`.
- `build_cli_command()` (lib.rs:157) = `Command::new("QMK Keyboard Communication Tool")…`
  — note it does NOT call `.bin_name(...)`, so clap derives the program/bin name
  from argv[0] (the first element) for help/usage/error DISPLAY ONLY.
- clap's `try_get_matches_from` parses only `args[1..]` as arguments; `args[0]` is
  the program name and never affects which flag maps to which Arg. So swapping
  `"qmk_notifier"` → `"qmk-notifier"` changes only the name shown in clap-generated
  text during tests, never the parse result.
- Test bodies assert on parsed structure — `matches!(params.command,
  RunCommand::QueryInfo)`, `params.vendor_id`, `params.verbose`, `.is_err()` for
  conflicts — NONE reference argv[0] or the program name. ⇒ all 72 tests stay green.

## WHY library identifiers stay underscore (the hard invariant)
`name = "qmk-notifier"` in Cargo.toml → Cargo derives the library crate identifier
`qmk_notifier` (hyphens are illegal in Rust identifiers; Cargo maps `-` → `_`).
`use qmk_notifier::…` / `mod qmk_notifier` are Rust identifier paths and MUST use
the underscore form — a hyphen there is a syntax error. So a blanket
`qmk_notifier → qmk-notifier` sed would be WRONG. The surgical replacement targets
ONLY the double-quoted string literal `"qmk_notifier"` (which, in lib.rs, is
exclusively argv[0]) plus the backtick code span in the one doc comment.

## Replacement strategy (safe + surgical)
1. The 10 argv[0] literals: they all match the exact quoted token `"qmk_notifier"`.
   A scoped replacement of that quoted token is safe — it does NOT match the
   backtick form `` `qmk_notifier` `` (different delimiters) and does NOT match any
   `use qmk_notifier::` (none in lib.rs anyway).
   Recommended: `sed -i 's/"qmk_notifier"/"qmk-notifier"/g' src/lib.rs` then verify
   `grep -cE '"qmk_notifier"' src/lib.rs` == 0 and
   `grep -cE '"qmk-notifier"' src/lib.rs` == 10.
   (Equivalent: 10 individual `edit` operations — same result.)
2. The doc comment (line 154): change the backtick code span `` `qmk_notifier` ``
   → `` `qmk-notifier` ``. Use a targeted edit with enough surrounding text to be
   unique, e.g. `` for `qmk_notifier` (PRD §11 `` → `` for `qmk-notifier` (PRD §11 ``.

## MUST NOT change
- `Command::new("QMK Keyboard Communication Tool")` (lib.rs:158) — clap display
  NAME (human-readable), not a binary identifier; the audit does not flag it.
- Any `use qmk_notifier::` / `mod qmk_notifier` (invariant; none in lib.rs anyway).
- `core::` / `error::` module references.
- Cargo.toml (`name = "qmk-notifier"` already correct).
- main.rs / core.rs / error.rs (sibling tasks own those; this task = lib.rs only).

## Parallel-sibling contract (P1.M1.T1.S2 — main.rs comment fix)
P1.M1.T1.S2 edits `src/main.rs:88` ONLY (SIGPIPE comment: `qmk_notifier --list` →
`qmk-notifier --list`) and explicitly states "src/lib.rs (sibling T2.S1)
untouched". ⇒ zero collision with this task (different files). P1.M1.T1.S1 (core.rs
commit-comment fix) is already Complete. Both siblings' work composes cleanly.

## Validation (verified baseline)
- `cargo test --lib` (cwd = crate root) → **72 passed, 0 failed** (current).
- After change → still 72 passed (cosmetic).
- `cargo build` → 0 warnings (current); stays 0 (comment/string-literal only).
- `cargo fmt --check` → exit 0 (string-literal swap doesn't alter formatting; the
  doc-comment edit is within a line).
- No rustfmt.toml / clippy.toml (default configs).

## Expected post-change grep gates (cwd = /home/dustin/projects/qmk-notifier)
- `grep -cE '"qmk_notifier"' src/lib.rs`  → **0**
- `grep -cE '"qmk-notifier"' src/lib.rs`  → **10**
- `grep -cE 'qmk_notifier' src/lib.rs`    → **0** (the doc comment also flipped)
- `grep -nE 'qmk-notifier' src/lib.rs`    → **11** hits (10 argv[0] + 1 doc comment)
- `grep -n 'Command::new' src/lib.rs`     → unchanged (display name untouched)

## Scope boundary / anti-collision
- ONLY `src/lib.rs` modified. NOT main.rs (sibling S2), core.rs (sibling S1 done),
  error.rs, Cargo.toml, README, PRD.
- DOCS: none — test-internal + one comment edit; no user-facing/config/API surface.