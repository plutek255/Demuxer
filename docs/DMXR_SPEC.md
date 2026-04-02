# .dmxr language specification
# demuxer scripting language v0.2
# bluejay electronics LLC

---

## overview

.dmxr is the demuxer runtime scripting language. it's a multi-paradigm embedded DSL that combines:
- **cpp** — outer shell, #defines, typed variables, block structure
- **runtime blocks** — start() / loop() lifecycle (arduino/game-engine style)
- **tsx** — component tree for touchscreen UI
- **svg + @pixelart** — inline graphics and sprite definitions
- **py::** — python-style logic blocks (indented scoping)
- **md::** — markdown content blocks (docs, labels, tooltips)
- **html::** — raw html terminal windows
- **.dmxi** — json bundles that package embedded sublanguage files

file extensions:
  .dmxr   runtime script (this language)
  .dmxb   binary definition (existing)
  .dmxi   embedded sublanguage bundle (json, see below)
  .dmxs   routes/server definition

---

## file structure

a .dmxr file has this shape:

```
#include <demuxer.h>
#include "myapp.dmxi"

#define APP_NAME "My Glove App"
#define VERSION  "1.0"

// global typed vars
int   counter = 0
float flexThreshold = 0.65
bool  wheelActive = false
str   activeProfile = "default"

// ── UI component definitions ──────────────────────────────────────────────
@sprite icon_grip {
  size: 8x8
  palette: #0af, #fff, #000, transparent
  pixels:
    . . # # # # . .
    . # . . . . # .
    # . . . . . . #
    # . . . . . . #
    # . . . . . . #
    . # . . . . # .
    . . # . . # . .
    . . . # # . . .
}

@svg badge_active {
  <circle cx="8" cy="8" r="6" fill="#0af" opacity="0.8"/>
  <circle cx="8" cy="8" r="3" fill="#fff"/>
}

// ── component tree ────────────────────────────────────────────────────────
<Screen id="main" width={240} height={135} bg="#080c10">

  <Panel id="header" x={0} y={0} w={240} h={18} bg="#0a1628">
    <Label text={APP_NAME} x={4} y={3} font="mono" size={11} color="#0af" />
    <Badge icon={badge_active} x={220} y={4} visible={connected} />
  </Panel>

  <FlexBar id="fingers" x={4} y={22} fingers={5} height={28} />

  <Button id="btn_mode" x={4} y={60} w={80} h={22}
    label="MODE" color="#0066ff"
    onPress={ py::
      if wheelActive:
        wheelActive = False
        send("CMD:WHEEL:OFF")
      else:
        wheelActive = True
        send("CMD:WHEEL:ON")
    }
  />

  <Button id="btn_cal" x={90} y={60} w={80} h={22}
    label="RECAL" color="#ffcc00"
    onPress={ send("CMD:CALIBRATE") }
  />

  <Terminal id="log" x={4} y={88} w={232} h={40}
    source="log.dmxi::terminal"
    scrollable={true}
    font="mono" size={10} color="#4a6080"
  />

</Screen>

// ── lifecycle ─────────────────────────────────────────────────────────────
runtime start() {
  print("runtime start — " + APP_NAME)
  loadProfile(activeProfile)
  screen.render("main")

  py::
    import json
    print(f"dmxr v{VERSION} booting")
    counter = 0
}

runtime loop() {
  sensor.readAll()

  py::
    if sensor.flex[1] > flexThreshold:
      gesture = "GRIP"
      haptic.pulse(1, 150, 60)

    if sensor.gesture == "JERK_FORWARD":
      send("CMD:MELEE")

  screen.update()
}
```

---

## syntax reference

### outer shell (cpp-style)

```
#include <demuxer.h>         // system includes
#include "file.dmxi"         // bundle includes

#define NAME "value"         // string constant
#define COUNT 5              // numeric constant

int   myInt   = 0            // typed global variable
float myFloat = 0.0
bool  myBool  = false
str   myStr   = "hello"
```

### runtime blocks

```
runtime start() {
  // runs once at boot
  // initialize state, render first screen, load profiles
}

runtime loop() {
  // runs every tick (~100Hz)
  // read sensors, update UI, send commands
}

runtime onGesture(gesture g) {
  // fires when a gesture is detected
  // g.type, g.magnitude available
}

runtime onFoil(int contact) {
  // fires when a foil contact is pressed
  // contact = 0–4 (A B C D RECAL)
}

runtime onSleep() {
  // fires before device sleeps
}
```

### tsx components

