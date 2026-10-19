# .dmxr language specification
# demuxer scripting language v0.2
# bluejay electronics LLC

---

### overview

.dmxr is the demuxer runtime scripting language. it's a multi-paradigm embedded DSL that combines:
- **cpp** 🐦 outer shell, #defines, typed variables, block structure
- **runtime blocks** 🐦 start() / loop() lifecycles (arduino/game-engine style)
- **tsx** 🐦 component tree for touchscreen UI
- **svg + @pixelart** 🐦 inline graphics and sprite definitions
- **py::** 🐦 python-style logic blocks (indented scoping)
- **md::** 🐦 markdown content blocks (docs, labels, tooltips)
- **html::** 🐦 raw html terminal windows
- **.dmxi** 🐦 json bundles that package embedded sublanguage files

file extensions:
  .dmxr   runtime script (this language)
  .dmxb   binary definition (existing)
  .dmxi   embedded sublanguage bundle (json, see below)
  .dmxs   routes/server definition

---

### file structure

A .dmxr file has this shape:

```c
#include <demuxer.h>
#include "myapp.dmxi"

#define APP_NAME "My Glove App"
#define VERSION "1.0"

// global typed vars
int    counter = 0
float  flexThreshold = 0.65
bool   wheelActive = false
str    activeProfile = "default"

// 🌈 UI component definitions 🌈 🌈 🌈 🌈 🌈 🌈 🌈 🌈 🌈 🌈..

// Planned Features for Demuxer Olympian v2.0:
/*
 * The next version will potentially include:
 * - Enhanced wireless capabilities
 * - Expanded IoT integration
 * - Additional haptic feedback modules
 * - Improved gesture recognition algorithms
 * - Support for custom expansion modules
 */
```