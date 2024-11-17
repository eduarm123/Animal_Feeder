/*************************************************************************************************/
/*! @file	Main_Screen.c
 *	@brief	Introducir breve descripcion del fichero
 *
 *	\b Descripcionn: Introducir aquï¿½ descripciï¿½n de las funcionalidades del fichero \n
 *
 *
 *		Compiler  :  \n
 *		Copyright :  \n
 *		Target    :  \n
 *
 *	@author		
 *	@date		22 mar. 2023
 *	@version	1.0
 *
 ****************************************************************************
 *
 *	Log: main.c
 * Revision [Date // Author]:  Modifications \n
 * -----------------------------------------------------------\n
 * V1.0 [ 28 oct. 2020 // earmijos ]: Original \n
 *
 */
/*************************************************************************************************/


/**********************************INCLUDES ****************************************************/
#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"

#include <string.h>
#include <time.h>
#include "ds3231.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "Main_Screen.h"


//#include "easyio.h"
//#include "picture.h"
#include <stdlib.h>

#include "NVM_drivers/NVM_drivers.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
 
#include "lvgl_helpers.h"
#include "ui_helpers.h"

#include "ui.h"

/********************************* (1) PUBLIC METHODS ********************************************/

/*********************************** (2) PUBLIC VARS *********************************************/
extern TaskHandle_t MainScreen_Handle;
extern TaskHandle_t AlarmaMenu_Handle;
extern bool is_alarm_set;
extern QueueHandle_t commandQueue;

tm_t time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

typedef struct {
    uint8_t u8_hour;
    uint8_t u8_min;
} time_result_t;


typedef enum{
    Manual=1,
    Automatico
}MENU_OPT;

typedef enum{
    Adulto=1,
    Cachorro
}AGE_t;

typedef enum{
    Manual_alarmas_1=1,
    Manual_alarmas_2,
    Manual_alarmas_3,
    Manual_alarmas_4,
    Manual_alarmas_5
}ACTIVAR_ALARM;


tm_t s_alarmas_manual[]={
    {0}, // Esta tiene interrupcion
    {0}, // Esta tiene interrupcion
    {0},
    {0},
    {0},
};

uint8_t u8_TimeConfigDone=0;


i2c_dev_t s_dev; // necessary for RTC_init()

uint8_t n_alarms; // Se guarda la configuracion las alarmas que estan declaradas en ACTIVAR_ALARM
//uint32_t alarm_type;
char contador_str[4];
char contador_str1[4];

/******************************** (3) DEFINES & MACROS *******************************************/

#define TAG "main"
#define LV_TICK_PERIOD_MS 1
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

/*********************************** (4) PRIVATE VARS ********************************************/

//static mutex_t lvgl_mutex;


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/
static void example_increase_lvgl_tick(void *arg);
static void ok_button_event_cb(lv_event_t * e);
void Time_config_touch(tm_t * const _time, uint8_t _hour,uint8_t _minute );

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

 

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
void Titilar(int indice, int n);
void init_DipTouch(void);


void Main_Screen( void * pvParameters )
{    
    char u8_timeconverted[9];
    uint32_t time_till_next;
    uint32_t size_in_px = DISP_BUF_SIZE;

    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    ESP_LOGI(TAG, "Initialize SPI bus");
    lvgl_driver_init();

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE* sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px );

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush; // Driver lvgl 

 
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    //lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    /* Esto inicializa el touch*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
    /*---------------------------------------------*/
 
    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));
 
    ESP_LOGI(TAG, "Application"); 


    /*------INICIALIZAR FLASH-----*/
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
 
    RTC_init(&s_dev); // Inicializa el i2c

    ui_init();
 
                        
    while(1)
    {  
     

        if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
           printf("Could not get time\n");
        }

        // printf("--- main screen ---\n");
        // printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);


        //mutex_lock(&lvgl_mutex);
        time_till_next = lv_timer_handler();
        //lv_task_handler();
        //vTaskDelay(10 / portTICK_PERIOD_MS);
        //mutex_unlock(&lvgl_mutex);

        vTaskDelay(pdMS_TO_TICKS(time_till_next));
        

        convertTime2StringDisplay(&time_tc,u8_timeconverted);


        lv_label_set_text(ui_Hora, u8_timeconverted);
    }

    /* A task should NEVER return */
    free(buf1);
    //free(buf2);                
    
}


void Time_config_touch(tm_t * const _time, uint8_t _hour,uint8_t _minute ){
    
    _time->tm_sec=0;
    _time->tm_hour=_hour;
    _time->tm_min=_minute;

}

/**
 * @brief Convert time values to a character string in "HH:MM:SS" format with leading zeros.
 *
 * This function takes a pointer to a tm_t structure _time2Convert containing the time values to convert,
 * and stores the resulting character string in the timeconverted array. The resulting string will have the
 * format "HH:MM:SS" with leading zeros for hours, minutes, and seconds if necessary.
 * 
 * @param[in] _time2Convert Pointer to the tm_t structure containing the time values to convert.
 * @param[out] timeconverted Pointer to the character array where the resulting time string will be stored.
 *
 * @note The character arrays 'hour_car', 'min_car', and 'seg_car' must have enough memory to store
 * the converted time components as strings. Make sure to allocate at least 3 characters for each
 * array (including the null terminator '\0').
 *
 * @see tm_t
 * 
 * @note The timeconverted array must have enough space to store the entire time string "HH:MM:SS" along with the null-terminator.
 */
void convertTime2StringDisplay(tm_t *_time2Convert, char timeconverted[]){

   timeconverted[0]='0' + (_time2Convert->tm_hour)/10;
   timeconverted[1]='0' + (_time2Convert->tm_hour)%10;
   timeconverted[2]=':';
   timeconverted[3]='0' + (_time2Convert->tm_min)/10;
   timeconverted[4]='0' + (_time2Convert->tm_min)%10;
   timeconverted[5]=':'; 
   timeconverted[6]='0' + (_time2Convert->tm_sec)/10;
   timeconverted[7]='0' + (_time2Convert->tm_sec)%10;
   timeconverted[8]='\0';

}