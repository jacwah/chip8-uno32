/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include "kypd.h"
#include "display.h"
#include "debug.h"
#include "c8e.h"
#include "menu.h"

struct c8 vm;
const struct prog *prog;

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

    for (int y = 0; y < DHEIGHT; y++) {
      for (int x = 0; x < DWIDTH; x++)
        dbuf[y * DWIDTH * 2 + x] = vm.display[y * DWIDTH + x];
      for (int x = DWIDTH; x < DWIDTH * 2; x++)
        dbuf[y * DWIDTH * 2 + x] = 0;
    }

    if (debug_enabled())
      debug_keys(dbuf, DWIDTH * 2, vm.keys);

    disp_convert(image, dbuf);
    disp_draw(image);
  }
}
