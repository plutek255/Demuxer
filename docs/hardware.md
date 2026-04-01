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
