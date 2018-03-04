/* debug.h

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

void debug_init(void);
int debug_enabled(void);
void debug_keys(unsigned char *bitmap, unsigned width, const unsigned char keys[16]);
