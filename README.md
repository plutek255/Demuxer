**Manufacturer:** BlueJay Electronics LLC
**Make:** Demuxer
**Current State:** Beta (Init)
**Logo:** BlueJay — circle body + triangle beak arrow + leg marks (two versions: full logo and app/boot icon)

---

## Product Lineup

| model | description |
| -- | -- |
| **Demuxer Init** | v1 gaming glove — current build (beta). flex sensors, foil contacts, 6DOF IMU, forearm touchscreen, haptic feedback |
| **Demuxer Olympian** | v2 — wireless expansion, IoT brick add-on, features too ambitious for Init hardware |
| **Demuxer Protocol 1–9** | higher-end cyber-arm series — emergency signal conversion (HDMI/aux → RCA), emergency bridge mode, audio/video relay |
| **Demuxer Elite** | premium tier |
| **Demuxer Elite Plus** | upgraded Elite |
| **Demuxer Elite Dev** | developer-focused version |
| **Demuxer Elite Pulse** | extra haptics + enhanced features |
| **Demuxer Optical** | VR/aim assistance focused |
| **Demuxer Tachyon GX** | earlier GX model — remote PC functionality + finger-walk gesture, sleek sci-fi full-forearm mechanical design |
| **Demuxer Parabola GX** | later GX model — sci-fi forearm design + deep smart home integration (e.g. F4+F6 gesture combo triggers lights, playlists, etc.) |

> **Tachyon GX and Parabola GX** are designed to look like a full sci-fi forearm plate — sleek mechanical cyber-arm aesthetic (reference: Borderlands-style arm).

---

## Demuxer Init — Full Spec

### Overview

A sensor-packed glove controller that tracks finger bends, adds foil contacts, haptic feedback, and detects wrist gestures with a 6DOF sensor, plus a forearm touchscreen for extra controls and IoT features.

**Primary goal:** Translate natural hand/finger/wrist movement into reliable game inputs while keeping common controls easy to reach.

---

### Target Users & Use Cases

**Target users**

* PC gamers who want motion/finger-based controls
* VR/AR users who want more natural hand input
* Accessibility-focused players who need alternative inputs

**Use cases**

* Gesture-based melee / aiming / quick actions (flick, push, pull)
* Finger bend inputs (grip strength, individual finger actions)
* "Macro" controls on the palm for fast access without reaching for a keyboard/controller
* Quick UI interactions on the touchscreen (maps, inventory, smart-home toggles)
* Wheel steering using any round object (plate, cardboard circle, frisbee, etc) — Wii Wheel style
* Finger-walk mode — use pointer + middle fingers as character legs to move ingame

---

### Core Features

