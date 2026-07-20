# System Context: qmk-notifier Naming Reconciliation (Session 002)

## Overview

Session 002 is a **naming reconciliation delta** on the fully-implemented v0.3.0
crate. Session 001 built the complete typed-command transport (72 tests passing,
clean build). This session swaps the hyphen/underscore assignment between the Rust
crate and the firmware C module — **no functional change**.

## Current Codebase State (verified: session 002 start)

The crate is **production-ready at v0.3.0**:

| File | Lines | Status |
|---|---|---|
| `src/lib.rs` | 960 | ✅ Full public API: `RunCommand` (5 variants), `HostOs`, `CommandResponse`, `RunParameters`, `parse_cli_args()`, `run()` → `Result<CommandResponse, QmkError>`, `--query-info`/`--list-callbacks` CLI flags, 72 unit tests |
| `src/core.rs` | 1690 | ✅ Framing constants, `parse_hex_or_decimal`, `list_hid_devices`, `send_raw_report` (returns `Option<Vec<u8>>` reply), `burst_to_one` (reply capture + pre-send drain), device cache, `build_command_data`, `parse_reply`, `parse_typed_reply`, `RawHid` trait + `FakeHid` test double |
| `src/error.rs` | 88 | ✅ `QmkError` (11 variants), `Display`, `Error`, `From<HidError>` |
| `src/main.rs` | 110 | ✅ CLI entry: SIGPIPE reset (`libc` crate), `--list-callbacks` sweep, `parse_cli_args()` → `run()` |
| `Cargo.toml` | 13 | ✅ `name = "qmk-notifier"`, deps: `hidapi 2.4.1`, `clap 4.5.31`, `libc 0.2` (unix) |
| `README.md` | 198 | ✅ Updated: hyphenated crate, underscored firmware, library-id clarification |
| `PRD.md` | 478 | ✅ Updated: §1.2 naming hazard, §2 repo layout, §13 cross-repo links |

**Build & test:** `cargo build` clean; `cargo test` → 72 passed, 0 failed.

## What Already Matches the Target State (verified by naming audit)

- **Cargo.toml**: `name = "qmk-notifier"`, `repository`/`homepage` →
  `dabstractor/qmk-notifier` ✓
- **README.md**: all ecosystem references correctly hyphenated/underscored;
  library-identifier clarification present ✓
- **PRD.md**: §1.2 naming hazard, §2, §13 all consistent ✓
- **Cargo.lock**: `name = "qmk-notifier"` (auto-generated) ✓
- **`use qmk_notifier::` imports** in `main.rs`, `lib.rs`: correct (Cargo-derived
  library identifier — invariant) ✓

## What's Outstanding (3 must-fix, 11 optional — see naming_audit.md)

### Must-fix (YES) — 3 comment-only edits, zero behavior risk

1. **`src/core.rs:129`** — `qmk-notifier commit \`01a51935\`` →
   `qmk_notifier commit \`01a51935\``. Commit `01a51935` is a **firmware** fix
   (PRD.md:210 writes it as `qmk_notifier`). Currently credits the Rust crate.
2. **`src/core.rs:425`** — identical fix, same commit reference.
3. **`src/main.rs:88`** — `qmk_notifier --list | head -1` →
   `qmk-notifier --list | head -1`. Binary invocation example uses firmware
   underscore name instead of the binary `qmk-notifier`. (Found by naming audit;
   not flagged in delta_prd.md.)

### Optional cosmetic (OPTIONAL) — 11 sites in `src/lib.rs`

- **lib.rs:154** — doc comment `for qmk_notifier` — ambiguous lib-id/binary ref.
- **10 test argv[0] strings** (lines 876, 887, 899, 912, 915, 924, 925, 926, 927,
  944) — clap derives `bin_name` from argv[0]; cosmetic-only for test diagnostics.

### Do NOT change

- All Cargo.toml / Cargo.lock / README / PRD references are correct for their
  classification.
- `use qmk_notifier::` / `mod qmk_notifier` — invariant (Cargo-derived library id).

## Ecosystem Naming Contract

| Artifact | Name | Repo |
|---|---|---|
| **This Rust crate** (package + repo + binary) | `qmk-notifier` (hyphen) | `dabstractor/qmk-notifier` |
| **Rust library identifier** | `qmk_notifier` (underscore) | Cargo auto-derives `_` from `-` |
| **Firmware C module** | `qmk_notifier` (underscore) | `dabstractor/qmk_notifier` |
| **Desktop daemon** | `qmkonnect` | `dabstractor/qmkonnect` |

**Key cross-reference:** firmware commit `01a51935` (RAW reply size fix 30→32)
lives in `dabstractor/qmk_notifier` (firmware repo), NOT
`dabstractor/qmk-notifier` (this Rust repo). Source comments attributing it to
`qmk-notifier` (hyphen) are bugs.

## PRD §2 Dependency-List Drift (documented, PRD is read-only)

PRD §2 says: `hidapi = "2.4.1"`, `clap = "4.5"`, `dirs = "5.0.1"`, and
"`toml`/`serde` currently listed but unused."

Actual Cargo.toml: `hidapi = "2.4.1"`, `clap = "4.5.31"`, `libc = "0.2"` (unix).
No `dirs`, no `toml`, no `serde`. The PRD §2 text is stale:
- `dirs` was removed when config-file support was dropped (commit `64d5f74`).
- `toml`/`serde` were removed entirely (they're not "currently listed").
- `libc` was added for the SIGPIPE fix (not mentioned in PRD §2).

**Action:** Document the drift; do not edit PRD.md (read-only per constraints).
The Cargo.toml is already correct.