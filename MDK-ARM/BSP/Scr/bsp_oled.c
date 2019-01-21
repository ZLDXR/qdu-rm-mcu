#include "bsp_oled.h"
#include "main.h"
#include "spi.h"

#define OLED_CMD_Set()	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)
#define OLED_CMD_Clr()	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)

#define OLED_RST_Set()	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)
#define OLED_RST_Clr()	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)

typedef enum {
	OLED_WriteCMD = 0,
	OLED_WriteData = 1,
}OLED_WriteTypedef;

typedef struct {
	uint8_t column;
	uint8_t page;
}OLED_CursorTtpedef;

const uint8_t oled_font[95][8] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}, /* " ", 0 */
	{0x00,0x00,0x00,0xcf,0xcf,0x00,0x00,0x00,}, /* "!", 1 */
	{0x00,0x0c,0x06,0x00,0x0c,0x06,0x00,0x00,}, /* """, 2 */
	{0x24,0xe4,0x3c,0x27,0xe4,0x3c,0x27,0x24,}, /* "#", 3 */
	{0x00,0x20,0x46,0xf9,0x9f,0x62,0x04,0x00,}, /* "$", 4 */
	{0x06,0x09,0xc6,0x30,0x0c,0x63,0x90,0x60,}, /* "%", 5 */
	{0x00,0x00,0x6e,0x91,0xa9,0x46,0xa0,0x00,}, /* "&", 6 */
	{0x00,0x00,0x00,0x1c,0x0e,0x00,0x00,0x00,}, /* "'", 7 */
	{0x00,0x00,0x3c,0x42,0x81,0x00,0x00,0x00,}, /* "(", 8 */
	{0x00,0x00,0x00,0x81,0x42,0x3c,0x00,0x00,}, /* ")", 9 */
	{0x00,0x10,0x54,0x38,0x38,0x54,0x10,0x00,}, /* "*", 10 */
	{0x00,0x10,0x10,0xfc,0x10,0x10,0x00,0x00,}, /* "+", 11 */
	{0x00,0x00,0x00,0xc0,0x60,0x00,0x00,0x00,}, /* ",", 12 */
	{0x00,0x00,0x10,0x10,0x10,0x10,0x00,0x00,}, /* "-", 13 */
	{0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,0x00,}, /* ".", 14 */
	{0x00,0x00,0x00,0xc0,0x38,0x07,0x00,0x00,}, /* "/", 15 */
	{0x00,0x00,0x7c,0x92,0x8a,0x7c,0x00,0x00,}, /* "0", 16 */
	{0x00,0x00,0x00,0x84,0xfe,0x80,0x00,0x00,}, /* "1", 17 */
	{0x00,0x00,0x8c,0xc2,0xa2,0x9c,0x00,0x00,}, /* "2", 18 */
	{0x00,0x00,0x44,0x92,0x92,0x6c,0x00,0x00,}, /* "3", 19 */
	{0x00,0x20,0x38,0x24,0xfe,0x20,0x00,0x00,}, /* "4", 20 */
	{0x00,0x00,0x5e,0x92,0x92,0x62,0x00,0x00,}, /* "5", 21 */
	{0x00,0x00,0x78,0x94,0x92,0x62,0x00,0x00,}, /* "6", 22 */
	{0x00,0x00,0x82,0x62,0x1a,0x06,0x00,0x00,}, /* "7", 23 */
	{0x00,0x00,0x6c,0x92,0x92,0x6c,0x00,0x00,}, /* "8", 24 */
	{0x00,0x00,0x8c,0x52,0x32,0x1c,0x00,0x00,}, /* "9", 25 */
	{0x00,0x00,0x00,0x6c,0x6c,0x00,0x00,0x00,}, /* ":", 26 */
	{0x00,0x00,0x80,0xec,0x6c,0x00,0x00,0x00,}, /* ";", 27 */
	{0x00,0x00,0x10,0x28,0x44,0x00,0x00,0x00,}, /* "<", 28 */
	{0x00,0x00,0x24,0x24,0x24,0x24,0x00,0x00,}, /* "=", 29 */
	{0x00,0x00,0x00,0x44,0x28,0x10,0x00,0x00,}, /* ">", 30 */
	{0x00,0x00,0x0c,0xa2,0x92,0x1c,0x00,0x00,}, /* "?", 31 */
	{0x00,0x3c,0x42,0x99,0xa5,0xa2,0x3c,0x00,}, /* "@", 32 */
	{0x00,0xe0,0x1c,0x12,0x12,0x1c,0xe0,0x00,}, /* "A", 33 */
	{0x00,0xfe,0x92,0x92,0x9c,0x90,0x60,0x00,}, /* "B", 34 */
	{0x00,0x38,0x44,0x82,0x82,0x82,0x44,0x00,}, /* "C", 35 */
	{0x00,0xfe,0x82,0x82,0x82,0x82,0x7c,0x00,}, /* "D", 36 */
	{0x00,0xfe,0x92,0x92,0x92,0x92,0x92,0x00,}, /* "E", 37 */
	{0x00,0xfe,0x12,0x12,0x12,0x12,0x02,0x00,}, /* "F", 38 */
	{0x00,0x7c,0x82,0x92,0x92,0x72,0x00,0x00,}, /* "G", 39 */
	{0x00,0xfe,0x10,0x10,0x10,0x10,0xfe,0x00,}, /* "H", 40 */
	{0x00,0x82,0x82,0xfe,0x82,0x82,0x00,0x00,}, /* "I", 41 */
	{0x00,0x82,0x82,0x7e,0x02,0x02,0x00,0x00,}, /* "J", 42 */
	{0x00,0xfe,0x10,0x28,0x44,0x82,0x00,0x00,}, /* "K", 43 */
	{0x00,0xfe,0x80,0x80,0x80,0x80,0x00,0x00,}, /* "L", 44 */
	{0xfc,0x02,0x04,0xf8,0x04,0x02,0xfc,0x00,}, /* "M", 45 */
	{0x00,0xfe,0x04,0x18,0x30,0x40,0xfe,0x00,}, /* "N", 46 */
	{0x00,0x7c,0x82,0x82,0x82,0x82,0x7c,0x00,}, /* "O", 47 */
	{0x00,0x00,0xfe,0x12,0x12,0x0c,0x00,0x00,}, /* "P", 48 */
	{0x00,0x00,0x3c,0x42,0xc2,0xbc,0x00,0x00,}, /* "Q", 49 */
	{0x00,0x00,0xfe,0x32,0x52,0x8c,0x00,0x00,}, /* "R", 50 */
	{0x00,0x00,0x4c,0x92,0x92,0x64,0x00,0x00,}, /* "S", 51 */
	{0x00,0x02,0x02,0xfe,0x02,0x02,0x00,0x00,}, /* "T", 52 */
	{0x00,0x7e,0x80,0x80,0x80,0x80,0x7e,0x00,}, /* "U", 53 */
	{0x00,0x0c,0x30,0xc0,0x30,0x0c,0x00,0x00,}, /* "V", 54 */
	{0x7c,0x80,0x80,0x78,0x80,0x80,0x7c,0x00,}, /* "W", 55 */
	{0x00,0x84,0x48,0x30,0x30,0x48,0x84,0x00,}, /* "X", 56 */
	{0x00,0x06,0x08,0xf0,0x08,0x06,0x00,0x00,}, /* "Y", 57 */
	{0x00,0x00,0xc2,0xa2,0x92,0x8e,0x00,0x00,}, /* "Z", 58 */
	{0x00,0x00,0xfe,0x82,0x82,0x82,0x00,0x00,}, /* "[", 59 */
	{0x00,0x00,0x06,0x18,0x60,0x80,0x00,0x00,}, /* "\", 60 */
	{0x00,0x00,0x82,0x82,0x82,0xfe,0x00,0x00,}, /* "]", 61 */
	{0x00,0x30,0x0c,0x02,0x0c,0x30,0x00,0x00,}, /* "^", 62 */
	{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,}, /* "_", 63 */
	{0x00,0x00,0x04,0x0c,0x18,0x00,0x00,0x00,}, /* "`", 64 */
	{0x00,0x00,0x60,0x90,0x90,0xe0,0x00,0x00,}, /* "a", 65 */
	{0x00,0x00,0xf8,0xa0,0xe0,0x00,0x00,0x00,}, /* "b", 66 */
	{0x00,0x00,0x60,0x90,0x90,0x00,0x00,0x00,}, /* "c", 67 */
	{0x00,0x00,0xe0,0xa0,0xf8,0x00,0x00,0x00,}, /* "d", 68 */
	{0x00,0x00,0x70,0xa8,0xa8,0x90,0x00,0x00,}, /* "e", 69 */
	{0x00,0x00,0x10,0xf8,0x14,0x00,0x00,0x00,}, /* "f", 70 */
	{0x00,0x00,0xd8,0xa4,0x7c,0x00,0x00,0x00,}, /* "g", 71 */
	{0x00,0x00,0xf8,0x20,0xe0,0x00,0x00,0x00,}, /* "h", 72 */
	{0x00,0x00,0x00,0xe8,0x00,0x00,0x00,0x00,}, /* "i", 73 */
	{0x00,0x00,0x40,0x90,0x74,0x00,0x00,0x00,}, /* "j", 74 */
	{0x00,0x00,0xf8,0x60,0x90,0x00,0x00,0x00,}, /* "k", 75 */
	{0x00,0x00,0x78,0x80,0x80,0x00,0x00,0x00,}, /* "l", 76 */
	{0x00,0xe0,0x10,0xe0,0x10,0xe0,0x00,0x00,}, /* "m", 77 */
	{0x00,0x00,0xf0,0x10,0x10,0xe0,0x00,0x00,}, /* "n", 78 */
	{0x00,0x00,0x60,0x90,0x90,0x60,0x00,0x00,}, /* "o", 79 */
	{0x00,0x00,0xf0,0x48,0x48,0x30,0x00,0x00,}, /* "p", 80 */
	{0x00,0x00,0x30,0x48,0x48,0xf0,0x00,0x00,}, /* "q", 81 */
	{0x00,0x00,0x00,0xf0,0x20,0x10,0x00,0x00,}, /* "r", 82 */
	{0x00,0x00,0x90,0xa8,0xa8,0x48,0x00,0x00,}, /* "s", 83 */
	{0x00,0x10,0x10,0xf8,0x90,0x90,0x00,0x00,}, /* "t", 84 */
	{0x00,0x00,0x78,0x80,0x80,0xf8,0x00,0x00,}, /* "u", 85 */
	{0x00,0x18,0x60,0x80,0x60,0x18,0x00,0x00,}, /* "v", 86 */
	{0x00,0x38,0xc0,0x38,0xc0,0x38,0x00,0x00,}, /* "w", 87 */
	{0x00,0x88,0x50,0x20,0x50,0x88,0x00,0x00,}, /* "x", 88 */
	{0x00,0x8c,0x50,0x20,0x10,0x0c,0x00,0x00,}, /* "y", 89 */
	{0x00,0x88,0xc8,0xa8,0x98,0x88,0x00,0x00,}, /* "z", 90 */
	{0x00,0x00,0x10,0x7c,0x82,0x00,0x00,0x00,}, /* "{", 91 */
	{0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,}, /* "|", 92 */
	{0x00,0x00,0x00,0x82,0x7c,0x10,0x00,0x00,}, /* "}", 93 */
	{0x00,0x08,0x04,0x04,0x08,0x10,0x10,0x08,}, /* "~", 94 */
};

