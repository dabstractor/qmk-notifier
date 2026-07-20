# PRP — P1.M1.T3.S2: Verify Cargo.toml Dependency List vs PRD §2 and Document Drift

---

## Goal

**Feature Goal**: Run the **dependency-correctness verification gate** for the
"Crate Naming Reconciliation" milestone (plan 002) against the Rust crate
`/home/dustin/projects/qmk-notifier`: (1) prove every dependency in `Cargo.toml`
is **actually used** by source (`hidapi` → HID I/O, `clap` → CLI, `libc` → SIGPIPE
reset, unix-only) and **no dead dependencies remain** (`dirs`, `toml`, `serde` all
absent); (2) prove the dependency set compiles and all **72 tests pass**
(`cargo build` clean + `cargo test --lib` 72/0); (3) confirm the **PRD §2
dep-list drift** (PRD lists `dirs`/`toml`/`serde` as present and omits `libc` —
all stale) is **documented in the architecture findings**; (4) cross-check that
`README.md`'s dependency info does **not** contradict `Cargo.toml`. This is the
**Mode B documentation-sync** sibling of T3.S1 (the naming gate): where T3.S1
verified naming tokens, this verifies the dependency manifest.

**Deliverable**: A **verification record** + a **small additive verification
stamp** appended to
`plan/002_bb250179a055/architecture/system_context.md`'s existing "PRD §2
Dependency-List Drift" section, recording: the per-dep source-usage evidence
(hidapi/clap/libc used; dirs/toml/serde absent), `cargo build` clean + `cargo
test --lib` 72 passed, the README system-lib-layer finding, and "PRD.md
untouched (read-only)." **PRD.md is NOT edited** (read-only per orchestrator
constraints). **No source code is modified.** **`Cargo.toml` is NOT modified**
(it is already correct — this task *verifies* that fact, it does not change it).

**Success Definition**: (a) `cargo build` → 0 warnings; `cargo test --lib` →
**72 passed; 0 failed** (unchanged baseline); (b) `grep -rn "use dirs\|dirs::"
src/` (and `toml`, `serde`) → empty; `grep` for `hidapi`/`clap`/`libc` → hits in
the expected files; (c) the three architecture drift notes
(`system_context.md`, `cargo_naming_convention.md` Part B, `findings_and_risks.md`
Finding 5) are present and complete; (d) `system_context.md` carries the new
"Verified (P1.M1.T3.S2)" stamp; (e) `git diff --name-only` lists **only**
`plan/002_bb250179a055/architecture/system_context.md` (+ this task's own plan/
artifacts) — **NO `src/`, NO `PRD.md`, NO `Cargo.toml`** in the diff.

## User Persona (if applicable)

**Target User**: (1) A future maintainer / reviewer who needs assurance the
crate's `Cargo.toml` has no dead dependencies and the PRD §2 dependency note's
staleness is *known and recorded* (not a silent surprise). (2) A downstream
`qmkonnect` integrator who reads PRD §2 to learn the crate's dependencies and
would be misled by the stale `dirs`/`toml`/`serde` mention — this task ensures
the architecture docs carry the authoritative, verified dep list to cross-check
against. (3) The milestone orchestrator, for whom this is the closing
"dependency documentation sync" subtask.

