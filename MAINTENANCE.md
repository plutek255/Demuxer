# Maintenance Guide — BlueJay Demuxer

This doc covers how to keep the codebase healthy: checking for bugs, finding duplicates, removing stale files, and keeping things in sync with Linear.

---

## How to Check the Code

### General health check
- Run a search for `TODO`, `FIXME`, `HACK`, `XXX` across the repo — these are deferred issues
- Look for commented-out blocks that are more than a few lines — they're usually dead code
- Check for files that haven't been touched in a long time relative to active development

### Linting / static analysis
- C firmware: `cppcheck firmware/` or `clang-tidy` on individual `.c` files
- `.dmxr` scripts: use `tools/dmxr-interpreter.html` in browser to catch parse errors
- Shell/JS: `shellcheck` on any `.sh`, `eslint` on `runtime/worker.js`

---

## Tricky / Bug-Prone Areas

- **Flex sensor calibration** — per-user hand size means hardcoded thresholds will break for some users. Always check `firmware/flex.c` for magic numbers and ensure they're pulled from `profiles/`
- **IMU gesture detection** (`firmware/imu.c`) — gesture thresholds + cooldown timers are fragile. A threshold too low = false positives on movement; too high = missed gestures. Test with real hardware
- **Foil contact debounce** (`firmware/foil.c`) — rapid palm contacts can double-fire. The debounce window is critical — too short causes spam, too long feels laggy
- **Haptic timing** (`firmware/haptic.c`) — simultaneous haptic + flex read can cause timing conflicts on shared GPIO. Watch for race conditions when multiple motors fire at once
- **Wheel mode axis math** — combining X tilt + vertical arm movement to infer wheel angle is approximation math. Edge cases at extreme tilts can produce wrap-around artifacts
- **`.dmxr` parser** (`tools/dmxr-interpreter.html`) — TSX blocks inside `.dmxr` files are eval'd. Malformed TSX can silently fail rather than throwing. Always test new scripts in the interpreter first

---

## What the Code Should Ideally Look Like

- `firmware/` — one file per subsystem (flex, foil, haptic, imu, touchscreen). No cross-subsystem calls except through `main.c`
- `profiles/` — all user-tunable thresholds live here, never hardcoded in firmware
- `runtime/` — `.dmxr` scripts are the app layer; `worker.js` is the bridge. Keep them decoupled
- `examples/` — every new gesture or mode should have a minimal `.dmxr` example
- `docs/` — `hardware.md` should always match the current physical build, not a planned one

---

## Finding and Removing Duplicates

### Duplicate files
```bash
find . -type f | sort | xargs md5sum | sort | awk 'seen[$1]++ {print}'
```

### Duplicate functions / logic
- Search for repeated function names: `grep -rn "void " firmware/ | awk -F: '{print $NF}' | sort | uniq -d`
- Look for the same gesture logic copy-pasted across `.dmxr` example files — extract to a shared include

### Duplicate imports / dependencies
- Check `info.json` and `runtime/` for redundant module references

---

## Finding Old / Redundant / Not-Updated Files

### Stale files checklist
- Files not modified in 60+ days while the project is actively developed
- Profile JSON files referencing old pin numbers or removed sensors
- `.dmxr` examples for gestures that were redesigned or removed

### Git-based staleness check
```bash
git log --name-only --since="60 days ago" --pretty=format: | sort -u > recent.txt
git ls-files | sort > all.txt
comm -23 all.txt recent.txt
```

---

## Syncing with Linear

This project is tracked in Linear under the **Zyne** team. When tickets are updated, closed, or added, this repo should reflect that.

### Workflow
1. When a Linear ticket moves to **Done** — check if the related code/docs were actually merged and complete
2. When a ticket is **cancelled** — find any WIP code for that feature and either remove it or leave a clear `// CANCELLED: TK-XX` comment
3. When a **new ticket** is created — if it touches an existing file, add a `TODO: TK-XX` comment near the relevant code so it's easy to find
4. Periodically check the repo for `TK-` references to old/closed tickets and clean them up

### Finding ticket references in code
```bash
grep -rn "TK-" . --include="*.py" --include="*.js" --include="*.ts" --include="*.c" --include="*.h" --include="*.dmxr" --include="*.md"
```

---

## Public Folder

The `assets/` folder holds project images and logo files. Do not store compiled binaries or user data here. The `public/` folder (if added) is for web-served assets only.

---

## Changelog

The `CHANGELOG.md` in this repo tracks meaningful changes to the project — features, fixes, hardware updates, and breaking changes. Keep it updated whenever something significant lands.

### Format

Follow [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) conventions:

```markdown
## [Unreleased]
### Added
- Short description of new feature or file

### Changed
- What was modified and why

### Fixed
- Bug fixed, what caused it

### Removed
- What was deleted and why

## [0.1.0] — YYYY-MM-DD
### Added
- Initial scaffold
```

### Rules
- **One entry per meaningful change** — don't log file renames or typo fixes unless they matter
- **Use the right section** — Added / Changed / Fixed / Removed / Deprecated / Security
- **Date every release** — use `YYYY-MM-DD` format
- **Keep `[Unreleased]` at the top** — move it to a version when you tag a release
- **Link to Linear** — if a changelog entry closes a ticket, reference it: `(closes TK-XX)`

### When to update
- When a Linear ticket moves to Done
- When a new feature or hardware component is confirmed working
- When a breaking change is made (API change, schema change, pin reassignment, etc.)
- Before tagging a release

---

## Periodic Maintenance Checklist

- [ ] Search for `TODO / FIXME / HACK` and address or ticket them
- [ ] Run duplicate file check
- [ ] Check for stale files (60+ day git check)
- [ ] Verify all `TK-` references are still open in Linear
- [ ] Confirm `assets/` only contains files meant to be shared
- [ ] Review dependencies for outdated versions
- [ ] Confirm README reflects current project state
- [ ] Check that CONTRIBUTING and LICENSE are still accurate
- [ ] Update CHANGELOG.md before any release tag

---

© BlueJay Electronics LLC