* **Flex sensors** — finger bend/grip detection (5 fingers)
* **Palm foil contacts** — wired conductive foil pads on palm/fingers, complete circuit on touch (no buttons, inspired by Eggman's gloves from Sonic Movie 1)
* **6DOF IMU (GPIO)** — wrist gesture detection (push, pull, twist, tilt)
* **Forearm touchscreen** — UI/settings/IoT controls on dorsal forearm
* **Pointer device** — dual-light emitter (Wiimote-style), cursor + rotation support
* **Haptic feedback** — 6 vibration motors, supports realistic arm-shooting feel
* **Autoclick** — haptic-assisted rapid input via fingertip vibration motors (servo/tendon cable system TBD)
* **Optional:** wrist flashlight, wired USB output, expansion modules

---

### Mechanical & Wearable Design

**Form factor**

* Glove that covers the hand and fingers (for flex tracking)
* Forearm module on the dorsal side (top side) of the forearm for touchscreen + extras

**Ergonomics considerations**

* Foil contacts positioned to avoid accidental triggers while gripping
* Touchscreen placement optimized so it's visible but not constantly bumped
* Balanced weight distribution: keep heavier parts closer to forearm rather than fingers

**Comfort considerations**

* Breathable material for long sessions
* Adjustable strap/fit system (wrist + forearm)
* Removable/replaceable liner (optional) for cleaning and sweat control

---

### Sensors & Input Systems

#### Finger Tracking (flex sensors)

**Purpose:** Detect bending of each finger to trigger analog or digital inputs.

**Typical interpretations**

* Individual finger curls (index, middle, ring, pinky)
* "Grip" action (combined finger bend)
* Pinch-like gestures (thumb + index bend threshold)

**Notes for calibration**

* Neutral position calibration (hand relaxed)
* Per-user sensitivity tuning (different hand sizes/strength)

---

#### Finger-Walk Mode

**Concept:** Use the pointer and middle fingers as the character's legs — alternating taps move the character forward, wrist twist controls turning. Fingers literally act as feet.

**How it works**

* Pointer + middle finger alternating taps = forward movement
* Tap speed = controls movement speed (slow taps = walk, fast taps = run)
* Wrist twist (IMU) = turning left/right ingame
* Standalone mode only — cannot be combined with other gestures while active

**Use cases**

* Games with character movement (RPGs, adventure, exploration)
* Fun/novelty control scheme
* Accessibility alternative to analog sticks

**Notes**

* Needs a reliable tap detection threshold to distinguish intentional alternating taps from accidental flex
* Debounce logic important here — fast tapping shouldn't double-fire

---

#### Wrist/Hand Motion (6DOF IMU via GPIO)

**Sensor type:** 6DOF (accelerometer + gyroscope)

**Purpose:** Detect wrist gestures like quick forward/backward jerks, twists, tilts.

**Supported gesture examples**

* Quick jerk forward = "push" / "dash" / "melee"
* Quick jerk backward = "pull" / "block"
* Twist = "reload" / "swap weapon"
* Tilt = "lean" / "steering"
* Wheel mode (see below)
* Finger-walk turning — wrist twist maps to turn left/right when finger-walk mode is active

**Signal handling (recommended)**

* Gesture detection thresholds + cooldown timers to prevent repeated triggers
* Noise filtering (basic smoothing) to reduce false positives

---

#### Wheel Mode (freeform steering)

**Concept:** Grab any round object (frisbee, plate, cardboard circle, etc) and use it as a steering wheel. The glove doesn't detect the object itself — it just tracks arm position and infers where the wheel is rotated to.

**How it works**

* Default grip assumed on the right side of the object
* X axis (left/right arm tilt) = primary steering input
* Vertical arm movement (up/down) is paired with the x axis to interpret the wheel's rotational position relative to the arm
* The firmware combines both axes to estimate the full wheel angle
* Single glove only — no second glove needed, all math is inferred from one arm

**Reliability tiers**

* Fixed wheel (sim rig, mounted steering wheel, etc) — most reliable, competitive quality
* Freeform object (frisbee, plate, book, etc) — works but less precise, casual/party use
* Air steering (no object) — least reliable, drifty and noisy, fun but not practical for serious use

**Calibration**

* Set center position (arm straight forward, object level)
* Set left/right lock positions (max tilt either direction)
* Dead zone adjustable per profile

---

#### Palm Controls (foil contacts)

**Purpose:** Provide reliable, always-works inputs even if sensors misread — inspired by Eggman's glove contacts from Sonic Movie 1.

**Implementation**

* Conductive foil contact pads placed on the palm/fingers
* Wires run from each pad directly to MCU GPIO pins
* When two contacts touch (e.g. fingertip to palm pad), circuit completes and registers as input
* No moving parts, no mechanical buttons

**Suggested roles**

* Menu / back / confirm
* Push-to-talk
* Modifier (hold to change what gestures do)
* Quick item / ability triggers
* Recalibrate — triggers a full sensor recalibration on demand without opening any menu

**Design notes**

* Foil pads should be positioned where fingers naturally rest on the palm
* Glove material around pads must be non-conductive
* Firmware needs debounce logic + resistance threshold to avoid false triggers from sweat or light grazes
* Wires should be routed flat against the glove to avoid snagging

---

#### Forearm Touchscreen

**Location:** Dorsal side of forearm

**Purpose:** UI interactions + optional IoT controls + secondary functions.

**Suggested functions**

* Quick settings (sensitivity profiles, modes)
* In-game macros (if allowed)
* Smart-home toggles (lights, music) when not gaming
* Battery status, connection status, sensor health

**Usability notes**

* "Lock screen" mode to prevent accidental touches during play
* Large buttons / high-contrast UI for fast use

---

### Wake System

**Manual wake sequence:** F1 → F4 → F5 → F3 → F2

* Prevents accidental wake
* Requires intentional finger movement pattern
* Hard to trigger while sleeping

**Failsafe reset:** F2 × 4 — cancels and resets wake sequence state

**Automatic wake (alarm mode)**

* System powers on automatically if alarm is scheduled
* Begins haptic vibration pattern
* Continues until user acknowledges

---

### Vibration Hardware Layout

6 vibration motors placed around the arm/hand:

1. Dorsal forearm (top side)
2. Ventral forearm (bottom/inner)
3. Left wrist side
4. Right wrist side
5. Back of hand
6. Palm

**Vibration patterns**

* WAKE_UP: forearm bottom → forearm top → wrist sides → back of hand → palm
* Downward alert: palm → back of hand → wrist sides → forearm top → forearm bottom
* Pulse alarm: ALL → pause → ALL → pause
* PANIC_WAKE: all motors rapid pulse + max volume sound

**Vibration as notification language:**

| pattern | meaning |
| -- | -- |
| arm → hand wave | notification |
| palm pulse | message |
| alternating wrist | incoming call |
| fast arm pulse | alarm |

---

### Alarm System

**Alarm sound sources**

1. Flash drive / SD card — plug into arm dock, system scans `/Alarm Sounds/` folder
2. Web-to-arm client (Protocol+) — upload via browser, arm receives + stores file, UI refreshes instantly
3. Built-in defaults

**Supported formats:** `.wav` `.ogg` `.mp3` `.dmxsfx`

**Sound output modes:** `ARM_SPEAKER`, `PHONE`, `BLUETOOTH_DEVICE`, `ALL`

**Fallback audio:** if phone disconnects mid-alarm, automatically switches to arm speaker — alarm never fails.

**Alarm actions:** call phone / play sound / vibration only

---

### Autoclick (Experimental)

**Feature:** Vibrates a specific finger at high frequency to simulate rapid clicking or key pressing.

**Servo mounting options (TBD):**

* Finger rings/thimbles — servo on back of hand, rod/cable pulls finger down
* Tendon-style cable system — servo on forearm, thin cables run along fingers (recommended)
* Dorsal servo mount — tiny micro servo on top of each finger, more precise but bulkier

**Notes / risks**

* Hard frequency cap needed for fair-use / anti-cheat compliance
* Long use at high frequency may cause discomfort — add usage timer/warning
* Should be toggle-on/off, not always active

---

### Connectivity & Data Output

**USB/data cable output (PC connection)**

* Low-latency wired mode + charging + stable data link
* More consistent latency for competitive gaming

**Wireless (future)**

* Bluetooth / proprietary 2.4GHz dongle
* Range + latency TBD

---

### Power System

* Internal rechargeable battery
* USB charging (same port as data output)
* Auto-sleep when inactive
* Low-power mode for basic sensor tracking
* Brightness control for touchscreen

---

### Optional Features & Expansions

**Wrist flashlight**

* Built into the wrist, switch next to touchscreen
* Use cases: dark rooms, cosplay/stream, emergency light

**Pointer device with receiver**

* Emits two lights in a row (Wiimote-style)
* Two-dot system allows cursor rotation in supported software
* Emulates Nintendo Power Glove behavior
* Realistic arm-shooting behavior with haptic feedback

**Extra expansion ideas**

* Swappable sensor modules
* Extra foil contacts/scroll wheel near thumb
* Detachable touchscreen module for weight reduction

---

### Software, Modes & Profiles

**Input modes**

* Gamepad emulation mode
* Keyboard/mouse mode
* Custom profiles per game

**Calibration flow**

1. Hand relaxed (baseline)
2. Full finger curl (max)
3. Wrist motion test (forward/back + twist)
4. Wheel mode calibration (center, left lock, right lock)
5. Finger-walk tap test (walk vs run threshold)
6. Save profile name

---

### BlueJay OS & DMX Platform

The Demuxer runs on **BlueJay OS** with its own scripting language and app ecosystem called **DMX**.

**DMX File Ecosystem:**

| extension | role |
| -- | -- |
| `.dmxr` | runtime script (source code) |
| `.dmxb` | compiled binary |
| `.dmxc` | cached bytecode |
| `.dmcf` | core file (system modules, restricted) |
| `.dmxa` | app archive (installable package, zip container) |
| `.dmxu` | update package (firmware updates) |
| `.dmxs` | server/network handler |
| `.dmxi` | interface definition (UI assets, notifications, themes) |
| `.dmxsfx` | sound effect file |

**DMX Runtime modules:** UI, Physics (touchscreen-only), Sensor, Network

---

### DMX App Package (.dmxa) Structure

`.dmxa` is a zip container (like `.mcworld`, `.apk`, `.jar`). Install flow: download → verify signature → extract → run install.dmxr → register app → done

**Gesture packs as installable packages:** `racing_controls.dmxa`, `vr_sword_pack.dmxa`, `piano_mode.dmxa`

---

### DMX App Store — Validation System

6-step validation before publishing:

1. **Package structure check** — required files: `manifest.mmd`, `main.dmxr` or `main.dmxb`, `interface.dmxi`
2. **Manifest validation** — name, version, device target, entry file, permissions
3. **Code scan** — restricted: `/system/*.dmcf`, `/system/kernel`, firmware APIs
4. **Permission verification** — APIs used must be declared in manifest
5. **UI validation** — icon, notification layout, touchscreen resolution
6. **Malware prevention** — blocks infinite loops, network spam, file deletion

**Compatibility badge system:** `BJ_INIT` `BJ_ELITE` `BJ_PROTOCOL_SERIES`

---

### DMX UI Scripting

UI scripting reads like CSS + scripting hybrid. Supports:

* `PERSISTENT` — render once, stay on screen (labels, icons, static UI)
* `DYNAMIC` — reflow/update each frame (counters, live sensor data, timers)
* Event hooks: `onTap`, `onHold`, `onSwipe`
* Hardware control via buttons (e.g. `Light.flash(on)`)
* Live sensor display: `Text_CMD_SHOW("Grip: " + F2.te)&DYNAMIC`

---

### DMX Physics Module (touchscreen only)

Mini UI game engine for the forearm touchscreen. Physics only affects UI sprites, not the external PC.

**Render loop:** Input.update() → Physics.SPRITE_CMD_STEP() → UI.render()

**Key commands:**

* `Physics.gravity(0, 9.8)` — set gravity
* `Physics.SPRITE_CMD_STEP()` — advance one frame
* `Physics.spring(sprite, x, y)` — smooth UI movement

Finger sensor and IMU tilt can both apply forces to sprites. Use cases: calibration apps, mini games, UI animations, draggable widgets.

---

### Emergency Features (Protocol Series)

**Emergency signal conversion (Protocol 1+)**

* Converts HDMI or aux → RCA on the fly
* For when u don't have a physical adapter

**Emergency bridge mode (Protocol 3+)**

Triggers: emergency button, long-press gesture, finger contact sequence, automatic (fall detection / panic alarm)

* Audio relay: arm mic → compress → send via wifi/cellular → remote device
* Silent distress mode: vibrates quietly, sends audio + GPS without drawing attention
* Video option (later versions): small camera streams to phone / web client

**Hardware required (Protocol 3+):** WiFi, Bluetooth, mic, speaker, optional camera, encryption chip

---

### Safety, Reliability & Constraints

* Consistent detection across different hand sizes
* Minimal false triggers (IMU jerks, foil contacts)
* Avoid tight straps that restrict circulation
* Sweat/splash resistance (optional)
* Temperature management (touchscreen + battery)

---

### Open Questions / To Be Specified

* Exact number of foil contacts and their layout
* Touchscreen size, resolution, and UI design
* Battery capacity and expected runtime
* Wired vs wireless priority (or both)
* Supported platforms (PC only? consoles? VR headsets?)
* Target weight and comfort limits
* Autoclick servo mounting method (finger rings vs tendon cable vs dorsal mount)
* Are F1–F5 contacts physical buttons or touch pads?
* Touchscreen always-on vs sleep-on-flip
* Does the IDE run on PC or arm touchscreen?
* Does `.dmxi` define layout elements directly or does `.dmxr` handle UI pages?
