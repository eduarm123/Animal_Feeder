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

extern TaskHandle_t MainScreen_Handle;
extern TaskHandle_t AlarmaMenu_Handle;

typedef enum{
    Manual=1,
    Automatico
}MENU_OPT;

typedef enum{
    Manual_alarmas_1=1,
    Manual_alarmas_2,
    Manual_alarmas_3,
    Adulto_alarmas,
    Cachorro_alarmas
}ACTIVAR_ALARM;

bool is_manual_alarm = false;
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
void init_adulto_alarm();
void init_cachorro_alarm();


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
    vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen
    for (;;)
    {
        printf("--- tarea alarm_menu---\n");    
        if (xQueueReceive(commandQueue, &command, portMAX_DELAY) == pdPASS) {
            if (command[0] == Automatico) {
                
                is_manual_alarm = false;
                is_alarm_set = true;
            } 
            else if (command[0] == Manual) {

                is_manual_alarm = true;
                is_alarm_set = true;
            }
        }    
        // Alarm is set, start monitoring the time
        while (is_alarm_set) {
            
            if (is_manual_alarm) {
                
                switch (n_alarms){

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
                    default:
                        printf("--- default ---\n");
                        break;
                }
            } 
            
            else {

                if (n_alarms == Adulto_alarmas){
                    printf("--- Automatico_Alarmas ---\n");
                    init_adulto_alarm();

                }
                else if (n_alarms == Cachorro_alarmas){
                    printf("--- Cachorro alarmas---\n");
                    init_cachorro_alarm();
                }
            }

            vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 500 milliseconds or any desired interval
        }
    
    vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen

    }
    
}

void init_manual_alarm_1(){

    printf("--- dentro de monitoreo alarma 1---\n");
    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
    {
        printf("Could not get time\n");
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

     if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min
    && time_tc.tm_sec ==s_alarmas_manual[2].tm_sec)
    {
        /* Send notification to prvTask1() */     
        printf("--- Se ha activado la alarma---\n");             
        Activacion_motor();      
    }
     
}

void init_adulto_alarm(){

    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

    if( time_tc.tm_hour ==s_alarmas_auto[0].tm_hour && time_tc.tm_min ==s_alarmas_auto[0].tm_min
    && time_tc.tm_sec ==s_alarmas_auto[0].tm_sec)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[1].tm_hour && time_tc.tm_min ==s_alarmas_auto[1].tm_min
    && time_tc.tm_sec ==s_alarmas_auto[1].tm_sec)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[2].tm_hour && time_tc.tm_min ==s_alarmas_auto[2].tm_min
    && time_tc.tm_sec ==s_alarmas_auto[2].tm_sec)
    {
        Activacion_motor();
    }
     
}

void init_cachorro_alarm(){

    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

   if( time_tc.tm_hour ==s_alarmas_auto[3].tm_hour && time_tc.tm_min ==s_alarmas_auto[3].tm_min
   && time_tc.tm_sec ==s_alarmas_auto[3].tm_sec)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[4].tm_hour && time_tc.tm_min ==s_alarmas_auto[4].tm_min
    && time_tc.tm_sec ==s_alarmas_auto[4].tm_sec)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[5].tm_hour && time_tc.tm_min ==s_alarmas_auto[5].tm_min
    && time_tc.tm_sec ==s_alarmas_auto[5].tm_sec)
    {
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

 