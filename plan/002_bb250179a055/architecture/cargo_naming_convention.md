# Cargo Naming Convention & Dead-Dependency Check

**Source:** researcher agent research brief
**Date:** Session 002

## Part A — Cargo's hyphen→underscore crate-name convention

**Confirmed:** `use qmk_notifier::` is correct for the package `qmk-notifier`.
Cargo replaces every `-` with `_` to form the default library crate name (Rust
identifiers cannot contain `-`).

### Evidence
1. `src/main.rs:1` uses `use qmk_notifier::{parse_cli_args, run, ...}` and
   compiles against `name = "qmk-notifier"` in `Cargo.toml`.
2. **README** (Programmatic Usage, ~line 104): "the library identifier is
   `qmk_notifier` (Cargo derives `_` from the package's `-`), so the import is
   `use qmk_notifier::`".
3. **PRD.md** §1.2 (~lines 77–78): "The crate's Rust *library* identifier is
   also `qmk_notifier` — Cargo auto-derives `_` from the package's `-` — so
   source reads `use qmk_notifier::`."

### Convention rules
- Each `-` in the package name becomes `_` in the library identifier.
- Multiple hyphens: `my-cool-crate` → `my_cool_crate`.
- Numbers adjacent to hyphens: `foo-2` → `foo_2` (untouched).
- Package names start with a letter, so the derived identifier is always valid.
- `[lib] name = "..."` can override the default (not set in this crate).
- The cross-repo dependency pin must use: `qmk_notifier = { package = "qmk-notifier", ... }`
  (dep-table key = library id underscore; `package` = Cargo.toml name hyphen).

## Part B — `dirs` is a dead dependency (PRD §2 is stale)

PRD §2 says: `hidapi = "2.4.1"` (HID I/O), `clap = "4.5"` (CLI),
`dirs = "5.0.1"` (home/config dirs). Also: "`toml`/`serde` are currently listed
but unused after config-file support was removed — they may be dropped."

### Actual Cargo.toml state
```toml
[dependencies]
clap = "4.5.31"
hidapi = "2.4.1"

[target.'cfg(unix)'.dependencies]
libc = "0.2"
```

### Findings
- **`dirs = "5.0.1"`** — NOT in Cargo.toml. NOT in Cargo.lock. Zero `use dirs::`
  in any source file. Dead — config-file support was removed (`--create-config`
  → `QmkError::RemovedFeature`). The PRD §2 mention is stale.
- **`toml` / `serde`** — NOT in Cargo.toml. NOT in Cargo.lock. Already removed
  entirely (they're not "currently listed" as PRD §2 claims). Stale.
- **`libc = "0.2"`** — Present in Cargo.toml (unix-only). Used by `src/main.rs`
  for the SIGPIPE disposition reset (`libc::signal`, `libc::SIGPIPE`,
  `libc::SIG_DFL`). NOT mentioned in PRD §2. Drift.
- **`hidapi = "2.4.1"`** — Present. Resolves to 2.6.6 in Cargo.lock (caret).
- **`clap = "4.5.31"`** — Present.

### Action
Document the drift; do NOT edit PRD.md (read-only). Cargo.toml is already
correct — no dead dependencies, all present dependencies are used by source.