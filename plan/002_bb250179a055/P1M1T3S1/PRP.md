# PRP — P1.M1.T3.S1: Final grep gate — confirm no stale naming references in README.md / PRD.md / Cargo.toml

---

## Goal

**Feature Goal**: Run the **final naming-verification gate** for the "Crate Naming
Reconciliation" milestone (plan 002) against the **Rust crate repo's** `README.md`,
`PRD.md`, and `Cargo.toml` — the three files the source-edit subtasks (T1.S1, T1.S2,
T2.S1) explicitly did **not** touch. The gate executes a single recursive grep over
all three files, classifies every `qmk[_-]notifier` / `dabstractor/qmk*` reference
against the four-class naming contract (CRATE/FIRMWARE/LIBRARY_ID/DAEMON), and
confirms **each reference already matches its target form**. Per the authoritative
`naming_audit.md` *and* an independent live grep run this session, the expected
outcome is **PASS with zero edits** — README.md/PRD.md/Cargo.toml were already
correct before any source edit and stay correct after (the source edits live in
`src/*.rs` only). This subtask is the **Mode B documentation-sync** gate for the
milestone: if (unexpectedly) a stale reference is found, it is fixed here; otherwise
the gate closes clean.

**Deliverable**: A completed verification record (the grep output + a per-line
classification cross-check against `naming_audit.md`) demonstrating **zero stale
naming references** in `README.md`, `PRD.md`, and `Cargo.toml` of
`/home/dustin/projects/qmk-notifier`. Concretely, the implementer confirms:
(a) every crate/binary/repo reference is `qmk-notifier` (hyphen); (b) every firmware
reference is `qmk_notifier` (underscore); (c) every library-identifier reference
(`use qmk_notifier::`, the dep-table key) stays underscore; (d) **no** `dabstractor/qmk_notifier`
URL appears where `dabstractor/qmk-notifier` is intended, nor vice versa; (e)
`README.md` carries the library-identifier clarification (PRD §1.2 wording); (f)
`PRD.md` §13's `qmkonnect/Cargo.toml` pin shows the exact shape
`{ package = "qmk-notifier", git = "…/qmk-notifier", tag = "v0.3.0" }`; (g) the
firmware commit reference reads `qmk_notifier commit 01a51935` (underscore).

**Success Definition**: `grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' README.md PRD.md Cargo.toml`
(cwd = crate root) returns only references whose class matches their token form, with
**zero stale references**. `git diff --name-only` ⇒ **empty** (the expected,
audit-confirmed outcome) OR, only if an unexpected stale reference was found and
fixed, exactly the one doc file edited. The library-identifier invariant
(`use qmk_notifier::`) is intact in `src/main.rs` / `src/core.rs` (sanity check that
the parallel source-edit siblings left it untouched). No `src/*.rs` file is modified
by this task.

## User Persona (if applicable)

**Target User**: (1) A future maintainer / reviewer who needs assurance the crate's
user-facing docs (`README.md`) and spec (`PRD.md`) name the three-repo ecosystem
correctly (the exact confusion PRD §1.2's "Naming hazard" warns about). (2) A
downstream `qmkonnect` integrator copy-pasting the §13 git-pin example, who must get
the real crate repo `dabstractor/qmk-notifier`. (3) The milestone orchestrator, for
whom this gate is the closing "documentation sync" (Mode B) subtask of P1.M1.

**Use Case**: After T1.S1/T1.S2/T2.S1 fixed mislabeled references in `src/*.rs`
comments/strings, a reviewer wants a single deterministic check proving the
user-facing and spec docs were never stale to begin with (and, as defense-in-depth,
catching anything that slipped past the audit).

**User Journey**: reviewer runs the grep gate → every line matches its class →
"documentation naming: PASS, zero fixes" → milestone P1.M1 closes; if any line
mismatches its class, the reviewer fixes that one token and re-runs the gate.

**Pain Points Addressed**: The cross-repo naming hazard (`qmk-notifier` hyphen =
this Rust crate vs `qmk_notifier` underscore = the firmware C module). Stale docs
that call the crate by the firmware name (or vice versa) mislead integrators and
clone the wrong repo. This gate guarantees the two most-read docs + the package
manifest are self-consistent and ecosystem-correct.

## Why

- **Closes the milestone's documentation loop (Mode B).** T1.S1 (core.rs commit
  comment, Complete), T1.S2 (main.rs binary-invocation comment, Complete), and T2.S1
  (lib.rs argv[0] + doc span, Implementing) reconciled the *source*. This subtask is
  the *documentation* counterpart — the final gate confirming `README.md`, `PRD.md`,
  and `Cargo.toml` have no stale `qmk[_-]notifier` references. Per the item contract:
  "DOCS: This IS the Mode B documentation sync subtask. If any README.md/PRD.md
  reference needed fixing, it was fixed here."
