# Research Notes — P1.M1.T3.S1: Final grep gate (no stale naming refs in README.md / PRD.md / Cargo.toml)

## Repo location (CRITICAL — get this right or the whole task is wrong)

This task targets the **Rust crate repo** at `/home/dustin/projects/qmk-notifier`
(HYPHEN). The harness session cwd `/home/dustin/projects/qmk_notifier` (UNDERSCORE)
is a DIFFERENT repo — the qmk **firmware** C project (`notifier.c`,
`pattern_match.c`, `qmk_stubs/`, no `Cargo.toml`, no `src/`). The prior T2.S1 PRP
calls this out explicitly in its "Known Gotchas" and "Current Codebase tree".

Verify before any work:
```bash
head -2 /home/dustin/projects/qmk-notifier/Cargo.toml   # -> name = "qmk-notifier"
```
The crate repo contains `Cargo.toml`, `src/{core,lib,main,error}.rs`, `README.md`,
`PRD.md`, and the plan/ tree (incl. `002_bb250179a055/`).

The plan dir `002_bb250179a055/` lives **inside the crate repo**
(`/home/dustin/projects/qmk-notifier/plan/002_bb250179a055/`), NOT in the firmware
cwd. The architecture doc `naming_audit.md`, the `prd_snapshot.md`, the `tasks.json`,
and sibling PRPs (P1M1T1S1 / P1M1T1S2 / P1M1T2S1) are all there.

## The naming contract (from architecture/naming_audit.md "Authority")

| Class | Token | Target form | Examples |
|---|---|---|---|
| CRATE (package/repo/binary) | `qmk-notifier` | **hyphen** | `name = "qmk-notifier"`, repo `dabstractor/qmk-notifier`, binary invocation `qmk-notifier --list` |
| FIRMWARE (C module/repo) | `qmk_notifier` | **underscore** | repo `dabstractor/qmk_notifier`, commit `qmk_notifier 01a51935`, `qmk_notifier/PRD.md` |
| LIBRARY_ID (Rust use/mod/dep-table key) | `qmk_notifier` | **underscore** (INVARIANT) | `use qmk_notifier::`, `qmk_notifier::CommandResponse`, dep key `qmk_notifier = { package = "qmk-notifier", … }` |
| DAEMON (qmkonnect) | `qmkonnect` / `QMKonnect` | as-is | `dabstractor/qmkonnect` |

The load-bearing cross-reference: firmware commit **`01a51935`** is a **FIRMWARE**
fix → must be written `qmk_notifier commit 01a51935` (underscore). PRD.md:210 is
the authoritative instance. (The two `core.rs` must-fix sites that mislabeled it as
`qmk-notifier` were fixed by P1.M1.T1.S1 — Complete — in source, NOT in docs.)

## What T1.S1 / T1.S2 / T2.S1 (the INPUTS) changed — and why it does NOT touch my files

| Subtask | File edited | What changed | Status |
|---|---|---|---|
| P1.M1.T1.S1 | `src/core.rs:129,425` | `qmk-notifier commit 01a51935` → `qmk_notifier commit …` | Complete |
| P1.M1.T1.S2 | `src/main.rs:88` | `qmk_notifier --list` → `qmk-notifier --list` (binary) | Complete |
| P1.M1.T2.S1 | `src/lib.rs` (10 argv[0] + 1 doc span) | `"qmk_notifier"` → `"qmk-notifier"` | Implementing |

**ALL THREE edit ONLY `src/*.rs`. NONE touch `README.md`, `PRD.md`, or `Cargo.toml`.**
Therefore this gate's result (README/PRD/Cargo naming correctness) is INDEPENDENT of
whether T2.S1 has landed. The docs were already correct before any source edit
(audit + live grep, below) and stay correct after. The "INPUT depends on T1/T2"
clause is a logical/milestone ordering, not a data-flow dependency.

## Live grep on the crate's README.md / PRD.md / Cargo.toml (VERIFIED this session)

Ran: `grep -rn 'qmk.notifier\|qmk_notifier\|qmk-notifier\|dabstractor/qmk' README.md PRD.md Cargo.toml`
(cwd = crate root). Every reference matches its class. Highlights:

### Cargo.toml — 3 refs, all CORRECT (CRATE → hyphen)
- `:2  name = "qmk-notifier"` ✓
- `:7  repository = "https://github.com/dabstractor/qmk-notifier"` ✓
- `:8  homepage  = "https://github.com/dabstractor/qmk-notifier"` ✓

### README.md — all CORRECT
- `:7,10,20,21,26,53,56,59,62,65,68` → `qmk-notifier` (crate/binary/repo) ✓
- `:12,173,196` → `[qmk_notifier](…/dabstractor/qmk_notifier)` (firmware) ✓
- `:104` → **library-id clarification PRESENT**: "the library identifier is
  `qmk_notifier` (Cargo derives `_` from the package's `-`), so the import is
  `use qmk_notifier::`" ✓ (this is exactly the item's README confirmation)
- `:107` → `use qmk_notifier::{RunParameters, RunCommand, HostOs, run};` ✓ (LIBRARY_ID)
- `:123,126` → `qmk_notifier::CommandResponse` ✓ (LIBRARY_ID)
- `:177` → "Include the qmk_notifier module" ✓ (firmware)

### PRD.md — all CORRECT
- `:1` → "SPECIFICATION — qmk-notifier (Rust Raw-HID transport crate)" ✓ (CRATE)
- `:8,16,46,48,85,86` → `qmk-notifier` ✓ (CRATE)
- `:12,20,21,166,210,344,445,476,477` → `qmk_notifier` / `dabstractor/qmk_notifier`
  / `qmk_notifier/PRD.md` ✓ (FIRMWARE) — **incl. `:210 fixed in qmk_notifier commit
  01a51935`** (the authoritative firmware-commit ref)
- `:68` table row: `qmk-notifier ← this repo | dabstractor/qmk-notifier | Rust` ✓ (CRATE)
- `:70` table row: `qmk_notifier | dabstractor/qmk_notifier | C` ✓ (FIRMWARE)
- `:73-78` → **naming-hazard note + library-id clarification PRESENT**: "the crate's
  Rust library identifier is also `qmk_notifier` — Cargo auto-derives `_` from the
  package's `-` — so source reads `use qmk_notifier::`" ✓ (item's PRD §1.2 confirmation)
