# PRP — P1.M1.T1.S1: Create `qmk_module.json` at repo root

## Goal

**Feature Goal**: Add the QMK Community Module manifest `qmk_module.json` at the
repository root so the module is self-declaring (PRD §18.3 R1): a user adds one
`"modules": ["<org>/qmk_notifier"]` entry to `keymap.json` and QMK discovers the
module's `rules.mk`, sources, and include path automatically.

**Deliverable**: The file `qmk_module.json` at the repo root — well-formed JSON,
valid against the `qmk.community_module.v1` schema, containing exactly five keys
(`module_name`, `maintainer`, `license`, `url`, `keycodes`) and **no** `features` block.

**Success Definition**:
- `qmk_module.json` exists at the repo root and `python3 -m json.tool` parses it.
- Fields: `module_name="QMK Notifier"`, `maintainer="dabstractor"`,
  `license="MIT"`, `url="https://github.com/dabstractor/qmk_notifier"`, `keycodes=[]`.
- No `features` key (RAW_ENABLE is a `rules.mk` variable, not a data-driven feature key).
- `license` is GPL-compatible (MIT is) so it satisfies QMK's GPLv2+ inclusion requirement.

## User Persona (if applicable)

**Target User**: End users installing the module via the Community Module flow, and
the QMK build system (`lib/python/qmk/cli/generate/community_modules.py`).

**Use Case**: User clones the repo to `modules/<org>/qmk_notifier`, adds
`"modules": ["<org>/qmk_notifier"]` to `keymap.json`; QMK reads the manifest to
register the module (auto-compiles `<leaf>.c`, adds the dir to VPATH, sets
`-DCOMMUNITY_MODULE_<LEAF>_ENABLE`).

**Pain Points Addressed**: Removes the hand-written `include .../rules.mk` line, the
`SRC +=` / `RAW_ENABLE` wiring, and the relative `#include` path (PRD §18.1 / R5).

## Why

- **R1 (this task)** is the manifest prerequisite for the §18 Community Module migration.
- Self-declaring install (one `keymap.json` line) vs. today's multi-step submodule flow.
- The `license` field is required to be GPL-compatible for QMK inclusion (QMK is GPLv2+);
  this repo's actual license — **MIT** — is GPL-compatible and is the correct SPDX value.

## What

A static JSON manifest at the repo root:

```json
{
    "module_name": "QMK Notifier",
    "maintainer": "dabstractor",
    "license": "MIT",
    "url": "https://github.com/dabstractor/qmk_notifier",
    "keycodes": []
}
```

**Why `license: "MIT"` (not the `GPL-2.0-or-later` mentioned in the task note):**
verified directly from the working tree —
- `LICENSE` (1070 bytes) is the standard **MIT License** text ("Copyright (c) 2025 Dustin Schultz").
- `Cargo.toml` declares `license = "MIT"` (the authoritative SPDX for this package).
- MIT is GPL-compatible, so it satisfies QMK's GPLv2+ inclusion requirement and the
  `qmk.community_module.v1` schema.

The task note's "There is NO LICENSE file … use GPL-2.0-or-later" is **stale** for this
repo (a LICENSE file exists and is MIT). PRD §18.3 R1 explicitly says *"confirm the actual
repo license"* — the actual repo license is MIT, so `MIT` is the value to write.

### Success Criteria

- [ ] `qmk_module.json` at repo root, valid JSON.
- [ ] Exactly 5 top-level keys: `module_name`, `maintainer`, `license`, `url`, `keycodes`.
- [ ] `license="MIT"`; no `features` key; `keycodes` is `[]`.
- [ ] `url` is the underscore firmware repo (`dabstractor/qmk_notifier`).

## All Needed Context

### Context Completeness Check