static uint8_t oled_gram[8][128] = {
	{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcf,0xcf,0x00,0x00,0x00,
	0x00,0x0c,0x06,0x00,0x0c,0x06,0x00,0x00,0x24,0xe4,0x3c,0x27,0xe4,0x3c,0x27,0x24,
	0x00,0x20,0x46,0xf9,0x9f,0x62,0x04,0x00,0x06,0x09,0xc6,0x30,0x0c,0x63,0x90,0x60,
	0x00,0x00,0x6e,0x91,0xa9,0x46,0xa0,0x00,0x00,0x00,0x00,0x1c,0x0e,0x00,0x00,0x00,
	0x00,0x00,0x3c,0x42,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0x42,0x3c,0x00,0x00,
	0x00,0x10,0x54,0x38,0x38,0x54,0x10,0x00,0x00,0x10,0x10,0xfc,0x10,0x10,0x00,0x00,
	0x00,0x00,0x00,0xc0,0x60,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x00,0x00,
	0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0xc0,0x38,0x07,0x00,0x00,},
	{
	0x00,0x00,0x7c,0x92,0x8a,0x7c,0x00,0x00,0x00,0x00,0x00,0x84,0xfe,0x80,0x00,0x00,
	0x00,0x00,0x8c,0xc2,0xa2,0x9c,0x00,0x00,0x00,0x00,0x44,0x92,0x92,0x6c,0x00,0x00,
	0x00,0x20,0x38,0x24,0xfe,0x20,0x00,0x00,0x00,0x00,0x5e,0x92,0x92,0x62,0x00,0x00,
	0x00,0x00,0x78,0x94,0x92,0x62,0x00,0x00,0x00,0x00,0x82,0x62,0x1a,0x06,0x00,0x00,
	0x00,0x00,0x6c,0x92,0x92,0x6c,0x00,0x00,0x00,0x00,0x8c,0x52,0x32,0x1c,0x00,0x00,
	0x00,0x00,0x00,0x6c,0x6c,0x00,0x00,0x00,0x00,0x00,0x80,0xec,0x6c,0x00,0x00,0x00,
	0x00,0x00,0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x00,0x24,0x24,0x24,0x24,0x00,0x00,
	0x00,0x00,0x00,0x44,0x28,0x10,0x00,0x00,0x00,0x00,0x0c,0xa2,0x92,0x1c,0x00,0x00,},
	{
	0x00,0x3c,0x42,0x99,0xa5,0xa2,0x3c,0x00,0x00,0xe0,0x1c,0x12,0x12,0x1c,0xe0,0x00,
	0x00,0xfe,0x92,0x92,0x9c,0x90,0x60,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x44,0x00,
	0x00,0xfe,0x82,0x82,0x82,0x82,0x7c,0x00,0x00,0xfe,0x92,0x92,0x92,0x92,0x92,0x00,
	0x00,0xfe,0x12,0x12,0x12,0x12,0x02,0x00,0x00,0x7c,0x82,0x92,0x92,0x72,0x00,0x00,
	0x00,0xfe,0x10,0x10,0x10,0x10,0xfe,0x00,0x00,0x82,0x82,0xfe,0x82,0x82,0x00,0x00,
	0x00,0x82,0x82,0x7e,0x02,0x02,0x00,0x00,0x00,0xfe,0x10,0x28,0x44,0x82,0x00,0x00,
	0x00,0xfe,0x80,0x80,0x80,0x80,0x00,0x00,0xfc,0x02,0x04,0xf8,0x04,0x02,0xfc,0x00,
	0x00,0xfe,0x04,0x18,0x30,0x40,0xfe,0x00,0x00,0x7c,0x82,0x82,0x82,0x82,0x7c,0x00,},
	{
	0x00,0x00,0xfe,0x12,0x12,0x0c,0x00,0x00,0x00,0x00,0x3c,0x42,0xc2,0xbc,0x00,0x00,
	0x00,0x00,0xfe,0x32,0x52,0x8c,0x00,0x00,0x00,0x00,0x4c,0x92,0x92,0x64,0x00,0x00,
	0x00,0x02,0x02,0xfe,0x02,0x02,0x00,0x00,0x00,0x7e,0x80,0x80,0x80,0x80,0x7e,0x00,
	0x00,0x0c,0x30,0xc0,0x30,0x0c,0x00,0x00,0x7c,0x80,0x80,0x78,0x80,0x80,0x7c,0x00,
	0x00,0x84,0x48,0x30,0x30,0x48,0x84,0x00,0x00,0x06,0x08,0xf0,0x08,0x06,0x00,0x00,
	0x00,0x00,0xc2,0xa2,0x92,0x8e,0x00,0x00,0x00,0x00,0xfe,0x82,0x82,0x82,0x00,0x00,
	0x00,0x00,0x06,0x18,0x60,0x80,0x00,0x00,0x00,0x00,0x82,0x82,0x82,0xfe,0x00,0x00,
	0x00,0x30,0x0c,0x02,0x0c,0x30,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,},
	{
	0x00,0x00,0x04,0x0c,0x18,0x00,0x00,0x00,0x00,0x00,0x60,0x90,0x90,0xe0,0x00,0x00,
	0x00,0x00,0xf8,0xa0,0xe0,0x00,0x00,0x00,0x00,0x00,0x60,0x90,0x90,0x00,0x00,0x00,
	0x00,0x00,0xe0,0xa0,0xf8,0x00,0x00,0x00,0x00,0x00,0x70,0xa8,0xa8,0x90,0x00,0x00,
	0x00,0x00,0x10,0xf8,0x14,0x00,0x00,0x00,0x00,0x00,0xd8,0xa4,0x7c,0x00,0x00,0x00,
	0x00,0x00,0xf8,0x20,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0xe8,0x00,0x00,0x00,0x00,
	0x00,0x00,0x40,0x90,0x74,0x00,0x00,0x00,0x00,0x00,0xf8,0x60,0x90,0x00,0x00,0x00,
	0x00,0x00,0x78,0x80,0x80,0x00,0x00,0x00,0x00,0xe0,0x10,0xe0,0x10,0xe0,0x00,0x00,
	0x00,0x00,0xf0,0x10,0x10,0xe0,0x00,0x00,0x00,0x00,0x60,0x90,0x90,0x60,0x00,0x00,},
	{
	0x00,0x00,0xf0,0x48,0x48,0x30,0x00,0x00,0x00,0x00,0x30,0x48,0x48,0xf0,0x00,0x00,
	0x00,0x00,0x00,0xf0,0x20,0x10,0x00,0x00,0x00,0x00,0x90,0xa8,0xa8,0x48,0x00,0x00,
	0x00,0x10,0x10,0xf8,0x90,0x90,0x00,0x00,0x00,0x00,0x78,0x80,0x80,0xf8,0x00,0x00,
	0x00,0x18,0x60,0x80,0x60,0x18,0x00,0x00,0x00,0x38,0xc0,0x38,0xc0,0x38,0x00,0x00,
	0x00,0x88,0x50,0x20,0x50,0x88,0x00,0x00,0x00,0x8c,0x50,0x20,0x10,0x0c,0x00,0x00,
	0x00,0x88,0xc8,0xa8,0x98,0x88,0x00,0x00,0x00,0x00,0x10,0x7c,0x82,0x00,0x00,0x00,
	0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0x7c,0x10,0x00,0x00,
	0x00,0x08,0x04,0x04,0x08,0x10,0x10,0x08,}
};

