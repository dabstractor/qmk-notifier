# Findings & Risks — Session 002 (Naming Reconciliation)

## Research Summary

### Naming Audit (scout agent — full table in `naming_audit.md`)

The comprehensive audit grep'd every `qmk-notifier`, `qmk_notifier` reference
across `src/*.rs`, `Cargo.toml`, `Cargo.lock`, `README.md`, and `PRD.md`. Each
reference was classified by what it names (CRATE, FIRMWARE, LIBRARY_ID, ARGV0)
and whether it matches the target form.

**Result: 3 must-fix, 11 optional cosmetic, ~46 already correct.**

### Cargo Naming Convention (researcher agent — `cargo_naming_convention.md`)

- `use qmk_notifier::` is confirmed correct for package `qmk-notifier` (Cargo
  replaces `-` with `_` to form the default library crate name).
- `dirs = "5.0.1"` listed in PRD §2 is stale — already removed from Cargo.toml.
  PRD §2 also omits `libc = "0.2"` (added for SIGPIPE fix) and the
  `toml`/`serde` note is itself stale (already gone).
- **Action:** Document drift in `system_context.md`; PRD.md is read-only.

## Key Findings

### Finding 1: Two firmware-commit comment references are mislabeled (CONFIRMED)

`src/core.rs` lines ~129 and ~425 attribute commit `01a51935` to `qmk-notifier`
(hyphen = Rust crate). PRD.md:210 writes it as `qmk_notifier` (underscore =
firmware). The commit fixed the RAW reply size from 30 to 32 bytes — a firmware-
side change that lives in the firmware repo `dabstractor/qmk_notifier`.

**Impact:** A maintainer hunting commit `01a51935` in the wrong repo would fail
to find it. Correctness issue for code archaeology, not runtime behavior.

### Finding 2: main.rs binary-invocation comment uses firmware name (NEW — not in delta PRD)

`src/main.rs:88` shows `qmk_notifier --list | head -1` in the SIGPIPE comment.
The binary is `qmk-notifier` (hyphen). Using the firmware underscore name as a
CLI example is misleading.

**Impact:** Low. Comment-only; no runtime effect. But contradicts the naming
contract.

### Finding 3: lib.rs test argv[0] strings are cosmetic-only (OPTIONAL)

10 test sites use `"qmk_notifier"` as argv[0] for clap's `try_get_matches_from`.
clap derives the display `bin_name` from argv[0], so test failure output renders
`qmk_notifier` instead of the actual binary `qmk-notifier`. Changing these to
`"qmk-notifier"` improves diagnostic fidelity but changes no pass/fail outcomes.

**Impact:** None (cosmetic).

### Finding 4: lib.rs:154 doc comment is ambiguous but defensible

`/// Build the clap Command for qmk_notifier` — `qmk_notifier` is the valid
library identifier. The comment could also be read as "for the binary." Either
reading is defensible; changing it is optional.

### Finding 5: PRD §2 dependency list has drift (DOCUMENT ONLY)

PRD §2 lists `dirs = "5.0.1"` and says `toml`/`serde` are "currently listed but
unused." The actual Cargo.toml has neither `dirs` nor `toml`/`serde`, and it has
`libc = "0.2"` (not mentioned in PRD §2). The PRD text is stale.

**Action:** Document only. PRD.md is read-only per orchestrator constraints.
Cargo.toml is already correct.

## Risks

| Risk | Severity | Mitigation |
|---|---|---|
| Editting wrong reference class (e.g. changing `use qmk_notifier::` to hyphen) | **HIGH** — compile breakage | Context_scope explicitly lists which references are invariant (LIBRARY_ID class). Naming_audit.md table is the authority. |
| PRD §2 dep-list drift causes downstream confusion | Low | Documented in system_context.md; PRD is read-only. Cargo.toml is correct. |
| Cosmetic lib.rs edits break tests | Very low | argv[0] changes are string-only; clap `try_get_matches_from` ignores argv[0] for parsing logic. Run `cargo test` after any change. |

## Test Strategy

All changes in this session are **comment-only or cosmetic string edits** in
non-executable code paths. The existing 72-test suite is the complete regression
gate:

```bash
cargo build   # must remain clean
cargo test    # all 72 tests must pass
```

No new tests are needed — the changes affect no executable code paths.

## Estimated Effort

| Task | SP | Description |
|---|---|---|
| P1.M1.T1.S1 | 0.5 | Fix 2 core.rs firmware-commit comments |
| P1.M1.T1.S2 | 0.5 | Fix 1 main.rs binary-invocation comment |
| P1.M1.T2.S1 | 0.5 | Optional: align 11 lib.rs cosmetic naming sites |
| P1.M1.T3.S1 | 0.5 | Mode B docs sync: verify README/PRD consistency + grep gate |
| P1.M1.T3.S2 | 0.5 | Mode B docs sync: document PRD §2 dep-list drift |
| **Total** | **2.5** | |