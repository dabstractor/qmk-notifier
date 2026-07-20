# DELTA PRD — Crate Naming Reconciliation (session 002)

**Delta from:** session 001 PRD (v0.3.0 typed-command transport, crate named `qmk_notifier`)
**Delta to:** current PRD (v0.3.0, crate package named `qmk-notifier`)
**Scope:** naming convention only. **No functional change** — wire protocol, public
API, behavior, dependencies, and version (still `0.3.0`) are byte-for-byte
identical to session 001's deliverable.

---

## 1. What Changed (diff summary)

The two PRDs are identical except for the assignment of the hyphen vs underscore
name to the two ecosystem artifacts. The hyphen/underscore assignment has been
**swapped**:

| Artifact | Session 001 PRD (previous) | Current PRD (target) |
|---|---|---|
| **This Rust crate** (package + repo) | `qmk_notifier` (underscore) | `qmk-notifier` (hyphen) |
| **The firmware** (C module) | `qmk-notifier` (hyphen) | `qmk_notifier` (underscore) |
| **Rust library identifier** | `qmk_notifier` | `qmk_notifier` *(unchanged — Cargo derives `_` from `-`)* |

**Rationale:** adopt Cargo's hyphen convention for the package + repo name
(`qmk-notifier`), and let the firmware revert to underscore (`qmk_notifier`),
which matches the C/QMK identifier convention. The single *new* substantive
clarification is that the Rust **library identifier** is still `qmk_notifier`
because Cargo auto-derives `_` from the package `-` — so every `use qmk_notifier::`
line in source (this crate and `qmkonnect`) is **unchanged**.

Nothing in §3 (Public API), §4 (Wire Protocol), §6–§8 (cache / send / response),
§10 (typed-command transport), §11 (CLI semantics), §12 (NFRs), or §14
(Invariants) changed. This is a config + documentation change.

### Concrete change set

1. **`Cargo.toml`** — `name` `qmk_notifier` → `qmk-notifier`; `repository`/`homepage`
   URLs `dabstractor/qmk_notifier` → `dabstractor/qmk-notifier`. *(Version stays
   `0.3.0`; no `[lib]`/`[[bin]]` overrides are added — names stay Cargo-defaults.)*
2. **`PRD.md` + `README.md`** — swap every ecosystem reference; add the
   library-identifier clarification (PRD §1.2 "Naming hazard", §2 repo layout;
   README "Programmatic Usage" note).
3. **`src/core.rs`** *(residual fix)* — two comments (lines ~129, ~425) still call
   the firmware `qmk-notifier`; the current PRD §4.4 now calls it `qmk_notifier`.
   Align them.
4. **Cross-repo consumer** *(flag, not implementable here)* — `qmkonnect/Cargo.toml`
   dependency pin must change to `package = "qmk-notifier"`,
   `git = ".../qmk-notifier"`, `tag = "v0.3.0"`. Note for awareness; tracked in
   `qmkonnect`, not this repo.

### Optional cosmetic polish (not required for correctness)

- `src/lib.rs` test `argv[0]` strings and the `lib.rs:154` doc comment still use
  `qmk_notifier` as the clap program name; the actual binary is `qmk-notifier`.
  Updating these only affects help/usage text. Safe to align but non-blocking.

---

## 2. Prior Research / Completed Work to Leverage

- Session 001 fully implemented v0.3.0 (typed-command transport). **All of that
  work stands unchanged.** This delta touches only naming/config/docs.
- `plan/001_b92a9b2b603f/architecture/system_context.md` documents the
  `qmkonnect` dependency pin as `package = "qmk_notifier"` at tag `v0.2.1` — that
  snippet is now stale (consumer side) and is the source of the cross-repo note
  above. Do not re-research; reuse it.
- The working tree is already at the target state for items 1 and 2 (commit
  `9ee9a98` "Align docs with hyphenated crate naming" + its predecessor). The
  **only genuinely outstanding code edit** is item 3 (the two `src/core.rs`
  comments). The tasks below are written as the complete change set so the plan
  is self-contained; verify-then-fix each rather than assume.

---

## 3. Plan

### Phase P1 — Crate Naming Reconciliation

> One phase, one milestone, three tasks. Estimated ≤ 1 SP total. No hardware
> needed; `cargo build` + `cargo test` + `grep` are the only verification gates.

#### Milestone P1.M1 — Hyphenated crate, underscored firmware, consistent docs

**Task P1.M1.T1 — Cargo.toml: hyphenate package name + repo URLs**
- Change `name` to `qmk-notifier` (if not already), `repository`/`homepage` to
  `https://github.com/dabstractor/qmk-notifier`. Leave `version = "0.3.0"`.
- Verify `cargo build` and `cargo test` still pass (library identifier stays
  `qmk_notifier`, so no source edits flow from this).
- *Mode A docs:* none — config-only field; no doc file.

**Task P1.M1.T2 — Align firmware-name references in source**
- Fix the two `src/core.rs` comments (~129, ~425): `qmk-notifier commit 01a51935`
  → `qmk_notifier commit 01a51935`, matching PRD §4.4's current wording.
- *Optional polish:* update `src/lib.rs` test `argv[0]` / `lib.rs:154` doc comment
  program name to `qmk-notifier` for help-text accuracy. Non-blocking.
- *Mode A docs:* none — inline comment/test edits.

**Task P1.M1.T3 — Sync changeset-level documentation (Mode B)**
- Depends on T1 + T2. Sync `README.md` and `PRD.md` ecosystem references to the
  hyphenated crate / underscored firmware, and add the **library-identifier
  clarification** (PRD §1.2, §2; README Programmatic Usage: "import is
  `use qmk_notifier::` because Cargo derives `_` from `-`").
- Include the `qmkonnect/Cargo.toml` consumer-pin example as
  `{ package = "qmk-notifier", git = ".../qmk-notifier", tag = "v0.3.0" }`
  (PRD §13), and note it as a cross-repo coordination item, not this repo's work.
- Final grep gate: confirm no stray `dabstractor/qmk_notifier` repo-URL or
  hyphenated-firmware reference remains in `PRD.md`/`README.md`/`src/`.

---

## 4. Key Invariants (unchanged from session 001)

All eight PRD §14 invariants hold unchanged. The only naming-relevant note:
**the library identifier `qmk_notifier` is an invariant** — Cargo's `_`-from-`-`
derivation means `use qmk_notifier::` must never be rewritten to `use qmk-notifier::`
(that is not valid Rust). The hyphenated name lives only in the package/repo/binary
and in prose.

---

*End of delta. This is a naming reconciliation of session 001's v0.3.0 deliverable;
no transport behavior is affected.*