OLED_CursorTtpedef oled_cursor = {0};

static volatile bool modified = true;

static void OLED_WriteByte(uint8_t data, OLED_WriteTypedef type) {
	switch(type) {
		case OLED_WriteCMD:
			OLED_CMD_Clr();
		break;
		
		case OLED_WriteData:
			OLED_CMD_Set();
		break;
	}
	HAL_SPI_Transmit(&hspi1, &data, 1, 10);
}

BSP_StatusTypedef OLED_DisplayOn(void) {
	OLED_WriteByte(0x8d, OLED_WriteCMD);
	OLED_WriteByte(0x14, OLED_WriteCMD);
	OLED_WriteByte(0xaf, OLED_WriteCMD);
	return BSP_OK;
}

BSP_StatusTypedef OLED_DisplayOff(void) {
	OLED_WriteByte(0x8d, OLED_WriteCMD);
	OLED_WriteByte(0x10, OLED_WriteCMD);
	OLED_WriteByte(0xae, OLED_WriteCMD);
	return BSP_OK;
}

BSP_StatusTypedef OLED_Refresh(void) {
	if (modified) {
		for (uint8_t i = 0; i < 8; i++)	{
			OLED_WriteByte((0xb0 + i), OLED_WriteCMD); /* Set page address y. */
			OLED_WriteByte((2 & 0x0f), OLED_WriteCMD); /* Set column low address. */
			OLED_WriteByte(((2 >> 4) + 0x10), OLED_WriteCMD); /* Set column high address. */
			for (uint8_t n = 0; n < 128; n++) {
				OLED_WriteByte(oled_gram[i][n], OLED_WriteData);
			}
		}
		modified = false;
	}
	
	return BSP_OK;
}