**Use Case**: After T1/T2 reconciled naming in `src/*.rs`, a reviewer wants a
single deterministic check proving the dependency manifest is lean (all-used,
no-dead) and that the PRD §2 dep-list drift is recorded in the architecture
findings (since PRD.md can't be touched).

**User Journey**: reviewer runs the dep-usage grep + build + test → all deps
used, no dead deps, 72 pass → confirms the drift docs → records the verification
stamp in `system_context.md` → milestone P1.M1 dependency-documentation loop
closes; the architecture docs now carry a freshly verified dep record.

**Pain Points Addressed**: Stale PRD §2 that lists deps no longer present
(`dirs`/`toml`/`serde`) and omits one that is (`libc`) — misleading anyone reading
the spec. Dead dependencies silently bloating the build. This task verifies the
manifest is clean and records the drift authoritatively (without editing the
read-only PRD).

## Why

- **Closes the milestone's dependency-documentation loop (Mode B).** T3.S1
  verified the *naming* tokens in README/PRD/Cargo; this subtask verifies the
  *dependency* manifest in Cargo.toml vs the actual source usage and vs PRD §2's
  (stale) dep note. Together they are the milestone's two documentation gates.
- **Defense-in-depth over the architecture audit.** `cargo_naming_convention.md`
  Part B and `findings_and_risks.md` Finding 5 already asserted the drift. This
  task *re-runs* the dep-usage grep + build + test against the live tree (which
  may have shifted) and converts those assertions into freshly verified facts,
  then stamps the canonical `system_context.md` record.
- **Catches real risk cheaply.** A dead dependency that survived a refactor, or a
  used-but-undeclared dependency, would be a real defect. The source-usage grep
  + `cargo build`/`cargo test` gate catches both deterministically. (Verified this
  session: clean — but the gate is the proof.)
- **Zero blast radius.** The only file touched outside `plan/` is the architecture
  doc `system_context.md` (an additive stamp). No source, no PRD, no Cargo.toml,
  no Cargo.lock. The crate's behavior is unchanged.

## What

### The verification procedure (run in the CRATE root, hyphen repo)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (HYPHEN) — NOT the firmware cwd
head -2 Cargo.toml                       # verify: name = "qmk-notifier"
```

**(a) Dependency correctness — every present dep is used; no dead deps:**

```bash
# Present deps — each MUST have source usage:
grep -rn "use hidapi\|hidapi::" src/   # expect hits: error.rs:1, core.rs:2 (+RawHid impl)
grep -rn "use clap\|clap::"   src/     # expect hits: lib.rs:7 (+value_parser! macro uses)
grep -rn "libc::"              src/     # expect hit:  main.rs:118 (cfg(unix) SIGPIPE reset)

# Absent deps — each MUST have ZERO source usage (dead-dep check):
grep -rn "use dirs\|dirs::"    src/     # expect: empty
grep -rn "use toml\|toml::"    src/     # expect: empty
grep -rn "use serde\|serde::\|#\[derive(.*Serialize" src/   # expect: empty
```

Expected (pre-verified this session): `hidapi`/`clap`/`libc` each have hits in
the expected files; `dirs`/`toml`/`serde` each return empty. ⇒ Cargo.toml is
correct (all-used, no-dead).

**(b) Build + test gate:**

```bash
cargo build            # expect: "Finished `dev` profile …" with 0 warnings
cargo test --lib 2>&1 | grep "test result"
                       # expect: "test result: ok. 72 passed; 0 failed; ..."
```

**(c) Drift-documentation presence check** (confirm the three architecture docs
carry the PRD §2 drift note):

```bash
grep -n "dirs\|libc\|drift\|PRD §2" \
  plan/002_bb250179a055/architecture/system_context.md
grep -n "dirs is a dead dependency\|Part B" \
  plan/002_bb250179a055/architecture/cargo_naming_convention.md
grep -n "Finding 5\|dep-list drift" \
  plan/002_bb250179a055/architecture/findings_and_risks.md
```

Expected: each doc has a hit (pre-verified this session). ⇒ The drift the contract
OUTPUT requires is already documented; this task adds the verification stamp.

**(d) README cross-check** (README's dependency section must not contradict
Cargo.toml):

```bash
sed -n '29,44p' README.md   # the "### Dependencies" section
```

Expected (pre-verified): the section lists **OS-level system libraries**
(`libhidapi-dev`, `libudev-dev`, `hidapi-devel`, `hidapi`, brew `hidapi`) — the
shared libs `hidapi-sys` links against. It is NOT a mirror of Cargo.toml's
`[dependencies]` (clap/hidapi/libc). ⇒ There is no crate-dep list in README to
drift against Cargo.toml; the README section is correct for its layer. (Finding
to record in the stamp: "README 'Dependencies' = system-lib layer, not crate
deps ⇒ no drift vs Cargo.toml.")

### The deliverable edit — the verification stamp in `system_context.md`

Append a new subsection at the END of the existing
`## PRD §2 Dependency-List Drift (documented, PRD is read-only)` section in
`plan/002_bb250179a055/architecture/system_context.md`. Suggested text (adapt the
date to today):

```markdown
### Verified (P1.M1.T3.S2 — YYYY-MM-DD)

Re-verified against the live tree:

- **Cargo.toml is correct** — all present dependencies are used by source,
  no dead dependencies remain:
  - `hidapi` → `src/error.rs:1` (`use hidapi::HidError`), `src/core.rs:2`
    (`use hidapi::{HidApi, HidDevice}`) + the `RawHid` trait/impl.
  - `clap` → `src/lib.rs:7` (`use clap::{Arg, ArgAction, ArgGroup, ArgMatches,
    Command}`) + `clap::value_parser!` macro uses.
  - `libc` → `src/main.rs:118` (`libc::signal(libc::SIGPIPE, libc::SIG_DFL)`),
    gated by the `[target.'cfg(unix)'.dependencies]` declaration.
  - `dirs` / `toml` / `serde` → **absent** from `Cargo.toml`, `Cargo.lock`, and
    all source (`grep` for `use dirs`/`use toml`/`use serde` returns empty).
- **Build & test:** `cargo build` → 0 warnings; `cargo test --lib` → 72 passed,
  0 failed (unchanged baseline).
- **README cross-check:** README.md "### Dependencies" (lines 29-44) lists
  OS-level *system* libraries (`libhidapi-dev`, `libudev-dev`, …) — the shared
  libs `hidapi-sys` links — NOT a mirror of `Cargo.toml`'s `[dependencies]`.
  No crate-dep list in README ⇒ no drift vs Cargo.toml; the section is correct
  for its layer.
- **PRD.md untouched** (read-only per orchestrator constraints); the §2 dep-list
  drift above stands as the authoritative record.
```

This is an **additive** edit to an existing architecture doc — it does not rewrite
the existing drift paragraphs, it appends a verified stamp beneath them.

### Success Criteria

- [ ] `cargo build` → 0 warnings; `cargo test --lib` → **72 passed; 0 failed**.
- [ ] `grep -rn "use hidapi\|hidapi::" src/` → hits in `error.rs` + `core.rs`;
      `grep -rn "use clap\|clap::" src/` → hits in `lib.rs`; `grep -rn "libc::"
      src/` → hit in `main.rs`.
- [ ] `grep -rn "use dirs\|dirs::" src/` → empty; same for `toml` and `serde`.
- [ ] The three architecture drift notes are present (`system_context.md`,
      `cargo_naming_convention.md` Part B, `findings_and_risks.md` Finding 5).
- [ ] `system_context.md` carries the new "### Verified (P1.M1.T3.S2 …)" stamp
      with the dep-usage evidence, build/test result, and README finding.
- [ ] README.md "### Dependencies" confirmed as the system-lib layer (no crate-dep
      mirror ⇒ no drift vs Cargo.toml).
- [ ] `git diff --name-only` ⇒ only
      `plan/002_bb250179a055/architecture/system_context.md` (outside this task's
      own `plan/.../P1M1T3S2/` artifacts). **NO `src/`, NO `PRD.md`, NO `Cargo.toml`,
      NO `Cargo.lock`** in the diff.

## All Needed Context

### Context Completeness Check

> _"If someone knew nothing about this codebase, would they have everything
> needed to implement this successfully?"_ — **Yes.** The exact repo (crate,
> hyphen — plus the firmware-cwd trap and its verify step), the exact grep
> commands with pre-verified expected outputs, the build/test baseline (72
> passed), the PRD §2 stale text quoted verbatim with its line numbers, the
> three architecture docs that already carry the drift note (cited by section),
> the README system-lib-layer finding, the ready-to-paste verification-stamp
> text, and the deterministic grep/git gates are all below. The implementer
> exercises one judgment per dependency — "is this dep used by source?" — and
> every answer is pre-verified.

