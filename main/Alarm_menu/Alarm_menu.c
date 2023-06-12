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

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/

typedef enum{
    Manual,
    Automatico
}MENU_OPT;

typedef enum{
    Manual_alarmas_3=1,
    Adulto_alarmas,
    Cachorro_alarmas
}ACTIVAR_ALARM;

tm_t s_alarmas_auto[]={
    {.tm_hour=6,.tm_min=0,.tm_sec=0,}, // adulto
    {.tm_hour=12,.tm_min=0,.tm_sec=0,}, // adulto
    {.tm_hour=18,.tm_min=0,.tm_sec=0,}, // adulto 
    {.tm_hour=7,.tm_min=0,.tm_sec=0,}, // cachorro 
    {.tm_hour=12,.tm_min=0,.tm_sec=0,}, // cachorro 
    {.tm_hour=17,.tm_min=0,.tm_sec=0,}, // cachorro 
}; // Esta la implementamos por polling

tm_t s_alarmas_manual[]={
    {0}, // Esta tiene interrupcion
    {0}, // Esta tiene interrupcion
    {0}, // Esta de aqui no tendra
};

// Esto se usa como flags para saber que se ha activado alguna de esas alarmas.
// mirar si se puede hacer con bitfields
// uint8_t manual_3     = 0;
// uint8_t automatico_1 = 0;
// uint8_t automatico_2 = 0;
// uint8_t automatico_3 = 0;
// uint8_t automatico_4 = 0;
// uint8_t automatico_5 = 0;
// uint8_t automatico_6 = 0;


extern tm_t time_tc;
extern i2c_dev_t s_dev; // Configurado en Main_screen.c 
extern SemaphoreHandle_t xSemaphore;


/******************************** (3) DEFINES & MACROS *******************************************/
#define ARRAY_SIZE(a) (sizeof(a)/ sizeof(a[0]))

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/
void Activacion_motor();
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
  
    printf("Estoy fuera de alarma");

    vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen
    
    for (;;)
    {
        printf("esta es la tarea alarma");
        vTaskDelay(pdMS_TO_TICKS(100));
    }

        
}

void init_manual_alarm_3(){


    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

    if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min)
    {
        Activacion_motor();
    }
     
}

void init_adulto_alarm(){


    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

    if( time_tc.tm_hour ==s_alarmas_auto[0].tm_hour && time_tc.tm_min ==s_alarmas_auto[0].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[1].tm_hour && time_tc.tm_min ==s_alarmas_auto[1].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[2].tm_hour && time_tc.tm_min ==s_alarmas_auto[2].tm_min)
    {
        Activacion_motor();
    }
     
}

void init_cachorro_alarm(){


    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

   if( time_tc.tm_hour ==s_alarmas_auto[3].tm_hour && time_tc.tm_min ==s_alarmas_auto[3].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[4].tm_hour && time_tc.tm_min ==s_alarmas_auto[4].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[5].tm_hour && time_tc.tm_min ==s_alarmas_auto[5].tm_min)
    {
        Activacion_motor();
    }
     
}

void Activacion_motor()
{
    // Esto es para alarma de manual 1 y 2 
    // if (ds3231_get_alarm_flags(&s_dev,(ds3231_alarm_t *)DS3231_ALARM_BOTH)
    // &&manual_3 && automatico_1 && automatico_2 && automatico_3 && automatico_4
    // && automatico_5 && automatico_6)  // Hay un warnig. tengo que mirarlo bien. creo que ahi tengo q
    // //poner las alarmas que configure
    
        // Hay que poner mutex
        WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_50);
        vTaskDelay(pdMS_TO_TICKS(2000));
        // Hay que poner mutex
        WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_0);
    
}


