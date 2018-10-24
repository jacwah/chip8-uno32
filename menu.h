/* menu.h

   This file written 2018 by Jacob Wahlgren

   For copyright and licensing, see file COPYING */

struct prog {
    char *name;
    unsigned char *code;
    unsigned len;
    unsigned syncpoint;
    unsigned char *keymap;
};
struct prog *menu_pick(void);