### Documentation & References

```yaml
# MUST READ — the canonical drift record (the doc the item contract names; THIS
# task appends the verification stamp to its drift section).
- file: plan/002_bb250179a055/architecture/system_context.md
  why: "§'PRD §2 Dependency-List Drift (documented, PRD is read-only)' is the
        authoritative drift record. It already lists dirs/toml/serde removed +
        libc added + the action. This task APPENDS a '### Verified (P1.M1.T3.S2)'
        stamp beneath it (additive; do NOT rewrite the existing paragraphs)."
  section: "PRD §2 Dependency-List Drift (documented, PRD is read-only)"
  critical: "Append ONLY. The existing drift paragraphs are correct and complete;
             this task adds the verification record (grep evidence + build/test +
             README finding + 'PRD untouched'). Do not edit the existing text."

# MUST READ — the manifest under verification.
- file: Cargo.toml
  why: "The [dependencies] under audit: clap='4.5.31', hidapi='2.4.1' +
        [target.'cfg(unix)'.dependencies] libc='0.2'. This task VERIFIES it is
        correct (all-used, no-dead) — it does NOT edit it."
  pattern: "Two-block deps: top-level [dependencies] (clap, hidapi) +
            target-gated [target.'cfg(unix)'.dependencies] (libc)."
  gotcha: "Do NOT edit Cargo.toml. It is already correct (verified this session).
           Editing it would be a defect (the manifest matches actual source usage)."

# MUST READ — the PRD §2 stale text (the drift SOURCE; READ-ONLY — do not edit).
- file: PRD.md
  why: "§2 lines 101-103 carry the stale dep note: 'hidapi = 2.4.1, clap = 4.5,
        dirs = 5.0.1; toml/serde currently listed but unused.' Actual Cargo.toml
        has neither dirs nor toml/serde, and has libc (unmentioned). This is the
        drift to document — NOT to fix in PRD.md (read-only)."
  section: "2. Repository Layout & Deliverables — **Dependencies** note (lines 101-103)"
  critical: "PRD.md is READ-ONLY. Do NOT edit it. The drift is recorded in the
             architecture docs instead (system_context.md + the stamp this task adds)."

# MUST READ — the two other architecture docs that ALSO carry the drift (confirm
# they are present + complete; this task does NOT need to edit them — system_context
# is the canonical record).
- file: plan/002_bb250179a055/architecture/cargo_naming_convention.md
  why: "§'Part B — dirs is a dead dependency (PRD §2 is stale)' has the exact
        Cargo.toml state + per-dep findings + action. Confirm it is present
        (grep). No edit needed (it is already complete)."
  section: "Part B — dirs is a dead dependency (PRD §2 is stale)"
- file: plan/002_bb250179a055/architecture/findings_and_risks.md
  why: "Finding 5 'PRD §2 dependency list has drift (DOCUMENT ONLY)' (lines 59-66)
        + the risk-register row (line 73) + lines 18-21. Confirm present (grep).
        Line 97 maps P1.M1.T3.S2 to 'Mode B docs sync: document PRD §2 dep-list
        drift'. No edit needed."
  section: "Finding 5", "Risk register (PRD §2 dep-list drift row)"

# MUST READ — the README cross-check target.
- file: README.md
  why: "'### Dependencies' (lines 29-44) lists OS-level system libraries
        (libhidapi-dev, libudev-dev, hidapi-devel, hidapi, brew hidapi) — the
        shared libs hidapi-sys links. It is NOT a mirror of Cargo.toml's
        [dependencies]. Line 148 mentions 'the hidapi crate' in passing. No
        crate-dep list ⇒ no drift vs Cargo.toml."
  gotcha: "Do NOT 'fix' the README Dependencies section to list clap/libc — it
           intentionally documents the SYSTEM-lib build prerequisites, a different
           layer. There is no crate-dep list in README to sync."

# MUST READ — the source files that use each dependency (the verification evidence).
- file: src/error.rs
  why: "Line 1: `use hidapi::HidError;` — proves hidapi is used (the From<HidError>
        impl + error variants wrap it)."
- file: src/core.rs
  why: "Line 2: `use hidapi::{HidApi, HidDevice};` + the `RawHid` trait/impl
        delegate to `hidapi::HidDevice` — proves hidapi is the HID transport."
- file: src/lib.rs
  why: "Line 7: `use clap::{Arg, ArgAction, ArgGroup, ArgMatches, Command};` +
        `clap::value_parser!` (lines 173,181,189,197) — proves clap drives the CLI."
- file: src/main.rs
  why: "Line 118: `libc::signal(libc::SIGPIPE, libc::SIG_DFL);` inside the `unsafe`
        SIGPIPE reset (gated by the cfg(unix) target dep) — proves libc is used."

# REFERENCE — the sibling naming gate (Mode B twin; scope boundary).
- file: plan/002_bb250179a055/P1M1T3S1/PRP.md
  why: "T3.S1 = the NAMING grep gate (qmk[_-]notifier tokens). Its 'scope boundary
        vs T3.S2' explicitly says: 'T3.S2 = Cargo.toml [dependencies]
        version/package drift vs PRD §2. Do NOT audit dependency versions here.'
        This task (T3.S2) is that dependency gate. Confirm no overlap: T3.S1
        touched no files this task touches (it expected an empty diff)."
  critical: "T3.S1 and T3.S2 are independent gates (naming vs deps). T3.S1 edits
             nothing (expected); T3.S2 edits only system_context.md (the stamp).
             Neither edits src/, PRD.md, or Cargo.toml."

# REFERENCE — research notes (verified evidence + per-dep map + deliverable design).
- docfile: plan/002_bb250179a055/P1M1T3S2/research/notes.md
  why: "Documents the verified working-tree state (Cargo.toml correct, 72 tests
        pass, all deps used, no dead deps), the PRD §2 stale text, the three
        drift docs, the README system-lib-layer finding, and the deliverable
        design (verification + stamp)."
```