- `:97-98` → "the library identifier `qmk_notifier`, and the binary `qmk-notifier`" ✓ (both)
- `:440-441` → **§13 qmkonnect Cargo.toml pin shows EXACTLY the required shape**:
  `qmk_notifier = { package = "qmk-notifier", git = "https://github.com/dabstractor/qmk-notifier", tag = "v0.3.0" }`
  ✓ (LIBRARY_ID dep key `qmk_notifier` + CRATE package/git `qmk-notifier` — both correct)

### Cross-wired repo-URL check (item clause: "no stale dabstractor/qmk_notifier
### where qmk-notifier is intended, and vice versa")
- Every crate/repo/binary URL → `dabstractor/qmk-notifier` ✓ (Cargo.toml:7,8;
  README:20; PRD:68,441). NONE use the firmware URL for the crate.
- Every firmware URL → `dabstractor/qmk_notifier` ✓ (README:12,173,196;
  PRD:12,70,166,210,344,445,476). NONE use the crate URL for the firmware.

## naming_audit.md classification for README/PRD/Cargo (the authoritative table)

naming_audit.md (the doc the item description references) states verbatim:
- "### README.md — All references verified CORRECT: crate = qmk-notifier (hyphen),
   firmware = qmk_notifier (underscore), library id = qmk_notifier (underscore).
   The library-identifier clarification is present (line ~104)."
- "### PRD.md — All references verified CORRECT. Authoritative source: PRD.md:210
   writes `qmk_notifier commit 01a51935` (underscore = firmware)."
- "### Cargo.toml — name/repo/homepage all CORRECT (NO change)."
- "### Do NOT change — All Cargo.toml / Cargo.lock / README / PRD references are
   correct for their class."
- The audit's only CHANGE items (2 YES in core.rs, 1 YES in main.rs, 11 OPTIONAL in
   lib.rs) are ALL in `src/*.rs` — ZERO in README/PRD/Cargo.

**Dual evidence (audit + this session's live grep): zero stale naming references in
README.md, PRD.md, or Cargo.toml.** The expected gate outcome is PASS with NO edits.

## Scope boundary vs P1.M1.T3.S2 (sibling — do NOT overlap)

- **T3.S1 (this task)** = naming-only grep gate on README.md / PRD.md / Cargo.toml
  (the `qmk[_-]notifier` token + the `dabstractor/qmk*` repo URLs + the library-id
  clarification + the §13 pin shape).
- **P1.M1.T3.S2 (sibling)** = "Verify Cargo.toml dependency list vs PRD §2 and
  document any drift" — i.e. the `[dependencies]` *versions/packages* (clap/hidapi
  vs PRD §2's dirs/toml/serde note). The live Cargo.toml shows `clap=4.5.31`,
  `hidapi=2.4.1`, plus `[target.'cfg(unix)'] libc=0.2` and NO dirs/toml/serde —
  that *drift* is T3.S2's finding to document, NOT this task's. T3.S1 checks ONLY
  the naming fields (name/repo/homepage = qmk-notifier); it must NOT opine on the
  dependency list.

## The library-identifier invariant (sanity check, not primary scope)

The contract says "library-id references (`use qmk_notifier::`) stay underscore."
Confirm the source-edit siblings did not corrupt the LIBRARY_ID:
```bash
grep -n "^use qmk_notifier::" src/main.rs src/core.rs   # expected: present, underscore
```
This is a belt-and-suspenders gate confirming T1/T2 left `use qmk_notifier::` intact
(their PRPs all promise this; verify it). It is NOT a README/PRD/Cargo edit.

## Validation strategy (docs-gate task — grep is the primary tool)

Because this is a documentation/naming verification task (not a code change), the
authoritative gates are GREP-based + a classification cross-check. A `cargo build`
is only a sanity check that the (already-landed) source edits compile and the
library-id invariant is intact; it is NOT the gate itself. Expected end state:
- README.md, PRD.md, Cargo.toml naming refs all match their class (gate PASSES).
- Zero edits required (dual evidence above). If a stale ref IS found (defying both
  the audit and this session's grep), the task is Mode-B-authorized to fix it in
  README.md / PRD.md — but this branch is not expected to trigger.
- `git diff --name-only` ⇒ empty (or, if an unexpected fix was needed, exactly the
  doc file fixed).

## Why confidence is 10/10

The deliverable is a deterministic grep + a classification cross-check against an
audit that ALREADY classified every README/PRD/Cargo reference as correct, confirmed
by an independent live grep this session. There is nothing to invent: the contract,
the classification table, the exact confirmations demanded (README:104 library-id
clarification; PRD §13 pin shape; no cross-wired repo URLs; PRD:210 firmware
commit), the repo-location gotcha, and the scope boundary with T3.S2 are all pinned.
The only judgment the implementer exercises is "does each grepped line match its
class?" — and the answer is pre-verified YES for every line.