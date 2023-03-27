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
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "thaifont-esp32.h"
#include "ili9341.h"

#define OFS_X1	0
#define OFS_X2	1
#define OFS_Y1	2

char* _font;
int16_t _cursorX;
int16_t _cursorY;
int16_t _fontHeight;
uint16_t _fontColor;
TEXT_STYLE textStyle;

// byte 0-2 are first font offset, next three bytes are second font offset
const signed char ofs_tab1[6] = {0, -2, -2, 0, -3, -3};
const signed char ofs_tab2[6] = {0, -2, -2, 0, -3, -3};

void thaifont_set_color(uint16_t color)
{
	_fontColor = color;
}

void thaifont_set_align(uint8_t align)
{
    if(align >= '1' && align <= '3')
    {
        textStyle.align = align;
    }
}


/***********************************************************************************************
Function: get character offset
Argument: uint8_t *ofs pointer point to 4 bytes result buffer, x-, x+, y
		  XCHAR *textString pointer point to text string
***********************************************************************************************/
void thaifont_get_offset(int16_t *ofs, XCHAR *textString)
{
	//uint16_t tmpY;
	uint8_t i;
	
	// set offset index
	if(_fontHeight==48)
		i = 6;
	else if(_fontHeight==40)
		i = 3;
	else
		i = 0;
	
	// ¶éÒµÑÇÍÑ¡ÉÃµÑÇ¶Ñ´ä»¤×ÍÊÃÐÍÓ áÅÐµÑÇÍÑ¡ÉÃ»Ñ¨¨ØºÑ¹à»ç¹ àÍ¡ â· µÃÕ ¨ÑµÇÒ ãËéÂ¡¢Öé¹àË¹×ÍÊÃÐÍÓ
	if(textString[1]==(XCHAR)0xD3)
	{
		// àÍ¡ â· µÃÕ ¨ÑµÇÒ
		if(textString[0]==(XCHAR)0xE8 || textString[0]==(XCHAR)0xE9 || textString[0]==(XCHAR)0xEA || textString[0]==(XCHAR)0xEB)
		{
			ofs[OFS_X1] = ofs_tab1[i+0];
			ofs[OFS_X2] = ofs_tab1[i+1];
			ofs[OFS_Y1] = ofs_tab1[i+2];
			return;
		}
	}
	// » ¿ ½
	if(textString[-1]==(XCHAR)0xBB || textString[-1]==(XCHAR)0xBD || textString[-1]==(XCHAR)0xBF)
	{
		// Ñ Ô Õ Ö × ç è é ê ë ì ¹Ô¤ËÔµ
		if(textString[0]==(XCHAR)0xD1 || (textString[0]>=(XCHAR)0xD4 && textString[0]<=(XCHAR)0xD7) || (textString[0]>=(XCHAR)0xE7 && textString[0]<=(XCHAR)0xED) )
		{
			ofs[OFS_Y1] = 0;
			// è é ê ë ì
			if( (textString[0]>=(XCHAR)0xE8 && textString[0]<=(XCHAR)0xED) )
			{
				ofs[OFS_Y1] = 4;
			}
			ofs[OFS_X1] = -2;
			ofs[OFS_X2] = 3;
			// Ñ
			if(textString[0]==(XCHAR)0xD1)
				ofs[OFS_X2] = -1;
			return;
		}
	}
	// » ½ ¿
	if(textString[-2]==(XCHAR)0xBB || textString[-2]==(XCHAR)0xBD || textString[-2]==(XCHAR)0xBF)
	{
		// Ô ÍÕ Ö ×
		if( (textString[-1]>=(XCHAR)0xD4 && textString[-1]<=(XCHAR)0xD7) )
		{
			// è é ê ë
			if( (textString[0]>=(XCHAR)0xE8 && textString[0]<=(XCHAR)0xEC) )
			{
				ofs[OFS_X1] = -4;
				ofs[OFS_Y1] = -2;
				ofs[OFS_X2] = 6;
				return;
			}
		}
	}
	
	// äÁéËÑ¹ÍÒ¡ÒÈ ÊÃÐÍÔ ÊÃÐÍÕ ÊÃÐÍÖ ÊÃÐÍ×
	if(textString[-1]==(XCHAR)0xD1 || textString[-1]==(XCHAR)0xD4 || textString[-1]==(XCHAR)0xD5 || textString[-1]==(XCHAR)0xD6 || textString[-1]==(XCHAR)0xD7)
	{
		// àÍ¡ â· µÃÕ ¨ÑµÇÒ ¡ÒÃÑ¹µì
		if(textString[0]==(XCHAR)0xE8 || textString[0]==(XCHAR)0xE9 || textString[0]==(XCHAR)0xEA || textString[0]==(XCHAR)0xEB || textString[0]==(XCHAR)0xEC)
		{
			ofs[OFS_X1] = 3;
			ofs[OFS_X2] = 0;
			// äÁéËÑ¹ÍÒ¡ÒÈ
			if(textString[-1]==(XCHAR)0xD1)
				ofs[OFS_X2] = -3;
			ofs[OFS_Y1] = 0;
			return;
		}
	}
	
	// äÁéàÍ¡ â· µÃÕ ¨ÑµÇÒ ÍÂÙèËÅÑ§ÊÃÐÍØ ÍÙ áÅÐ » ½ ¿
	if(textString[0]==(XCHAR)0xE8 || textString[0]==(XCHAR)0xE9 || textString[0]==(XCHAR)0xEA || textString[0]==(XCHAR)0xEB)
	{
		if(textString[-1]==(XCHAR)0xD8 || textString[-1]==(XCHAR)0xD9)
		{
			if(textString[-2]==(XCHAR)0xBB || textString[-2]==(XCHAR)0xBD || textString[-2]==(XCHAR)0xBF)
			{
				ofs[OFS_Y1] = 4;
				ofs[OFS_X1] = -2;
				ofs[OFS_X2] = 6;
				return;
			}
		}
	}
	ofs[OFS_X1] = 0;
	ofs[OFS_X2] = 0;
	ofs[OFS_Y1] = 0;
	if(textString[-1]==(XCHAR)0xD1) // äÁéËÑ¹ÍÒ¡ÒÈ
		ofs[OFS_X1] += 4;
	// Ô Õ Ö ×
	if( (textString[-1]>=(XCHAR)0xD4 && textString[-1]<=(XCHAR)0xD7) )
		ofs[OFS_X1] += 2;
	if(textString[0]==(XCHAR)0xE9) // äÁéâ·
		ofs[OFS_X1] += 2;
		
	// äÁéËÑ¹ÍÒ¡ÒÈ // » ½ ¿
	if(textString[-1]==(XCHAR)0xD1 && textString[-2]!=(XCHAR)0xBB && textString[-2]!=(XCHAR)0xBD && textString[-2]!=(XCHAR)0xBF)
		ofs[OFS_X1] -= 3;
	// äÁéàÍ¡ // » ½ ¿
	if(textString[-1]==(XCHAR)0xE8 && textString[-3]!=(XCHAR)0xBB && textString[-3]!=(XCHAR)0xBD && textString[-3]!=(XCHAR)0xBF )
		ofs[OFS_X1] += 3;
	// ÊÃÐÍØ ÊÃÐÍÙ
	if(textString[-1]==(XCHAR)0xD8 || textString[-1]==(XCHAR)0xD9)
		ofs[OFS_X1] += 2;
	
	// äÁéàÍ¡ â· µÃÕ ¨ÑµÇÒ ¡ÒÃÑ¹µì ãËéà¾ÔèÁ y offset à©¾ÒÐ¿Í¹µì loma
	if( (textString[0]>=(XCHAR)0xE8 && textString[0]<=(XCHAR)0xEC) )
	{
		ofs[OFS_Y1] = 4;
	}
}

