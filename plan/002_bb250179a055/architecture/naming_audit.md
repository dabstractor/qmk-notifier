# Naming Audit — `qmk-notifier` (Rust crate) vs `qmk_notifier` (firmware C module)

**Source:** scout agent comprehensive grep of `src/*.rs`, `Cargo.toml`, `Cargo.lock`, `README.md`, `PRD.md`
**Date:** Session 002

## Authority / naming contract
Source of truth: **PRD.md §3** (esp. lines 68–78) and **PRD.md:210**.

- `qmk-notifier` (hyphen) = **this Rust crate**: package name, repo
  `dabstractor/qmk-notifier`, binary `qmk-notifier`. Cargo auto-derives the
  *library identifier* `qmk_notifier` (underscore) from the package's hyphen.
- `qmk_notifier` (underscore) = **the firmware C module / repo**
  `dabstractor/qmk_notifier`. It owns the canonical wire protocol and is where
  commit `01a51935` lives.
- `use qmk_notifier::` / `mod qmk_notifier` = **Rust library identifier** —
  must stay underscore regardless of intent.

The **critical cross-reference** is commit `01a51935`: it is a **firmware**
fix. `PRD.md:210` writes it as `fixed in qmk_notifier commit \`01a51935\``.
Any code comment attributing `01a51935` to `qmk-notifier` (hyphen) is a bug.

## Classification legend
- **CRATE** → `qmk-notifier` (package / repo / binary). Target = hyphen.
- **FIRMWARE** → `qmk_notifier` (C module / repo). Target = underscore.
- **LIBRARY_ID** → `qmk_notifier` (Rust `use`/`mod`/dep-table key). Target = underscore.
- **ARGV0** → binary `qmk-notifier` (clap argv[0] / program name). Cosmetic.
- **CORRECT** → already matches the target form for its class.
- **Change?** = YES (real bug) / OPTIONAL (cosmetic) / NO (correct).

## Summary of actionable items
| Count | Change? | Items |
|------:|:-------:|-------|
|  2 | YES  | `core.rs:129`, `core.rs:425` — firmware commit mislabeled as crate |
|  1 | YES  | `main.rs:88` — binary-invocation example uses firmware underscore |
|  1 | OPTIONAL | `lib.rs:154` — doc comment subject ambiguous (lib vs binary) |
| 10 | OPTIONAL | `lib.rs` test argv[0] = `"qmk_notifier"` (clap cosmetic) |
| ~46 | NO | all remaining refs already correct |

---

## Full reference table

### `src/main.rs`
| file:line | exact text | class | target | change? | note |
|---|---|---|---|---|---|
| src/main.rs:1 | `use qmk_notifier::{parse_cli_args, run, CommandResponse, RunCommand, RunParameters};` | LIBRARY_ID | `qmk_notifier` | NO | Rust import; underscore required |
| src/main.rs:88 | `(e.g. \`qmk_notifier --list | head -1\`). This restores that behavior.` | CRATE (binary) | `qmk-notifier` | **YES** | Shell example invoking the **binary**; uses firmware underscore, misleading. Fix → `qmk-notifier --list | head -1` |

### `src/core.rs`
| file:line | exact text | class | target | change? | note |
|---|---|---|---|---|---|
| src/core.rs:129 | `/// sends a 32-byte reply per report (fixed in qmk-notifier commit \`01a51935\`,` | FIRMWARE | `qmk_notifier` | **YES** | Commit `01a51935` is a **firmware** fix (see PRD.md:210). Currently credits the crate. Fix → `qmk_notifier commit \`01a51935\`` |
| src/core.rs:425 | `    // valid 32-byte reply per report (qmk-notifier commit \`01a51935\`, which` | FIRMWARE | `qmk_notifier` | **YES** | Same commit, same mislabel. Fix → `qmk_notifier commit \`01a51935\`` |

