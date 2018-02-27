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
#define CMD_PA_PAGE_ADDR	0xB0
#define CMD_HV_PAGE_ADDR	0x22

#define CMD_CHARGE_PUMP		0x8D
#define CMD_CHARGE_PUMP_ON	0x14

#define CMD_PRECHARGE		0xD9

#define CMD_SEG_REVERSE		0xA0
#define CMD_COM_REVERSE		0xC8

#define CMD_COM_PINS		0xDA
// Should be 0x22 according to data sheet?
#define CMD_COM_PINS_REMAP	0x20

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

  for (int line = 0; line < 4; line++) {
	MODE_CMD;
	send_sync(CMD_HV_PAGE_ADDR);
	send_sync(line);
	send_sync(0);

	// mixed vh and page addr mode??
	send_sync(CMD_HIGH_COL_ADDR);

	MODE_DATA;

	for (int pos = 0; pos < 16; pos++) {
	  for (int row = 0; row < 8; row++) {
		unsigned char pixels = 0x0f;
		if ((line * 4 * 16 + pos * 16 + row) % (4 * 16 * 8) == count++)
		  pixels = 0;
		send_sync(pixels);
	  }
	}
  }
}
