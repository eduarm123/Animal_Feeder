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
#include "Button_Handler.h"


#include "easyio.h"
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

/*lv_obj_t *roller;
lv_obj_t *roller1;
lv_obj_t *button;
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Label1;
lv_obj_t *ui_Panel1;
lv_obj_t *ui____initial_actions0;*/

//char buf[2];
//char buf1[2];
uint8_t u8_TimeConfigDone=0;


i2c_dev_t s_dev; // necessary for RTC_init()

uint8_t n_alarms; // Se guarda la configuracion las alarmas que estan declaradas en ACTIVAR_ALARM
uint32_t alarm_type;


/******************************** (3) DEFINES & MACROS *******************************************/

#define TAG "main"
#define LV_TICK_PERIOD_MS 1
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/
static void example_increase_lvgl_tick(void *arg);
static uint8_t select_option(void);
static void Alarma_menu(void);
static void Config_time(void);
static void ok_button_event_cb(lv_event_t * e);
void Time_config_touch(tm_t * const _time, uint8_t _hour,uint8_t _minute );

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}


/*static void Config_time(void)
{
    // Crea el objeto "rodillo"
    roller = lv_roller_create(lv_scr_act());
    lv_roller_set_options(roller, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(roller, 3); // Configura el número de filas visibles
    lv_obj_set_width(roller, 100); // Establece el ancho del rodillo
    lv_obj_align(roller, LV_ALIGN_LEFT_MID, 0, 0); // Alinea el rodillo en el centro
    //----lv_obj_add_event_cb(roller, ok_button_event_cb, LV_EVENT_ALL, NULL);
    lv_roller_set_selected(roller, 2, LV_ANIM_OFF);

    roller1 = lv_roller_create(lv_scr_act());
    lv_roller_set_options(roller1, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(roller1, 3); // Configura el número de filas visibles
    lv_obj_set_width(roller1, 100); // Establece el ancho del rodillo
    lv_obj_set_style_text_align(roller1, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(roller1, LV_ALIGN_RIGHT_MID, 0, 0); // Alinea el rodillo en el centro
    //----lv_obj_add_event_cb(roller1, ok_button_event_cb, LV_EVENT_ALL, NULL);
    lv_roller_set_selected(roller1, 5, LV_ANIM_OFF);

    // Crea el botón "OK"
    button = lv_btn_create(lv_scr_act());
    lv_obj_align(button, LV_ALIGN_OUT_BOTTOM_MID, 0, 10); // Alinea el botón debajo del rodillo
    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, "OK");

    // Configura la función de control de eventos para el botón OK
    lv_obj_add_event_cb(button, ok_button_event_cb, LV_EVENT_ALL, NULL);
}

static void ok_button_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    uint8_t u8_hour;
    uint8_t u8_min;

    if (code == LV_EVENT_CLICKED) {
        
        // Obtiene el valor actual del rodillo
        lv_roller_get_selected_str(roller, buf, sizeof(buf));
        lv_roller_get_selected_str(roller1, buf1, sizeof(buf1));

        u8_hour = (uint8_t)atoi(buf);
        u8_min = (uint8_t)atoi(buf1); 
  

        Time_config_touch(&time_tc,u8_hour, u8_min ); //Aqui se rellena la estructura time con la hora
        // Imprime el valor actual en pantalla (puedes personalizar cómo lo haces)
        printf("Hora: %d\n", time_tc.tm_hour);
        printf("minuto: %d\n", time_tc.tm_min);

        u8_TimeConfigDone=1;
    }
}*/

 

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
time_result_t obtenerHora();
void Titilar(int indice, int n);
void init_DipTouch(void);