### Current Codebase tree (verified this session; cwd = the HYPHEN crate)

```bash
/home/dustin/projects/qmk-notifier/        # <-- THIS task's repo (Rust crate, hyphen)
├── Cargo.toml          # clap 4.5.31, hidapi 2.4.1, libc 0.2 (cfg unix) ← VERIFIED (no edit)
├── Cargo.lock          # top-level: clap, hidapi, libc only ← no dirs/toml/serde
├── README.md           # "### Dependencies" = OS system libs (lines 29-44) ← cross-check
├── PRD.md              # §2 lines 101-103 = STALE dep note ← READ-ONLY (do not edit)
├── .gitignore
├── LICENSE
└── src
    ├── main.rs         # :118 libc::signal(SIGPIPE, SIG_DFL) ← libc usage evidence
    ├── core.rs         # :2 use hidapi::{HidApi,HidDevice} ← hidapi usage evidence
    ├── error.rs        # :1 use hidapi::HidError ← hidapi usage evidence
    └── lib.rs          # :7 use clap::{...} ← clap usage evidence
```

> NOTE: the harness session cwd may report `/home/dustin/projects/qmk_notifier`
> (UNDERSCORE) — that is a DIFFERENT repo, the qmk **firmware** C project
> (`notifier.c`, `pattern_match.c`, `qmk_stubs/`, no `Cargo.toml`, no `src/`). It has
> its OWN README.md/PRD.md which are out of scope. All work for THIS task is in the
> hyphen crate above. The `read` tool anchors to the firmware cwd, so use ABSOLUTE
> paths (`/home/dustin/projects/qmk-notifier/...`) or `cd` in bash. Verify with
> `head -2 Cargo.toml` ⇒ `name = "qmk-notifier"`.

### Desired Codebase tree with files to be added/modified

```bash
# The ONLY non-plan/ file modified:
plan/002_bb250179a055/architecture/system_context.md   # APPEND the "Verified (P1.M1.T3.S2)" stamp
                                                        #   to the existing drift section.
# Everything else UNCHANGED:
Cargo.toml    # unchanged (verified correct — do NOT edit)
Cargo.lock    # unchanged
README.md     # unchanged (system-lib layer; correct for its layer)
PRD.md        # unchanged (READ-ONLY)
src/*.rs      # unchanged (NOT this task's scope)
```

