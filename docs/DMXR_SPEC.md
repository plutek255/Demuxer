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