void Main_Screen( void * pvParameters )
{
    uint8_t u8_key=0;
    char u8_timeconverted[9];
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
    // esp_err_t err = nvs_flash_init();
    // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     // NVS partition was truncated and needs to be erased
    //     // Retry nvs_flash_init
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     err = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK( err );
    /*-----------------------------*/
    /*------INICIALIZAR FTF-----*/
    // spi_master_init(SPI3_HOST, LCD_DEF_DMA_CHAN, LCD_DMA_MAX_SIZE, SPI3_DEF_PIN_NUM_MISO, SPI3_DEF_PIN_NUM_MOSI, SPI3_DEF_PIN_NUM_CLK);
    // spi_lcd_init(SPI3_HOST, 40*1000*1000, LCD_SPI3_DEF_PIN_NUM_CS0);
    // LCD_Display_Resolution(horizontal);
    // LCD_Clear(LGRAYBLUE);
    /*-------INICIALIZAR teclado------------*/

    //keypadInit();// 
    RTC_init(&s_dev); // Inicializa el i2c

    
    // uint32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    
    // NNVM_read_memory_u32("restart_counter", &restart_counter);

    // // Write
    // printf("Updating restart counter in NVS ... ");
    
    // if(restart_counter==0){
    //     restart_counter=1;
    //     LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"*************",24,1);
    //     LCD_ShowString(20-1,60-1,LGRAYBLUE,BLACK,"CAT Feeder",24,1);
    //     LCD_ShowString(60-1,100-1,LGRAYBLUE,BLACK,"Welcome!",32,1);
    //     LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
    //     LCD_ShowPicture_16b(250-1, 50-1, 40, 40, gImage_qq);
    //     Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto
    //     printf("hora configurada... ");
    //     ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc));
    //     LCD_Clear(LGRAYBLUE); 
        
    //     NNVM_write_memory_u32("restart_counter",restart_counter);         
    // }
    //Config_time();
    ui_init();
    while(!u8_TimeConfigDone){
       vTaskDelay(pdMS_TO_TICKS(10));
       lv_timer_handler();
        
    }
    //ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
    
                        
    while(1)
    {  
        // LCD_Clear(LGRAYBLUE);
        // LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"*************",24,1);
        // LCD_ShowString(50-1,50-1,LGRAYBLUE,BLACK,"CAT FEEDER",24,1);
        // LCD_ShowString(1-1,80-1,LGRAYBLUE,BLACK,"*************",24,1);       

        if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
           printf("Could not get time\n");
        }

        // printf("--- main screen ---\n");
        // printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);

        /*---Conversión entero a caracter para imprimir en HMI sin problema---*/
        // convertTime2StringDisplay(&time_tc,u8_timeconverted);
        //LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,u8_timeconverted,32,1);  
                
        // u8_key = readKeypad();
        // if (u8_key=='C')
        // {               
        //     Alarma_menu();
        //     LCD_Clear(LGRAYBLUE);
        // }
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_timer_handler();
        printf("--- main screen ---\n");
        printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);
        
    }

    /* A task should NEVER return */
    free(buf1);
    //free(buf2);                
    
}


//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){
    
    uint8_t hour_total; //Para ingresar el 2do dígito de la hora
    uint8_t min_total;
    time_result_t time2send;

    time2send = obtenerHora();
    hour_total=time2send.u8_hour;
    min_total=time2send.u8_min;

    _time->tm_sec=0;
    _time->tm_hour=hour_total;
    _time->tm_min=min_total;
}

void Time_config_touch(tm_t * const _time, uint8_t _hour,uint8_t _minute ){
    
    _time->tm_sec=0;
    _time->tm_hour=_hour;
    _time->tm_min=_minute;

}

time_result_t obtenerHora() 
{
    char numeroStr[5];
    uint8_t u8_key=0;
    int indice = 0;
    uint8_t num_dec[4]={0};
    time_result_t time_set;
    
    for (size_t i = 0; i <= 3; i++)
    {
        numeroStr[0]='_';
        numeroStr[i+1]=' ';
    }

    while (indice <= 3) {

        //Titilar(indice);
        LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,numeroStr[0],32,1); // Mejorar
        LCD_ShowChar(130-1,180-1,LGRAYBLUE,BLACK,numeroStr[1],32,1); // Mejorar
        LCD_ShowChar(180-1,180-1,LGRAYBLUE,BLACK,numeroStr[2],32,1); // Mejorar
        LCD_ShowChar(210-1,180-1,LGRAYBLUE,BLACK,numeroStr[3],32,1); // Mejorar
        vTaskDelay(pdMS_TO_TICKS(10));
        u8_key = readKeypad();

        if (u8_key != '\0') {

            if (u8_key =='A'){
                u8_key ='_';
                indice++; 
                numeroStr[indice] = u8_key;            
            }
            else if (u8_key == 'B') {
                indice=0;
                for (size_t i = 0; i <= 3; i++)
                {                                     
                    numeroStr[0]='_';
                    numeroStr[i+1]=' ';
                }               
            }
            else{

                num_dec[indice]= u8_key - '0';

                if(num_dec[0]>2 || (num_dec[0] == 2 && num_dec[1] > 3) || num_dec[2]>5 || num_dec[3]>9 ){
                    numeroStr[indice] = '0';
                }
                else{
                    numeroStr[indice] = u8_key;
                }   

            }            
            vTaskDelay(pdMS_TO_TICKS(10));
            printf("Número actual: %s\n", numeroStr);
        }
              
    }

    time_set.u8_hour = (numeroStr[0]-'0') * 10 + (numeroStr[1]-'0');
    time_set.u8_min = (numeroStr[2]-'0') * 10 + (numeroStr[3]-'0');

    LCD_Clear(LGRAYBLUE);

    return (time_set);
}


