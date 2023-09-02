/*************************************************************************************************/
/*! @file	Alarm_menu.c
 *	@brief	Introducir breve descripcion del fichero
 *
 *	\b Descripciï¿½n: Introducir aquï¿½ descripciï¿½n de las funcionalidades del fichero \n
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

/**********************************INCLUDES ******************************************************/
#include "Alarm_menu.h"
#include "Main_Screen.h"
#include "ds3231.h"
#include "Pwm_motor.h"


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include <time.h>
#include "Button_Handler.h"
#include "easyio.h" //error si incluyo esto de nuevo
//#include "picture.h"

#include "NVM_drivers/NVM_drivers.h"
/********************************* (1) PUBLIC METHODS ********************************************/

#define CONFIG_LED_PIN       (2)//2
/*********************************** (2) PUBLIC VARS *********************************************/

extern unsigned num;
// Esto se usa como flags para saber que se ha activado alguna de esas alarmas.

extern QueueHandle_t commandQueue;
extern tm_t time_tc;
extern i2c_dev_t s_dev; // Configurado en Main_screen.c 

extern tm_t s_alarmas_auto[];
extern tm_t s_alarmas_manual[];
extern uint8_t n_alarms;
extern uint32_t alarm_type;

extern TaskHandle_t MainScreen_Handle;
extern TaskHandle_t AlarmaMenu_Handle;

typedef enum{
    Manual=1
}MENU_OPT;

typedef enum{
    Manual_alarmas_1=1,
    Manual_alarmas_2,
    Manual_alarmas_3,
    Manual_alarmas_4,
    Manual_alarmas_5
}ACTIVAR_ALARM;


bool is_alarm_set = false;
uint8_t command[10]; // tipo de alarmas configurado

QueueHandle_t colaPulsador; // Cola para notificar a las tareas
/******************************** (3) DEFINES & MACROS *******************************************/
#define ARRAY_SIZE(a) (sizeof(a)/ sizeof(a[0]))

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/
void Activacion_motor();
void init_manual_alarm_1();
void init_manual_alarm_2();
void init_manual_alarm_3();
void init_manual_alarm_4();
void init_manual_alarm_5();


/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

/**
 * @brief Menu alarma. Aqui se configura la hora y se monitorea las alarmas. Si alguna alarma salta
 *        se activa el motor.  
 * @param pvParameters 
 * @return void
 */
void Alarm_menu( void * pvParameters )
{
    gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT); // Para el motor
    vTaskDelay(pdMS_TO_TICKS(1000)); // espera de x tiempo para que las otras tareas se inicialicen
    for (;;)
    {   
        printf("--- tarea alarm_menu---\n");    
        // Alarm is set, start monitoring the time                        
        NNVM_read_memory_u32(ALARM_NAMESPACE, &alarm_type);
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 500 milliseconds or any desired interval            
        switch (alarm_type){

            case Manual_alarmas_1: 
                printf("--- Manual_alarmas_1 ---\n");
                init_manual_alarm_1();
                break;
            case Manual_alarmas_2: 
                printf("--- Manual_alarmas_2 ---\n");
                init_manual_alarm_2();
                break;
            case Manual_alarmas_3: 
                printf("--- Manual_alarmas_3 ---\n");
                init_manual_alarm_3();
                break;
            case Manual_alarmas_4: 
                printf("--- Manual_alarmas_4 ---\n");
                init_manual_alarm_4();
                break;
            case Manual_alarmas_5: 
                printf("--- Manual_alarmas_5 ---\n");
                init_manual_alarm_5();
                break;          
            default:
                printf("--- No alarm configured---\n");
                vTaskDelay(pdMS_TO_TICKS(5000));
                break;
        }
         
    }
    
}

void init_manual_alarm_1(){

    printf("--- dentro de monitoreo alarma 1---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
    }

    if(!is_alarm_set){ // variable global. Vuelve a 0 cuando se reinicia el uC

        uint32_t alarm1_NVM = 0;                     
        NNVM_read_memory_u32("alarm1_NVM",&alarm1_NVM);

        if(alarm1_NVM != 0){
            uint8_t hour = (alarm1_NVM >> 8) & 0xFF;
            uint8_t minute = alarm1_NVM & 0xFF;

            s_alarmas_manual[0].tm_hour = hour;
            s_alarmas_manual[0].tm_min = minute;
            is_alarm_set =1;
        }
    }
  
    if( time_tc.tm_hour ==s_alarmas_manual[0].tm_hour && time_tc.tm_min ==s_alarmas_manual[0].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[0].tm_sec)
    {
        /* Send notification to prvTask1() */     
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }   
}

