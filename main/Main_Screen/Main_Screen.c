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


/**********************************INCLUDES ******************************************************/
#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include <string.h>
#include <time.h>
#include "ds3231.h"
#include "Main_Screen.h"
#include "Button_Handler.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/

tm_t time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

i2c_dev_t s_dev; // necessary for RTC_init()

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/


void Main_Screen( void * pvParameters )
{
    
    int bufff=0;
    int ret;
        while(1){

/*           if(ReadKey('1')){
            printf("Well done");
          }
          
          vTaskDelay(1); */

          ret=Read2digits(bufff);

          printf("%d", ret);

        }

    
}




//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){

    int hour_1=65; // Esto se debe cambiar a 0. Lo pongo asi para poder configurar desde consola UART
    int min_1=65;  // Esto se debe cambiar a 0. Lo pongo asi para poder configurar desde consola UART

     while (1) {
        printf("hora: ");
        scanf("%d", &hour_1);
        if (hour_1 >= 0 && hour_1 < 24) {
            break;
        }
        printf("Invalid hour value. Please enter a value between 0 and 23.\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (1) {
        printf("min: ");
        scanf("%d", &min_1);
        if (min_1 >= 0 && min_1 < 60) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Invalid minute value. Please enter a value between 0 and 59.\n");
    }

    _time->tm_sec=0;
    _time->tm_hour=hour_1;
    _time->tm_min=min_1;
}