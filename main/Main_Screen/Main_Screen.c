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


/**********************************INCLUDES ******************************************************/
#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include <ds3231.h>
#include <string.h>
#include <time.h>
#include "ds3231.h"
#include "Main_Screen.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/
struct tm time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

extern i2c_dev_t dev; // necessary for RTC_init()

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/


void Main_Screen( void * pvParameters )
{
    bool b_ret=FALSE;
    uint32_t hour_1=0;
    uint32_t min_1=0;

    RTC_init();

    
        printf("Set the time:hour ");
        scanf("%d", &hour_1);
        time_tc.tm_hour=hour_1;
        printf("Set the time:min ");
        scanf("%d", &min_1);
        time_tc.tm_min=min_1;
        time_tc.tm_sec=0;

    ESP_ERROR_CHECK(ds3231_set_time(&dev, &time_tc));

    for (;;)
    {      

        vTaskDelay(pdMS_TO_TICKS(250));
        if (ds3231_get_time(&dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
            continue;
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);
    }

}