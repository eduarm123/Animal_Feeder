/*************************************************************************************************/
/*! @file	main.c
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

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************INCLUDES ******************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include "freertos/semphr.h"

#include <freertos/task.h>
#include <ds3231.h>
#include <string.h>
#include <time.h>

#include "ds3231.h"
#include "Main_Screen.h"
#include "Alarm_menu.h"
#include "Button_Handler.h"

#include "Pwm_motor.h"
//#include "easyio.h"

//#include "esp_err.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/


/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
//extern SemaphoreHandle_t LlaveGlobal;

void app_main(void)
{
    //extern SemaphoreHandle_t LlaveGlobal;
    LlaveGlobal = xSemaphoreCreateBinary();
    xSemaphoreGive(LlaveGlobal);

	static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;

    xTaskCreatePinnedToCore(Main_Screen,
                "Main_Screen",
                configMINIMAL_STACK_SIZE * 3,
                &ucParameterToPass,
                1, //tskIDLE_PRIORITY (Prioridad)
                &xHandle,
                1);

    xTaskCreatePinnedToCore(Alarm_menu, 
                "Alarm_menu",
                configMINIMAL_STACK_SIZE * 3,
                &ucParameterToPass,
                1, //tskIDLE_PRIORITY (Prioridad)
                &xHandle,
                1);
    
}

#ifdef __cplusplus
};
#endif