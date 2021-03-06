/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file modified 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <string.h>
#include "kypd.h"
#include "display.h"
#include "debug.h"
#include "c8e.h"
#include "menu.h"

// To make linker happy with -lc
void *stdout;

struct c8 vm;
const struct prog *prog;

enum {
    DMODE_RAW,
    DMODE_TILT,
};


/* Interrupt Service Routine */
void user_isr( void )
{
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  kypd_init();
  disp_init();

  prog = menu_pick();
  c8_load(&vm, prog->code, prog->len);

  disp_clear();

  TRISDSET = 1 << 10;

  // 256 prescale, off
  T2CON = 7 << 4;
  T4CON = 7 << 4;

  TMR2 = 0;
  TMR4 = 0;

  // 60 Hz clock
  PR2 = (80000000 / 256) / 60;

  // Emulator clock
  PR4 = (80000000 / 256) / 800;

  IFSCLR(0) = (1 << 8) | (1 << 16);

  // prio 7, subprio 3
  IPCSET(2) = 0x1f;
  IPCSET(4) = 0x1f;

  // On
  T2CONSET = 1 << 15;
  T4CONSET = 1 << 15;
}

void dbuf_blit(unsigned char *dbuf, const unsigned char *display, int dmode)
{
  if (dmode == DMODE_RAW) {
    for (int y = 0; y < DHEIGHT; y++) {
      for (int x = 0; x < DWIDTH; x++)
        dbuf[y * DWIDTH * 2 + x] = vm.display[y * DWIDTH + x];
      for (int x = DWIDTH; x < DWIDTH * 2; x++)
        dbuf[y * DWIDTH * 2 + x] = 0;
    }
  } else if (dmode == DMODE_TILT) {
    memset(dbuf, 0, 32 * 128);

    // Draw play area
    for (int y = 0; y < 32; y++) {
      for (int x = 0; x < 64; x++) {
        int vy = 31 - x / 2;
        int vx = y / 2 + 3*8;
        dbuf[y * 128 + x] = vm.display[vy * 64 + vx];
      }

      // Draw score area
      if (y < 3*8)
        for (int x = 64; x < 64 + 32; x++) {
          int vy = 64 + 32 - x - 1;
          int vx = y + 5 * 8;
          dbuf[y * 128 + x] = vm.display[vy * 64 + vx];
        }
    }
  }
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  int t60hz = 0, emuclock = 0;

  if (IFS(0) & (1 << 8)) {
    t60hz = 1;
    IFSCLR(0) = 1 << 8;
  }

  if (IFS(0) & (1 << 16)) {
    emuclock = 1;
    IFSCLR(0) = 1 << 16;
  }

  if (emuclock) {
    unsigned char keys[16];

    kypd_read(keys);

    for (int i = 0; i < 16; i++)
      vm.keys[prog->keymap[i]] = keys[i];

    c8_step(&vm);
  }

  if (t60hz)
    c8_tick(&vm);

  if (prog->syncpoint == 0 && t60hz
      || prog->syncpoint == vm.pc) {
    unsigned char dbuf[32*128];
    unsigned char image[4*128];
    int dmode = DMODE_RAW;

    if (debug_enabled())
      debug_keys(dbuf, DWIDTH * 2, vm.keys);

    if (PORTD & (1 << 10))
      dmode = DMODE_TILT;

    dbuf_blit(dbuf, vm.display, dmode);
    disp_convert(image, dbuf);
    disp_draw(image);
  }
}
