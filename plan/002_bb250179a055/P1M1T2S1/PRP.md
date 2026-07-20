# PRP — P1.M1.T2.S1: Update 10 lib.rs test argv[0] strings + align doc comment

---

## Goal

**Feature Goal**: Change the 10 clap argv[0] string literals in `src/lib.rs`
test code from the firmware/library-identifier form `"qmk_notifier"` (underscore)
to the real **binary** form `"qmk-notifier"` (hyphen), and align one ambiguous
doc comment that names the binary's CLI. `qmk-notifier` is the actual binary
(`Cargo.toml name = "qmk-notifier"`); clap uses argv[0] only as the program
name for **display** in help/usage/error text — it never affects argument
parsing — so today's tests render the wrong program name (`qmk_notifier`) in any
clap-generated diagnostic. This is a **cosmetic, comment/string-literal-only**
edit: no parsing logic, no signatures, no public API, no behavior change.

**Deliverable**: 11 one-token edits in **`/home/dustin/projects/qmk-notifier/src/lib.rs`**:
10 occurrences of the quoted literal `"qmk_notifier"` → `"qmk-notifier"` (the
argv[0] first element in `cli_for(&[…])` / `try_get_matches_from([…])` test
inputs, at lines 876, 887, 899, 912, 915, 924, 925, 926, 927, 944) plus the
backtick code span `` `qmk_notifier` `` → `` `qmk-notifier` `` in the doc
comment at line 154 (`/// Build the clap \`Command\` for … (PRD §11 *CLI*).`).
**`src/lib.rs` is the only file modified.** The library-identifier usages
(`use qmk_notifier::` / `mod qmk_notifier`, where they exist in other files) are
**invariant** and must NOT be touched.

**Success Definition**:
- `grep -cE '"qmk_notifier"' src/lib.rs` → **0** (was 10).
- `grep -cE '"qmk-notifier"' src/lib.rs` → **10**.
- `grep -cE 'qmk_notifier' src/lib.rs` → **0** (the doc comment is also flipped;
  no underscore form remains anywhere in lib.rs).
- `cargo build` → **0 warnings** (identical to baseline; string-literal/comment only).
- `cargo test --lib` → **72 passed, 0 failed** (unchanged; cosmetic argv[0]).
- `cargo fmt --check` → exit 0.
- `git diff --name-only` → exactly `src/lib.rs`.
- `src/main.rs` (sibling S2), `src/core.rs` (sibling S1, done), `src/error.rs`,
  and `Cargo.toml` are untouched.

## User Persona (if applicable)

**Target User**: A developer (or downstream `qmkonnect` integrator) reading a
failing lib.rs test's clap error/usage output, or skimming the `build_cli_command`
doc comment.

**Use Case**: When a CLI test fails (e.g. a conflict-case returns an unexpected
`Ok`), clap's error text prints the program name derived from argv[0]; the reader
should see the real binary `qmk-notifier`, not the firmware module name
`qmk_notifier`.

**User Journey**: test fails → clap prints `error: …\nUsage: qmk-notifier [OPTIONS]…`
(after) instead of `Usage: qmk_notifier [OPTIONS]…` (before) → the developer
copy-pastes the correct binary name into a shell.

**Pain Points Addressed**: Removes a cross-repo naming hazard — `qmk_notifier`
(underscore) is the **firmware** C module, `qmk-notifier` (hyphen) is **this**
crate/binary. Surfacing the firmware name in this crate's test diagnostics is
exactly the confusion PRD §1.2 warns about.

## Why

- This crate's binary IS `qmk-notifier` (`Cargo.toml name = "qmk-notifier"`,
  PRD §2). The argv[0] passed to clap in tests should be that binary so generated
  diagnostics are faithful. Today they carry the firmware module name — a
  mislabel, albeit cosmetic (the audit classifies these 11 lib.rs sites as
  OPTIONAL; see `architecture/naming_audit.md`).