BSP_StatusTypedef OLED_SetAll(OLED_PenTypedef pen) {
	switch(pen) {
		case OLED_PEN_WRITE:
			memset(oled_gram, -1, sizeof(oled_gram));
		break;
		
		case OLED_PEN_CLEAR:
			memset(oled_gram, 0, sizeof(oled_gram));
		break;
				
		case OLED_PEN_INVERSION:
			for (uint8_t i = 0; i < 8; i++) {
				for (uint8_t n = 0; n < 128; n++) {
					oled_gram[n][i] = ~oled_gram[n][i];
				}
			}
		break;
	}
	return BSP_OK;
}

BSP_StatusTypedef OLED_Init(void) {
	OLED_RST_Clr();
	BSP_Delay(500);
	OLED_RST_Set();
	
	OLED_WriteByte(0xae, OLED_WriteCMD);	/* Dot martix display off. */
	OLED_WriteByte(0x32, OLED_WriteCMD);	/* Set pump voltage 8v. */
	OLED_WriteByte(0x40, OLED_WriteCMD);	/* Set display start line. */
	OLED_WriteByte(0x81, OLED_WriteCMD);	/* Contarst control. */
	OLED_WriteByte(0xcf, OLED_WriteCMD);
	OLED_WriteByte(0xa1, OLED_WriteCMD);	/* Set segment re-map. */
	OLED_WriteByte(0xa4, OLED_WriteCMD);	/* Entire display off. */
	OLED_WriteByte(0xa6, OLED_WriteCMD);	/* Set normal display. */
	OLED_WriteByte(0xa8, OLED_WriteCMD);	/* Set multiples ration. */
	OLED_WriteByte(0x3f, OLED_WriteCMD);
	OLED_WriteByte(0xad, OLED_WriteCMD);	/* Set DC/DC booster. */
	OLED_WriteByte(0x8b, OLED_WriteCMD);
	OLED_WriteByte(0xc8, OLED_WriteCMD);	/* Com scan COM0-COM63. */
	OLED_WriteByte(0xd3, OLED_WriteCMD);	/* Set display offset. */
	OLED_WriteByte(0x00, OLED_WriteCMD);
	OLED_WriteByte(0xd5, OLED_WriteCMD);	/* Set frame frequency. */
	OLED_WriteByte(0x80, OLED_WriteCMD);
	OLED_WriteByte(0xd9, OLED_WriteCMD);	/* Set pre_charge period. */
	OLED_WriteByte(0x1f, OLED_WriteCMD);
	OLED_WriteByte(0xda, OLED_WriteCMD);	/* Com pin configuration. */
	OLED_WriteByte(0x12, OLED_WriteCMD);
	OLED_WriteByte(0xdb, OLED_WriteCMD);	/* Set vcom deselect level. */
	OLED_WriteByte(0x40, OLED_WriteCMD);
	OLED_WriteByte(0xaf, OLED_WriteCMD);	/* Display on. */

	OLED_Refresh(); /* Display initial picture. */
	OLED_SetAll(OLED_PEN_CLEAR);  /* Clear memory. */
	return BSP_OK;
}

BSP_StatusTypedef OLED_Print(const char* str) {
	modified = true;
	
	for(uint16_t i = 0; str[i] != '\0'; i++) {
		uint8_t c = str[i];
		if (c < 32) {
			switch (c) {
				case '\n':
					oled_cursor.page ++;
					oled_cursor.column = 0;
				break;
				
				case '\t':
					oled_cursor.column += 16;
				break;
			}
		} else if (c < 126) {
			if (oled_cursor.column == 128) {
				oled_cursor.page ++;
				oled_cursor.column = 0;
			}
			if (oled_cursor.page == 8) {
				OLED_SetAll(OLED_PEN_CLEAR);
				oled_cursor.page = 0;
			}
			memcpy(&oled_gram[oled_cursor.page][oled_cursor.column], oled_font[c- 32], sizeof(oled_font[0]));
			oled_cursor.column += 8;
		}
	}
	return BSP_OK;
}

BSP_StatusTypedef OLED_Rewind(void) {
	OLED_SetAll(OLED_PEN_CLEAR);
	
	oled_cursor.page = 0;
	oled_cursor.column = 0;
	return BSP_OK;
}
