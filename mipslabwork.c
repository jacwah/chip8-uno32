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
#include "c8e.h"

struct c8 vm;

unsigned char testprog[] = {
  0x6a, 0x00, 0x6b, 0x1d, 0x6c, 0x0d, 0x00, 0xe0, 0xfa, 0x29, 0xdb, 0xc5,
  /*0xf0, 0x0a,*/ 0x7a, 0x01, 0x12, 0x06
};

/* Interrupt Service Routine */
void user_isr( void )
{
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  //kypd_init();
  disp_init();
  c8_load(&vm, testprog, sizeof(testprog));
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  unsigned char dbuf[32*128];
  unsigned char image[4*128];

  kypd_read(vm.keys);
  c8_step(&vm);
  c8_tick(&vm);

  for (int y = 0; y < DHEIGHT; y++) {
    for (int x = 0; x < DWIDTH; x++)
      dbuf[y * DWIDTH * 2 + x] = vm.display[y * DWIDTH + x];
    for (int x = DWIDTH; x < DWIDTH * 2; x++)
      dbuf[y * DWIDTH * 2 + x] = 0;
  }

  disp_convert(image, dbuf);
  disp_draw(image);

  delay(10);
}
