# yak-unshorn

A software development environment to make your own. A yak to shave.

Like the lisp machine part of emacs but polyglot

## What it is

The bones of a text editor and development environment

A text based (GUI not terminal) code execution interface

## What it isn't

A productive IDE (you'll need to build one in it)

Something with modal editing, or a keybindings system (you'll have
to add that if you want it)

## Developing

Install `node-devel` for node.js runtime

## Architecture

https://claude.ai/chat/446db313-769f-4102-be33-a231ff4d2da2

kernel layer:
- Language runtimes (polyglot plugin hub)
  - Inter-runtime dynamic linking/loading ffi rpc etc. whatever
    processes register their call interfaces with the kernel,
    kernel works out fastest way for other processes to make that call
    other processes might need to register as callers to be dynamically
    linked
  - consider PLT/GOT Hooking, LD_AUDIT Interposition, Wrapper Functions with Trampolines, or libffi
  - look at the python c api (numpi, etc.), GraalVM Polyglot, lua embedding, plugin systems
- fundamental subsystems that can benefit from being fast native calls
  - Windowing
  - Input events
  - File I/O
  - Text buffers (rope)
  - Text display/redisplay
  - Code build/execute/load

Plugins
- Modes, keybindings
- Command events (insert character, delete line, save file, etc.)
- Syntax highlighting
- Theming (font, colors)
- LSPs 

## Starting up

1. reads config file
2. starts configured runtimes
3. runs configured init processes in the appropriate runtime
   - Maybe text buffers are implemented in C and keybindings
     are implemented in javascript