void init_manual_alarm_2(){

    printf("--- dentro de monitoreo alarma 2---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
    }

    if(!is_alarm_set){ // variable global. Vuelve a 0 cuando se reinicia el uC

        uint32_t alarm1_NVM = 0;                     
        NNVM_read_memory_u32("alarm1_NVM",&alarm1_NVM);
        
        uint32_t alarm2_NVM = 0;                     
        NNVM_read_memory_u32("alarm2_NVM",&alarm2_NVM);

        if(alarm1_NVM != 0 && alarm2_NVM != 0 ){
            uint8_t hour1 = (alarm1_NVM >> 8) & 0xFF;
            uint8_t minute1 = alarm1_NVM & 0xFF;

            uint8_t hour2 = (alarm2_NVM >> 8) & 0xFF;
            uint8_t minute2 = alarm2_NVM & 0xFF;

            s_alarmas_manual[0].tm_hour = hour1;
            s_alarmas_manual[0].tm_min = minute1;

            s_alarmas_manual[1].tm_hour = hour2;
            s_alarmas_manual[1].tm_min = minute2;
            is_alarm_set =1;
        }
    }

    if( time_tc.tm_hour ==s_alarmas_manual[0].tm_hour && time_tc.tm_min ==s_alarmas_manual[0].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[0].tm_sec)
    {
        /* Send notification to prvTask1() */     
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[1].tm_hour && time_tc.tm_min ==s_alarmas_manual[1].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[1].tm_sec)
    {
        /* Send notification to prvTask1() */     
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }
     
}

void init_manual_alarm_3(){

    printf("--- dentro de monitoreo alarma 3---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
    }

    if(!is_alarm_set){ // variable global. Vuelve a 0 cuando se reinicia el uC

        uint32_t alarm1_NVM = 0;                     
        NNVM_read_memory_u32("alarm1_NVM",&alarm1_NVM);
        
        uint32_t alarm2_NVM = 0;                     
        NNVM_read_memory_u32("alarm2_NVM",&alarm2_NVM);

        uint32_t alarm3_NVM = 0;                     
        NNVM_read_memory_u32("alarm3_NVM",&alarm3_NVM);

        if(alarm1_NVM != 0 || alarm2_NVM != 0 || alarm3_NVM != 0 ){
            uint8_t hour1 = (alarm1_NVM >> 8) & 0xFF;
            uint8_t minute1 = alarm1_NVM & 0xFF;

            uint8_t hour2 = (alarm2_NVM >> 8) & 0xFF;
            uint8_t minute2 = alarm2_NVM & 0xFF;

            uint8_t hour3 = (alarm3_NVM >> 8) & 0xFF;
            uint8_t minute3 = alarm3_NVM & 0xFF;

            s_alarmas_manual[0].tm_hour = hour1;
            s_alarmas_manual[0].tm_min = minute1;

            s_alarmas_manual[1].tm_hour = hour2;
            s_alarmas_manual[1].tm_min = minute2;

            s_alarmas_manual[2].tm_hour = hour3;
            s_alarmas_manual[2].tm_min = minute3;
            is_alarm_set =1;
        }
    }

    if( time_tc.tm_hour ==s_alarmas_manual[0].tm_hour && time_tc.tm_min ==s_alarmas_manual[0].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[0].tm_sec)
    {    
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[1].tm_hour && time_tc.tm_min ==s_alarmas_manual[1].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[1].tm_sec)
    {    
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

        if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[2].tm_sec)
    {    
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }
     
}

void init_manual_alarm_4(){

    printf("--- dentro de monitoreo alarma 4---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
    }

    if(!is_alarm_set){ // variable global. Vuelve a 0 cuando se reinicia el uC

        uint32_t alarm1_NVM = 0;                     
        NNVM_read_memory_u32("alarm1_NVM",&alarm1_NVM);
        
        uint32_t alarm2_NVM = 0;                     
        NNVM_read_memory_u32("alarm2_NVM",&alarm2_NVM);

        uint32_t alarm3_NVM = 0;                     
        NNVM_read_memory_u32("alarm3_NVM",&alarm3_NVM);

        uint32_t alarm4_NVM = 0;                     
        NNVM_read_memory_u32("alarm4_NVM",&alarm4_NVM);


        if(alarm1_NVM != 0 || alarm2_NVM != 0 || alarm3_NVM != 0 || alarm4_NVM != 0){
            uint8_t hour1 = (alarm1_NVM >> 8) & 0xFF;
            uint8_t minute1 = alarm1_NVM & 0xFF;

            uint8_t hour2 = (alarm2_NVM >> 8) & 0xFF;
            uint8_t minute2 = alarm2_NVM & 0xFF;

            uint8_t hour3 = (alarm3_NVM >> 8) & 0xFF;
            uint8_t minute3 = alarm3_NVM & 0xFF;

            uint8_t hour4 = (alarm4_NVM >> 8) & 0xFF;
            uint8_t minute4 = alarm4_NVM & 0xFF;


            s_alarmas_manual[0].tm_hour = hour1;
            s_alarmas_manual[0].tm_min = minute1;

            s_alarmas_manual[1].tm_hour = hour2;
            s_alarmas_manual[1].tm_min = minute2;

            s_alarmas_manual[2].tm_hour = hour3;
            s_alarmas_manual[2].tm_min = minute3;

            s_alarmas_manual[3].tm_hour = hour4;
            s_alarmas_manual[3].tm_min = minute4;


            is_alarm_set =1;
        }
    }

    if( time_tc.tm_hour ==s_alarmas_manual[0].tm_hour && time_tc.tm_min ==s_alarmas_manual[0].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[0].tm_sec)
    {
            
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[1].tm_hour && time_tc.tm_min ==s_alarmas_manual[1].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[1].tm_sec)
    {
             
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

     if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[2].tm_sec)
    {
             
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[3].tm_hour && time_tc.tm_min ==s_alarmas_manual[3].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[3].tm_sec)
    {
            
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[4].tm_hour && time_tc.tm_min ==s_alarmas_manual[4].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[4].tm_sec)
    {         
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }
  
}

