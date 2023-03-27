/*
 * Test on ESP32-WROOM-32
 * esp-idf version 4.0.1
 * Eclipse version 2020-06
 * Author: Jirawat Kongkaen
 * https://www.micro.in.th
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"

typedef struct {
	uint8_t  lineHeight;
	uint8_t  reserve;
} FONT_HEADER;

typedef struct {
	char  ascii;
	uint8_t  width;
	signed char  xoffset;
	uint8_t  xadvance;
	uint16_t  offset;
} GLYPH_ENTRY;

typedef struct
{
	uint8_t inverse;
	uint8_t align;
	uint8_t flipver;
	uint8_t rotate;
} TEXT_STYLE;

extern TEXT_STYLE textStyle;

#define  MoveTo(x,y) _cursorX=x; _cursorY=y;

extern const char loma40px[];

#define LEFT_ALIGNMENT		'1'
#define CENTER_ALIGNMENT	'2'
#define RIGHT_ALIGNMENT		'3'

#define XCHAR   char
#define GetX() _cursorX
#define GetY() _cursorY

#define thaifont_get_font_height()	_fontHeight

extern char* _font;
extern int16_t _cursorX;
extern int16_t _cursorY;
extern int16_t _fontHeight;
extern uint8_t textBuffer[];
extern uint16_t textBufferIndex;

void thaifont_get_offset(int16_t *ofs, char *textString);
void thaifont_draw_char(char ch);
void thaifont_draw_text(XCHAR* textString, int x, int y);
int16_t thaifont_get_char_width(char ch);
uint16_t thaifont_get_text_width(char* textString);
void thaifont_text_alignment(char* txt, int y);
void thaifont_init(void);
void thaifont_set_align(uint8_t align);
void thaifont_clear_buffer(void);
void thaifont_print_text(void);
void thaifont_set_color(uint16_t color);
void thaifont_set_font(void* fontcode);