// static void Alarma_menu( void)
// {
//     uint8_t u8_key=10; // on porpuse
//     uint32_t alarm1_NVM=0;
//     uint32_t alarm2_NVM=0;
//     uint32_t alarm3_NVM=0;
//     uint32_t alarm4_NVM=0;
//     uint32_t alarm5_NVM=0;  

//     LCD_Clear(LGRAYBLUE);
//     LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
//     LCD_ShowString(10-1,60-1,LGRAYBLUE,BLACK,"1.Configurar hora",16,1);
//     LCD_ShowString(10-1,100-1,LGRAYBLUE,BLACK,"2.Configurar alarmas",16,1);
//     LCD_ShowString(10-1,140-1,LGRAYBLUE,BLACK,"3.Mirar alarmas",16,1);
//     LCD_ShowString(10-1,180-1,LGRAYBLUE,BLACK,"4.Ir a main screen",16,1);
//     u8_key=select_option();
//     switch (u8_key) 
//     {
//         case '1':
//             LCD_Clear(LGRAYBLUE);
//             LCD_ShowString(20,70,LGRAYBLUE,BLACK,"CONFIG HORA",24,1);
//             LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
//             Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto.
//             ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
//             break;
//         case '2':
//         {
//             LCD_Clear(LGRAYBLUE);
//             printf("ESTAS EN MANUAL.\n");
//             LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"Numero de Alarmas",16,1);
//             u8_key=select_option(); // TODO: se extiende a 5 alarmas
//             switch (u8_key)
//             {
//             case '1':
//                 LCD_Clear(LGRAYBLUE);
//                 LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma",16,1);
//                 Time_config(&s_alarmas_manual[0]);

//                 alarm1_NVM = ((uint32_t)s_alarmas_manual[0].tm_hour << 8)| s_alarmas_manual[0].tm_min;
//                 NNVM_write_memory_u32("alarm1_NVM",alarm1_NVM);                       
//                 alarm_type = 1;
//                 NNVM_write_memory_u32(ALARM_NAMESPACE,alarm_type);
//                 break;
//             case '2':
//                 LCD_Clear(LGRAYBLUE);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 1",16,1);
//                 Time_config(&s_alarmas_manual[0]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 2",16,1);
//                 Time_config(&s_alarmas_manual[1]);
                
//                 alarm1_NVM = ((uint32_t)s_alarmas_manual[0].tm_hour << 8)| s_alarmas_manual[0].tm_min;
//                 NNVM_write_memory_u32("alarm1_NVM",alarm1_NVM);  
                
//                 alarm2_NVM = ((uint32_t)s_alarmas_manual[1].tm_hour << 8)| s_alarmas_manual[1].tm_min;
//                 NNVM_write_memory_u32("alarm2_NVM",alarm2_NVM);             

//                 alarm_type = 2;
//                 NNVM_write_memory_u32(ALARM_NAMESPACE,alarm_type);               
//                 break;
//             case '3':
//                 LCD_Clear(LGRAYBLUE);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 1",16,1);
//                 Time_config(&s_alarmas_manual[0]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 2",16,2);
//                 Time_config(&s_alarmas_manual[1]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 3",16,2);
//                 Time_config(&s_alarmas_manual[2]);

//                 alarm1_NVM = ((uint32_t)s_alarmas_manual[0].tm_hour << 8)| s_alarmas_manual[0].tm_min;
//                 NNVM_write_memory_u32("alarm1_NVM",alarm1_NVM);  
                
