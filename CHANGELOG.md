# Changelog

All notable changes to the BlueJay Demuxer project are documented here.

Format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased]

- Autoclick haptic rapid-input system (servo/tendon TBD)
- Final sweep / bug check before prototype completion
- NeoPixel LED strip installation (forearm crevice lighting)

---

## [Init — Beta] · Current

### Added
- Glove base form factor (fabric/wearable layer, finger coverage)
- ILI9341 2.8" 320×240 touchscreen on dorsal forearm module
- 5× flex sensors (one per finger) mapped to touchscreen UI + USB output
- Palm foil contacts (conductive, circuit-complete on touch)
- 6DOF IMU for wrist gesture detection (push, pull, twist, tilt)
- 6× haptic vibration motors
- Custom aluminum plating on forearm module
- Parent desktop program for USB data link + remote touchscreen access
- Pointer device (dual-light emitter, Wiimote-style)

### Acquired (BOM — $361.09)
- SparkFun: vibration motors ×6, flex sensors ×5, 6DOF IMU, ILI9341 touchscreen, ESP32-S3, sub-micro servos ×4, IR emitter, buttons — $251.04
- Adafruit: NeoPixel LED strip 4m — $34.95
- Amazon: Rolyan compression gloves ×2, aluminum flashing ×2 — $75.10

---

## Roadmap

### [Demuxer Olympian — v2.0] · Planned
- Wireless WiFi brick wearable add-on (home IoT: lights, devices, etc.)
- Wireless expansion features deferred from Init hardware

### [Demuxer Protocol 1–9] · Future
- Emergency signal conversion (HDMI/aux → RCA)
- Emergency bridge mode
- Audio/video relay

### [Demuxer Tachyon GX / Parabola GX] · Concept
- Full sci-fi forearm plate aesthetic (Borderlands-style cyber-arm)
- Remote PC functionality + finger-walk gesture (Tachyon)
- Deep smart home integration via gestures (Parabola)
