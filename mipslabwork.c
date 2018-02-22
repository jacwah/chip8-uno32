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

int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

int timeoutcount = 0;
int prime = 1234567;

/* Interrupt Service Routine */
void user_isr( void )
{
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  // KYPD functions
  // C4 C3 C2 C1 GD VC
  // R4 R3 R2 R1 GD VC

  // KYPD pins
  // 01 02 03 04 05 06
  // 07 08 09 10 11 12

  // chipKit pins
  // 05 04 03 02 01 00
  // 31 30 29 28 27 26

  // Registers
  // D1 F1 D0 D8 F3 F2
  // E5 E4 E3 E2 E1 E0

  // COLx
  TRISDCLR = 1 << 1;
  TRISFCLR = 1 << 1;
  TRISDCLR = 1 << 0;
  TRISDCLR = 1 << 8;

  PORTDSET = 1 << 1;
  PORTFSET = 1 << 1;
  PORTDSET = 1 << 0;
  PORTDSET = 1 << 8;

  // ROWx
  TRISESET = 1 << 5;
  TRISESET = 1 << 4;
  TRISESET = 1 << 3;
  TRISESET = 1 << 2;

  // VCC
  TRISFCLR = 1 << 2;
  TRISECLR = 1 << 0;

  PORTFSET = 1 << 2;
  PORTESET = 1 << 0;

  // GND
  TRISFCLR = 1 << 3;
  TRISECLR = 1 << 1;

  PORTFCLR = 1 << 3;
  PORTECLR = 1 << 1;
}

void keypad_read(unsigned char keys[16])
{
  PORTDCLR = 1 << 1;
  keys[3 + 3 * 4] = !(PORTE & (1 << 5));
  keys[2 + 3 * 4] = !(PORTE & (1 << 4));
  keys[1 + 3 * 4] = !(PORTE & (1 << 3));
  keys[0 + 3 * 4] = !(PORTE & (1 << 2));
  PORTDSET = 1 << 1;

  PORTFCLR = 1 << 1;
  keys[3 + 2 * 4] = !(PORTE & (1 << 5));
  keys[2 + 2 * 4] = !(PORTE & (1 << 4));
  keys[1 + 2 * 4] = !(PORTE & (1 << 3));
  keys[0 + 2 * 4] = !(PORTE & (1 << 2));
  PORTFSET = 1 << 1;

  PORTDCLR = 1 << 0;
  keys[3 + 1 * 4] = !(PORTE & (1 << 5));
  keys[2 + 1 * 4] = !(PORTE & (1 << 4));
  keys[1 + 1 * 4] = !(PORTE & (1 << 3));
  keys[0 + 1 * 4] = !(PORTE & (1 << 2));
  PORTDSET = 1 << 0;

  PORTDCLR = 1 << 8;
  keys[3 + 0 * 4] = !(PORTE & (1 << 5));
  keys[2 + 0 * 4] = !(PORTE & (1 << 4));
  keys[1 + 0 * 4] = !(PORTE & (1 << 3));
  keys[0 + 0 * 4] = !(PORTE & (1 << 2));
  PORTDSET = 1 << 8;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  unsigned char keys[16];
  char str[17];
  int i;

  keypad_read(keys);

  for (i = 0; i < 16; i++)
    str[i] = keys[i] ? 'x' : 'o';

  str[16] = '\0';

  display_string(0, str);
  display_update();

  delay(100);
}