//                 alarm2_NVM = ((uint32_t)s_alarmas_manual[1].tm_hour << 8)| s_alarmas_manual[1].tm_min;
//                 NNVM_write_memory_u32("alarm2_NVM",alarm2_NVM);

//                 alarm3_NVM = ((uint32_t)s_alarmas_manual[2].tm_hour << 8)| s_alarmas_manual[2].tm_min;
//                 NNVM_write_memory_u32("alarm3_NVM",alarm3_NVM);              

//                 alarm_type = 3;
//                 NNVM_write_memory_u32(ALARM_NAMESPACE,alarm_type);                       
//                 break;
//             case '4':
//                 LCD_Clear(LGRAYBLUE);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 1",16,1);
//                 Time_config(&s_alarmas_manual[0]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 2",16,2);
//                 Time_config(&s_alarmas_manual[1]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 3",16,2);
//                 Time_config(&s_alarmas_manual[2]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 4",16,2);
//                 Time_config(&s_alarmas_manual[3]);

//                 alarm1_NVM = ((uint32_t)s_alarmas_manual[0].tm_hour << 8)| s_alarmas_manual[0].tm_min;
//                 NNVM_write_memory_u32("alarm1_NVM",alarm1_NVM);  
                
//                 alarm2_NVM = ((uint32_t)s_alarmas_manual[1].tm_hour << 8)| s_alarmas_manual[1].tm_min;
//                 NNVM_write_memory_u32("alarm2_NVM",alarm2_NVM);

//                 alarm3_NVM = ((uint32_t)s_alarmas_manual[2].tm_hour << 8)| s_alarmas_manual[2].tm_min;
//                 NNVM_write_memory_u32("alarm3_NVM",alarm3_NVM);

//                 alarm4_NVM = ((uint32_t)s_alarmas_manual[3].tm_hour << 8)| s_alarmas_manual[3].tm_min;
//                 NNVM_write_memory_u32("alarm4_NVM",alarm4_NVM);                

//                 alarm_type = 4;
//                 NNVM_write_memory_u32(ALARM_NAMESPACE,alarm_type);                       
//                 break;
//             case '5':
//                 LCD_Clear(LGRAYBLUE);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 1",16,1);
//                 Time_config(&s_alarmas_manual[0]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 2",16,2);
//                 Time_config(&s_alarmas_manual[1]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 3",16,2);
//                 Time_config(&s_alarmas_manual[2]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 4",16,2);
//                 Time_config(&s_alarmas_manual[3]);
//                 LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"Ingrese la alarma 5",16,2);
//                 Time_config(&s_alarmas_manual[4]);

//                 alarm1_NVM = ((uint32_t)s_alarmas_manual[0].tm_hour << 8)| s_alarmas_manual[0].tm_min;
//                 NNVM_write_memory_u32("alarm1_NVM",alarm1_NVM);  
                
//                 alarm2_NVM = ((uint32_t)s_alarmas_manual[1].tm_hour << 8)| s_alarmas_manual[1].tm_min;
//                 NNVM_write_memory_u32("alarm2_NVM",alarm2_NVM);

//                 alarm3_NVM = ((uint32_t)s_alarmas_manual[2].tm_hour << 8)| s_alarmas_manual[2].tm_min;
//                 NNVM_write_memory_u32("alarm3_NVM",alarm3_NVM);

//                 alarm4_NVM = ((uint32_t)s_alarmas_manual[3].tm_hour << 8)| s_alarmas_manual[3].tm_min;
//                 NNVM_write_memory_u32("alarm4_NVM",alarm4_NVM);

//                 alarm5_NVM = ((uint32_t)s_alarmas_manual[4].tm_hour << 8)| s_alarmas_manual[4].tm_min;
//                 NNVM_write_memory_u32("alarm5_NVM",alarm5_NVM); 

//                 alarm_type = 5;
//                 NNVM_write_memory_u32(ALARM_NAMESPACE,alarm_type);                       
//                 break;    
//             default:
//                 break;
//             }
            
//             break;
//         }                               
//         case '3': //See alarms
//             switch (alarm_type) // variable guardada en flash
//             {                                                     
//                 case Manual_alarmas_1:
//                     while(u8_key != '1' )
//                     {             
//                         char u8_timeconverted[9];                      
//                         LCD_Clear(LGRAYBLUE);
//                         LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
//                         LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 to come back",16,1);                        
//                         convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted);            
//                         LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted,24,1);