_Pass_: The exact JSON content (verified against the repo's actual LICENSE/Cargo.toml),
the schema rules, and executable validation commands are all specified below.

### Documentation & References

```yaml
# MUST READ — the requirement (authoritative)
- file: PRD.md
  section: "## 18. Community Module Distribution → ### 18.3 Requirements → R1"
  why: "R1 is the canonical manifest content (module_name/maintainer/license/url/keycodes)
        and explicitly says 'confirm the actual repo license'."
  critical: "No `features` block; no keycodes entries. RAW_ENABLE lives in rules.mk (R2)."

- file: PRD.md
  section: "### 18.2 Verified build-system mechanics"
  why: "Explains WHY there is no `features` entry: RAW_ENABLE is NOT a data-driven feature
        key (no `rawhid` schema in data/schemas/), so it must be in rules.mk."
  critical: "Do not invent a `features: {rawhid: ...}` entry — it does not exist in the schema."

- file: LICENSE
  why: "Determines the `license` SPDX value. This repo's LICENSE is the MIT License."
  critical: "MIT is GPL-compatible → valid for QMK (GPLv2+) inclusion. Use \"MIT\"."

- file: Cargo.toml
  why: "Cross-check: declares `license = \"MIT\"` and `repository` URL (hyphenated crate URL).
        Confirms MIT is the authoritative package license."
  critical: "The manifest `url` uses the UNDERSCORE firmware repo
        (https://github.com/dabstractor/qmk_notifier) per PRD §18.3 R1 — NOT the Cargo
        hyphenated crate URL. Do not copy Cargo's repository field into the manifest."

# Schema reference (upstream QMK)
- url: https://github.com/qmk/qmk_firmware/blob/master/data/schemas/qmk.community_module.v1.json
  why: "The jsonschema the manifest must satisfy. REQUIRED: module_name, maintainer."
  critical: "license/url/keycodes/features are all OPTIONAL; omitting features is valid."

# Pattern to follow (real example modules)
- url: https://github.com/qmk/qmk_firmware/tree/master/modules
  why: "hello_world and super_alt_tab ship qmk_module.json. Mirror their field set and
        4-space indent + trailing newline. (Those use GPL-2.0-or-later; this repo uses MIT.)"
```

### Current Codebase tree (repo root)

```bash
Cargo.toml     # Rust transport crate; license="MIT"; repository=hyphenated crate URL
Cargo.lock
LICENSE         # MIT License (1070 bytes) — source of the `license` SPDX value
src/            # Rust sources
target/         # Rust build output
PRD.md  README.md  .gitignore  plan/
qmk_module.json # ← CREATE (this task). Absent today.
# NOTE: R2 (rules.mk) and R3 (notifier.c guard) reference firmware C files; they are
#       SEPARATE tasks (P1.M1.T2 / P1.M1.T3). This task is manifest-only.
```

### Desired Codebase tree with files to be added

```bash
qmk_module.json # NEW — static manifest at repo root (this task's only deliverable).
```

### Known Gotchas of our codebase & Library Quirks

```python
# CRITICAL (license value): use "MIT". The repo's LICENSE file is the MIT License and
#   Cargo.toml declares license="MIT". The task note's "no LICENSE / GPL-2.0-or-later"
#   is STALE. MIT is GPL-compatible, so it satisfies QMK's GPLv2+ requirement.

# GOTCHA (no features block): do NOT add a `features` map. RAW_ENABLE is a rules.mk
#   variable, not a data-driven feature key — there is no `rawhid` schema entry
#   (PRD §18.2). Adding an unknown feature key FAILS `qmk lint`.

# GOTCHA (no keycodes): the public surface is macros (DEFINE_SERIAL_*) + functions
#   invoked from keymap.c, NOT keymap-bindable keys. keycodes MUST be [] (or omitted).

# GOTCHA (url uses UNDERSCORE): https://github.com/dabstractor/qmk_notifier — per
#   PRD §18.3 R1. Do NOT copy Cargo.toml's hyphenated repository URL.

# GOTCHA (validation needs qmk for full schema check): `python3 -m json.tool` validates
#   JSON SYNTAX only. Full qmk.community_module.v1 schema validation requires `qmk lint`
#   in a qmk_userspace build that lists the module (outside this task's scope).
```

## Implementation Blueprint

### Data models and structure

None beyond the JSON manifest (static, 5 keys).

### Implementation Tasks (ordered by dependencies)

```yaml
Task 1: CREATE qmk_module.json at the repo root
  - CONTENT (4-space indent, single trailing newline):
        {
            "module_name": "QMK Notifier",
            "maintainer": "dabstractor",
            "license": "MIT",
            "url": "https://github.com/dabstractor/qmk_notifier",
            "keycodes": []
        }
  - KEYS: exactly module_name, maintainer, license, url, keycodes (in that order).
  - license VALUE: "MIT" (verified from LICENSE file + Cargo.toml; GPL-compatible).
  - OMIT: `features` (no data-driven feature keys); do not add keycodes entries.
  - NAMING: module_name is the human display name ("QMK Notifier"); the leaf/identifier
    "qmk_notifier" comes from the clone directory, NOT from this file (PRD §18.2).
  - PLACEMENT: repo root (top level, alongside README.md / Cargo.toml).
  - DEPENDENCIES: none.

Task 2: DO NOT touch anything else
  - rules.mk rewrite is R2 (separate task P1.M1.T2).
  - notifier.c API-version guard is R3 (separate task P1.M1.T3).
  - README install rewrite is R5 (separate task P1.M2.T1).
  - Do not modify PRD.md, Cargo.toml, LICENSE, tasks.json, prd_snapshot.md, .gitignore.
```

### Implementation Patterns & Key Details

```python
# The manifest is deliberately minimal — qmk.community_module.v1 only REQUIRES
# module_name + maintainer. We add license/url/keycodes for completeness and GPL
# compliance, and OMIT features because the module declares no data-driven features.
#
# ANTI-PATTERN: do not add "features": {"rawhid": ...}. No such schema entry exists;
#   RAW_ENABLE is set in rules.mk. Adding it fails `qmk lint`.
# ANTI-PATTERN: do not list keycodes. The API is macros + C functions, not bindable keys.
# ANTI-PATTERN: do not copy Cargo.toml's hyphenated repository URL; use the underscore
#   firmware-repo URL per R1.
# ANTI-PATTERN: do not write "GPL-2.0-or-later" — that misrepresents this MIT repo.
```

### Integration Points

```yaml
BUILD (QMK community-module generator):
  - read by: "lib/python/qmk/cli/generate/community_modules.py (in qmk_firmware)"
  - effect: "registers the module dir so SRC/VPATH/rules.mk are discovered (§18.2)."
LICENSE:
  - field: "license = \"MIT\" (repo's actual SPDX; GPL-compatible)."
CONFIG/ROUTES/DATABASE:
  - none (static JSON manifest).
```

## Validation Loop

> These run in the current repo (repo root). Full schema validation additionally needs a
> `qmk_userspace` build (outside this task); `qmk` is not required for Levels 1 & 4.

### Level 1: Syntax & Style (Immediate Feedback)

```bash
cd /home/dustin/projects/qmk-notifier

# Valid JSON syntax.
python3 -m json.tool qmk_module.json > /dev/null && echo "JSON OK"

# Exactly the expected key set, correct values, no extras.
python3 - <<'PY'
import json
d = json.load(open("qmk_module.json"))
assert list(d.keys()) == ["module_name","maintainer","license","url","keycodes"], d.keys()
assert d["module_name"] == "QMK Notifier"
assert d["maintainer"] == "dabstractor"
assert d["license"] == "MIT"
assert d["url"] == "https://github.com/dabstractor/qmk_notifier"
assert d["keycodes"] == []
assert "features" not in d
print("fields OK")
PY

# License is GPL-compatible (QMK is GPLv2+).
python3 - <<'PY'
import json
lic = json.load(open("qmk_module.json"))["license"]
GPL_COMPAT = {"MIT","GPL-2.0-only","GPL-2.0-or-later","GPL-3.0-only","GPL-3.0-or-later",
              "BSD-3-Clause","BSD-2-Clause","ISC","Apache-2.0","LGPL-2.1-or-later","MPL-2.0"}
assert lic in GPL_COMPAT, f"license {lic!r} not in known GPL-compatible set"
print("license GPL-compatible OK:", lic)
PY
# Expected: "JSON OK"; "fields OK"; "license GPL-compatible OK: MIT".
```

### Level 2: Unit Tests (Component Validation)

```bash
# A static JSON manifest has no unit-test harness beyond Level 1's structural checks.
# Its behavioral effect (module registration) is validated at Level 3 via `qmk lint`.
echo "Level 2: N/A for a static JSON manifest (covered by Level 1 + Level 3)."
```

### Level 3: Integration Testing (System Validation)

```bash
# Authoritative gate (PRD §18.5): qmk lint must be clean with the module registered.
# Requires: the repo cloned to modules/<org>/qmk_notifier inside a qmk_userspace checkout
# and listed in a keymap.json. If such an environment is available:
#   qmk lint -kb <keyboard> -km <keymap>    # expect exit 0
# This is environment-dependent and outside the repo-local validation; run if available.
echo "Level 3: run 'qmk lint' in a userspace build that lists the module (§18.5)."
```

### Level 4: Creative & Domain-Specific Validation

```bash
cd /home/dustin/projects/qmk-notifier

# Cross-check field values against PRD §18.3 R1.
python3 - <<'PY'
import json
d=json.load(open("qmk_module.json"))
assert d["module_name"]=="QMK Notifier" and d["maintainer"]=="dabstractor"
assert d["url"].endswith("dabstractor/qmk_notifier")   # underscore firmware repo
assert d["keycodes"]==[] and "features" not in d
assert d["license"]=="MIT"
print("R1 conformance OK")
PY

# Confirm the license field matches the repo's actual LICENSE / Cargo.toml.
grep -qi 'MIT License' LICENSE && echo "LICENSE file is MIT (matches manifest)"
grep -q '^license = "MIT"' Cargo.toml && echo "Cargo.toml license=MIT (matches manifest)"

# Diff hygiene: only qmk_module.json is added in this task.
git status --short
# Expected: only `qmk_module.json` (new/untracked). No other source changes.
```

## Final Validation Checklist

### Technical Validation

- [ ] Level 1: `python3 -m json.tool` parses; field-set + license checks pass.
- [ ] Level 3 (if env available): `qmk lint` clean in a userspace build listing the module (§18.5).
- [ ] Level 4: R1 conformance; license matches LICENSE + Cargo.toml; diff hygiene OK.

### Feature Validation

- [ ] `qmk_module.json` at repo root with the 5 required keys.
- [ ] `license="MIT"` (repo's actual GPL-compatible SPDX).
- [ ] No `features` block; `keycodes` is `[]`.
- [ ] `url` uses the underscore firmware repo.

### Code Quality Validation

- [ ] 4-space indent + trailing newline, matching `qmk_firmware/modules/*` style.
- [ ] No anti-patterns (no features block, no guessed license, no keycodes, correct URL).
- [ ] Only `qmk_module.json` created — Cargo.toml/LICENSE/rules.mk/notifier.c/README untouched.

### Documentation & Deployment

- [ ] Fields self-document (Mode A); `license` is the repo's real SPDX (`MIT`), not a blind default.

---

## Anti-Patterns to Avoid

- ❌ Don't write `"license": "GPL-2.0-or-later"` — this repo is MIT (LICENSE + Cargo.toml both say MIT).
- ❌ Don't add a `features` block — no `rawhid` schema entry exists; RAW_ENABLE is a rules.mk var.
- ❌ Don't list keycodes — the surface is macros + functions, not bindable keys.
- ❌ Don't copy Cargo.toml's hyphenated repository URL — `url` is the underscore firmware repo per R1.
- ❌ Don't touch rules.mk (R2/P1.M1.T2), notifier.c (R3/P1.M1.T3), README (R5/P1.M2.T1), Cargo.toml,
  LICENSE, PRD.md, tasks.json, prd_snapshot.md, or .gitignore.

---

## Confidence Score: 9/10

The deliverable is a single 5-key static JSON file whose content is fully pinned down and
verified against the repo's actual LICENSE (`MIT`) and Cargo.toml. Levels 1 and 4 validate
locally with `python3`; the only gate requiring external tooling (`qmk lint`) needs a
qmk_userspace build that is outside this task's scope (hence 9, not 10). The corrected
`license: "MIT"` (vs. the stale `GPL-2.0-or-later` note) is evidence-based and GPL-compatible.