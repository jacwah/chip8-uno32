/* display.h

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

void disp_init(void);
void disp_convert(unsigned char *image, const unsigned char *bytemap);
void disp_draw(unsigned char *image);
void disp_clear();