- It closes out the **only remaining `qmk_notifier`-as-binary references in
  lib.rs** after the must-fix `core.rs`/`main.rs` comment edits (siblings S1
  complete, S2 in progress). After this task, `src/lib.rs` contains zero
  underscore-form `qmk_notifier` tokens, making the file internally consistent
  with the binary identity.
- It is **risk-free and dependency-free**: a quoted-string-literal swap in test
  inputs + one doc-comment code span. No parsing path reads argv[0] as data, so
  no test can change behavior — 72 tests stay green.

## What

### The 10 argv[0] string-literal edits (current → target)

All 10 are the **first element** of arrays passed to `cli_for(&[…])` or
`.try_get_matches_from([…])`. Each is the exact quoted token `"qmk_notifier"` →
`"qmk-notifier"`:

| Line | Current (excerpt) | Target (excerpt) |
|-----:|---|---|
| 876 | `cli_for(&["qmk_notifier", "--query-info"])` | `cli_for(&["qmk-notifier", "--query-info"])` |
| 887 | `cli_for(&["qmk_notifier", "--list-callbacks"])` | `cli_for(&["qmk-notifier", "--list-callbacks"])` |
| 899 | `        "qmk_notifier",` (multi-line array element) | `        "qmk-notifier",` |
| 912 | `cli_for(&["qmk_notifier", "hello"])` | `cli_for(&["qmk-notifier", "hello"])` |
| 915 | `cli_for(&["qmk_notifier", "--list"])` | `cli_for(&["qmk-notifier", "--list"])` |
| 924 | `&["qmk_notifier", "--query-info", "msg"],` | `&["qmk-notifier", "--query-info", "msg"],` |
| 925 | `&["qmk_notifier", "--query-info", "--list-callbacks"],` | `&["qmk-notifier", "--query-info", "--list-callbacks"],` |
| 926 | `&["qmk_notifier", "--list-callbacks", "msg"],` | `&["qmk-notifier", "--list-callbacks", "msg"],` |
| 927 | `&["qmk_notifier", "--list", "--query-info"],` | `&["qmk-notifier", "--list", "--query-info"],` |
| 944 | `.try_get_matches_from(["qmk_notifier", "--verbose"])` | `.try_get_matches_from(["qmk-notifier", "--verbose"])` |

> These line numbers are verified against the CURRENT working tree (no drift from
> the audit). If they have shifted by the time you edit, **locate by content**:
> `grep -nE '"qmk_notifier"' src/lib.rs` must return exactly 10 hits, all in the
> `#[cfg(test)] mod tests` block's `cli_for`/`try_get_matches_from` call sites.

### Recommended method for the 10 literals (safe + surgical)

