/*
 * Test on EPS32-WROOM-32
 * esp-idf version 4.0.1
 * Eclipse version 2020-06
 * Author: Jirawat Kongkaen
 * https://www.micro.in.th
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

#define PIN_NUM_DC   23
#define PIN_NUM_RST  22
#define PIN_NUM_BCKL 21

#define MAX_TRANSFER_SIZE	500 //2048

spi_device_handle_t spi;

/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

DRAM_ATTR static const lcd_init_cmd_t ili_init_cmds[]={
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    {0xCF, {0x00, 0x83, 0X30}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    {0xE8, {0x85, 0x01, 0x79}, 3},
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    /* Pump ratio control, DDVDH=2xVCl */
    {0xF7, {0x20}, 1},
    /* Driver timing control, all=0 unit */
    {0xEA, {0x00, 0x00}, 2},
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x26}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x35, 0x3E}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xBE}, 1},
    /* Memory access contorl, MX=1,MY=0, MV=0, ML=0, BGR=1, MH=0 */
    {0x36, {0x48}, 1},
    /* Pixel format, 16bits/pixel for RGB/MCU interface *///0x66; 18bits/pixel
    {0x3A, {0x55}, 1},
    /* Interface Control, Endian=1 */
    //{0xF6, {0x01, 0x00, 0x20}, 3}, // no effect
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x08}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    /* Negative gamma correction */
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    /* Column address set, SC=0, EC=0xEF */
    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
    /* Page address set, SP=0, EP=0x013F */
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    /* Memory write */
    {0x2C, {0}, 0},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    /* Sleep out */
    {0x11, {0}, 0x80},
    /* Display on */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void ili9341_init(void)
{
	int cmd=0;
	esp_err_t ret;
    const lcd_init_cmd_t* lcd_init_cmds;

    spi_bus_config_t buscfg={
		.miso_io_num=PIN_NUM_MISO,
		.mosi_io_num=PIN_NUM_MOSI,
		.sclk_io_num=PIN_NUM_CLK,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1,
		.max_transfer_sz=MAX_TRANSFER_SIZE
	};
	spi_device_interface_config_t devcfg={
		.clock_speed_hz=40*1000*1000,           //Clock out at 40 MHz
		.mode=0,                                //SPI mode 0
		.spics_io_num=PIN_NUM_CS,               //CS pin
		.queue_size=7,                          //We want to be able to queue 7 transactions at a time
		.pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};
	//Initialize the SPI bus
	ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	ESP_ERROR_CHECK(ret);
	//Attach the LCD to the SPI bus
	ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	ESP_ERROR_CHECK(ret);

    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //Send all the commands
    lcd_init_cmds = ili_init_cmds;
	while (lcd_init_cmds[cmd].databytes!=0xff) {
		lcd_cmd(spi, lcd_init_cmds[cmd].cmd);
		lcd_data(spi, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
		if (lcd_init_cmds[cmd].databytes&0x80) {
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		cmd++;
	}

	///Enable backlight
	gpio_set_level(PIN_NUM_BCKL, 1);
}

/*
 * Set column and page address from x,y position and image width/height
 */
void ili9341_set_address(int x, int y, int w, int h)
{
	esp_err_t ret;
	static spi_transaction_t trans[5];
	int i;

	for(i=0; i<5; i++)
		memset(&trans[i], 0, sizeof(spi_transaction_t));

	trans[0].tx_data[0] = 0x2A;           //Column Address Set
	trans[0].length = 8;	// 8-bit
	trans[0].user = (void*)0; // DC = 0
	trans[0].flags = SPI_TRANS_USE_TXDATA;

	trans[1].tx_data[0] = x >> 8;              //Start Col High
	trans[1].tx_data[1] = x & 0xff;              //Start Col Low
	trans[1].tx_data[2] = (x+w-1) >> 8;       //End Col High
	trans[1].tx_data[3] = (x+w-1) & 0xff;     //End Col Low
	trans[1].length = 8*4;
	trans[1].user = (void*)1; // DC = 1
	trans[1].flags = SPI_TRANS_USE_TXDATA;

	trans[2].tx_data[0] = 0x2B;           //Page address set
	trans[2].length = 8;	// 8-bit
	trans[2].user = (void*)0; // DC = 0
	trans[2].flags = SPI_TRANS_USE_TXDATA;

	trans[3].tx_data[0] = y >> 8;        //Start page high
	trans[3].tx_data[1] = y & 0xff;      //start page low
	trans[3].tx_data[2] = (y+h-1) >> 8;    //end page high
	trans[3].tx_data[3] = (y+h-1) & 0xff;  //end page low
	trans[3].length = 8*4;
	trans[3].user = (void*)1; // DC = 1
	trans[3].flags = SPI_TRANS_USE_TXDATA;

	trans[4].tx_data[0] = 0x2C;           //memory write
	trans[4].length = 8;	// 8-bit
	trans[4].user = (void*)0; // DC = 0
	trans[4].flags = SPI_TRANS_USE_TXDATA;

	//Queue all transactions.
	for (i=0; i<5; i++) {
		ret = spi_device_polling_transmit(spi, &trans[i]);
		assert(ret==ESP_OK);
	}
}

/*
 * Put color pixel on x,y position
 */
void ili9341_put_pixel(uint16_t color, int x, int y)
{
	esp_err_t ret;
	spi_transaction_t t;

	ili9341_set_address(x, y, 1, 1);

	memset(&t, 0, sizeof(t));
	t.tx_buffer = (char*)&color;        //finally send the line data
	t.length = sizeof(color)*8;          //Data length, in bits
	t.user = (void*)1; // DC = 1

	ret = spi_device_polling_transmit(spi, &t);
	assert(ret==ESP_OK);            //Should have had no issues.
}

/*
 * Draw rectangle bar on x,y position, w=bar width, h=bar height
 */
void ili9341_fill_bar(uint16_t color, int x, int y, int w, int h)
{
	uint16_t buffer[MAX_TRANSFER_SIZE/2];
	esp_err_t ret;

	ili9341_set_address(x, y, w, h);

	/*
	On top of that, ESP32 is a little-endian chip,
	which means that the least significant byte of uint16_t and uint32_t variables is stored at the smallest address.
	Hence, if uint16_t is stored in memory, bits [7:0] are sent first, followed by bits [15:8].
	*/
	for(int i=0; i<MAX_TRANSFER_SIZE/2; i++)
	{
		buffer[i] = color;
	}

	int barSize = w * h * 2;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));

	while(barSize > MAX_TRANSFER_SIZE)
	{
		t.tx_buffer = (char*)&buffer;        //finally send the line data
		t.length = MAX_TRANSFER_SIZE*8;          //Data length, in bits
		t.user = (void*)1; // DC = 1

		ret = spi_device_polling_transmit(spi, &t);
		assert(ret==ESP_OK);            //Should have had no issues.

		if(barSize >= MAX_TRANSFER_SIZE)
		{
			barSize -= MAX_TRANSFER_SIZE;
		}
	}

	memset(&t, 0, sizeof(t));
	if(barSize) // remaining image data
	{
		t.tx_buffer = (char*)&buffer;        //finally send the line data
		t.length = barSize*8;          //Data length, in bits
		t.user = (void*)1; // DC = 1

		ret = spi_device_polling_transmit(spi, &t);
		assert(ret==ESP_OK);            //Should have had no issues.
	}
}

/*
 * Put image on x,y position, w=image width, h=image height
 */
void ili9341_put_image(uint16_t *image, int x, int y, int w, int h)
{
	esp_err_t ret;

	ili9341_set_address(x, y, w, h);

	int imgSize = w * h * 2; // 16-bit RGB565
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));

	while(imgSize > MAX_TRANSFER_SIZE)
	{
		t.tx_buffer = image;        //finally send the line data
		t.length = MAX_TRANSFER_SIZE*8;          //Data length, in bits
		t.user = (void*)1; // DC = 1

		ret = spi_device_polling_transmit(spi, &t);
		assert(ret==ESP_OK);            //Should have had no issues.

		if(imgSize >= MAX_TRANSFER_SIZE)
		{
			imgSize -= MAX_TRANSFER_SIZE;
			image += MAX_TRANSFER_SIZE/2;
		}
	}

	memset(&t, 0, sizeof(t));
	if(imgSize) // remaining image data
	{
		t.tx_buffer = image;        //finally send the line data
		t.length = imgSize*8;          //Data length, in bits
		t.user = (void*)1; // DC = 1

		ret = spi_device_polling_transmit(spi, &t);
		assert(ret==ESP_OK);            //Should have had no issues.
	}
}
