# Chippy

`chippy` is an emulator of the [chip-8](https://en.wikipedia.org/wiki/CHIP-8) architecture, written completely in C, with graphics and windowing done with [raylib](https://www.raylib.com/).

`chippy` implements the SUPER-CHIP specification, as it is the most well-documented one.
It aims to be fully compatible with, at the very least, any program written using the [octo](https://github.com/JohnEarnest/Octo) assembler.

## Building
`chippy`'s only external dependency is [raylib](https://www.raylib.com/).
### Linux
To build it on Linux, download raylib on your machine, then copy the `lib/` and `include/` directories into this repo's directory.
### Mac
To build it on mac, install raylib on your machine with homebrew

Then, simply run

```bash
make
```

`chippy` is licensed under the GPL-3.0 (see LICENSE.md).
