# TODO
Checklist of every part of the chip-8 specification that need to be implemented.
Some can be implemented in a vaccuum, some depend on other parts/features.

## Operations
### Symbols
NNN: address
NN: 8-bit constant
N: 4-bit constant
X and Y: 4-bit register identifier
PC : Program Counter
I : 12bit register (For memory address) (Similar to void pointer);
VN: One of the 16 available variables. N may be 0 to F (hexadecimal);

- [ ] 0NNN - Call routine at address NNN  **(is this necessary?)**
- [x] 00E0 - `disp_clear()`
- [x] 00EE - `return;`
- [x] 1NNN - `goto NNN;`
- [x] 2NNN - `*(0xNNN)()`
- [x] 3XNN - `if (Vx == NN)`
- [x] 4XNN - `if (Vx != NN)`
- [x] 5XY0 - `if (Vx == Vy)`
- [x] 6XNN - `Vx = NN`
- [x] 7XNN - `Vx += NN`
- [x] 8XY0 - `Vx = Vy`
- [x] 8XY1 - `Vx |= Vy`
- [x] 8XY2 - `Vx &= Vy`
- [x] 8XY3 - `Vx ^= Vy`
- [x] 8XY4 - `Vx += Vy`
- [x] 8XY5 - `Vx -= Vy`
- [x] 8XY6 - `Vx >>= 1`
- [x] 8XY7 - `Vx = Vy - Vx`
- [x] 8XYE - `Vx <<= 1`
- [ ] 9XY0 - `if (Vx != Vy)`
- [ ] ANNN - `I = NNN`
- [ ] BNNN - `PC = V0 + NNN`
- [ ] CXNN - `Vx = rand() & NN`
- [ ] DXYN - `draw(Vx, Vy, N)`
- [ ] EX9E - `if (key() == Vx)`
- [ ] EXA1 - `if (key() != Vx)`
- [ ] FX07 - `Vx = get_delay()`
- [ ] FX0A - `Vx = get_key()`
- [ ] FX15 - `delay_timer(Vx)`
- [ ] FX18 - `sound_timer(Vx)`
- [ ] FX1E - `I += Vx`
- [ ] FX29 - `I = sprite_addr[Vx]`
- [ ] FX33 -
```
set_BCD(Vx)
*(I+0) = BCD(3);
*(I+1) = BCD(2);
*(I+2) = BCD(1);
```
- [ ] FX55 - `reg_dump(Vx, &I)`
- [ ] FX65 - `reg_load(Vx, &I)`

## Peripherals and misc
- [ ] Refactor opcode handling (change from if/else to switch/case)
- [ ] Working 64x32 screen with an interface to manipulate the individual pixels / sprites
- [ ] Delay Timer
- [ ] Sound Timer
- [ ] Input through Hex keyboard
- [x] Stack
- [x] Flash / ROM loading
- [x] Memory / RAM

## References
- https://chip8.gulrak.net/
- [wikipedia](https://en.wikipedia.org/wiki/CHIP-8#Opcode_table)
