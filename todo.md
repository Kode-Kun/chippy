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
- [x] 00E0 (CLEAR) - `disp_clear()`
- [x] 00EE (RETURN) - `return;`
- [x] 1NNN (B NNN)- `goto NNN;`
- [x] 2NNN (CALL NNN)- `*(0xNNN)()`
- [x] 3XNN (IFE VX, #NN)- `if (Vx == NN)`
- [x] 4XNN (IFNE VX, #NN)- `if (Vx != NN)`
- [x] 5XY0 (IFE VX, VY)- `if (Vx == Vy)`
- [x] 6XNN (MOV VX, #NN)- `Vx = NN`
- [x] 7XNN (ADD VX, #NN)- `Vx += NN`
- [x] 8XY0 (MOV VX, VY)- `Vx = Vy`
- [x] 8XY1 (OR VX, VY)- `Vx |= Vy`
- [x] 8XY2 (AND VX, VY)- `Vx &= Vy`
- [x] 8XY3 (XOR VX, VY)- `Vx ^= Vy`
- [x] 8XY4 (ADD VX, VY)- `Vx += Vy`
- [x] 8XY5 (SUB VX, VY)- `Vx -= Vy`
- [x] 8XY6 (LLS VX)- `Vx >>= 1`
- [x] 8XY7 (SUS VX, VY)- `Vx = Vy - Vx`
- [x] 8XYE (LRS VX)- `Vx <<= 1`
- [x] 9XY0 (IFNE VX, VY)- `if (Vx != Vy)`
- [x] ANNN (MOV I, NNN)- `I = NNN`
- [x] BNNN (BA NNN)- `PC = V0 + NNN`
- [x] CXNN (RAND VX, #NN)- `Vx = rand() & NN`
- [ ] DXYN (DRW VX, VY, #N)- `draw(Vx, Vy, N)`
- [ ] EX9E (IKE VX)- `if (key() == Vx)`
- [ ] EXA1 (IKN VX)- `if (key() != Vx)`
- [ ] FX07 (GDL VX)- `Vx = get_delay()`
- [ ] FX0A (GTK VX)- `Vx = get_key()`
- [ ] FX15 (SDL VX)- `delay_timer(Vx)`
- [ ] FX18 (SSD VX)- `sound_timer(Vx)`
- [ ] FX1E (ADD I, VX)- `I += Vx`
- [ ] FX29 (SPR VX)- `I = sprite_addr[Vx]`
- [ ] FX33 (BCD VX)-
```
set_BCD(Vx)
*(I+0) = BCD(3);
*(I+1) = BCD(2);
*(I+2) = BCD(1);
```
- [ ] FX55 (DPR VX)- `reg_dump(Vx, &I)`
- [ ] FX65 (LDR VX)- `reg_load(Vx, &I)`

## Peripherals and misc
- [ ] Working 64x32 screen with an interface to manipulate the individual pixels / sprites
- [ ] Delay Timer
- [ ] Sound Timer
- [ ] Input through Hex keyboard
- [ ] Check for invalid operations (example: 0x5XY1 instead of 0x5XY0)
- [x] Stack
- [x] Flash / ROM loading
- [x] Memory / RAM
- [x] Refactor opcode handling (change from if/else to switch/case)

## References
- https://chip8.gulrak.net/
- [wikipedia](https://en.wikipedia.org/wiki/CHIP-8#Opcode_table)
