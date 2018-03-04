/* c8e.c

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

#include "c8e.h"
#include <stdlib.h>
#include <string.h>

/* Instruction elements */
#define ETYPE(op) ((op & 0xF000) >> 12)
#define EREGX(op) ((op & 0x0F00) >> 8)
#define EREGY(op) ((op & 0x00F0) >> 4)
#define EADDR(op) ((op & 0x0FFF))
#define ELIMM(op) ((op & 0x00FF))
#define ESIMM(op) ((op & 0x000F))

/* Instruction types */
#define TCALLU  0x0     // AC
#define TJUMP   0x1
#define TCALL   0x2
#define TSEQI   0x3
#define TSNEI   0x4
#define TSEQR   0x5     // AS
#define TSETI   0x6
#define TADDI   0x7
#define TMATHR  0x8     // AM
#define TSNER   0x9     // AS
#define TSETM   0xA
#define TRJUMP  0xB
#define TRAND   0xC
#define TDRAW   0xD
#define TKEY    0xE     // AK
#define TUTIL   0xF     // AU

/* Additional instruction identifiers */
#define ACRET   0x0EE
#define ACCLS   0x0E0

#define ASREG   0x0

#define AMSET   0x0
#define AMOR    0x1
#define AMAND   0x2
#define AMXOR   0x3
#define AMADD   0x4
#define AMSUB   0x5
#define AMSHR   0x6
#define AMSUB2  0x7
#define AMSHL   0xE

#define AKEQ    0x9E
#define AKNE    0xA1

#define AUGTM   0x07
#define AUKEY   0x0A
#define AUSTM   0x15
#define AUSND   0x18
#define AUADDM  0x1E
#define AUDIG   0x29
#define AUBCD   0x33
#define AUSTR   0x55
#define AULD    0x65

static const unsigned char font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

/* Draw the 8*n sprite at ri to (x,y). */
static int draw(unsigned char *display, unsigned char *sprite, int x, int y, int n)
{
    int collision = 0;

    for (int dy = 0; dy < n && y + dy < DHEIGHT; dy++) {
        int row = sprite[dy];

        for (int dx = 0; dx < 8 && x + dx < DWIDTH; dx++) {
            int val = row & (0x80 >> dx);
            int idx = (y + dy) * DWIDTH + x + dx;

            if (val) {
                if (display[idx]) {
                    collision = 1;
                    display[idx] = 0;
                } else {
                    display[idx] = 1;
                }
            }
        }
    }

    return collision;
}

/* Load a program and initialize interpreter. */
int c8_load(struct c8 *vm, const unsigned char *program, unsigned plen)
{
    if (plen + PSTART > sizeof(vm->program))
        return -1;

    memset(vm, 0, sizeof(*vm));
    vm->pc = PSTART;

    memcpy(vm->program, font, sizeof(font));
    memcpy(vm->program + PSTART, program, plen);

    return 0;
}

/* Manage timers. Call this function at 60 Hz. */
void c8_tick(struct c8 *vm)
{
    if (vm->timer > 0)
        vm->timer--;

    if (vm->sound > 0)
        vm->sound--;
}

/* Execute a single instruction.
 * Should be called at a frequency appropriate for the program being executed.
 */