> No new files outside `plan/` (other than the PRP/research under
> `plan/.../P1M1T3S2/`, which is this task's own output). The only edit is the
> additive stamp in `system_context.md`.

### Known Gotchas of our codebase & Library Quirks

```text
CRITICAL — REPO LOCATION: this task's repo is /home/dustin/projects/qmk-notifier
  (HYPHEN). The harness session cwd /home/dustin/projects/qmk_notifier (UNDERSCORE)
  is the firmware C repo — a different project with its OWN README.md/PRD.md (the
  firmware spec) and NO Cargo.toml/src/. cd to the hyphen crate before any command.
  Verify: `head -2 Cargo.toml` => `name = "qmk-notifier"`. The `read` tool anchors
  to the firmware cwd — use absolute paths or `cd` in bash. (Same trap as T3.S1/T2.S1.)

CRITICAL — PRD.md is READ-ONLY. Do NOT edit PRD.md to "fix" the stale §2 dep note.
  The drift (dirs/toml/serde claimed present but absent; libc omitted) is documented
  in the architecture docs (system_context.md + cargo_naming_convention.md Part B +
  findings_and_risks.md Finding 5) and this task adds a verification stamp to
  system_context.md. Editing PRD.md would violate orchestrator constraints.

CRITICAL — Cargo.toml is ALREADY CORRECT; do NOT edit it. This task VERIFIES the
  manifest (all deps used, no dead deps), it does not change it. The present deps
  (clap/hidapi/libc) all have source usage; the absent deps (dirs/toml/serde) all
  have zero usage. Editing Cargo.toml would be a defect. `git diff -- Cargo.toml`
  must be EMPTY after this task.

CRITICAL — README's "### Dependencies" is the SYSTEM-LIB layer, not crate deps.
  It lists libhidapi-dev/libudev-dev/etc. (the shared libs hidapi-sys links). It is
  NOT a mirror of Cargo.toml's [dependencies]. Do NOT "sync" it to list clap/libc —
  that would be wrong (it documents build prerequisites, a different concern). There
  is no crate-dep list in README to drift. The cross-check's finding is "no drift;
  different layer".

CRITICAL — The stamp is ADDITIVE to system_context.md. Append a new "### Verified
  (P1.M1.T3.S2 — <date>)" subsection beneath the existing "## PRD §2 Dependency-List
  Drift" section. Do NOT rewrite or delete the existing drift paragraphs — they are
  correct and complete. This task only records the verification.

CRITICAL — SCOPE: dependencies only. Do NOT audit Cargo.toml's NAMING fields
  (name/repository/homepage = qmk-notifier) — that was T3.S1's gate. Do NOT audit
  Cargo.lock's pin versions. This task checks the [dependencies] correctness vs
  source usage + PRD §2 drift documentation only.

CRITICAL — Do NOT modify any src/*.rs file. This is a verification + architecture-
  doc task. The source files (error.rs/core.rs/lib.rs/main.rs) are the EVIDENCE this
  task greps; they are not edited.

NOTE — libc is TARGET-GATED (`[target.'cfg(unix)'.dependencies]`). It is used only
  on unix (main.rs SIGPIPE reset). On Windows/macOS the build still succeeds because
  libc isn't pulled. The dep-usage grep finds `libc::` in main.rs:118 regardless of
  host — that's the usage proof. (Do not "fix" the target gating; it is correct.)

NOTE — `serde` may appear as a TRANSITIVE dependency of clap/hidapi in Cargo.lock
  (clap pulls it via derive features on some configs). That is fine — the
  dead-dep check is about DIRECT [dependencies] in Cargo.toml, not transitive
  Cargo.lock entries. The grep for `use serde` in src/ is the proof serde is not
  DIRECTLY used by this crate's code (it isn't).

NOTE — the build/test gate is sanity-only for the DEPENDENCY claim (it proves the
  manifest compiles + the dep set is sufficient). It is NOT the primary gate (the
  dep-usage grep is). A green build confirms no used-but-undeclared dep; the grep
  confirms no declared-but-unused dep. Both together = "manifest is correct".
```

## Implementation Blueprint

### Data models and structure

No code, no types, no models. The "model" is the dependency-correctness contract:
*every direct dependency in `Cargo.toml` must have source usage; no absent dep may
have source usage.* The deliverable is a verification record (grep/build/test
output + the stamp in `system_context.md`), not a data structure.

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: LOCATE the correct repo + snapshot the baseline
  - VERIFY repo: `cd /home/dustin/projects/qmk-notifier && head -2 Cargo.toml`
          prints `name = "qmk-notifier"`. If it shows a C project or qmk_notifier,
          you are in the WRONG repo (firmware cwd) — cd to the hyphen crate.
  - SNAPSHOT the pre-task diff so you can prove the scope at the end:
          `git status --porcelain` (record; expect only plan/ artifacts untracked).
  - GOAL: confirm the working tree is the crate and capture the clean baseline.

Task 2: VERIFY every present dependency is used (dep-usage grep)
  - RUN (cwd = crate root):
          grep -rn "use hidapi\|hidapi::" src/    # expect error.rs:1, core.rs:2 (+RawHid impl)
          grep -rn "use clap\|clap::"   src/      # expect lib.rs:7 (+value_parser! uses)
          grep -rn "libc::"              src/      # expect main.rs:118
  - EXPECT: each grep returns ≥1 hit in the expected file. If ANY present dep has
          ZERO usage, STOP — that is a dead dependency (a real defect) and must be
          removed from Cargo.toml (but per the gotcha, Cargo.toml is verified
          correct this session, so this should not happen; if it does, surface it).
  - GOAL: prove clap/hidapi/libc are all used by source.

Task 3: VERIFY no dead dependencies remain (absent-dep grep)
  - RUN (cwd = crate root):
          grep -rn "use dirs\|dirs::"    src/      # expect: empty
          grep -rn "use toml\|toml::"    src/      # expect: empty
          grep -rn "use serde\|serde::\|#\[derive(.*Serialize" src/  # expect: empty
  - EXPECT: all three return empty. If ANY returns a hit, STOP — that dep is used
          but undeclared (a real defect); but per the gotcha this should not happen.
  - GOAL: prove dirs/toml/serde are genuinely unused (correctly absent from Cargo.toml).

Task 4: BUILD + TEST gate
  - RUN: `cargo build` (expect 0 warnings) and `cargo test --lib 2>&1 | grep
          "test result"` (expect "72 passed; 0 failed").
  - GOAL: prove the dependency set compiles and is sufficient (no
          used-but-undeclared dep) and the baseline is intact.

Task 5: CONFIRM the drift documentation is present in the architecture docs
  - RUN the three greps from "What (c)" against system_context.md,
          cargo_naming_convention.md, findings_and_risks.md. Expect a hit in each.
  - GOAL: confirm the PRD §2 drift the contract OUTPUT requires is already
          documented (it is — this task adds the verification stamp, not the
          drift note itself).

Task 6: README CROSS-CHECK
  - RUN: `sed -n '29,44p' README.md` (the "### Dependencies" section).
  - EXPECT: OS-level system libraries (libhidapi-dev, libudev-dev, hidapi-devel,
          hidapi, brew hidapi). NOT a crate-dep mirror.
  - GOAL: confirm README's dependency info does not contradict Cargo.toml (it
          documents a different layer; no drift). Record this finding in the stamp.

