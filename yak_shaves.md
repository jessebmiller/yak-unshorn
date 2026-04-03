# Yak Shaves

The todo list ;)

## Cool ideas

- Any function that fits a code generation signature can ad hoc be used
  to insert its output into buffers.
- Ad hoc interface to add things to this file
- code analysis that pulls or copies TODO items from source code to this   file
  - Would need a way to track it uniquely, like with an ID
- integrated TYM namer for cool unique tokens
- command to expand macro deffinitions
  - put cursor on macro <expand-macro> and it replaces the macro
    definition with its expanded text
- modes for reading pseudo file systems and performing operations
  based on them.
  - like sending signals or updating environment variables in /proc or
    whatever you can do in /sys

## Bugs

## To finish

## Refactoring

- undo wrapping of SDL3 events. Commit to SDL3 (revisit if I see a need)
- or factor out the layers, wrapping SDL3 (a la Eskil Steenberg)
  - Platform layer
  - Drawing layer
  - Text
  - UI Toolkit

# Bunuelib

- make a bunch of data structures that can be used dynamically or with
  fixed capacities. Set the default with a #define flag and have
  explicit functions for each option

# Strategy for stable interfaces

use versioned names and #define flags to select a version
the interface can be expanded with new versiond names without breaking
old code

like 

```
draw_circle_v0(int radius);

#ifdef BUNUELIB_API_V0
  #define draw_circle draw_circle_v0
...
```

then you realize it should be float, and take a line thickness so you
add 


```
draw_circle_v0(int radius);
draw_circle_v1(float radius, float thickness);

#ifdef BUNUELIB_API_V0
  #define draw_circle draw_circle_v0
#endif

#ifdef BUNUELIB_API_V1
  #define draw_circle draw_circle_v1
#endif
```

The user can also cherry pick versions they want by defining their own
names for whichever it is.

The commitment as a library developer is that the interface for any
published name will never change.



