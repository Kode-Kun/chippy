# Chippy

This repository, although named `chippy`, includes also `chasm`, a small toy assembler made to aid in the development of `chippy` by facilitating the task of writing test programs.
`chippy` is an emulator / virtual machine[^1] of the [chip-8](https://en.wikipedia.org/wiki/CHIP-8) architecture, written completely in C, with graphics and input done with [raylib](https://www.raylib.com/).
`chasm` is an assembler that compiles `chasm` assembly into chip-8 bytecode. For a reference on the language / mnemonics, consult the `todo.md` file or, better yet, my [blogpost](https://readthemanpage.dev/posts/into_assemblers) about this project. It is a toy assembler made for testing and educational purposes only. It currently only implements the base CHIP-8 instructions.

`chippy` and by extension `chasm` implement only the base CHIP-8 specification, as it is the most well-documented one. SUPER-CHIP support might come in the future.

## Building
`chippy`'s only external dependency is [raylib](https://www.raylib.com/).
`chasm` has no external dependencies.
### Linux
To build it on Linux, download raylib on your machine, then copy the `lib/` and `include/` directories into this repo's directory.
### Mac
To build it on mac, install raylib on your machine with homebrew and change the `RAYLIB_PATH` in the Makefile to the path of your raylib installation.

Then, simply run

```bash
make chippy
```
for the chip-8 virtual machine,
```bash
make chasm
```
for the chasm assembler and
```bash
make write
```
for the write utility.

[^1]: It may be more accurate to refer to any chip-8 implementation as a virtual machine, since it's not actually emulating any real hardware. However, since writing a chip-8 virtual machine is considered a great entry point into the world of emulator development, I often refer to it as an emulator.

`chippy` is licensed under the GPL-3.0 (see LICENSE.md).
