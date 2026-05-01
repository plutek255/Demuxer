# Changelog — BlueJay Demuxer

All notable changes to BlueJay Demuxer are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased]

### Planned
- Olympian model hardware finalization
- Wireless expansion module support
- IoT brick add-on integration

---

## [0.4.0] — 2026-04-04

### Added
- `docs/hardware.md` — updated Olympian model specs (fans, camera, IR emitter, voltmeter headers, UHF RFID tag, ultrasonic sensor)

---

## [0.3.0] — 2026-04-02

### Added
- `tools/dmxr-interpreter.html` — browser-based .dmxr interpreter tool
- `examples/hello.dmxi` — compiled .dmxi bundle example
- `examples/wheel_mode.dmxr` — wheel mode example app
- `examples/finger_walk.dmxr` — finger-walk mode example app
- `examples/hello_glove.dmxr` — hello glove example app
- `docs/DMXR_SPEC.md` — .dmxr language specification v0.2
- Updated `docs/gestures.md` and `docs/hardware.md`

### Changed
- CI pipeline disabled (GitLab Free plan — no runner minutes)

---

## [0.2.0] — 2026-04-01

### Added
- BlueJay logo asset (`assets/JPEG_image.jpeg`)
- Logo in README

---

## [0.1.0] — 2026-03-31

### Added
- Initial scaffold — firmware, profiles, runtime, docs, tools
- `firmware/` — main.c, flex.c, foil.c, haptic.c, imu.c, touchscreen.c, demuxer.h
- `profiles/` — default, gamepad, keyboard JSON profiles
- `runtime/` — app.dmxb, app.dmxr, install.dmxr, worker.js
- `docs/` — hardware reference, gesture reference
- `CHANGELOG.md`, `CONTRIBUTING.md`, `LICENSE.md` (CC BY 4.0), `README.md`
- `info.json`

---

© BlueJay Electronics LLC
