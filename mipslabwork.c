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
  kypd_init();
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  kypd_printdebug();
  display_update();
  delay(100);
}