The quoted token `"qmk_notifier"` (with the double quotes) appears **only** at
these 10 argv[0] sites in lib.rs — it does NOT match the backtick form
`` `qmk_notifier` `` in the doc comment (different delimiters) and does NOT match
any `use qmk_notifier::` (there are none in lib.rs). So a scoped quoted-token
replacement is exact:

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (hyphen), see Known Gotchas
# Before: confirm exactly 10 quoted occurrences
test "$(grep -cE '"qmk_notifier"' src/lib.rs)" -eq 10 || { echo "UNEXPECTED COUNT — ABORT"; exit 1; }
# Replace the 10 argv[0] quoted literals
sed -i 's/"qmk_notifier"/"qmk-notifier"/g' src/lib.rs
# After: 0 quoted underscore, 10 quoted hyphen
test "$(grep -cE '"qmk_notifier"' src/lib.rs)" -eq 0
test "$(grep -cE '"qmk-notifier"' src/lib.rs)" -eq 10
```

(Equivalent: 10 individual `edit` operations with each full `cli_for(&[…])` line
as `oldText`/`newText` — same result; use whichever your tooling prefers.)

### The doc-comment edit (line 154)

Change the backtick code span `` `qmk_notifier` `` → `` `qmk-notifier` ``. The
full line is:

```rust
/// Build the clap `Command` for `qmk_notifier` (PRD §11 *CLI*). Pure: it only
```

Target (`oldText` must be unique — include the `for ` prefix and ` (PRD §11`
suffix):

- oldText: `` for `qmk_notifier` (PRD §11 ``
- newText: `` for `qmk-notifier` (PRD §11 ``

Yielding:

```rust
/// Build the clap `Command` for `qmk-notifier` (PRD §11 *CLI*). Pure: it only
```

### Success Criteria

- [ ] All 10 argv[0] quoted literals are `"qmk-notifier"` (none `"qmk_notifier"`).
- [ ] The line-154 doc comment code span is `` `qmk-notifier` ``.
- [ ] `grep -cE 'qmk_notifier' src/lib.rs` → **0** (no underscore form remains).
- [ ] `Command::new("QMK Keyboard Communication Tool")` (line ~158) is UNCHANGED.
- [ ] No `use qmk_notifier::` / `mod qmk_notifier` was touched (invariant; none
      exist in lib.rs, but do not introduce any).
- [ ] `cargo build` → 0 warnings; `cargo test --lib` → 72 passed, 0 failed;
      `cargo fmt --check` → exit 0.
- [ ] Only `src/lib.rs` is modified.

## All Needed Context

### Context Completeness Check

> _"If someone knew nothing about this codebase, would they have everything
> needed to implement this successfully?"_ — **Yes.** The exact 10 argv[0]
> sites (verified current line numbers + before/after excerpts), the exact
> doc-comment edit (unique oldText/newText), the safe sed method with before/after
> count guards, the critical argv[0]-string-vs-library-identifier distinction
> (why a blanket replacement would break compilation), the repo-location gotcha
> (crate is the hyphen dir, not the harness cwd), the verified baseline test
> count (72), and the executable grep/cargo gates are all below.

### Documentation & References

```yaml
# MUST READ — the authoritative naming audit (classifies every qmk[_-]notifier site)
- file: plan/002_bb250179a055/architecture/naming_audit.md
  why: "Defines the naming contract (CRATE/FIRMWARE/LIBRARY_ID/ARGV0 classes) and
        lists exactly these 10 ARGV0 sites + the 1 OPTIONAL doc comment in lib.rs.
        Source of truth for what changes (ARGV0 → hyphen) vs what is invariant
        (LIBRARY_ID → underscore). Also confirms Cargo.toml/README/PRD are all
        already correct (not this task's concern)."
  section: "Authority / naming contract", "Full reference table → src/lib.rs",
           "Optional (cosmetic) — 11 sites in src/lib.rs", "Do NOT change"
  critical: "The quoted literal \"qmk_notifier\" is ARGV0 (change → hyphen). The
             identifier `use qmk_notifier::` / `mod qmk_notifier` is LIBRARY_ID
             (INVARIANT → underscore; a hyphen there is a Rust syntax error).
             A blanket identifier replacement is WRONG — replace ONLY the quoted
             string token + the one backtick doc span."

# MUST READ — the file being edited (confirm current state before editing)
- file: src/lib.rs
  why: "Holds the 10 argv[0] call sites (lines 876/887/899/912/915/924-927/944),
        the doc comment at 154, the cli_for helper (867), and build_cli_command
        (157, which sets the clap display NAME but NOT bin_name — hence argv[0]
        drives the program name shown in diagnostics). Read the #[cfg(test)] mod
        tests block to confirm the test bodies assert on parsed RunParameters,
        NOT on argv[0] or clap text (which is why the swap is cosmetic)."
  pattern: "Tests use cli_for(&[\"…\", \"--flag\"]) wrappers around
            build_cli_command().try_get_matches_from(args). argv[0] is the
            conventional program-name slot."
  gotcha: "Do NOT change Command::new(\"QMK Keyboard Communication Tool\") — it
           is clap's human-readable display NAME, not a binary identifier, and
           the audit does not flag it."

# MUST READ — the parallel sibling PRP (different file ⇒ no collision)
- file: plan/002_bb250179a055/P1M1T1S2/PRP.md
  why: "P1.M1.T1.S2 edits src/main.rs:88 ONLY (SIGPIPE comment: qmk_notifier →
        qmk-notifier) and explicitly states 'src/lib.rs (sibling T2.S1)
        untouched'. Confirms the two parallel edits target different files and
        compose cleanly. P1.M1.T1.S1 (core.rs commit-comment fix) is already
        Complete."
  section: "Goal / Deliverable" and "Success Definition"
  critical: "S2 owns main.rs; this task (T2.S1) owns lib.rs. Do NOT edit main.rs."

# REFERENCE — the CLI section the doc comment cites
- file: PRD.md
  why: "§11 (CLI) documents the binary's argument surface; §3 (Public API) fixes
        the naming authority (crate = qmk-notifier, library id = qmk_notifier).
        Confirms argv[0] is the binary name, not the library id."
  section: "11. CLI", "3. Public API"

# REFERENCE — research notes (repo-location gotcha, sed safety proof, grep gates)
- docfile: plan/002_bb250179a055/P1M1T2S1/research/notes.md
  why: "Documents the harness-cwd-vs-crate-root mismatch, why a quoted-token sed
        is surgical (does not match backtick/use forms), the cli_for/build_cli_command
        mechanics proving argv[0] is display-only, and the exact post-change grep
        gates."
```

### Current Codebase tree (verified; cwd = the hyphen crate)

```bash
/home/dustin/projects/qmk-notifier/        # <-- THIS task's repo (Rust crate, hyphen)
├── Cargo.toml          # name = "qmk-notifier"  (binary/crate = hyphen)
├── Cargo.lock
├── README.md
├── PRD.md
├── .gitignore
└── src
    ├── main.rs         # sibling P1.M1.T1.S2 edits line 88 — DO NOT TOUCH here
    ├── core.rs         # sibling P1.M1.T1.S1 (Complete) — DO NOT TOUCH here
    ├── error.rs        # QmkError — DO NOT TOUCH
    └── lib.rs          # <-- FILE TO EDIT: 10 argv[0] literals (lines 876..944) + doc comment (154)
```

> NOTE: the harness session cwd may be `/home/dustin/projects/qmk_notifier`
> (UNDERSCORE) — that is a DIFFERENT repo, the qmk **firmware** C project
> (notifier.c, pattern_match.c). Ignore it; all work is in the hyphen crate above.

### Desired Codebase tree with files to be added/modified

```bash
src/
└── lib.rs   # MODIFIED ONLY — 10 argv[0] quoted literals ("qmk_notifier"→"qmk-notifier")
             #                   + 1 doc-comment backtick code span (`qmk_notifier`→`qmk-notifier`)
└── (unchanged) main.rs, core.rs, error.rs, Cargo.toml, README.md, PRD.md
```

> No new files. All changes are string-literal/comment edits in `src/lib.rs`.

### Known Gotchas of our codebase & Library Quirks

```text
CRITICAL — REPO LOCATION: this task's repo is /home/dustin/projects/qmk-notifier
  (HYPHEN). The harness session cwd /home/dustin/projects/qmk_notifier (UNDERSCORE)
  is the firmware C repo — a different project. `cd` to the hyphen crate before
  any cargo/grep/edit. Verify with: head -2 Cargo.toml  =>  name = "qmk-notifier".

CRITICAL — argv[0] STRING vs LIBRARY IDENTIFIER (do not confuse them):
  * The QUOTED literal "qmk_notifier" (10 sites, all argv[0]) → change to "qmk-notifier".
  * The Rust IDENTIFIER path `use qmk_notifier::` / `mod qmk_notifier` is
    INVARIANT (underscore). Cargo derives the library id `qmk_notifier` from the
    package name `qmk-notifier` (hyphens are illegal in Rust identifiers). A
    hyphen there is a hard syntax error. There are NO such identifiers in lib.rs
    (it is the crate root), but a blanket `qmk_notifier→qmk-notifier` sed would
    still be the wrong tool — scope the replacement to the quoted token.

CRITICAL — scope the sed to the QUOTED token: replace "qmk_notifier" (with the
  double quotes), NOT bare qmk_notifier. The quoted form matches ONLY the 10
  argv[0] sites; it does not match the backtick doc span or any identifier. The
  pre/post count guards (==10 before, ==0 after) make this self-verifying.

NOTE — argv[0] is DISPLAY-ONLY in clap: build_cli_command() calls
  Command::new("QMK Keyboard Communication Tool") but NOT .bin_name(...), so clap
  derives the program name from args[0] for help/usage/error text only. Parsing
  consumes args[1..]; args[0] never selects an Arg. Hence swapping it cannot
  change any test's parse result — 72 tests stay green by construction.

NOTE — do NOT change Command::new("QMK Keyboard Communication Tool") (line ~158).
  It is clap's human-readable display NAME, not a binary identifier; the audit
  does not flag it and changing it is out of scope.

NOTE — the doc-comment edit changes a backtick CODE SPAN (`qmk_notifier`), not a
  quoted string. The sed above will NOT touch it (different delimiters) — apply
  it as a separate targeted edit (see "The doc-comment edit").

NOTE — fmt is unaffected: swapping a string literal's contents and a backtick
  span's contents does not change line structure, so `cargo fmt --check` stays 0.
  No rustfmt.toml / clippy.toml exist (default configs).

NOTE — these 11 sites are classified OPTIONAL (cosmetic) in the audit, not
  must-fix. They carry zero behavior risk because they are argv[0] display
  strings + one comment. The must-fix sites (core.rs:129/425, main.rs:88) belong
  to siblings S1 (done) and S2 (parallel) — do not touch them here.
```

## Implementation Blueprint

### Data models and structure

No data-model work. This task edits **string-literal contents and one doc-comment
code span** — no types, no structs, no signatures, no logic. The "model" is the
naming distinction (ARGV0 → hyphen; LIBRARY_ID → underscore) documented in
`naming_audit.md`.

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: LOCATE + VERIFY the current state (cwd = /home/dustin/projects/qmk-notifier)
  - VERIFY repo: `head -2 Cargo.toml` prints `name = "qmk-notifier"` (if it shows
          qmk_notifier or a C project, you are in the WRONG repo — cd to the
          hyphen crate).
  - COUNT: `grep -cE '"qmk_notifier"' src/lib.rs` MUST print 10. If not, lines
          shifted — re-derive the site list from `grep -nE '"qmk_notifier"' src/lib.rs`
          (all hits are argv[0] in #[cfg(test)] mod tests).
  - READ the doc comment at line 154 to confirm the `` `qmk_notifier` `` code span.
  - READ build_cli_command() (~157) to confirm Command::new(...) display name is
          NOT bin_name (i.e. argv[0] drives the program name) — context for "why
          cosmetic", not an edit.
  - GOAL: confirm exactly 11 edits (10 literals + 1 doc span) before touching anything.

Task 2: REPLACE the 10 argv[0] quoted literals
  - METHOD (sed, scoped to the quoted token):
        sed -i 's/"qmk_notifier"/"qmk-notifier"/g' src/lib.rs
    GUARD before: grep count == 10. GUARD after: quoted underscore == 0, quoted
    hyphen == 10.
  - OR (edit tool): 10 individual edits, each oldText = the full
        `cli_for(&["qmk_notifier", …])` / array-element / `try_get_matches_from([…])`
        line, newText = same with `"qmk-notifier"`.
  - DO NOT: run a blanket `s/qmk_notifier/qmk-notifier/g` (would, if any
            identifier existed, break it — and it does not match the backtick span).
  - DO NOT: touch Command::new("QMK Keyboard Communication Tool").

Task 3: ALIGN the doc-comment code span (line 154)
  - EDIT: oldText = `` for `qmk_notifier` (PRD §11 ``
          newText = `` for `qmk-notifier` (PRD §11 ``
  - YIELDS: `/// Build the clap `Command` for `qmk-notifier` (PRD §11 *CLI*). Pure: it only`
  - DO NOT: edit any other part of the doc comment.

Task 4: VALIDATE (do not skip — the grep gates ARE the proof)
  - RUN: `cargo fmt`, then `cargo build`, then `cargo fmt --check`, then
          `cargo test --lib`.
  - GREP GATES (cwd = crate root):
        grep -cE '"qmk_notifier"'  src/lib.rs  -> 0
        grep -cE '"qmk-notifier"'  src/lib.rs  -> 10
        grep -cE 'qmk_notifier'    src/lib.rs  -> 0   (doc comment flipped too)
  - CARGO: build 0 warnings; test 72 passed 0 failed; fmt --check exit 0.
  - GIT:   `git diff --name-only` -> exactly `src/lib.rs`.
  - IF a test fails: re-read it — no test asserts on argv[0]; a failure means you
          accidentally edited an identifier or a non-argv[0] string. Revert and
          re-scope to the quoted token.
```

### Implementation Patterns & Key Details

```rust
// === THE NAMING DISTINCTION (the whole task in one table) ===
//   "qmk_notifier"  (quoted literal, argv[0])  ->  "qmk-notifier"   [CHANGE, 10 sites]
//   `qmk_notifier`  (backtick doc span)        ->  `qmk-notifier`   [CHANGE, 1 site, line 154]
//   use qmk_notifier:: / mod qmk_notifier      ->  (unchanged)      [INVARIANT, none in lib.rs]
//   qmk_notifier commit `01a51935`             ->  (unchanged)      [FIRMWARE, in core.rs not here]
//   name = "qmk-notifier" (Cargo.toml)         ->  (unchanged)      [already correct]


// === WHY ARGV[0] IS COSMETIC (clap mechanics) ===
//   fn build_cli_command() -> Command {
//       Command::new("QMK Keyboard Communication Tool")   // display NAME (human text)
//           // NOTE: no .bin_name(...) — clap derives program name from argv[0]
//           .arg(/* … */)
//   }
//   fn cli_for(args: &[&str]) -> RunParameters {
//       build_cli_command().try_get_matches_from(args)…   // args[0]=prog name; args[1..]=flags
//   }
//   => swapping args[0] changes ONLY the name in generated help/usage/error text.


// === WHY THE QUOTED-TOKEN SED IS SAFE ===
//   grep -cE '"qmk_notifier"' src/lib.rs == 10  (all argv[0], verified)
//   The token "qmk_notifier" (with quotes) does NOT match:
//     - `qmk_notifier`  (backticks — the doc span, handled separately)
//     - use qmk_notifier::  (no quotes; and none exist in lib.rs anyway)
//   Pre/post count guards (==10 / ==0) make the replacement self-verifying.
```

### Integration Points

```yaml
SOURCE FILES:
  - modify: "src/lib.rs ONLY (cwd = /home/dustin/projects/qmk-notifier)"
  - edit:   "10 quoted literals \"qmk_notifier\" -> \"qmk-notifier\" (argv[0] sites)"
  - edit:   "1 backtick doc span `qmk_notifier` -> `qmk-notifier` (line 154)"
  - unchanged: "Command::new(\"QMK Keyboard Communication Tool\") (line ~158),
                all use/mod identifiers, all other files"

DEPENDENCIES / Cargo.toml:
  - none. No dependency change (the crate name is already `qmk-notifier`).

PUBLIC API SURFACE:
  - unchanged. "All edits are inside #[cfg(test)] mod tests (argv[0] inputs) +
                one private-fn doc comment. parse_cli_args/run/build_cli_command
                signatures and behavior are identical."

PARALLEL-SIBLING CONTRACT (P1.M1.T1.S2 — main.rs comment fix, in progress):
  - S2 edits: "src/main.rs:88 only (SIGPIPE comment qmk_notifier -> qmk-notifier)."
  - S2 explicitly: "'src/lib.rs (sibling T2.S1) untouched'."
  - This task: "src/lib.rs only. Do NOT edit main.rs."
  - Collision: "NONE — disjoint files."

SCOPE BOUNDARY:
  - ONLY src/lib.rs is modified. NOT main.rs (S2), core.rs (S1 done), error.rs,
    Cargo.toml, README.md, PRD.md.
  - DOCS: none — test-internal cosmetic edits + one comment; no user-facing/
    config/API surface change (matches contract).
```

## Validation Loop

### Level 1: Syntax & Style (Immediate Feedback)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (hyphen) — NOT the firmware cwd

cargo fmt

cargo build 2>&1 | tee /tmp/t2s1_build.log
# Expected: "Finished `dev` profile ..." and NO "warning:" lines.
# (String-literal/comment edits cannot introduce warnings.)

cargo fmt --check
# Expected: exit 0 (literal/span content swaps don't change formatting).
```

### Level 2: Unit Tests (Component Validation)

```bash
# Full lib suite — argv[0] swap is cosmetic; all 72 tests pass unchanged.
cargo test --lib 2>&1 | tee /tmp/t2s1_test.log
# Expected: "test result: ok. 72 passed; 0 failed; 0 ignored; ...".

# Targeted sanity: the two tests whose argv vectors we edited most heavily.
cargo test --lib test_action_selectors_are_mutually_exclusive -- --nocapture
cargo test --lib test_query_info_combines_with_device_flags -- --nocapture
# Expected: each passes (clap still rejects conflicts / still parses device flags,
#           because argv[0] never affects parsing).
```

### Level 3: Integration Testing (System Validation)

```text
NOT APPLICABLE for this subtask.
The edits are argv[0] display strings (test-only) + one doc comment. There is no
runtime/CLI path to exercise: argv[0] is never parsed as an argument, and the doc
comment is not executable. The Level 2 suite (72 tests) plus the Level 4 grep
gates ARE the end-to-end verification. (Running the real binary is unrelated to
in-test argv[0] strings.)
```

### Level 4: Creative & Domain-Specific Validation

```bash
cd /home/dustin/projects/qmk-notifier

# Proof the 10 argv[0] literals flipped (quoted underscore -> 0, quoted hyphen -> 10):
test "$(grep -cE '"qmk_notifier"' src/lib.rs)" -eq 0 && echo "argv[0] underscore: GONE (good)"
test "$(grep -cE '"qmk-notifier"' src/lib.rs)" -eq 10 && echo "argv[0] hyphen: 10 (good)"

# Proof NO underscore form remains anywhere in lib.rs (doc comment also flipped):
test "$(grep -cE 'qmk_notifier' src/lib.rs)" -eq 0 && echo "lib.rs: zero qmk_notifier (good)"

# Proof the library-identifier invariant is intact across the crate (main.rs line 1):
grep -n "^use qmk_notifier::" src/main.rs   # Expected: exactly one hit (unchanged)

# Proof the clap display NAME was NOT touched:
grep -n 'Command::new("QMK Keyboard Communication Tool")' src/lib.rs   # Expected: one hit, unchanged

# Proof the doc comment now names the binary:
grep -n 'for `qmk-notifier` (PRD §11' src/lib.rs   # Expected: one hit (line ~154)

# Proof only lib.rs changed:
git diff --name-only   # Expected: exactly src/lib.rs
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1 passed: `cargo build` → 0 warnings.
- [ ] Level 1 passed: `cargo fmt --check` → exit 0.
- [ ] Level 2 passed: `cargo test --lib` → 72 passed, 0 failed.
- [ ] Level 4 passed: all grep gates (`"qmk_notifier"`==0, `"qmk-notifier"`==10,
      `qmk_notifier`==0, `git diff --name-only`==`src/lib.rs`).

### Feature Validation

- [ ] All 10 argv[0] literals are `"qmk-notifier"`.
- [ ] The line-154 doc comment code span is `` `qmk-notifier` ``.
- [ ] `Command::new("QMK Keyboard Communication Tool")` unchanged.
- [ ] No `use qmk_notifier::` / `mod qmk_notifier` touched (invariant).
- [ ] Only `src/lib.rs` modified; main.rs/core.rs/error.rs/Cargo.toml untouched.

### Code Quality Validation

- [ ] Replacement scoped to the quoted token (no blanket identifier sed).
- [ ] Pre/post count guards used (==10 before, ==0 after).
- [ ] Doc-comment edit used a unique oldText (the `` for `…` (PRD §11 `` span).
- [ ] Edits follow the audit's classification (ARGV0→hyphen; LIBRARY_ID→underscore).

### Documentation & Deployment

- [ ] Doc comment now names the binary `qmk-notifier` (aligns with Cargo.toml).
- [ ] No external doc files changed (test-internal + one comment; DOCS: none).
- [ ] No Cargo.toml / env / config change.

---

## Anti-Patterns to Avoid

- ❌ Don't run a blanket `sed 's/qmk_notifier/qmk-notifier/g'` — it would (if any
  existed) corrupt Rust identifiers `use qmk_notifier::` / `mod qmk_notifier`,
  which MUST stay underscore (Cargo-derived library id; hyphen is a syntax error).
  Scope to the **quoted token** `"qmk_notifier"` (and the backtick doc span).
- ❌ Don't work in `/home/dustin/projects/qmk_notifier` (UNDERSCORE) — that is the
  firmware C repo. This task's crate is `/home/dustin/projects/qmk-notifier`
  (HYPHEN). Verify with `head -2 Cargo.toml`.
- ❌ Don't change `Command::new("QMK Keyboard Communication Tool")` — it is clap's
  human-readable display NAME, not a binary identifier; the audit doesn't flag it.
- ❌ Don't edit `src/main.rs` (sibling P1.M1.T1.S2 owns it), `src/core.rs`
  (sibling S1, done), `src/error.rs`, or `Cargo.toml`. This task = lib.rs only.
- ❌ Don't trust the contract's line numbers blindly — re-derive with
  `grep -nE '"qmk_notifier"' src/lib.rs` (verified == 10 today, but guard anyway).
- ❌ Don't skip the pre/post grep count guards — they are the cheapest proof the
  replacement was scoped correctly (10→0 quoted underscore, 0→10 quoted hyphen).
- ❌ Don't expect any test to change behavior — argv[0] is display-only in clap;
  test bodies assert on parsed `RunParameters`, never on argv[0] or clap text. If a
  test fails, you edited something beyond the quoted token (revert + re-scope).
- ❌ Don't treat these 11 sites as must-fix — the audit classes them OPTIONAL
  (cosmetic). The must-fix comment sites are core.rs:129/425 (S1, done) and
  main.rs:88 (S2). Keep this task to the lib.rs argv[0] strings + the one doc span.
- ❌ Don't change the doc comment's prose beyond the one code span — the edit is
  `` `qmk_notifier` `` → `` `qmk-notifier` ``, nothing else on that line.
- ❌ Don't add `#[allow(...)]`, derives, or any code — this is a literal/comment
  swap only.

---

**Confidence Score: 10/10** for one-pass implementation success. The deliverable
is 11 fully-specified one-token edits (10 verified argv[0] quoted literals at
confirmed line numbers + 1 unique doc-comment code span), each with before/after
text, against a file I have read in full. The one real hazard — confusing the
argv[0] **string** (change) with the library **identifier** (invariant) — is
called out repeatedly, with the safe scoped-to-quoted-token sed + pre/post count
guards as the self-verifying method, and the explicit "no blanket sed" anti-pattern.
The repo-location gotcha (hyphen crate vs underscore firmware cwd) is flagged with
a verify step. The change is cosmetic by clap mechanics (argv[0] = display name
only, never parsed), so the 72-test baseline stays green by construction; the grep
gates (0 quoted underscore / 10 quoted hyphen / 0 total underscore / only lib.rs
diffed) are the deterministic proof.