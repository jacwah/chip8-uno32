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
  // 42 41 01 00 GD VC
  // 17 16 15 14 GD VC

  // Registers
  // BE BC F3 F2
  // BA B8 B4 B2

  // Use as digital
  AD1PCFGSET = 1 << 0x0;
  AD1PCFGSET = 1 << 0x1;

  AD1PCFGSET = 1 << 0x2;
  AD1PCFGSET = 1 << 0x4;
  AD1PCFGSET = 1 << 0x8;
  AD1PCFGSET = 1 << 0xA;
  AD1PCFGSET = 1 << 0xC;
  AD1PCFGSET = 1 << 0xE;

  TRISFCLR = 1 << 0x2;
  TRISFCLR = 1 << 0x3;
  TRISBCLR = 1 << 0x0;
  TRISBCLR = 1 << 0x1;

  PORTFSET = 1 << 0x2;
  PORTFSET = 1 << 0x3;
  PORTBSET = 1 << 0x0;
  PORTBSET = 1 << 0x1;

  TRISBSET = 1 << 0x2;
  TRISBSET = 1 << 0x4;
  TRISBSET = 1 << 0x8;
  TRISBSET = 1 << 0xA;
}

void kypd_read(unsigned char keys[16])
{
  int col[4];

  PORTFCLR = 1 << 0x2;
  col[0] = PORTB;
  PORTFSET = 1 << 0x2;
  delay(1);

  PORTFCLR = 1 << 0x3;
  col[1] = PORTB;
  PORTFSET = 1 << 0x3;
  delay(1);

  PORTBCLR = 1 << 0x0;
  col[2] = PORTB;
  PORTBSET = 1 << 0x0;
  delay(1);

  PORTBCLR = 1 << 0x1;
  col[3] = PORTB;
  PORTBSET = 1 << 0x1;
  delay(1);

  for (int x = 0; x < 4; x++) {
    keys[0*4 + x] = !(col[x] & (1 << 0x2));
    keys[1*4 + x] = !(col[x] & (1 << 0x4));
    keys[2*4 + x] = !(col[x] & (1 << 0x8));
    keys[3*4 + x] = !(col[x] & (1 << 0xA));
  }
}

void kypd_printdebug(void)
{
  static const char idx2key[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '0', 'F', 'E', 'D',
  };

  unsigned char keys[16];
  char str[5];

  kypd_read(keys);

  str[4] = '\0';

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++)
      str[x] = keys[x + y*4] ? idx2key[x + y*4] : ' ';

    display_string(y, str);
  }
}
