# Chippy

`chippy` is an emulator of the [chip-8](https://en.wikipedia.org/wiki/CHIP-8) architecture, written completely in C, with graphics and windowing done with [raylib](https://www.raylib.com/).

`chippy` implements the SUPER-CHIP specification, as it is the most well-documented one.
It aims to be fully compatible with, at the very least, any program written using the [octo](https://github.com/JohnEarnest/Octo) assembler.

## Building
`chippy`'s only external dependency is [raylib](https://www.raylib.com/). To build it, install raylib on your machine, change the `RAYLIB_PATH` variable in the Makefile to the directory containing your raylib object, and copy [*raylib.h*](https://github.com/raysan5/raylib/blob/master/src/raylib.h) into this repo's directory.
Then, simply run
```bash
make
```

`chippy` is licensed under the GPL-3.0 (see LICENSE.md)