Task 7: APPEND the verification stamp to system_context.md (the ONE edit)
  - LOCATE the existing "## PRD §2 Dependency-List Drift (documented, PRD is
          read-only)" section in system_context.md (grep for it).
  - APPEND the "### Verified (P1.M1.T3.S2 — <today's date>)" subsection (text in
          "What / The deliverable edit") immediately AFTER the last paragraph of
          that section, filling in the dep-usage evidence (from Tasks 2-3), the
          build/test result (from Task 4), and the README finding (from Task 6).
  - DO NOT: rewrite or delete the existing drift paragraphs; edit PRD.md; edit
          Cargo.toml/Cargo.lock; edit src/*.rs; edit README.md; edit the other
          two architecture docs (they are complete).
  - GOAL: record the verification as a durable architecture artifact.

Task 8: VALIDATE (grep + git gates are the proof)
  - RUN the Level-1 dep-usage greps + the Level-4 git-diff scope check.
  - EXPECT: `git diff --name-only` (excluding this task's plan/ artifacts) lists
          ONLY plan/002_bb250179a055/architecture/system_context.md. NO src/,
          PRD.md, Cargo.toml, Cargo.lock, README.md.
```

### Implementation Patterns & Key Details

```text
// === THE DEP-CORRECTNESS CONTRACT (one line each) ===
//   present dep  ⇒ must have source usage  (grep "use <dep>\|<dep>::" src/  => ≥1 hit)
//   absent dep   ⇒ must have zero usage     (grep "use <dep>\|<dep>::" src/  => empty)
//   build        ⇒ compiles + 72 pass       (no used-but-undeclared, no declared-but-unused)


// === WHY CARGO.TOML IS NOT EDITED (it is already correct) ===
//   hidapi → error.rs:1 + core.rs:2 (HID transport).   clap → lib.rs:7 (CLI).
//   libc → main.rs:118 (SIGPIPE, cfg unix).            dirs/toml/serde → zero usage.
//   ⇒ manifest matches actual usage. Verifying (not editing) is the deliverable.


// === WHY PRD.md IS NOT EDITED (read-only) ===
//   PRD §2 (lines 101-103) is stale (claims dirs/toml/serde present, omits libc),
//   but PRD.md is owned by humans and read-only per orchestrator constraints. The
//   drift is recorded in architecture docs (system_context.md + the stamp) instead.


// === WHY README IS NOT EDITED (different layer) ===
//   README "### Dependencies" (lines 29-44) documents OS-level SYSTEM libraries
//   (libhidapi-dev, libudev-dev) — the shared libs hidapi-sys links. It is NOT a
//   crate-dep list. There is nothing to "sync" to Cargo.toml; the section is
//   correct for its layer. The finding to record: "no drift; different layer".


// === THE STAMP IS ADDITIVE (system_context.md) ===
//   Append "### Verified (P1.M1.T3.S2 — <date>)" beneath the existing drift
//   section. Keep the existing paragraphs intact. The stamp records: dep-usage
//   evidence (per-dep file:line), build/test result (72/0), README finding
//   (system-lib layer), and "PRD.md untouched (read-only)".
```

### Integration Points

```yaml
VERIFICATION TARGET FILES (cwd = /home/dustin/projects/qmk-notifier):
  - verify: "Cargo.toml  — [dependencies] correctness (clap/hidapi/libc all used)"
  - verify: "src/*.rs    — dep-usage evidence (error.rs/core.rs/lib.rs/main.rs)"
  - verify: "README.md   — '### Dependencies' = system-lib layer (no crate-dep drift)"
  - verify: "PRD.md      — §2 lines 101-103 stale dep note (READ-ONLY, do not edit)"

DELIVERABLE EDIT (the ONE non-plan/ file modified):
  - modify: "plan/002_bb250179a055/architecture/system_context.md — APPEND the
             '### Verified (P1.M1.T3.S2)' stamp to the existing drift section."

NO OTHER EDITS:
  - Cargo.toml:  "UNCHANGED (verified correct — do NOT edit)."
  - Cargo.lock:  "UNCHANGED."
  - README.md:   "UNCHANGED (system-lib layer; correct for its layer)."
  - PRD.md:      "UNCHANGED (READ-ONLY — drift documented in architecture docs)."
  - src/*.rs:    "UNCHANGED (the evidence, not the edit target)."
  - other arch docs: "cargo_naming_convention.md + findings_and_risks.md UNCHANGED
                      (already complete; this task only confirms presence)."

SCOPE BOUNDARY vs P1.M1.T3.S1 (sibling naming gate):
  - "T3.S1 = naming grep gate (qmk[_-]notifier tokens; expected empty diff). T3.S2 =
     Cargo.toml [dependencies] correctness + PRD §2 drift documentation stamp. No
     overlap: T3.S1 touches nothing; T3.S2 touches only system_context.md."

DEPENDENCY ON T1/T2 (soft / ordering, NOT data-flow):
  - "T1.S1/T1.S2 (Complete) + T2.S1 (parallel) edit src/*.rs only (comment/string
     cosmetic naming). They do NOT touch Cargo.toml, README.md, PRD.md, or the
     architecture docs, so this verification's result is independent of their landing."
```

## Validation Loop

### Level 1: The dependency-correctness gate (the primary gate)

```bash
cd /home/dustin/projects/qmk-notifier   # CRATE root (HYPHEN) — NOT the firmware cwd

# (a) Present deps — each MUST have source usage:
grep -rn "use hidapi\|hidapi::" src/ | tee /tmp/t3s2_hidapi.log   # expect: error.rs:1, core.rs:2 (+impl)
grep -rn "use clap\|clap::"   src/ | tee /tmp/t3s2_clap.log       # expect: lib.rs:7 (+value_parser!)
grep -rn "libc::"              src/ | tee /tmp/t3s2_libc.log       # expect: main.rs:118

# (b) Absent deps — each MUST have ZERO usage:
grep -rn "use dirs\|dirs::"    src/ ; echo "dirs: exit $?"        # expect: empty (exit 1)
grep -rn "use toml\|toml::"    src/ ; echo "toml: exit $?"        # expect: empty (exit 1)
grep -rn "use serde\|serde::\|#\[derive(.*Serialize" src/ ; echo "serde: exit $?"  # expect: empty (exit 1)

# (c) Build + test:
cargo build 2>&1 | tee /tmp/t3s2_build.log | tail -2              # expect: "Finished" + 0 warnings
cargo test --lib 2>&1 | grep "test result" | tee /tmp/t3s2_test.log  # expect: "72 passed; 0 failed"

# (d) Drift-doc presence (the three architecture docs):
grep -n "PRD §2 Dependency-List Drift" plan/002_bb250179a055/architecture/system_context.md   # expect: 1 hit
grep -n "Part B — \`dirs\` is a dead dependency" plan/002_bb250179a055/architecture/cargo_naming_convention.md  # expect: 1 hit
grep -n "Finding 5" plan/002_bb250179a055/architecture/findings_and_risks.md                   # expect: 1 hit

# (e) README cross-check:
sed -n '29,44p' README.md   # expect: OS-level system libs (libhidapi-dev, libudev-dev, ...)
```

### Level 2: Stamp-presence check (the deliverable edit)

```bash
cd /home/dustin/projects/qmk-notifier
# Confirm the verification stamp was appended to system_context.md:
grep -n "Verified (P1.M1.T3.S2" plan/002_bb250179a055/architecture/system_context.md
# Expected: exactly ONE hit — the new "### Verified (P1.M1.T3.S2 — <date>)" subsection.

# Confirm the existing drift paragraphs are INTACT (additive edit, not a rewrite):
grep -n "PRD §2 Dependency-List Drift (documented, PRD is read-only)" \
  plan/002_bb250179a055/architecture/system_context.md
# Expected: the original section header is still present (unchanged).
```

### Level 3: Build sanity (confirms the manifest compiles — already run in Level 1c)

```bash
cd /home/dustin/projects/qmk-notifier
cargo build 2>&1 | tail -2          # "Finished `dev` profile …" + 0 warnings
cargo test --lib 2>&1 | grep "test result"   # "72 passed; 0 failed"
# This is the dependency-sufficiency proof (no used-but-undeclared dep) + baseline check.
```

### Level 4: Gate-close scope cross-checks

```bash
cd /home/dustin/projects/qmk-notifier

# (a) The ONLY non-plan/ file modified is system_context.md:
git diff --name-only | grep -v '^plan/002_bb250179a055/P1M1T3S2/'
# Expected: exactly `plan/002_bb250179a055/architecture/system_context.md`.

# (b) No source / PRD / Cargo files touched:
git diff --name-only -- src/ PRD.md Cargo.toml Cargo.lock README.md
# Expected: EMPTY.

# (c) Cargo.toml is byte-identical to the baseline (verified correct, not edited):
git diff --stat -- Cargo.toml
# Expected: empty (no stat line).

# (d) PRD.md is untouched (read-only honored):
git diff --stat -- PRD.md
# Expected: empty.

# (e) Confirm the dep set one more time end-to-end:
grep -E '^(clap|hidapi|libc) = ' Cargo.toml
# Expected: clap = "4.5.31", hidapi = "2.4.1", libc = "0.2" (libc under cfg(unix)).
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1: present-dep greps (`hidapi`/`clap`/`libc`) each return ≥1 source hit.
- [ ] Level 1: absent-dep greps (`dirs`/`toml`/`serde`) each return empty.
- [ ] Level 1: `cargo build` → 0 warnings; `cargo test --lib` → **72 passed; 0 failed**.
- [ ] Level 1: the three architecture drift docs each have a hit (drift already documented).
- [ ] Level 1: README "### Dependencies" confirmed as the system-lib layer.
- [ ] Level 2: `system_context.md` carries the new "### Verified (P1.M1.T3.S2)" stamp;
      existing drift paragraphs intact.
- [ ] Level 4: `git diff --name-only` (excl. plan/.../P1M1T3S2/) ⇒ ONLY
      `plan/002_bb250179a055/architecture/system_context.md`.

### Feature Validation

- [ ] Cargo.toml confirmed correct: all deps used (hidapi/clap/libc), no dead deps
      (dirs/toml/serde absent).
- [ ] PRD §2 dep-list drift documented in `system_context.md` (existing) + the new
      verification stamp.
- [ ] README dependency info does not contradict Cargo.toml (system-lib layer).
- [ ] `cargo build` clean; `cargo test` 72 passed.
- [ ] PRD.md untouched (read-only honored).

### Code Quality Validation

- [ ] Gate run in the correct repo (`/home/dustin/projects/qmk-notifier`, hyphen).
- [ ] No `src/*.rs`, `PRD.md`, `Cargo.toml`, `Cargo.lock`, or `README.md` modified.
- [ ] The stamp is additive (existing `system_context.md` drift paragraphs unchanged).
- [ ] No naming-field audit performed (that was T3.S1); no version-pin audit.

### Documentation & Deployment

- [ ] Verification record (grep/build/test output + README finding) recorded in the
      `system_context.md` stamp.
- [ ] Mode B authorization respected: only `system_context.md` edited (additive stamp).
- [ ] No Cargo.toml / Cargo.lock / env / config change (manifest verified correct).

---

## Anti-Patterns to Avoid

- ❌ Don't run the gate in `/home/dustin/projects/qmk_notifier` (UNDERSCORE) — that
  is the firmware C repo with no `Cargo.toml`/`src/`. The gate's repo is
  `/home/dustin/projects/qmk-notifier` (HYPHEN). Verify with `head -2 Cargo.toml`.
- ❌ Don't edit **PRD.md** to "fix" the stale §2 dep note. PRD.md is READ-ONLY per
  orchestrator constraints. The drift is recorded in the architecture docs
  (`system_context.md` + the stamp this task adds) instead.
- ❌ Don't edit **Cargo.toml**. It is already correct (verified this session: all
  deps used, no dead deps). Editing it would be a defect. `git diff -- Cargo.toml`
  must be empty after this task.
- ❌ Don't edit **README.md** to "sync" its Dependencies section to list clap/libc.
  That section documents OS-level *system* libraries (libhidapi-dev, libudev-dev) —
  a different layer from Cargo.toml's `[dependencies]`. It is correct for its
  purpose; there is no crate-dep list in README to drift.
- ❌ Don't edit any **`src/*.rs`** file. This is a verification + architecture-doc
  task; the source files are the *evidence* (grep targets), not the edit targets.
- ❌ Don't rewrite the existing `system_context.md` drift paragraphs. The stamp is
  **additive** — append a new "### Verified (P1.M1.T3.S2)" subsection beneath the
  existing "## PRD §2 Dependency-List Drift" section. The existing text is correct
  and complete.
- ❌ Don't edit the other two architecture docs (`cargo_naming_convention.md`,
  `findings_and_risks.md`). They already carry the drift note (Part B / Finding 5)
  and are complete. This task only *confirms* their presence + adds the stamp to the
  canonical record (`system_context.md`).
- ❌ Don't confuse **transitive** Cargo.lock entries (e.g. serde pulled by clap) with
  **direct** `[dependencies]`. The dead-dep check is about direct deps in Cargo.toml;
  transitive deps in Cargo.lock are not this crate's concern. The proof a dep is
  unused is `grep "use <dep>" src/` returning empty, not its Cargo.lock absence.
- ❌ Don't audit Cargo.toml's **naming fields** (name/repository/homepage) — that was
  T3.S1's gate. Don't audit **version pins** in Cargo.lock. This task checks
  `[dependencies]` correctness vs source usage + PRD §2 drift documentation only.
- ❌ Don't "fix" the `libc` target gating (`[target.'cfg(unix)'.dependencies]`). It
  is correct — libc is used only on unix (main.rs SIGPIPE reset). On Windows/macOS
  the build succeeds without it.
- ❌ Don't treat the verification as "nothing to do." The deliverable is the
  verification record (the grep/build/test proof) + the durable stamp in
  `system_context.md`. Running the gate and recording the result IS the task.

---

**Confidence Score: 10/10** for one-pass implementation success. The deliverable is
a deterministic verification gate (dep-usage grep + `cargo build` + `cargo test`) +
one additive documentation stamp, every expected output of which was pre-verified
this session: `Cargo.toml` is correct (hidapi→error.rs/core.rs, clap→lib.rs,
libc→main.rs:118; dirs/toml/serde all absent from source), `cargo build` is clean,
`cargo test --lib` reports 72 passed, the PRD §2 drift is already documented in
three architecture docs, and README's "Dependencies" section is the OS system-lib
layer (no crate-dep drift). The only real hazards — working in the wrong (firmware)
repo, editing the read-only PRD.md, editing the already-correct Cargo.toml/Cargo.lock,
"fixing" the README's system-lib section, or rewriting (instead of appending to)
the existing `system_context.md` drift paragraphs — are each called out with a
verify step or an explicit anti-pattern. The ready-to-paste stamp text and the
exact grep/git gates make the implementer's path mechanical. An implementer who
follows the blueprint produces exactly one additive edit to `system_context.md`
with a verified dep record, 72 tests still green, and no `src/`/`PRD.md`/`Cargo.toml`
in the diff.