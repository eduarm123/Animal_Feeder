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
#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include <ds3231.h>
#include <string.h>
#include <time.h>

/********************************* (1) PUBLIC METHODS ********************************************/
void ds3231_test(void *pvParameters);

/*********************************** (2) PUBLIC VARS *********************************************/

struct tm time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};
   

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/***************************** (6) PUBLIC METHODS IMPLEMENTATION *********************************/

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
     xTaskCreate(ds3231_test, "ds3231_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}


void ds3231_test(void *pvParameters)
{
    i2c_dev_t dev;
    uint16_t min=0;
    uint16_t hora=0;

    memset(&dev, 0, sizeof(i2c_dev_t));

    ESP_ERROR_CHECK(ds3231_init_desc(&dev, 0, 21, 22));

    printf("Inserte los minutos");
    min=getchar();
    time_tc.tm_min=min;
    printf("Inserte la hora");
    hora=getchar();
    time_tc.tm_hour=hora;
    time_tc.tm_sec=0;

    ESP_ERROR_CHECK(ds3231_set_time(&dev, &time_tc));

    while (1)
    {
        float temp;

        vTaskDelay(pdMS_TO_TICKS(250));

        if (ds3231_get_temp_float(&dev, &temp) != ESP_OK)
        {
            printf("Could not get temperature\n");
            continue;
        }

        if (ds3231_get_time(&dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
            continue;
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("%02d:%02d:%02d, %.2f deg Cel\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec, temp);
    }
}

/************************* (7)  STATIC METHODS IMPLEMENTATION ************************************/






#ifdef __cplusplus
};
#endif