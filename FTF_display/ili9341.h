/*
 * Test on EPS32-WROOM-32
 * esp-idf version 4.0.1
 * Eclipse version 2020-06
 * Author: Jirawat Kongkaen
 * https://www.micro.in.th
 */
void ili9341_init(void);
void ili9341_set_address(int x, int y, int w, int h);
void ili9341_put_image(uint16_t *image, int x, int y, int w, int h);
void ili9341_fill_bar(uint16_t color, int x, int y, int w, int h);
void ili9341_put_pixel(uint16_t color, int x, int y);

/*
On top of that, ESP32 is a little-endian chip,
which means that the least significant byte of uint16_t and uint32_t variables is stored at the smallest address.
Hence, if uint16_t is stored in memory, bits [7:0] are sent first, followed by bits [15:8].
*/
// color code in little-endian
#define COLOR_BLACK		0x0000
#define COLOR_WHITE		0xffff
#define COLOR_RED		0x00f8
#define COLOR_GREEN		0xe007
#define COLOR_BLUE		0x1f00
#define COLOR_YELLOW	0xe0ff
#define COLOR_MAGENTA	0x1ff8
#define COLOR_CYAN		0xff07
