# hardware overview

> demuxer init | bluejay electronics LLC | beta

this doc covers the physical hardware components, wiring approach, and what's finalized vs still TBD.

---

## form factor

- **glove** — covers full hand and all fingers for flex sensor routing
- **forearm module** — mounted on the dorsal (top) side of the forearm, houses touchscreen + battery + MCU

weight distribution goal: keep heavy stuff closer to the forearm, not the fingers.

---

## sensors

### flex sensors (×5)
- one per finger (thumb, index, middle, ring, pinky)
- analog resistive flex sensors wired to MCU ADC pins
- output: normalized 0.0 (straight) → 1.0 (full curl)
- per-user calibration required on first use

**ADC pin assignments:** TBD — see `firmware/src/flex.c`, replace `FLEX_ADC_PINS` with your board's ADC pins

### 6DOF IMU
- accelerometer + gyroscope
- compatible chips: MPU-6050, ICM-42688-P, LSM6DS3 (pick one)
- communicates via I2C or SPI
- used for: gesture detection, wheel mode, finger-walk turning

**I2C/SPI wiring:** TBD — see `firmware/src/imu.c`, replace `_imu_hw_read()` stub

### foil contacts (×5, layout TBD)
- conductive foil pads on palm + fingertips
- wired directly to MCU GPIO pins (input with pull-up)
- contact = circuit completed, GPIO pulled LOW
- no mechanical parts

**GPIO pin assignments:** TBD — see `firmware/src/foil.c`, replace `FOIL_GPIO_PINS`

contact layout is **not finalized** — exact pad positions and count pending ergonomics testing

### haptic motors
- small LRA or ERM motors at each fingertip (×5)
- driven via PWM
- used for: gesture confirmation feedback, experimental autoclick feature

**PWM pin assignments:** TBD — see `firmware/src/haptic.c`, replace `HAPTIC_PWM_PINS`

---

## forearm module

### touchscreen
- location: dorsal forearm
- purpose: status display, quick settings, IoT toggles
- size/resolution: **TBD** (see open questions)
- interface: SPI or I2C LCD (placeholder: 240×135)
- touch controller: e.g. FT6206 or XPT2046

see `firmware/src/touchscreen.c` — replace stubs with actual display driver

### battery
- internal rechargeable LiPo/Li-ion
- capacity: **TBD** (target runtime unknown, pending component selection)
- charging via USB (same port as data output if combined)
- battery gauge IC recommended for accurate % reporting

### connectivity
- **wired USB** — primary, low-latency, also charges
- **wireless** — not in v0.1 scope, planned for later

---

## optional hardware (not in base build)

| addon | notes |
|-------|-------|
| wrist flashlight | LED + switch next to touchscreen |
| pointer IR emitter | 2 IR LEDs for cursor control (wiimote-style) |
| scroll wheel | near thumb, extra foil contacts |
| detachable screen | reduces weight when not needed |

---

## wiring notes

- foil contact wires should be routed **flat against the glove** to avoid snagging
- all wires need proper strain relief, especially at the forearm module junction
- glove material around foil pads must be **non-conductive**
- forearm module casing edges must be smooth — no sharp edges

---

## open questions

- [ ] exact foil contact count + pad layout
- [ ] touchscreen size + resolution + touch IC
- [ ] battery capacity + expected runtime
- [ ] target glove weight
- [ ] wired vs wireless priority for v1
- [ ] supported platforms (PC only? VR headsets? consoles?)
- [ ] MCU selection (ESP32? RP2040? STM32? custom?)


---

# olympian model — hardware additions

> demuxer olympian | bluejay electronics LLC | planned

the olympian builds on init with expanded sensing, environmental awareness, and passive identification capabilities.

---

## thermal management

### active cooling fans
- small brushless DC fans (e.g. 5×5mm or 10×10mm blower type) mounted on dorsal hand + fingers
- purpose: sweat management and active temperature regulation during extended use
- driven via PWM (speed control) or simple GPIO on/off
- placement: back of hand (1–2 fans), possibly back of fingers for airflow across palm side
- TBD: exact fan model, RPM range, noise tolerance threshold

**firmware note:** add `firmware/src/fans.c` — `FANS_PWM_PINS[]`, duty cycle tied to thermal/sweat threshold or manual override via touchscreen

---

## vision & optical

### camera module
- compact camera module (e.g. OV2640, OV7670, or compatible) mounted on dorsal side of glove near wrist/knuckles
- purpose: visual input, gesture recognition assist, barcode/QR scanning, POV capture
- interface: DVP parallel or MIPI CSI depending on MCU choice
- TBD: resolution, frame rate, power budget

**firmware note:** add `firmware/src/camera.c` — stub for frame capture + streaming/processing

### IR emitter (upgraded)
- promoted from optional → standard on olympian
- 2× IR LEDs for wiimote-style cursor control + pointer tracking
- additionally: can be used for IR blasting (home automation, TV control)
- driver: simple GPIO high/low or PWM for modulation
- placement: fingertip or dorsal wrist area

**firmware note:** `firmware/src/ir.c` — extend with IR blasting (NEC/RC5 protocol support)

---

## electrical measurement

### GPIO voltage probe headers (voltmeter)
- 2–4 female GPIO headers exposed on the forearm module (GND + signal pins)
- purpose: easy in-field voltage/current measurement without removing the glove
- pairs with a small inline ADC (e.g. ADS1115) for accurate readings
- readings displayed on forearm touchscreen
- use cases: debugging embedded projects, field electronics work, hardware hacking

**firmware note:** add `firmware/src/probe.c` — ADC read + display output via touchscreen

---

## identification

### UHF RFID tag (passive)
- a passive UHF RFID tag embedded in the glove (not a reader)
- purpose: glove identification, authentication, asset tracking
- freq: 860–960 MHz (ISO 18000-6C / EPC Gen2)
- no power required — powered by reader field
- placement: back of hand or forearm module casing (avoid metal interference)
- tag selection: any standard EPC Gen2 inlay (e.g. Impinj Monza series or equivalent)

**note:** this is passive — the glove does NOT read tags, it IS the tag

---

## spatial awareness

### ultrasonic distance sensor
- purpose: dark-room spatial awareness, obstacle detection without visual input
- recommended: HC-SR04 or JSN-SR04T (waterproof variant for sweat resistance)
- range: 2cm – 400cm, ~1cm resolution
- interface: GPIO trigger + echo (2 pins)
- placement: dorsal wrist or forearm module front edge, pointing forward
- feedback: haptic pulse intensity scales with proximity (closer = stronger buzz)

**firmware note:** add `firmware/src/ultrasonic.c` — `ULTRASONIC_TRIG_PIN`, `ULTRASONIC_ECHO_PIN`, non-blocking echo timing via interrupt

---

## olympian open questions

- [ ] fan model + max RPM + noise floor acceptable during gaming
- [ ] camera resolution + whether full video stream or just snapshot mode
- [ ] probe header count (2 or 4 pins) + ADC resolution needed
- [ ] RFID tag model + antenna form factor (flexible inlay vs rigid)
- [ ] ultrasonic vs LiDAR (VL53L0X or similar) for distance — ultrasonic cheaper/simpler, LiDAR more accurate
- [ ] MCU upgrade needed? (more peripherals = more pins + processing)
- [ ] power budget impact of all new sensors combined
