/* kypd.h

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

void kypd_init(void);
void kypd_read(unsigned char keys[16]);
void kypd_printdebug(void);
extern const char idx2key[];
