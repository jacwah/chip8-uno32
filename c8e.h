/* c8e.h
 
   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

#define PSTART  0x200

#define DWIDTH  64
#define DHEIGHT 32

struct c8 {
    unsigned pc;
    unsigned sp;
    unsigned ri;
    unsigned timer;
    unsigned sound;
    unsigned stack[0x10];
    unsigned char regs[0x10];
    unsigned char keys[0x10];
    unsigned char prev_keys[0x10];
    unsigned char display[DWIDTH * DHEIGHT];
    unsigned char program[0x1000];
};

int c8_load(struct c8 *vm, const unsigned char *program, unsigned plen);
void c8_tick(struct c8 *vm);
int c8_step(struct c8 *vm);
