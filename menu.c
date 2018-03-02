#include <stdlib.h>
#include "mipslab.h"
#include "c8code.h"
#include "menu.h"
#include "display.h"
#include "kypd.h"
#include <string.h>

unsigned char keymap_default[] = {
    0x0, 0x1, 0x2, 0x3,
    0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB,
    0xC, 0xD, 0xE, 0xF,
};

// CHIP-8 controls: 1, 4 for left and C, D for right.
unsigned char keymap_pong[] = {
    0x1, 0x0, 0x2, 0x3,
    0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xC,
    0xB, 0xE, 0xF, 0xD,
};

struct prog menu[] = {
    { "FONT", FONT, sizeof(FONT), 0x000, keymap_default },
    { "FONT2", FONT2, sizeof(FONT2), 0x000, keymap_default },
    { "PONG3", PONG3, sizeof(PONG3), 0x22A, keymap_pong },
    { "TETRIS", TETRIS, sizeof(TETRIS), 0x000, keymap_default },
};

#define NUM_PROG (sizeof(menu) / sizeof(menu[0]))

void writec(unsigned char *image, int line, int col, char c)
{
    for (int sl = 0; sl < 8; sl++)
        image[line*128 + 8 * col + sl] = font[8 * c + sl];
}

void writes(unsigned char *image, int line, int col, const char *s)
{
    for (int i = 0; s[i]; i++) {
        writec(image, line, col + i, s[i]);
    }
}

int key2item[] = {
    0, 4, -1, -1,
    1, 5, -1, -1,
    2, 6, -1, -1,
    3, 7, -1, -1,
};

int item2key[] = {
    0, 4, 8, 12,
    1, 5, 9, 13,
};

struct prog *menu_pick(void)
{
    unsigned char image[4*128];

    memset(image, 0, sizeof(image));

    for (int i = 0; i < NUM_PROG; i++) {
        int line = i % 4;
        int col = 0;

        if (i >= 4)
            col = 8;

        writec(image, line, col + 0, idx2key[item2key[i]]);
        writes(image, line, col + 2, menu[i].name);
    }
    disp_draw(image);

    unsigned char keys[16];
    int pressed = -1;

    while (pressed < 0 || key2item[pressed] < 0 || key2item[pressed] >= NUM_PROG) {
        kypd_read(keys);

        for (int i = 0; i < 16; i++)
            if (keys[i])
                pressed = i;
    }

    return menu + key2item[pressed];
}