- **Defense-in-depth over the audit.** `naming_audit.md` already classified ~46
  README/PRD references as CORRECT and zero as needing change. This gate *re-runs*
  the grep against the live tree (the tree may have shifted between the audit and
  now) and converts the audit's claim into a freshly verified fact. If the tree
  drifted, the gate catches it.
- **Zero expected blast radius.** Because the docs were already correct (dual
  evidence: audit + this session's live grep), the gate is almost certainly a
  no-op edit-wise. It is cheap, deterministic, and authoritative.
- **Protects the library-identifier invariant.** The contract says
  `use qmk_notifier::` must stay underscore (Cargo derives the library id from the
  package name; a hyphen is a Rust syntax error). This gate sanity-checks that the
  parallel source-edit siblings did not corrupt that invariant in `src/main.rs` /
  `src/core.rs` — a cross-check the milestone owes before closing.

## What

### The gate procedure (run in the crate root)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (HYPHEN) — NOT the firmware cwd
grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' README.md PRD.md Cargo.toml
```

Every returned line is classified into exactly one of four classes (see
*naming_audit.md* "Authority / naming contract") and must match its target token:

| Class | Target token | Where it appears |
|---|---|---|
| **CRATE** (package / repo / binary) | `qmk-notifier` (hyphen) | `Cargo.toml name/repo/homepage`; README crate/binary mentions + clone/invocation; PRD title/§2/§1.1/§13 git URL |
| **FIRMWARE** (C module / repo) | `qmk_notifier` (underscore) | `dabstractor/qmk_notifier` repo URLs; `qmk_notifier/PRD.md` cross-refs; the commit `qmk_notifier 01a51935` |
| **LIBRARY_ID** (Rust `use`/`mod`/dep-table key) | `qmk_notifier` (underscore, **INVARIANT**) | `use qmk_notifier::`; `qmk_notifier::CommandResponse`; the `qmkonnect` dep key `qmk_notifier = { package = "qmk-notifier", … }` |
| **DAEMON** | `qmkonnect` / `QMKonnect` (as-is) | `dabstractor/qmkonnect` |

### Specific confirmations the contract demands (item clause 3)

These six facts must be true in the live tree (each was verified CORRECT during
research — see research/notes.md for the exact line numbers):

1. **README.md library-identifier clarification present** — README.md contains the
   PRD §1.2 wording that the import is `use qmk_notifier::` *because Cargo derives
   `_` from the package's `-`. (Live: README.md:104.)
2. **PRD.md §13 qmkonnect/Cargo.toml pin shape exact** — the example shows
   `qmk_notifier = { package = "qmk-notifier", git = "https://github.com/dabstractor/qmk-notifier", tag = "v0.3.0" }`
   (LIBRARY_ID dep key + CRATE package/git). (Live: PRD.md:440-441.)
3. **No cross-wired crate repo URL** — no `dabstractor/qmk_notifier` URL appears
   where the crate `dabstractor/qmk-notifier` is intended (Cargo.toml:7,8;
   README.md:20; PRD.md:68,441).
4. **No cross-wired firmware repo URL** — no `dabstractor/qmk-notifier` URL appears
   where the firmware `dabstractor/qmk_notifier` is intended (README.md:12,173,196;
   PRD.md:12,70,166,210,344,445,476).
5. **Firmware commit reference correct** — the commit reads
   `qmk_notifier commit 01a51935` (underscore = firmware). PRD.md:210 is the
   authoritative instance; README.md has no such commit ref. (The two `core.rs`
  sites that previously mislabeled it as `qmk-notifier` were fixed by T1.S1 in
   source — not in docs.)
6. **Cargo.toml naming fields all hyphen** — `name`, `repository`, `homepage` are
   all `qmk-notifier`. (Live: Cargo.toml:2,7,8.)

### Expected outcome

**PASS, zero edits.** Both `naming_audit.md` ("README/PRD/Cargo all CORRECT for
their class") and an independent live grep this session confirm every reference
matches its class. `git diff --name-only` should be empty after the gate.

### Success Criteria

- [ ] The grep over `README.md PRD.md Cargo.toml` returns only class-matching
      references; **zero stale references**.
- [ ] README.md carries the library-identifier clarification (`use qmk_notifier::`,
      Cargo-derived `_` from `-`).
- [ ] PRD.md §13 qmkonnect pin shows `{ package = "qmk-notifier", git = "…/qmk-notifier", tag = "v0.3.0" }`.
- [ ] No `dabstractor/qmk_notifier` URL where `dabstractor/qmk-notifier` is intended
      (and vice versa).
- [ ] The firmware commit reads `qmk_notifier commit 01a51935` (underscore).
- [ ] Cargo.toml `name`/`repository`/`homepage` are all `qmk-notifier`.
- [ ] The library-identifier invariant is intact in source:
      `grep -n "^use qmk_notifier::" src/main.rs src/core.rs` returns the unchanged
      underscore imports.
- [ ] No `src/*.rs` file is modified by this task. `git diff --name-only` is empty
      (expected) or names exactly one doc file (only if an unexpected fix was needed).

## All Needed Context

### Context Completeness Check

> _"If someone knew nothing about this codebase, would they have everything needed
> to implement this successfully?"_ — **Yes.** The exact repo (crate, hyphen — plus
> the firmware-cwd trap and its verify step), the exact grep command, the four-class
> naming contract, a per-file reference map with verified line numbers, the six
> specific contract confirmations each pinned to a live line, the dual-evidence
> expected outcome (PASS, zero edits), the scope boundary with the sibling T3.S2
> (dependency-list drift), and the deterministic grep/git gates are all below. The
> implementer exercises one judgment per grepped line — "does this token match its
> class?" — and every answer is pre-verified YES.

### Documentation & References

```yaml
# MUST READ — the authoritative naming classification (the doc the item references)
- file: plan/002_bb250179a055/architecture/naming_audit.md
  why: "Defines the 4-class naming contract (CRATE/FIRMWARE/LIBRARY_ID/ARGV0/DAEMON)
        and classifies EVERY qmk[_-]notifier reference in the crate. Its README.md,
        PRD.md, and Cargo.toml sections state verbatim 'All references verified
        CORRECT' and list the exact correct lines. Its 'Do NOT change' section lists
        Cargo.toml/Cargo.lock/README/PRD as already-correct. Its only CHANGE items
        (2 YES core.rs, 1 YES main.rs, 11 OPTIONAL lib.rs) are ALL in src/*.rs —
        NONE in README/PRD/Cargo. This gate re-validates the audit's README/PRD/Cargo
        claim against the live tree."
  section: "Authority / naming contract", "### README.md", "### PRD.md",
           "### Cargo.toml", "Do NOT change"
  critical: "The audit is the source of truth for CLASS. A crate/repo/binary ref →
             qmk-notifier (hyphen); a firmware ref → qmk_notifier (underscore); a
             Rust library id (use/mod/dep-key) → qmk_notifier (underscore, INVARIANT).
             The load-bearing cross-ref is commit 01a51935 = a FIRMWARE fix ⇒
             'qmk_notifier commit 01a51935' (PRD.md:210 is authoritative)."

# MUST READ — the file set under the gate (the crate's docs + manifest)
- file: README.md
  why: "User-facing doc. Contains crate mentions (qmk-notifier, hyphen), firmware
        cross-links ([qmk_notifier](…/dabstractor/qmk_notifier)), and the
        library-identifier clarification (line ~104: 'the library identifier is
        `qmk_notifier` (Cargo derives `_` from the package's `-`), so the import is
        `use qmk_notifier::`') + the use-example (line ~107). All verified CORRECT."
  pattern: "Crate/binary mentions use the hyphen form; firmware module cross-links
            use the underscore form + the firmware repo URL; Rust library-id
            references use the underscore form."
  gotcha: "Run the grep in /home/dustin/projects/qmk-notifier (HYPHEN). The firmware
           cwd (/home/dustin/projects/qmk_notifier, UNDERSCORE) has its OWN README.md
           that is a different doc (firmware README) — grepping it is out of scope and
           would be confusing."

- file: PRD.md
  why: "The crate spec. Contains the §1.1/§1.2 ecosystem table (qmk-notifier ← this
        repo; qmk_notifier = firmware), the §1.2 naming-hazard + library-id
        clarification (lines 73-78), the §2 repo-layout (package qmk-notifier), the
        authoritative firmware-commit ref (line 210: qmk_notifier 01a51935), and the
        §13 qmkonnect Cargo.toml pin (lines 440-441). All verified CORRECT."
  pattern: "Same 4-class rule. Note the §13 pin uses the LIBRARY_ID dep key
            `qmk_notifier` paired with the CRATE package `qmk-notifier` — BOTH are
            correct (Cargo requires the underscore dep key even for a hyphen package)."
  gotcha: "The §13 pin's dep key `qmk_notifier` (underscore) is NOT stale — it is
           correct Cargo syntax. Do NOT 'fix' it to qmk-notifier."

- file: Cargo.toml
  why: "The crate manifest. name/repo/homepage all qmk-notifier (hyphen). Verified
        CORRECT. (The [dependencies] list — clap/hidapi + cfg(unix) libc — is drift
        vs PRD §2's dirs/toml/serde note, but that is P1.M1.T3.S2's concern, NOT this
        task's. This task checks the NAMING fields only.)"
  gotcha: "Do NOT audit the dependency VERSIONS/PACKAGES here — that is T3.S2. This
           task checks only name/repository/homepage = qmk-notifier."

# MUST READ — the source-edit siblings (inputs; confirm they left docs alone)
- file: plan/002_bb250179a055/P1M1T1S1/PRP.md
  why: "T1.S1 fixed core.rs:129,425 (qmk-notifier commit → qmk_notifier commit
        01a51935). Edits src/core.rs ONLY. Confirms it does NOT touch README/PRD/Cargo
        — hence this gate's docs result is independent of it. Status: Complete."
- file: plan/002_bb250179a055/P1M1T1S2/PRP.md
  why: "T1.S2 fixed main.rs:88 (qmk_notifier --list → qmk-notifier --list). Edits
        src/main.rs ONLY. Does NOT touch README/PRD/Cargo. Status: Complete."
- file: plan/002_bb250179a055/P1M1T2S1/PRP.md
  why: "T2.S1 (Implementing, parallel) flips 10 lib.rs argv[0] quoted literals +
        1 doc span to qmk-notifier. Edits src/lib.rs ONLY; its PRP's 'Integration
        Points' explicitly lists README.md/PRD.md/Cargo.toml as untouched. Confirms
        this gate's docs result is independent of T2.S1's landing. Also documents the
        crate-cwd-vs-firmware-cwd trap this gate inherits."
  critical: "T2.S1 may or may not have landed when this gate runs — it does NOT
             matter, because T2.S1 never touches README/PRD/Cargo. Do NOT block this
             gate on T2.S1."

# REFERENCE — the PRD sections whose wording the gate confirms
- file: PRD.md
  section: "1.2 The broader ecosystem (naming hazard + library-id note)", "13 Versioning (qmkonnect pin)"
  why: "§1.2 is the naming-contract authority the README clarification mirrors; §13
        is the qmkonnect-pin shape the gate confirms. Both verified present + correct."

# REFERENCE — the PRD selectors this item was planned against (merged PRD context)
- file: plan/002_bb250179a055/prd_snapshot.md
  why: "The merged-PRD snapshot used for planning. §1.2 (ecosystem/naming hazard),
        §2 (repo layout), §13 (versioning/cross-repo links), §14 (invariants). The
        naming contract derives from §1.2; §13 holds the qmkonnect pin to confirm."

# REFERENCE — research notes (live-grep evidence + per-line map + scope boundary)
- docfile: plan/002_bb250179a055/P1M1T3S1/research/notes.md
  why: "Documents the repo-location trap, the 4-class contract, the live-grep result
        (every README/PRD/Cargo ref CORRECT), the exact line numbers for the six
        contract confirmations, the independence from T1/T2 (docs untouched by
        source edits), and the scope boundary with T3.S2 (dependency-list drift)."
```

### Current Codebase tree (verified; relevant slice — cwd = the HYPHEN crate)

```bash
/home/dustin/projects/qmk-notifier/        # <-- THIS task's repo (Rust crate, hyphen)
├── Cargo.toml          # name/repository/homepage = "qmk-notifier"  ← GATE TARGET (naming fields)
├── Cargo.lock
├── README.md           # user-facing README  ← GATE TARGET
├── PRD.md              # crate spec  ← GATE TARGET
├── .gitignore
├── LICENSE
└── src
    ├── main.rs         # T1.S2 edited :88 (Complete)  ← NOT modified by this task
    ├── core.rs         # T1.S1 edited :129,425 (Complete)  ← NOT modified by this task
    ├── error.rs        #  ← NOT modified
    └── lib.rs          # T2.S1 edits argv[0]+doc (Implementing/parallel)  ← NOT modified by this task
```

> NOTE: the harness session cwd may report `/home/dustin/projects/qmk_notifier`
> (UNDERSCORE) — that is a DIFFERENT repo, the qmk **firmware** C project
> (`notifier.c`, `pattern_match.c`, `qmk_stubs/`, no `Cargo.toml`, no `src/`). It has
> its OWN README.md/PRD.md which are out of scope. All work for THIS task is in the
> hyphen crate above. Verify with `head -2 Cargo.toml` ⇒ `name = "qmk-notifier"`.

### Desired Codebase tree with files to be added/modified

```bash
# Expected: NO changes (gate passes clean).
README.md     # unchanged (verified correct)
PRD.md        # unchanged (verified correct)
Cargo.toml    # unchanged (naming fields verified correct)
src/*.rs      # unchanged (NOT this task's scope)
# ONLY IF an unexpected stale reference is found and fixed:
#   exactly ONE of {README.md, PRD.md} edited by one token (see Anti-Patterns).
```

> No new files (other than the PRP/research under plan/, which is this task's own
> output). The expected end state is an empty `git diff`.

### Known Gotchas of our codebase & Library Quirks

```text
CRITICAL — REPO LOCATION: this task's repo is /home/dustin/projects/qmk-notifier
  (HYPHEN). The harness session cwd /home/dustin/projects/qmk_notifier (UNDERSCORE)
  is the firmware C repo — a different project with its OWN README.md/PRD.md (the
  firmware spec). cd to the hyphen crate before the grep. Verify:
  `head -2 Cargo.toml`  =>  `name = "qmk-notifier"`. (Same trap as T2.S1's PRP.)

CRITICAL — LIBRARY_ID is UNDERSCORE and INVARIANT (do not "fix" it to hyphen):
  `use qmk_notifier::`, `mod qmk_notifier`, and the qmkonnect dep-table KEY
  `qmk_notifier = { package = "qmk-notifier", … }` MUST stay underscore. Cargo
  derives the library id `qmk_notifier` from the package `qmk-notifier` (hyphens are
  illegal in Rust identifiers). A hyphen there is a syntax error. The §13 dep key
  `qmk_notifier` is CORRECT — do not flag or change it.

CRITICAL — The §13 qmkonnect pin uses BOTH forms intentionally:
  `qmk_notifier = { package = "qmk-notifier", git = "…/qmk-notifier", tag = "v0.3.0" }`
  — the dep KEY is `qmk_notifier` (LIBRARY_ID, underscore, correct) and the package
  + git URL are `qmk-notifier` (CRATE, hyphen, correct). Both halves are right.

CRITICAL — Commit 01a51935 is a FIRMWARE fix ⇒ `qmk_notifier commit 01a51935`
  (underscore). PRD.md:210 is the authoritative instance and is correct. The crate's
  src/core.rs previously mislabeled it as `qmk-notifier` (T1.S1 fixed that in SOURCE);
  the docs were never affected. Do not "correct" PRD.md:210 — it is already right.

CRITICAL — This task is Mode-B-authorized to edit README.md/PRD.md for naming fixes,
  BUT the expected outcome is ZERO edits (audit + live grep both confirm correctness).
  Only if a stale reference is actually found do you edit; otherwise close the gate
  clean (empty git diff). Do not invent edits.

CRITICAL — SCOPE: naming only. Do NOT audit Cargo.toml's [dependencies] versions/
  packages (clap/hidapi/libc vs PRD §2's dirs/toml/serde note) — that drift is
  P1.M1.T3.S2's deliverable. This task checks name/repository/homepage = qmk-notifier
  ONLY.

CRITICAL — Do NOT modify any src/*.rs file. T1.S1/T1.S2/T2.S1 own the source edits.
  This task's only permissible edits are README.md or PRD.md naming tokens, and only
  if a stale reference is found (not expected).

NOTE — the gate is INDEPENDENT of T2.S1's landing. T2.S1 edits src/lib.rs only; it
  never touches README/PRD/Cargo. So whether T2.S1 has landed does not change this
  gate's result. Do not block the gate on T2.S1.

NOTE — docs-gate task ⇒ grep is the PRIMARY tool. `cargo build`/`cargo test` are only
  a sanity check that the (already-landed) source edits still compile and the
  library-id invariant is intact; they are NOT the gate. A docs task does not need a
  green build to pass its naming gate, but a green build confirms no collateral.

NOTE — no rustfmt.toml / clippy.toml (default configs); `cargo fmt --check` is
  unaffected by docs edits (markdown/toml aren't formatted by rustfmt anyway).
```

## Implementation Blueprint

### Data models and structure

No code, no types, no models. The "model" is the four-class naming contract
documented in `naming_audit.md`. The deliverable is a verification record (grep
output + classification cross-check), not a data structure.

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: LOCATE the correct repo + snapshot the baseline
  - VERIFY repo: `cd /home/dustin/projects/qmk-notifier && head -2 Cargo.toml`
          prints `name = "qmk-notifier"`. If it shows a C project or qmk_notifier,
          you are in the WRONG repo (firmware cwd) — cd to the hyphen crate.
  - SNAPSHOT the pre-gate diff so you can prove "no change" at the end:
          `git status --porcelain` (record; expect only plan/ artifacts untracked).
  - GOAL: confirm the working tree is the crate and capture the clean baseline.

Task 2: RUN the naming grep across the three gate files
  - RUN (cwd = crate root):
        grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' \
            README.md PRD.md Cargo.toml
  - CAPTURE the full output (it is the verification record). Expect ~30-40 lines,
          every one class-correct (per the per-file map in research/notes.md).
  - GOAL: produce the raw evidence to classify.

Task 3: CLASSIFY each returned line + confirm the six contract facts
  - FOR EACH grep line, assign a class (CRATE/FIRMWARE/LIBRARY_ID/DAEMON) and verify
          the token matches the class target (CRATE→hyphen, FIRMWARE→underscore,
          LIBRARY_ID→underscore, DAEMON→as-is).
  - CONFIRM the six specific contract facts (item clause 3):
      (1) README.md library-id clarification present (≈line 104).
      (2) PRD.md §13 qmkonnect pin == `{ package = "qmk-notifier",
          git = "…/qmk-notifier", tag = "v0.3.0" }` (≈lines 440-441).
      (3) No dabstractor/qmk_notifier URL where dabstractor/qmk-notifier is intended.
      (4) No dabstractor/qmk-notifier URL where dabstractor/qmk_notifier is intended.
      (5) Firmware commit reads `qmk_notifier commit 01a51935` (PRD.md:210).
      (6) Cargo.toml name/repository/homepage all `qmk-notifier` (lines 2,7,8).
  - EXPECTED: every line class-correct; all six facts true; zero stale references.
  - GOAL: convert the raw grep into a verified PASS (or surface the one stale ref).

Task 4: SANITY-CHECK the library-identifier invariant in source (no edit)
  - RUN: `grep -n "^use qmk_notifier::" src/main.rs src/core.rs`
  - EXPECT: the unchanged underscore imports (e.g. main.rs:1
          `use qmk_notifier::{parse_cli_args, run, …}`). This confirms T1.S1/T1.S2
          did not corrupt the LIBRARY_ID. It is a read-only check — do NOT edit src.
  - GOAL: cross-check the milestone's invariant before closing.

Task 5: CLOSE the gate (expected: no edit; conditional: fix)
  - IF (expected) zero stale references: the gate PASSES. No edit. Proceed to
          validation; `git diff --name-only` must be empty.
  - IF (unexpected) a stale reference is found: fix THAT ONE token in README.md or
          PRD.md only (never src/*.rs; never Cargo.toml unless it is a naming field),
          matching the class target, then re-run Task 2 to confirm the fix and that
          no other line regressed. Document the fix in the verification record.
  - DO NOT: invent edits; touch src/*.rs; audit Cargo dependency versions (T3.S2);
            change the §13 dep key qmk_notifier (it is correct underscore); change
            PRD.md:210 (correct); work in the firmware cwd.

Task 6: VALIDATE (grep + git gates are the proof; cargo is sanity-only)
  - RUN the Level-1 grep gates + the Level-4 cross-checks (see Validation Loop).
  - RUN a `cargo build` + `cargo test --lib` SANITY check ONLY to confirm the source
          tree still compiles and the library-id invariant holds (NOT the gate
          itself). Expected: 0 warnings; the baseline test count passes.
  - RUN `git diff --name-only` — expected EMPTY (or exactly one doc file if a fix
          was needed). No src/*.rs in the diff.
```

### Implementation Patterns & Key Details

```text
// === THE GATE IN ONE LINE ===
//   grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' \
//       README.md PRD.md Cargo.toml
//   => every line must be class-correct:
//        crate/repo/binary ............ qmk-notifier  (hyphen)
//        firmware module/repo ......... qmk_notifier  (underscore)
//        Rust library id (use/mod/key) qmk_notifier  (underscore, INVARIANT)
//        daemon ....................... qmkonnect     (as-is)


// === THE SIX CONTRACT CONFIRMATIONS (each pre-verified CORRECT) ===
//   (1) README ~104: "library identifier is `qmk_notifier` (Cargo derives `_` …) …
//                    so the import is `use qmk_notifier::`"
//   (2) PRD ~440-441: qmk_notifier = { package = "qmk-notifier",
//                        git = "https://github.com/dabstractor/qmk-notifier",
//                        tag = "v0.3.0" }
//   (3)/(4) cross-wired repo URLs: NONE (crate URLs hyphen, firmware URLs underscore)
//   (5) PRD:210: "fixed in qmk_notifier commit `01a51935`"  (firmware ⇒ underscore)
//   (6) Cargo.toml 2/7/8: name/repository/homepage = qmk-notifier


// === THE DEP-KEY TRAP (do not "fix" the §13 key) ===
//   Cargo requires the [dependencies] table KEY to be a valid Rust identifier, so
//   the hyphen package `qmk-notifier` is referenced under the underscore key
//   `qmk_notifier`. The §13 example `{ package = "qmk-notifier", … }` is the
//   canonical way to spell this. BOTH the underscore key AND the hyphen package
//   value are correct. Flagging the key as "stale" would be a false positive.


// === WHY ZERO EDITS IS EXPECTED (dual evidence) ===
//   naming_audit.md (the doc the item references) classifies every README/PRD/Cargo
//   reference as CORRECT and lists ZERO change items in those files. An independent
//   live grep this session re-confirmed the same. The source-edit siblings (T1/T2)
//   touch only src/*.rs — never the docs — so the docs cannot have regressed since
//   the audit. Hence the gate's job is to *verify*, not to *edit*.
```

### Integration Points

```yaml
GATE TARGET FILES (cwd = /home/dustin/projects/qmk-notifier):
  - verify: "README.md  — crate/firmware/library-id references (all correct)"
  - verify: "PRD.md     — §1.2 naming hazard + library-id note; §13 qmkonnect pin;
                          §210 firmware commit; ecosystem table rows (all correct)"
  - verify: "Cargo.toml — name/repository/homepage = qmk-notifier (naming fields only)"

NO SOURCE EDITS:
  - src/main.rs:  "NOT modified (T1.S2 owns it; this task only sanity-greps use qmk_notifier::)."
  - src/core.rs:  "NOT modified (T1.S1 owns it; this task only sanity-greps use qmk_notifier::)."
  - src/lib.rs:   "NOT modified (T2.S1 owns it)."
  - src/error.rs: "NOT modified."

CONDITIONAL EDIT AUTHORIZATION (Mode B, only if a stale ref is found):
  - README.md / PRD.md: "a single token fix to match its class target."
  - Cargo.toml:         "only if a NAMING field (name/repository/homepage) is wrong."

SCOPE BOUNDARY vs P1.M1.T3.S2 (sibling):
  - "T3.S1 = naming grep gate (qmk[_-]notifier tokens + repo URLs + library-id note +
     §13 pin shape). T3.S2 = Cargo.toml [dependencies] version/package drift vs PRD §2.
     Do NOT audit dependency versions here."

DEPENDENCY ON T1/T2 (soft / ordering, NOT data-flow):
  - "T1.S1/T1.S2 (Complete) + T2.S1 (parallel) edit src/*.rs only. They never touch
     README/PRD/Cargo, so this gate's result is independent of their landing. The
     'INPUT depends on T1/T2' clause is milestone ordering, not a data dependency."
```

## Validation Loop

### Level 1: The naming grep gate (the primary gate)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (HYPHEN) — NOT the firmware cwd

# The gate. Capture this output; it is the verification record.
grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' README.md PRD.md Cargo.toml \
  | tee /tmp/t3s1_naming_grep.log
# Expected: ~30-40 lines, every one class-correct (crate→hyphen, firmware→underscore,
#           library-id→underscore, daemon→qmkonnect). ZERO stale references.

# Confirm the six contract facts each have a live hit:
grep -n "library identifier is \`qmk_notifier\`" README.md            # (1) library-id clarification
grep -nE 'package = "qmk-notifier"' PRD.md                            # (2) §13 pin (package half)
grep -nE 'git = "https://github.com/dabstractor/qmk-notifier"' PRD.md # (2) §13 pin (git half)
grep -n 'qmk_notifier commit .01a51935.' PRD.md                       # (5) firmware commit (underscore)
grep -nE '^(name|repository|homepage) = "qmk-notifier"' Cargo.toml    # (6) naming fields

# Cross-wired-URL absence checks (item clause 3, "and vice versa"):
#   crate fields must NOT point at the firmware repo:
! grep -qE 'github.com/dabstractor/qmk_notifier' Cargo.toml && echo "Cargo.toml: no firmware URL (good)"
#   the §13 crate pin must use the hyphen repo (not the firmware repo):
grep -q 'dabstractor/qmk-notifier' PRD.md && echo "PRD §13 pin uses hyphen crate URL (good)"
```

### Level 2: Library-identifier invariant (source sanity — read-only)

```bash
cd /home/dustin/projects/qmk-notifier
# Confirm the parallel source-edit siblings left the LIBRARY_ID intact (no edit).
grep -n "^use qmk_notifier::" src/main.rs src/core.rs
# Expected: the unchanged underscore imports (e.g. src/main.rs:1). This is a READ;
#           do NOT modify src. Its purpose is to confirm T1.S1/T1.S2 didn't regress
#           the invariant before the milestone closes.
```

### Level 3: Build sanity (NOT the gate — confirms no collateral from landed source edits)

```bash
cd /home/dustin/projects/qmk-notifier
cargo build 2>&1 | tee /tmp/t3s1_build.log
# Expected: "Finished `dev` profile …" with NO warnings. (This proves the source
#           edits from T1/T2 compile; the docs gate itself needs no build. If the
#           tree fails to build, that is a sibling task's defect, not this gate's —
#           surface it, do not fix src here.)
cargo test --lib 2>&1 | tee /tmp/t3s1_test.log | tail -3
# Expected: the baseline test count passes, 0 failed. (Sanity only.)
```

### Level 4: Gate-close cross-checks

```bash
cd /home/dustin/projects/qmk-notifier

# (a) The naming grep has zero lines whose token mismatches its class. Manual: for
#     each line in /tmp/t3s1_naming_grep.log, the qmk[_-]notifier token must match
#     its class (crate→hyphen, firmware→underscore, library-id→underscore). All
#     pre-verified CORRECT — a mismatch would be the (unexpected) stale ref to fix.

# (b) No src/*.rs was modified by this task:
git diff --name-only -- src/
# Expected: EMPTY.

# (c) Overall diff is empty (expected PASS) or names exactly one doc file (conditional fix):
git diff --name-only
# Expected: EMPTY (the audit-confirmed outcome). If non-empty and the file is
#           README.md or PRD.md, it must be exactly one naming-token fix; re-run the
#           Level-1 grep to confirm the fix and no collateral regression.

# (d) The §13 dep KEY is underscore (LIBRARY_ID) — do not let a "fix" touch it:
grep -nE 'qmk_notifier = \{ package = "qmk-notifier"' PRD.md   # expected: present, correct

# (e) The firmware commit is underscore (PRD.md:210):
grep -n 'qmk_notifier commit .01a51935.' PRD.md                 # expected: present, correct
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1: the naming grep over `README.md PRD.md Cargo.toml` returns only
      class-correct references — **zero stale references**.
- [ ] Level 1: all six contract facts have live hits (library-id clarification;
      §13 pin package + git; no cross-wired URLs; firmware commit underscore;
      Cargo naming fields hyphen).
- [ ] Level 2: `use qmk_notifier::` invariant intact in `src/main.rs` / `src/core.rs`.
- [ ] Level 3 (sanity): `cargo build` → 0 warnings; `cargo test --lib` → baseline passes.
- [ ] Level 4: `git diff --name-only -- src/` empty; overall diff empty (expected)
      or exactly one doc file (conditional fix).

### Feature Validation

- [ ] README.md carries the library-identifier clarification (`use qmk_notifier::`,
      Cargo-derived `_` from `-`).
- [ ] PRD.md §13 qmkonnect pin == `{ package = "qmk-notifier", git = "…/qmk-notifier", tag = "v0.3.0" }`.
- [ ] No `dabstractor/qmk_notifier` URL where `dabstractor/qmk-notifier` is intended
      (and vice versa).
- [ ] Firmware commit reads `qmk_notifier commit 01a51935` (underscore).
- [ ] Cargo.toml `name`/`repository`/`homepage` all `qmk-notifier`.

### Code Quality Validation

- [ ] Gate run in the correct repo (`/home/dustin/projects/qmk-notifier`, hyphen).
- [ ] No `src/*.rs` modified.
- [ ] No Cargo.toml dependency-version audit performed (that is T3.S2).
- [ ] The §13 dep key `qmk_notifier` left underscore (not flagged as stale).
- [ ] No edits invented (expected outcome is an empty diff).

### Documentation & Deployment

- [ ] Verification record (grep output + classification) captured (the deliverable).
- [ ] Mode B authorization respected: README/PRD edited ONLY if a stale ref was found.
- [ ] No Cargo.toml / env / config change (naming fields already correct).

---

## Anti-Patterns to Avoid

- ❌ Don't run the gate in `/home/dustin/projects/qmk_notifier` (UNDERSCORE) — that
  is the firmware C repo with its own (different) README.md/PRD.md. The gate's repo
  is `/home/dustin/projects/qmk-notifier` (HYPHEN). Verify with `head -2 Cargo.toml`.
- ❌ Don't "fix" the §13 dep key `qmk_notifier` to `qmk-notifier` — Cargo requires
  the dependency-table key to be a valid Rust identifier (underscore); the package
  VALUE is `qmk-notifier`. Both halves are correct; the underscore key is NOT stale.
- ❌ Don't "fix" `PRD.md:210` (`qmk_notifier commit 01a51935`) — it is already the
  correct firmware (underscore) form. (The mislabel was in `src/core.rs`, fixed by
  T1.S1 in source.)
- ❌ Don't modify any `src/*.rs` file — T1.S1/T1.S2/T2.S1 own the source edits. This
  task's only permissible edit is a README.md/PRD.md naming token, and only if a
  stale reference is actually found (not expected).
- ❌ Don't audit Cargo.toml's `[dependencies]` versions/packages (clap/hidapi/libc
  vs PRD §2's dirs/toml/serde note) — that drift is P1.M1.T3.S2's deliverable. This
  task checks the NAMING fields (name/repository/homepage) only.
- ❌ Don't invent edits to "look productive." The audit + the live grep both confirm
  every README/PRD/Cargo naming reference is correct. The expected gate outcome is
  PASS with an empty `git diff`. Editing correct references would INTRODUCE a
  regression.
- ❌ Don't block the gate on T2.S1 landing — T2.S1 edits `src/lib.rs` only and never
  touches README/PRD/Cargo. This gate's result is independent of T2.S1.
- ❌ Don't treat a green `cargo build` as the gate — this is a docs/naming gate; the
  grep + classification is the gate. Build/test are sanity-only (confirm the landed
  source edits compile and the library-id invariant holds).
- ❌ Don't confuse the firmware README/PRD (underscore repo) with the crate README/PRD
  (hyphen repo). Each repo's docs correctly name their OWN project; the gate is on
  the CRATE's docs only.
- ❌ Don't trust the audit blindly without re-running the grep — re-validate against
  the live tree (it may have shifted since the audit). That re-validation IS this task.

---

**Confidence Score: 10/10** for one-pass implementation success. The deliverable is
a deterministic grep + a classification cross-check against a naming contract that
the authoritative `naming_audit.md` ALREADY applied to these exact three files
(classifying all ~46 README/PRD references CORRECT and listing ZERO change items in
README/PRD/Cargo), re-confirmed by an independent live grep this session that found
every reference class-correct — including each of the six specific contract facts
the item demands (README:104 library-id clarification; PRD §13 pin shape; no
cross-wired repo URLs either direction; PRD:210 firmware-commit underscore;
Cargo.toml:2/7/8 naming fields hyphen). The only real hazards — working in the wrong
(firmware) repo, "fixing" the invariant underscore dep key / the correct firmware
commit ref, overlapping into T3.S2's dependency-list audit, or inventing edits — are
each called out with a verify step or an explicit anti-pattern. The gate's expected
outcome (empty `git diff`, PASS) is dual-evidenced; the conditional fix-path is
specified for the unexpected case. The parallel T2.S1 does not gate this task
(docs-only result, independent of `src/lib.rs` edits).