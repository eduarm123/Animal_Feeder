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


int8_t num1;
char m[1]; //Sirve para almacenar el número ingresado por teclado tipo char
/********************************* (1) PUBLIC METHODS ********************************************/
#define CONFIG_LED_PIN       (2)//2

/*********************************** (2) PUBLIC VARS *********************************************/

/// keypad pinout
///                     R1  R2  R3  R4  C1  C2  C3  C4 
//gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};

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
    ///                     R1  R2  R3  R4  C1  C2  C3  C4 
    gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};
    keypad_initalize(keypad); /// Inicializa keyboard


    RTC_init(&s_dev); // Inizializa el i2c
    Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto. TODO: hay que reemplazar por teclado.
    ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
    //gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen 
    //gpio_set_level(CONFIG_LED_PIN,1); // Para probar en debug
    for (;;)
    {   
        
        printf("Presionar la tecla x para continuar\n");
        //while(!ReadKey("2"));
        printf("Well done\n"); 

        vTaskDelay(pdMS_TO_TICKS(250));
        if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
            continue;
        }

        printf("1.--- Configurar alarmas ---\n");
        printf("2.--- Configurar hora ------\n");

        printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);
    }


}

//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){

    int hour_1; // Esto se debe cambiar a 0. Lo pongo asi para poder configurar desde consola UART
    unsigned num;
    unsigned num1;
    int min_1;  // Esto se debe cambiar a 0. Lo pongo asi para poder configurar desde consola UART

     while (1) {
        //printf("Pressed key: %c\n", hour_1);
        num = keypad_getkey();  /// gets from key queue
        //char m[]={keypressed}; //es otra manera de almacenar el caracter del teclado
        *m = num; //Lo convertimos a puntero al caracter del teclado para usar el atoi
        //fgets(keypressed, sizeof(keypressed),stdin);
        hour_1= atoi(m);
       // printf("Pressed key: %c\n", num1);
        //scanf("%d", &hour_1);
        if (hour_1 > 0 && hour_1 < 24) {
            printf("Presionado: %c\n", hour_1);
            vTaskDelay(pdMS_TO_TICKS(2000));
            break;
        }
        printf("Invalid hour value. Please enter a value between 0 and 23.\n");
        printf("Pressed key: %c\n", hour_1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (1) {
//printf("Pressed key: %c\n", hour_1);
        num1 = keypad_getkey();  /// gets from key queue
        //char m[]={keypressed}; //es otra manera de almacenar el caracter del teclado
        *m = num1; //Lo convertimos a puntero al caracter del teclado para usar el atoi
        //fgets(keypressed, sizeof(keypressed),stdin);
        min_1= atoi(m);
       // printf("Pressed key: %c\n", num1);
        //scanf("%d", &hour_1);
        if (min_1 > 0 && min_1 < 60) {
            printf("Presionado: %c\n", min_1);
            vTaskDelay(pdMS_TO_TICKS(2000));
            break;
        }
        printf("Invalid minute value. Please enter a value between 0 and 59.\n");
        printf("Pressed key: %c\n", min_1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    _time->tm_sec=0;
    _time->tm_hour=hour_1;
    _time->tm_min=min_1;
}