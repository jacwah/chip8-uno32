#include <stdlib.h>
#include "mipslab.h"
#include "c8code.h"
#include "menu.h"
#include "display.h"
#include "kypd.h"
#include <string.h>

struct prog menu[] = {
    { "FONT", FONT, sizeof(FONT) },
    { "FONT2", FONT2, sizeof(FONT2) },
    { "PONG3", PONG3, sizeof(PONG3) },
};

#define NUM_PROG (sizeof(menu) / sizeof(menu[0]))

void writec(unsigned char *image, int line, int col, char c)
{
    for (int sl = 0; sl < 8; sl++)
        image[line*128 + 8 * (15 - col) + 8 - sl] = font[8 * c + sl];
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