### `src/lib.rs`
| file:line | exact text | class | target | change? | note |
|---|---|---|---|---|---|
| src/lib.rs:154 | `/// Build the clap \`Command\` for \`qmk_notifier\` (PRD §11 *CLI*).` | LIBRARY_ID/CRATE | `qmk_notifier` or `qmk-notifier` | OPTIONAL | Ambiguous: `qmk_notifier` is the lib id (valid), but the CLI is the binary `qmk-notifier`. Reads acceptably as lib; flip to `qmk-notifier` only if intending "the binary" |
| src/lib.rs:876 | `let params = cli_for(&["qmk_notifier", "--query-info"]);` | ARGV0 | `qmk-notifier` | OPTIONAL | clap derives bin_name from argv[0]; shows `qmk_notifier` in test errors. Cosmetic |
| src/lib.rs:887 | `let params = cli_for(&["qmk_notifier", "--list-callbacks"]);` | ARGV0 | `qmk-notifier` | OPTIONAL | same as above |
| src/lib.rs:899 | `            "qmk_notifier",` | ARGV0 | `qmk-notifier` | OPTIONAL | argv[0] inside device-flag test vector |
| src/lib.rs:912 | `let params = cli_for(&["qmk_notifier", "hello"]);` | ARGV0 | `qmk-notifier` | OPTIONAL | same |
| src/lib.rs:915 | `let params = cli_for(&["qmk_notifier", "--list"]);` | ARGV0 | `qmk-notifier` | OPTIONAL | same |
| src/lib.rs:924 | `            &["qmk_notifier", "--query-info", "msg"],` | ARGV0 | `qmk-notifier` | OPTIONAL | conflict-case vector |
| src/lib.rs:925 | `            &["qmk_notifier", "--query-info", "--list-callbacks"],` | ARGV0 | `qmk-notifier` | OPTIONAL | conflict-case vector |
| src/lib.rs:926 | `            &["qmk_notifier", "--list-callbacks", "msg"],` | ARGV0 | `qmk-notifier` | OPTIONAL | conflict-case vector |
| src/lib.rs:927 | `            &["qmk_notifier", "--list", "--query-info"],` | ARGV0 | `qmk-notifier` | OPTIONAL | conflict-case vector |
| src/lib.rs:944 | `            .try_get_matches_from(["qmk_notifier", "--verbose"])` | ARGV0 | `qmk-notifier` | OPTIONAL | same |

### `Cargo.toml`
| file:line | exact text | class | target | change? | note |
|---|---|---|---|---|---|
| Cargo.toml:2 | `name = "qmk-notifier"` | CRATE | `qmk-notifier` | NO | package name correct |
| Cargo.toml:7 | `repository = "https://github.com/dabstractor/qmk-notifier"` | CRATE | `qmk-notifier` | NO | repo URL correct |
| Cargo.toml:8 | `homepage = "https://github.com/dabstractor/qmk-notifier"` | CRATE | `qmk-notifier` | NO | homepage URL correct |

### `Cargo.lock`
| file:line | exact text | class | target | change? | note |
|---|---|---|---|---|---|
| Cargo.lock:148 | `name = "qmk-notifier"` | CRATE | `qmk-notifier` | NO | generated from Cargo.toml; never hand-edit |

### `README.md`
All references verified CORRECT: crate = `qmk-notifier` (hyphen), firmware = `qmk_notifier` (underscore), library id = `qmk_notifier` (underscore). The library-identifier clarification is present (line ~104).

### `PRD.md`
All references verified CORRECT. Authoritative source: PRD.md:210 writes `qmk_notifier commit \`01a51935\`` (underscore = firmware).

---

## Recommendations

### Must-fix (YES) — 3 sites, all comments, zero behavior risk
1. **src/core.rs:129** — `qmk-notifier commit \`01a51935\`` → `qmk_notifier commit \`01a51935\``.
2. **src/core.rs:425** — identical fix.
3. **src/main.rs:88** — `qmk_notifier --list | head -1` → `qmk-notifier --list | head -1`.

All three are comment-only edits → no compile/test impact.

### Optional (cosmetic) — 11 sites in src/lib.rs
- **lib.rs:154** doc-comment subject — leave as `qmk_notifier` (valid lib id) or
  change to `qmk-notifier` if the intent is "the binary's CLI".
- **lib.rs argv[0] test inputs (876, 887, 899, 912, 915, 924–927, 944)** — change
  to `"qmk-notifier"` for diagnostic fidelity. If changed, update all 10 consistently.

### Do NOT change
- All Cargo.toml / Cargo.lock / README / PRD references are correct for their class.
- `use qmk_notifier::` / `mod qmk_notifier` — invariant (Cargo-derived library identifier).