/********************************************************************************************
Function: text alignment
********************************************************************************************/
void thaifont_text_alignment(XCHAR* txt, int y)
{
	XCHAR *ptr;
	XCHAR buf[255];
	XCHAR ch;
	int16_t x;

	ptr = (XCHAR*)&buf;
	while(1)
    {
	    ch = *txt++;
		if(ch!='\n' && ch!='\0')
		{
			*ptr++ = ch;
			continue;
		}
		*ptr++ = 0x00;
		if(textStyle.align==CENTER_ALIGNMENT)
			x = (GetX() - thaifont_get_text_width((XCHAR*)&buf))/2;
		else if(textStyle.align==RIGHT_ALIGNMENT)
			x = GetX() - thaifont_get_text_width((XCHAR*)&buf);
		else
			x = 0;
		
		MoveTo(x, GetY() );
		thaifont_draw_text((XCHAR*)&buf, x, y);
		ptr = (XCHAR*)&buf;

		if(ch=='\0')
			break;
	}
}

/***********************************************************************************************/
void thaifont_draw_text(XCHAR* textString, int x, int y)
{
	int16_t ofs[3];
	int16_t tmpY;

	MoveTo(x, y);

    while(*textString!=0)
    {
	    tmpY = GetY();
	    thaifont_get_offset((int16_t*)&ofs, textString);
	    _cursorX += ofs[OFS_X1];
	    _cursorY += ofs[OFS_Y1];
	    // ­Ø ­Ù ­Ú °Ø °Ù °Ú
		if(textString[0]==(XCHAR)0xAD || textString[0]==(XCHAR)0xB0)
		{
			// Ø Ù
			if( (textString[1]>=(XCHAR)0xD8 && textString[1]==(XCHAR)0xDA) )
			{
				tmpY = _fontHeight;
				_fontHeight -= 5;
				thaifont_draw_char(textString[0]);
				_fontHeight = tmpY;
				textString++;
				continue;
			}
		}
		thaifont_draw_char(textString[0]);
		_cursorX += ofs[OFS_X2];
	    _cursorY = tmpY;
		textString++;
    }
    
    _cursorY = GetY()+_fontHeight;
}

