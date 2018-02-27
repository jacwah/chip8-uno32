/* DISPLAY CONFIGURATION
 * Only pages 4-7 are used by display
 * We remap pages to get 0-4 and use page addressing
 */

#include <pic32mx.h>

#define MODE_CMD	(PORTFCLR = 0x10)
#define MODE_DATA 	(PORTFSET = 0x10)

#define RESET_START	(PORTGCLR = 0x200)
#define RESET_END	(PORTGSET = 0x200)

#define VDD_ON		(PORTFCLR = 0x40)
#define VDD_OFF		(PORTFSET = 0x40)

#define VBAT_ON		(PORTFCLR = 0x20)
#define VBAT_OFF	(PORTFSET = 0x20)

/* SPI commands */
#define CMD_DISPLAY			0xA4
#define CMD_OFF				0xAE
#define CMD_ON 				0xAF

#define CMD_LOW_COL_ADDR	0x00
#define CMD_HIGH_COL_ADDR	0x10
// In example code, 0x22 is used instead for
// page addressing which is not documented
// for page addressing mode.
#define CMD_PAGE_ADDR		0xB0

#define CMD_CHARGE_PUMP		0x8D
#define CMD_CHARGE_PUMP_ON	0x14

#define CMD_PRECHARGE		0xD9

#define CMD_SEG_REVERSE		0xA0
#define CMD_COM_REVERSE		0xC8

#define CMD_COM_PINS		0xDA
// This should be 0x22 according to data sheet
// but is 0x20 in example code.
#define CMD_COM_PINS_REMAP	0x22

static void spin(int n)
{
  while (n--);
}

static void send_sync(unsigned char data)
{
  // Wait for empty transmit buffer
  while(!(SPI2STAT & 0x08));

  SPI2BUF = data;

  // Wait for transmit
  while(!(SPI2STAT & 1));
}

void disp_init(void)
{
  MODE_CMD;
  spin(10);
  VDD_ON;
  spin(1000000);

  send_sync(CMD_OFF);
  RESET_START;
  spin(10);
  RESET_END;
  spin(10);

  send_sync(CMD_CHARGE_PUMP);
  send_sync(CMD_CHARGE_PUMP_ON);

  send_sync(CMD_PRECHARGE);
  send_sync(0xF1);

  VBAT_ON;
  spin(10000000);

  send_sync(CMD_SEG_REVERSE);
  send_sync(CMD_COM_REVERSE);

  send_sync(CMD_COM_PINS);
  send_sync(CMD_COM_PINS_REMAP);

  send_sync(CMD_ON);
}

void disp_draw(void)
{
  static int count = 0;

  for (int page = 0; page < 4; page++) {
	MODE_CMD;

	send_sync(CMD_PAGE_ADDR | page);

	send_sync(CMD_LOW_COL_ADDR | 0);
	send_sync(CMD_HIGH_COL_ADDR | 0);

	MODE_DATA;
	for (int col = 0; col < 128; col++) {
	  unsigned char seg = 0x00;
	  if ((page % 2) == (count % 2))
		seg = ((col / 8) % 2) ? 0x00 : 0xFF;
	  else
		seg = ((col / 8) % 2) ? 0xFF : 0x00;
	  send_sync(seg);
	}
  }

  count++;
}
