/* debug.c

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

#include <pic32mx.h>
#include "debug.h"

void debug_init(void)
{
    TRISDSET = 1 << 11;
}

/* Draw currently pressed keys to right side of screen. */
void debug_keys(unsigned char *bitmap, unsigned width, const unsigned char keys[16])
{
  for (int y = 0; y < 4*4; y++)
    for (int x = width / 2; x < width / 2 + 4*4; x++) {
      int ky = y / 4;
      int kx = (x - width / 2) / 4;
      bitmap[y * width + x] = keys[ky*4 + kx];
    }
}

int debug_enabled(void)
{
    return !!(PORTD & (1 << 11));
}