/***********************************************************************************************/
void thaifont_draw_char(XCHAR ch)
{
	GLYPH_ENTRY* pChTable;
	uint8_t *pChImage;
	int16_t chWidth;
	int16_t xCnt, yCnt, x=0, y;
	uint8_t temp, mask;

	pChTable = (GLYPH_ENTRY*)(_font+sizeof(FONT_HEADER) );
	
	mask = 0;
	for(temp=32;temp<251;temp++)
	{
		if(pChTable->ascii == ch)
		{
			mask = 1;
			break;
		}
		pChTable += 1;
	}
	if(!mask) return;

	pChImage = (uint8_t*)( _font + pChTable->offset );
	chWidth = pChTable->xadvance;
	if(chWidth < pChTable->width)
		chWidth = pChTable->width;
	
	// ÊÃÐ´éÒ¹º¹áÅÐ´éÒ¹ÅèÒ§ã¹ÀÒÉÒä·Â ¨ÐÁÕ xoffset à»ç¹Åº
	if(pChTable->xoffset < 0)
		_cursorX += pChTable->xoffset;

	//printf("ascii=%d width=%d xoffset=%d xadvance=%d, offset=%d\n",
	//		pChTable->ascii, pChTable->width, pChTable->xoffset, pChTable->xadvance, pChTable->offset);

	y = GetY();
	for(yCnt=0;yCnt<_fontHeight;yCnt++)
	{
		x = GetX();
		mask = 0;
		for(xCnt=0;xCnt<chWidth;xCnt++)
		{
			if(mask == 0){
				temp = *pChImage++;
			    mask = 0x80;
            }
            if(temp&mask){
            	ili9341_put_pixel(_fontColor, x, y);
            }
            x++;
            mask >>= 1;
		}
		y++;
	}
	_cursorX = x;
}

/***********************************************************************************************/
void thaifont_set_font(void* fontcode)
{
	FONT_HEADER* pHeader;
	
	_font = fontcode;
	
	pHeader = (FONT_HEADER*)_font;
	_fontHeight = pHeader->lineHeight;
	//printf("_fontHeight=%d\n", _fontHeight);
}

/***********************************************************************************************/
int16_t thaifont_get_char_width(XCHAR ch)
{
	GLYPH_ENTRY* pChTable;
	//uint8_t*        pChImage;
	
	int16_t        chWidth=0;
	uint8_t         temp, mask;
	
	pChTable = (GLYPH_ENTRY*)(_font+sizeof(FONT_HEADER));
	
	mask = 0;
	for(temp=32;temp<251;temp++)
	{
		if(pChTable->ascii == ch)
		{
			mask = 1;
			break;
		}
		pChTable += 1;
	}
	if(!mask) return 0;

	//pChImage = (uint8_t*)(_font + pChTable->offset );
	chWidth = pChTable->xadvance;
	if(chWidth<pChTable->width)chWidth = pChTable->width;
	
	if(pChTable->xoffset<0)
		chWidth += pChTable->xoffset;
	
	return chWidth;
}

/***********************************************************************************************/
uint16_t thaifont_get_text_width(XCHAR* textString)
{
	int16_t ofs[3];
	int16_t textWidth = 0;

    while(*textString)
    {
	    // horizontal tab
		if(textString[0]=='\t')
		{
			textWidth += thaifont_get_char_width(' ')*4;
			textString++;
		    continue;
		}
	    thaifont_get_offset((int16_t*)&ofs, textString);
		textWidth += ofs[OFS_X1];
		textWidth += thaifont_get_char_width(textString[0]);
		textWidth += ofs[OFS_X2];
		textString++;
    }
    return textWidth;
}
