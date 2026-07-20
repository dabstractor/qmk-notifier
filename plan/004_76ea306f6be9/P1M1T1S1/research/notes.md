# Research Notes — P1.M1.T1.S1 (Create qmk_module.json) — ⚠️ CROSS-REPO ROUTING ERROR

## TL;DR (the headline finding)
**This task is routed to the WRONG repository.** `qmk_module.json` (PRD §18
Community Module Distribution, R1) belongs in the **C firmware** repo
`dabstractor/qmk_notifier` (underscore), NOT in this repo — which is the
**Rust transport crate** `dabstractor/qmk-notifier` (hyphen). Creating the
manifest here is meaningless: a QMK Community Module compiles C firmware into
qmk_firmware; this repo has no `.c`, no `rules.mk`, no `keymap.c`.

## Evidence (all read directly from the working tree)
1. `pwd` = `/home/dustin/projects/qmk-notifier`
2. `Cargo.toml`: `name = "qmk-notifier"`; `description = "Raw-HID transport
   layer for the QMKonnect ecosystem"`; `license = "MIT"`; `repository =
   "https://github.com/dabstractor/qmk-notifier"` (HYPHEN). → this is the
   transport crate, a Rust library+CLI.
3. `PRD.md` line 1: `"# SPECIFICATION — qmk-notifier (Rust Raw-HID transport
   crate)"`. §1.2 ecosystem table: "qmk-notifier ← *this repo* | Rust |
   Transport crate (+CLI)" and "qmk_notifier | dabstractor/qmk_notifier | C |
   On-keyboard receiver + matcher + actor (firmware)".
4. Repo root has: `Cargo.toml`, `Cargo.lock`, `src/` (Rust), `target/`,
   `LICENSE` (MIT, 1070 bytes), `PRD.md`, `README.md`, `plan/`.
   Absent: `notifier.c`, `notifier.h`, `pattern_match.c`, `rules.mk`,
   `qmk_stubs/`, `test_*.c`, `run_all_tests.sh`. → no firmware artifacts at all.
5. `plan/` contains ONLY `001_b92a9b2b603f` and `002_bb250179a055`.
   `plan/004_76ea306f6be9/` DOES NOT EXIST here. The plan ID `004_76ea306f6be9`
   is from the firmware repo.
6. The `<prd_index>`/`<selected_prd_content>` in the task are the FIRMWARE PRD
   (`h1.0 SPECIFICATION — qmk_notifier (QMK Firmware Module)`, §18 Community
   Module Distribution). This repo's OWN PRD has sections 1–14 only (no §18).

## Why the manifest is firmware-only (not transport-crate)
QMK Community Modules (`lib/python/qmk/cli/generate/community_modules.py` in
qmk_firmware) compile a module's `<leaf>.c` INTO the keyboard firmware, set
`-DCOMMUNITY_MODULE_<LEAF>_ENABLE`, and add `<module_path>` to VPATH. All of
that requires C source + `rules.mk` + a `keymap.c` consumer. The Rust transport
crate is a desktop library linked by `qmkonnect`; it is never compiled into the
keyboard. A `qmk_module.json` here would point QMK at a directory with no
compilable `<leaf>.c` → inert/broken module.

## Secondary finding: stale license note
The item says "There is NO LICENSE file … use GPL-2.0-or-later". In THIS repo
that is false: there IS an MIT LICENSE and `Cargo.toml` declares `license =
"MIT"`. (MIT is GPL-compatible, so it would still be a valid manifest license
*if* a manifest belonged here — which it does not.)
For the FIRMWARE repo, the implementer must read ITS LICENSE file; if none,
`GPL-2.0-or-later` is the safe default (matches qmk_firmware + all example
modules in `qmk_firmware/modules/`).

## qmk_module.json schema (for the correct/firmware repo; from PRD §18.3 R1 + item)
- Schema id: `qmk.community_module.v1` (QMK data/schemas).
- REQUIRED: `module_name`, `maintainer`.
- OPTIONAL: `license`, `url`, `keycodes`, `features`.
- R1 values: module_name="QMK Notifier"; maintainer="dabstractor";
  license=<firmware repo's actual SPDX, GPL-compatible; GPL-2.0-or-later if none>;
  url="https://github.com/dabstractor/qmk_notifier" (UNDERSCORE — firmware repo);
  keycodes=[].
- NO `features` block (RAW_ENABLE is a rules.mk var, not a data-driven feature
  key; no `rawhid` schema entry exists — PRD §18.2).
- NO keycodes (public surface = macros + functions, not keymap-bindable keys).

## Recommended action
Re-route plan `004_76ea306f6be9` (and the whole §18 Community Module
Distribution milestone) to the firmware repo `dabstractor/qmk_notifier`
(underscore). The PRP below is written for THAT repo so it succeeds in one pass
once re-routed. Do NOT create `qmk_module.json` in this Rust transport-crate repo.

## Scope boundaries (if executed in the correct repo)
- Create ONLY `qmk_module.json` at the firmware repo root.
- Do NOT touch rules.mk (R2 is a SEPARATE task P1.M1.T2), notifier.c (R3 is
  P1.M1.T3), README (R5 is P1.M2.T1), PRD.md, tasks.json, prd_snapshot.md.