int c8_step(struct c8 *vm)
{
    int status = 0;
    int op = (vm->program[vm->pc] << 8) | vm->program[vm->pc + 1];

    vm->pc += 2;

    switch (ETYPE(op)) {
        case TCALLU:
            switch (EADDR(op)) {
                case ACRET:
                    if (vm->sp == 0)
                        status = -1;
                    else
                        vm->pc = vm->stack[--vm->sp];
                    break;
                case ACCLS:
                    memset(vm->display, 0, sizeof(vm->display));
                    break;
                default:
                    // Legacy instruction: call native subroutine
                    status = -1;
                    break;
            }
            break;
        case TJUMP:
            vm->pc = EADDR(op);
            break;
        case TCALL:
            if (vm->sp == sizeof(vm->stack) / sizeof(vm->stack[0]))
                status = -1;
            else {
                vm->stack[vm->sp++] = vm->pc;
                vm->pc = EADDR(op);
            }
            break;
        case TSEQI:
            if (vm->regs[EREGX(op)] == ELIMM(op))
                vm->pc += 2;
            break;
        case TSNEI:
            if (vm->regs[EREGX(op)] != ELIMM(op))
                vm->pc += 2;
            break;
        case TSEQR:
            if (ESIMM(op) == ASREG) {
                if (vm->regs[EREGX(op)] == vm->regs[EREGY(op)])
                    vm->pc += 2;
            } else
                status = -1;
            break;
        case TSETI:
            vm->regs[EREGX(op)] = ELIMM(op);
            break;
        case TADDI:
            vm->regs[EREGX(op)] += ELIMM(op);
            break;
        case TMATHR:
            switch (ESIMM(op)) {
                case AMSET:
                    vm->regs[EREGX(op)] = vm->regs[EREGY(op)];
                    break;
                case AMOR:
                    vm->regs[EREGX(op)] |= vm->regs[EREGY(op)];
                    break;
                case AMAND:
                    vm->regs[EREGX(op)] &= vm->regs[EREGY(op)];
                    break;
                case AMXOR:
                    vm->regs[EREGX(op)] ^= vm->regs[EREGY(op)];
                    break;
                case AMADD:
                    {
                    // Sum with overflow flag in VF
                    unsigned sum = vm->regs[EREGX(op)] + vm->regs[EREGY(op)];
                    vm->regs[EREGX(op)] = sum;
                    vm->regs[0xF] = sum > 0xFF;
                    }
                    break;
                case AMSUB:
                    {
                    // if VX > VY then VF=1, else VF=0 (!burrow)
                    int flag = vm->regs[EREGX(op)] > vm->regs[EREGY(op)];
                    vm->regs[EREGX(op)] -= vm->regs[EREGY(op)];
                    vm->regs[0xF] = flag;
                    }
                    break;
                case AMSHR:
                    {
                    int lsb = vm->regs[EREGX(op)] & 1;
                    vm->regs[EREGX(op)] >>= 1;
                    vm->regs[0xF] = lsb;
                    }
                    break;
                case AMSUB2:
                    {
                    int flag = vm->regs[EREGY(op)] > vm->regs[EREGX(op)];
                    vm->regs[EREGX(op)] = vm->regs[EREGY(op)] - vm->regs[EREGX(op)];
                    vm->regs[0xF] = flag;
                    }
                    break;
                case AMSHL:
                    {
                    int msb = vm->regs[EREGX(op)] & 0x80;
                    vm->regs[EREGX(op)] <<= 1;
                    vm->regs[0xF] = msb;
                    }
                    break;
                default:
                    status = -1;
                    break;
            }
            break;
        case TSNER:
            if (ESIMM(op) == ASREG) {
                if (vm->regs[EREGX(op)] != vm->regs[EREGY(op)])
                    vm->pc += 2;
            } else
                status = -1;
            break;
        case TSETM:
            vm->ri = EADDR(op);
            break;
        case TRJUMP:
            vm->pc = EADDR(op) + vm->regs[0x0];
            break;
        case TRAND:
            // srand() should have been called before this to set a seed
            vm->regs[EREGX(op)] = rand() & ELIMM(op);
            break;
        case TDRAW:
            vm->regs[0xF] = draw(vm->display, vm->program + vm->ri,
                                 vm->regs[EREGX(op)], vm->regs[EREGY(op)], ESIMM(op));
            break;
        case TKEY:
            switch (ELIMM(op)) {
                case AKEQ:
                    if (vm->keys[vm->regs[EREGX(op)] & 0xF])
                        vm->pc += 2;
                    break;
                case AKNE:
                    if (!vm->keys[vm->regs[EREGX(op)] & 0xF])
                        vm->pc += 2;
                    break;
                default:
                    status = -1;
                    break;
            }
            break;
        case TUTIL:
            switch (ELIMM(op)) {
                case AUGTM:
                    vm->regs[EREGX(op)] = vm->timer;
                    break;
                case AUKEY:
                    {
                    int pressed = -1;

                    for (int i = 0; i < sizeof(vm->keys); i++)
                        if (vm->keys[i] && !vm->prev_keys[i]) {
                            pressed = i;
                            break;
                        }

                    memcpy(vm->prev_keys, vm->keys, sizeof(vm->keys));

                    if (pressed == -1)
                        vm->pc -= 2; // Wait until key press
                    else
                        vm->regs[EREGX(op)] = pressed;
                    }
                    break;
                case AUSTM:
                    vm->timer = vm->regs[EREGX(op)];
                    break;
                case AUSND:
                    vm->sound = vm->regs[EREGX(op)];
                    break;
                case AUADDM:
                    // VF set on overflow (undocumented feature
                    // according to Wikipedia page).
                    vm->ri += vm->regs[EREGX(op)];
                    break;
                case AUDIG:
                    vm->ri = (0xF & vm->regs[EREGX(op)]) * 5;
                    break;
                case AUBCD:
                    vm->program[vm->ri + 0] = vm->regs[EREGX(op)] / 100;
                    vm->program[vm->ri + 1] = (vm->regs[EREGX(op)] % 100) / 10;
                    vm->program[vm->ri + 2] = vm->regs[EREGX(op)] % 10;
                    break;
                case AUSTR:
                    memcpy(vm->program + vm->ri, vm->regs, EREGX(op) + 1);
                    vm->ri += EREGX(op) + 1;
                    break;
                case AULD:
                    memcpy(vm->regs, vm->program + vm->ri, EREGX(op) + 1);
                    vm->ri += EREGX(op) + 1;
                    break;
                default:
                    status = -1;
                    break;
            }
            break;
        default:
            status = -1;
            break;
    }

    return status;
}
