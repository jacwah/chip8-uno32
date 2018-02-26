#include <pic32mx.h>
#include "mipslab.h"

void kypd_init(void)
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
  /* TRISDCLR = 1 << 1; */
  /* TRISFCLR = 1 << 1; */
  /* TRISDCLR = 1 << 0; */
  /* TRISDCLR = 1 << 8; */

  PORTDSET = 1 << 1;
  PORTFSET = 1 << 1;
  PORTDSET = 1 << 0;
  PORTDSET = 1 << 8;

  TRISDSET = 1 << 1;
  TRISFSET = 1 << 1;
  TRISDSET = 1 << 0;
  TRISDSET = 1 << 8;

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

void kypd_read(unsigned char keys[16])
{
  // Set cols to input when not reading
  // seems to work ok.
  // Reading PORTE once works better than four times per col
  int pe[4];

  TRISDCLR = 1 << 1;
  PORTDCLR = 1 << 1;
  pe[3] = PORTE;
  PORTDSET = 1 << 1;
  TRISDSET = 1 << 1;

  TRISFCLR = 1 << 1;
  PORTFCLR = 1 << 1;
  pe[2] = PORTE;
  PORTFSET = 1 << 1;
  TRISFSET = 1 << 1;

  TRISDCLR = 1 << 0;
  PORTDCLR = 1 << 0;
  pe[1] = PORTE;
  PORTDSET = 1 << 0;
  TRISDSET = 1 << 0;

  TRISDCLR = 1 << 8;
  PORTDCLR = 1 << 8;
  pe[0] = PORTE;
  PORTDSET = 1 << 8;
  TRISDSET = 1 << 8;

  // Set keys
  for (int col = 0; col < 4; col++)
    for (int row = 0; row < 4; row++)
      keys[row + col*4] = !(pe[col] & (1 << (2 + row)));

  /* keys[3 + 0 * 4] = !(pe & (1 << 5)); */
  /* keys[2 + 0 * 4] = !(pe & (1 << 4)); */
  /* keys[1 + 0 * 4] = !(pe & (1 << 3)); */
  /* keys[0 + 0 * 4] = !(pe & (1 << 2)); */
}

void kypd_printdebug(void)
{
  static const char idx2key[] = {
    '1', '4', '7', '0',
    '2', '5', '8', 'F',
    '3', '6', '9', 'E',
    'A', 'B', 'C', 'D',
  };

  unsigned char keys[16];
  char str[5];
  int c, r;

  kypd_read(keys);

  str[4] = '\0';

  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++)
      str[c] = keys[c + r*4] ? idx2key[c*4 + r] : ' ';

    display_string(r, str);
  }
}