void init_manual_alarm_5(){

    printf("--- dentro de monitoreo alarma 5---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
    }

    if(!is_alarm_set){ // variable global. Vuelve a 0 cuando se reinicia el uC

        uint32_t alarm1_NVM = 0;                     
        NNVM_read_memory_u32("alarm1_NVM",&alarm1_NVM);
        
        uint32_t alarm2_NVM = 0;                     
        NNVM_read_memory_u32("alarm2_NVM",&alarm2_NVM);

        uint32_t alarm3_NVM = 0;                     
        NNVM_read_memory_u32("alarm3_NVM",&alarm3_NVM);

        uint32_t alarm4_NVM = 0;                     
        NNVM_read_memory_u32("alarm4_NVM",&alarm4_NVM);

        uint32_t alarm5_NVM = 0;                     
        NNVM_read_memory_u32("alarm5_NVM",&alarm5_NVM);

        if(alarm1_NVM != 0 || alarm2_NVM != 0 || alarm3_NVM != 0 || alarm4_NVM != 0 || alarm5_NVM != 0 ){
            uint8_t hour1 = (alarm1_NVM >> 8) & 0xFF;
            uint8_t minute1 = alarm1_NVM & 0xFF;

            uint8_t hour2 = (alarm2_NVM >> 8) & 0xFF;
            uint8_t minute2 = alarm2_NVM & 0xFF;

            uint8_t hour3 = (alarm3_NVM >> 8) & 0xFF;
            uint8_t minute3 = alarm3_NVM & 0xFF;

            uint8_t hour4 = (alarm4_NVM >> 8) & 0xFF;
            uint8_t minute4 = alarm4_NVM & 0xFF;

            uint8_t hour5 = (alarm5_NVM >> 8) & 0xFF;
            uint8_t minute5 = alarm5_NVM & 0xFF;

            s_alarmas_manual[0].tm_hour = hour1;
            s_alarmas_manual[0].tm_min = minute1;

            s_alarmas_manual[1].tm_hour = hour2;
            s_alarmas_manual[1].tm_min = minute2;

            s_alarmas_manual[2].tm_hour = hour3;
            s_alarmas_manual[2].tm_min = minute3;

            s_alarmas_manual[3].tm_hour = hour4;
            s_alarmas_manual[3].tm_min = minute4;

            s_alarmas_manual[4].tm_hour = hour5;
            s_alarmas_manual[4].tm_min = minute5;

            is_alarm_set =1;
        }
    }

    if( time_tc.tm_hour ==s_alarmas_manual[0].tm_hour && time_tc.tm_min ==s_alarmas_manual[0].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[0].tm_sec)
    {
            
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[1].tm_hour && time_tc.tm_min ==s_alarmas_manual[1].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[1].tm_sec)
    {
             
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

     if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[2].tm_sec)
    {
             
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[3].tm_hour && time_tc.tm_min ==s_alarmas_manual[3].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[3].tm_sec)
    {
            
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }

    if( time_tc.tm_hour ==s_alarmas_manual[4].tm_hour && time_tc.tm_min ==s_alarmas_manual[4].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[4].tm_sec)
    {         
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }
  
}


void Activacion_motor()
{

    //LCD_Clear(LGRAYBLUE);
    //LCD_ShowString(50-1,110-1,LGRAYBLUE,BLACK,"Moviendo motor",24,1);
    // Hay que poner mutex
    gpio_set_level(CONFIG_LED_PIN,1); // Para probar en debug
    //WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_50);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // Hay que poner mutex
    //WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_0);
    gpio_set_level(CONFIG_LED_PIN,0);   
}


 
 