```
<Screen id="name" width={n} height={n} bg="color">
  // children
</Screen>

<Panel id="name" x={n} y={n} w={n} h={n} bg="color" border="color">
  // children
</Panel>

<Label text="string or {var}" x={n} y={n} font="mono|sans" size={n} color="color" />

<Button id="name" x={n} y={n} w={n} h={n} label="text" color="color"
  onPress={ /* inline or py:: block */ }
  onHold={ /* fires after 500ms hold */ }
/>

<FlexBar id="name" x={n} y={n} fingers={5} height={n} />
// renders 5 animated flex sensor bars

<FoilDots id="name" x={n} y={n} count={5} />
// renders foil contact indicators

<Sprite id="name" src={icon_name} x={n} y={n} scale={n} />
// renders a @sprite definition

<Terminal id="name" x={n} y={n} w={n} h={n}
  source="file.dmxi::key"
  scrollable={bool} font="mono" size={n} color="color"
/>
// renders an embedded terminal from a .dmxi bundle

<GestureLabel id="name" x={n} y={n} />
// auto-renders the current gesture name

<BatteryBar id="name" x={n} y={n} w={n} h={n} />
// renders battery % bar

<SVG id="name" x={n} y={n}>
  // inline svg — any valid svg elements
  <circle cx="8" cy="8" r="6" fill="#0af"/>
</SVG>
```

component props:
- string values: `label="text"` or `label={variable}`
- numeric: `x={4}` — always braces
- bool: `visible={true}` or `visible={boolVar}`
- event handlers: `onPress={ ... }` — inline expression or `py::` block

### py:: blocks

py:: blocks use python indentation scoping. they can appear:
- inside runtime blocks
- inside component event handlers (onPress, onHold)
- at the top level (runs at parse time)

```
py::
  if sensor.flex[1] > 0.6:
    haptic.pulse(1, 200, 80)
    counter += 1
  
  for i in range(5):
    if sensor.foil[i]:
      send(f"CMD:FOIL:{i}")

  def myFunc(x):
    return x * 2

  result = myFunc(counter)
```

available globals in py:: blocks:
  sensor.flex[0..4]       normalized flex values
  sensor.foil[0..4]       foil contact booleans
  sensor.gesture          current gesture string
  sensor.imu              [ax, ay, gz]
  haptic.pulse(f, hz, ms) fire haptic on finger f
  send(str)               send command to arduino
  screen.render(id)       render a screen
  screen.update()         refresh current screen
  loadProfile(name)       switch profile
  print(str)              trace output
  json                    json module available
  all dmxr globals        accessible by name

### md:: blocks

markdown content — renders as styled text in Terminal or Label components

```
md::
  # my heading
  
  this is **bold** and this is `code`
  
  - item one
  - item two
  
  > a quote block
```

### html:: blocks

raw html — renders inside a Terminal component as an iframe-style window

```
html::
  <div style="color:#0af;font-family:monospace">
    <h3>terminal output</h3>
    <pre id="log">waiting...</pre>
  </div>
```

### @sprite blocks

pixel art sprite definitions — 1 char = 1 pixel

```
@sprite name {
  size: WxH              // e.g. 8x8, 16x16, 32x32
  scale: 2               // optional display scale multiplier
  palette: #color, ...   // colors in order, . = transparent
  pixels:
    . . # # # # . .      // use palette index 0=first color, #=second, etc
    . # . . . . # .      // or use . for transparent
    ...
}
```

palette mapping:
  .  = transparent (skip)
  0  = palette[0]
  1  = palette[1]
  #  = last non-transparent color (shorthand)
  any hex = literal color override for that pixel

### @svg blocks

named svg fragments — referenced by components

```
@svg name {
  // any valid svg elements, no outer <svg> tag needed
  // viewBox is inferred from context
  <circle cx="8" cy="8" r="6" fill="#0af"/>
  <path d="M2,8 L14,8" stroke="#fff" stroke-width="2"/>
}
```

---

## .dmxi bundle format

a .dmxi file is a json bundle that packages embedded sublanguage content.
referenced in .dmxr as: `#include "file.dmxi"` then `source="file.dmxi::key"`

```json
{
  "name": "my-bundle",
  "version": "1.0",
  "entries": {
    "terminal": {
      "type": "html",
      "content": "<div style='color:#0af'>hello terminal</div>"
    },
    "docs": {
      "type": "md",
      "content": "# docs\n\nthis is **markdown** content"
    },
    "config": {
      "type": "json",
      "content": { "key": "value" }
    },
    "script": {
      "type": "py",
      "content": "print('hello from embedded python')\ncounter = 42"
    }
  }
}
```

entry types: html | md | py | json | svg | sprite

---

## built-in globals

### sensor.*
  sensor.flex[0..4]      float 0.0–1.0 per finger
  sensor.foil[0..4]      bool per contact
  sensor.gesture         str gesture name
  sensor.imu             {ax, ay, gz}
  sensor.calibrated      bool
  sensor.batteryPct      int 0–100
  sensor.connected       bool

### haptic.*
  haptic.pulse(finger, hz, ms)
  haptic.autoclickStart(finger, hz)
  haptic.autoclickStop(finger)

### screen.*
  screen.render(id)
  screen.update()
  screen.lock()
  screen.unlock()

### profile.*
  profile.load(name)
  profile.save(name)
  profile.list()          returns str[]

### device.*
  device.send(cmd)        send raw command string to arduino
  device.mode             "gamepad" | "keyboard" | "custom"
  device.wheelActive      bool
  device.fingerWalkActive bool

---

## comments

  // single line
  /* multi
     line */
  # also valid single line (py style)

---

## example — full minimal app

see: examples/hello_glove.dmxr
see: examples/wheel_mode.dmxr
see: examples/finger_walk.dmxr