//                         u8_key=select_option();
//                     }
//                     break;
//                 case Manual_alarmas_2:
//                     while(u8_key != '1' )
//                     {
//                         char u8_timeconverted_1[9];  
//                         char u8_timeconverted_2[9];  

//                         LCD_Clear(LGRAYBLUE);
//                         LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
//                         LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 to come back",16,1); 

//                         convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
//                         LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);                       
   
//                         convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
//                         LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
//                         u8_key=select_option();
//                     }
//                     break;
//                 case Manual_alarmas_3:
//                     while(u8_key != '1' )
//                     {
//                         char u8_timeconverted_1[9];  
//                         char u8_timeconverted_2[9];
//                         char u8_timeconverted_3[9];

//                         LCD_Clear(LGRAYBLUE);
//                         LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
//                         LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 to come back",16,1); 

//                         convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
//                         LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);           
                             
//                         convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
//                         LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
 
//                         convertTime2StringDisplay(&s_alarmas_manual[2],u8_timeconverted_3);                       
//                         LCD_ShowString(25-1,150-1,LGRAYBLUE,BLACK,u8_timeconverted_3,24,1);           

//                         u8_key=select_option();
//                     }
//                     break;
//                 case Manual_alarmas_4:
//                     while(u8_key != '1' )
//                     {
//                         char u8_timeconverted_1[9];  
//                         char u8_timeconverted_2[9];
//                         char u8_timeconverted_3[9];
//                         char u8_timeconverted_4[9];

//                         LCD_Clear(LGRAYBLUE);
//                         LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
//                         LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 to come back",16,1); 

//                         convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
//                         LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);           
                             
//                         convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
//                         LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
 
//                         convertTime2StringDisplay(&s_alarmas_manual[2],u8_timeconverted_3);                       
//                         LCD_ShowString(25-1,150-1,LGRAYBLUE,BLACK,u8_timeconverted_3,24,1);
                        
//                         convertTime2StringDisplay(&s_alarmas_manual[3],u8_timeconverted_4);                       
//                         LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,u8_timeconverted_4,24,1);            

//                         u8_key=select_option();
//                     }
//                     break;
//                 case Manual_alarmas_5:
//                     while(u8_key != '1' )
//                     {
//                         char u8_timeconverted_1[9];  
//                         char u8_timeconverted_2[9];
//                         char u8_timeconverted_3[9];
//                         char u8_timeconverted_4[9];
//                         char u8_timeconverted_5[9];

//                         LCD_Clear(LGRAYBLUE);
//                         LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
//                         LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 to come back",16,1); 

//                         convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
//                         LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);           
                             
//                         convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
//                         LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
 
//                         convertTime2StringDisplay(&s_alarmas_manual[2],u8_timeconverted_3);                       
//                         LCD_ShowString(25-1,150-1,LGRAYBLUE,BLACK,u8_timeconverted_3,24,1);
                        
//                         convertTime2StringDisplay(&s_alarmas_manual[3],u8_timeconverted_4);                       
//                         LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,u8_timeconverted_4,24,1);

//                         convertTime2StringDisplay(&s_alarmas_manual[4],u8_timeconverted_5);                       
//                         LCD_ShowString(25-1,210-1,LGRAYBLUE,BLACK,u8_timeconverted_5,24,1);             

//                         u8_key=select_option();
//                     }
//                     break;
//                 default:
//                     printf("---------default de Mirar alarmas-------.\n");
//                     vTaskDelay(pdMS_TO_TICKS(100));
//                     break;
//             }                  
//             break;    
//         default:
//             break;
//     }        
// }

// static uint8_t select_option(void)
// {
//     uint8_t ret=1;
//     uint8_t _u8_key;

//     while(ret)
//     {
//         _u8_key = readKeypad();
//         if (_u8_key =='1' || _u8_key =='2' || _u8_key =='3' || _u8_key =='4' || _u8_key =='5' )
//         {
//             break;
//         }
//         printf("Seleccione una opcion\n");
//         vTaskDelay(pdMS_TO_TICKS(100)); // Esto evita que salte el WD ya que da tiempo a que se ejecute la tarea que refresca el WD
//     }

//     return (_u8_key);                         
// }

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
