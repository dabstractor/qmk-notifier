# Research Notes — P1.M1.T3.S2 (Verify Cargo.toml dep list vs PRD §2; document drift)

## Task nature

This is a **verification + documentation-stamp** task, NOT a code-change task.
The Cargo.toml is already correct; the PRD §2 drift is already documented in THREE
architecture docs. The deliverable is: (1) verify Cargo.toml correctness via
source-usage grep + `cargo build` + `cargo test`; (2) confirm the drift docs are
present + complete; (3) cross-check README.md's dependency section; (4) record the
verification (a small additive stamp to `system_context.md`). **No PRD.md edit**
(read-only). No source-code edit.

## Repo location (CRITICAL — same trap as T3.S1 / T2.S1)

- This task's repo is **`/home/dustin/projects/qmk-notifier`** (HYPHEN) — the Rust crate.
- The harness session cwd **`/home/dustin/projects/qmk_notifier`** (UNDERSCORE) is the
  **firmware C repo** (`notifier.c`, `pattern_match.c`, `qmk_stubs/`, NO `Cargo.toml`,
  NO `src/`). It has its OWN README.md/PRD.md — OUT OF SCOPE.
- Verify: `cd /home/dustin/projects/qmk-notifier && head -2 Cargo.toml` ⇒
  `name = "qmk-notifier"`. The `read` tool anchors to the firmware cwd, so use
  **absolute paths** (`/home/dustin/projects/qmk-notifier/...`) or `cd` in bash.

## Verified: Cargo.toml is correct (all deps used, no dead deps) — this session

`Cargo.toml`:
```toml
[dependencies]
clap = "4.5.31"
hidapi = "2.4.1"

[target.'cfg(unix)'.dependencies]
libc = "0.2"
```

Source-usage grep (run this session):
- **hidapi** — `src/error.rs:1` (`use hidapi::HidError`), `src/core.rs:2`
  (`use hidapi::{HidApi, HidDevice}`) + the `RawHid` trait/impl. ✅ USED.
- **clap** — `src/lib.rs:7` (`use clap::{Arg, ArgAction, ArgGroup, ArgMatches, Command}`)
  + `clap::value_parser!` (lib.rs:173,181,189,197). ✅ USED.
- **libc** — `src/main.rs:118` (`libc::signal(libc::SIGPIPE, libc::SIG_DFL)`) inside
  the `unsafe` SIGPIPE reset, gated by the `cfg(unix)` target dep. ✅ USED (unix-only).
- **dirs** — NO `use dirs` / `dirs::` anywhere. ✅ correctly ABSENT.
- **toml** — NO `use toml` / `toml::` anywhere. ✅ correctly ABSENT.
- **serde** — NO `use serde` / `serde::` / `#[derive(...Serialize...)` anywhere. ✅ ABSENT.

Cargo.lock top-level deps (grep `^name = "(dirs|toml|serde|libc|clap|hidapi)"`):
only `clap`, `hidapi`, `libc` match ⇒ dirs/toml/serde are not even transitive
direct deps. (clap 4.x default-features don't pull serde here.)

⇒ Cargo.toml has NO dead dependencies and ALL present dependencies are used.
The Cargo.toml is correct — no edit needed.

## Verified: build + test baseline — this session

- `cargo build` → `Finished dev profile ...` **0 warnings** (cached; 0.01s).
- `cargo test --lib` → **`72 passed; 0 failed; 0 ignored`** (46 in core.rs + 26 in lib.rs).
  This matches the system_context.md baseline ("72 tests passing, clean build").

## Verified: PRD §2 drift text (READ-ONLY — do not edit)

`PRD.md` lines 101-103 (§2 "Repository Layout & Deliverables", **Dependencies** note):
```
**Dependencies** (`Cargo.toml`): `hidapi = "2.4.1"` (HID I/O), `clap = "4.5"`
(CLI), `dirs = "5.0.1"` (home/config dirs). (`toml`/`serde` are currently listed
but unused after config-file support was removed — they may be dropped.)
```
Stale vs actual Cargo.toml:
- Claims `dirs = "5.0.1"` present ⇒ ACTUAL: absent (config-file support dropped;
  `--create-config` → `QmkError::RemovedFeature`).
- Claims `toml`/`serde` "currently listed but unused" ⇒ ACTUAL: already removed
  entirely (not in Cargo.toml at all).
- Omits `libc = "0.2"` (added for the SIGPIPE disposition reset in main.rs).
- `clap = "4.5"` ⇒ ACTUAL `4.5.31` (caret-compatible, minor wording drift).
PRD.md is **READ-ONLY** per orchestrator constraints ⇒ document the drift, do NOT edit.

## Verified: drift ALREADY documented in THREE architecture docs

1. **`plan/002_bb250179a055/architecture/system_context.md`** §
   "PRD §2 Dependency-List Drift (documented, PRD is read-only)" — the canonical
   drift record the item contract names. Lists dirs/toml/serde removed + libc added
   + the action ("Document the drift; do not edit PRD.md. Cargo.toml is already correct.").
2. **`.../architecture/cargo_naming_convention.md`** § "Part B — `dirs` is a dead
   dependency (PRD §2 is stale)" — exact Cargo.toml state + per-dep findings + action.
3. **`.../architecture/findings_and_risks.md`** — Finding 5 "PRD §2 dependency list
   has drift (DOCUMENT ONLY)" (lines 59-66) + the risk-register row (line 73:
   "Low | Documented in system_context.md; PRD is read-only. Cargo.toml is correct.")
   + the naming-audit drift note (lines 18-21). Also line 97 maps P1.M1.T3.S2 to
   "Mode B docs sync: document PRD §2 dep-list drift".

