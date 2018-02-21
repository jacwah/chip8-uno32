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
  if (IFS(0) & (1 << 8)) {
    IFSCLR(0) = 1 << 8;
    timeoutcount++;

    if (timeoutcount == 10) {
      timeoutcount = 0;

      time2string(textstring, mytime);
      display_string(3, textstring);
      display_update();
      tick(&mytime);
    }
  }

  if (IFS(0) & (1 << 7)) {
    IFSCLR(0) = 1 << 7;
    PORTE++;
  }
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  volatile int *trise = (int *) 0xbf886100;

  *trise &= ~0xff;

  TRISDSET = 0xfe0;

  PR2 = 31250;	// 0.1 s
  TMR2 = 0;

  IECSET(0) = 1 << 8;
  IFSCLR(0) = 1 << 8;
  IPCSET(2) = 0x1f;	// prio=7, sub=3

  T2CON = (1 << 15)	// ON
	| (7 << 4)	// 256 prescale
	;

  IECSET(0) = 1 << 7;
  IPCSET(1) = 0x1f << 24;

  PORTE = 0;

  enable_interrupt();
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  prime = nextprime(prime);
  display_string(0, itoaconv(prime));
  display_update();
}