⇒ The drift documentation the contract OUTPUT requires **already exists**. This
task's net new work is the VERIFICATION (build/test/grep) + recording the
verification result (a stamp in system_context.md) + the README cross-check.

## Verified: README.md dependency section — different layer, no drift

`README.md` "### Dependencies" (lines 29-44) lists **OS-level SYSTEM libraries**
needed to build `hidapi-sys`:
```
sudo apt install libhidapi-dev libudev-dev    # Debian/Ubuntu
sudo dnf install hidapi-devel                 # Fedora
sudo pacman -S hidapi                         # Arch
brew install hidapi                           # macOS
```
This is the **system-dependency layer** (the shared libs hidapi-sys links against),
NOT a mirror of Cargo.toml's `[dependencies]` (clap/hidapi/libc). README does NOT
list the crate dependencies at all (line 148 only mentions "the `hidapi` crate" in
passing under "How it works"). ⇒ There is **no crate-dep list in README to drift**
against Cargo.toml. The README section is correct for its layer (system libs).
Finding to record: README's "Dependencies" = system libs, not crate deps ⇒ no drift.

## Deliverable design (the verification + stamp)

This task mirrors T3.S1's pattern (a verification gate whose record converts a doc
claim into a freshly verified fact). Concretely:

1. **Verification gate** (the proof): run the source-usage grep (deps + dead-deps),
   `cargo build`, `cargo test --lib`. Confirm Cargo.toml correct + 72 pass.
2. **Drift-doc confirmation**: confirm the three architecture docs' drift notes are
   present and complete (grep each for the dirs/libc drift).
3. **README cross-check**: confirm README's "Dependencies" is the system-lib layer
   (no crate-dep mirror ⇒ no drift).
4. **Verification stamp** (the ONE additive edit): append a
   `### Verified (P1.M1.T3.S2 — <date>)` subsection to `system_context.md`'s existing
   "PRD §2 Dependency-List Drift" section recording: the dep-usage grep evidence,
   `cargo build` clean / `cargo test --lib` 72 passed, the README system-lib finding,
   and "PRD.md untouched (read-only)". This is an architecture-doc edit (permitted —
   NOT a source file, NOT PRD.md). It is the durable record the milestone closes on.
5. **No PRD.md edit.** No source-code edit. No Cargo.toml edit (already correct).

## Validation expectations

- `cargo build` → 0 warnings; `cargo test --lib` → **72 passed; 0 failed** (unchanged).
- `grep -rn "use dirs\|dirs::" src/` → empty; same for toml/serde.
- `grep -rn "use hidapi\|hidapi::" src/` → hits (error.rs, core.rs);
  `grep -rn "use clap\|clap::" src/` → hits (lib.rs);
  `grep -rn "libc::" src/` → hit (main.rs:118).
- `git diff --name-only` ⇒ exactly `plan/002_bb250179a055/architecture/system_context.md`
  (the stamp) + the plan/ PRP/research artifacts (this task's own output). NO src/,
  NO PRD.md, NO Cargo.toml in the diff.