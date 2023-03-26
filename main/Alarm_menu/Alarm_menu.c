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


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <time.h>
#include "Button_Handler.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/

typedef enum{
    Manual,
    Automatico
}MENU_OPT;

tm_t time_alarm_1=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

tm_t time_alarm_2=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};


extern i2c_dev_t s_dev; // Configurado en Main_screen.c 
extern SemaphoreHandle_t xSemaphore;

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/


// Aqui se deberia entrar por una interrupcion
void Alarm_menu( void * pvParameters )
{
    int manual_alarm=-1; // utilizo esto de momento por la funcion scanf. Valor -1 por la UART. Se cambio cuando se utilice display
    int age_option=-1;
    static uint32_t alarm_state = Manual; 

    vTaskDelay(pdMS_TO_TICKS(1000)); // espera de x tiempo para que las otras tareas se inicialicen

    for (;;)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY); // Aqui se espera la interrupcion del boton PUSH_BUTTON_PIN 33

        switch (alarm_state) // TODO: cambiar esto. Ahora entra directo a Manual
        {
        case Manual:
            
            printf("Please slecte how many alarms you want");
             while (1) {
                printf("Alarms: ");
                scanf("%d", &manual_alarm);
                if (manual_alarm > 0 && manual_alarm <3 ) {
                    break;
                }
                printf("Invalid alarm value. Please enter a value greater than 0.\n");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            if (manual_alarm ==1)
            {
                Time_config(&time_alarm_1);
                ds3231_set_alarm(&s_dev, DS3231_ALARM_1, &time_alarm_1, DS3231_ALARM1_MATCH_SECMINHOUR, 0, 0);
                ds3231_enable_alarm_ints(&s_dev, DS3231_ALARM_1); // Se activa las interrupciones de alarma
                // Esto deberia activar la funcion motor
            }
            else if (manual_alarm ==2)
            {
                Time_config(&time_alarm_1);
                Time_config(&time_alarm_2);
                ds3231_set_alarm(&s_dev, DS3231_ALARM_1, &time_alarm_1, DS3231_ALARM1_MATCH_SECMINHOUR, &time_alarm_2,
                 DS3231_ALARM2_MATCH_MINHOUR);
                ds3231_enable_alarm_ints(&s_dev, DS3231_ALARM_BOTH); // Se activa las interrupciones de alarma
                // Esto deberia activar la funcion motor
            }
            break;
        case Automatico:
             printf("Select 1.Adulto o 2.Cachorro");
             while (1) { // Esto se debe eliminar once we have the keyboard
                printf("Age: ");
                scanf("%d", &age_option);
                if (age_option > 0 && age_option <3 ) {
                    break;
                }
                printf("Invalid alarm value. Please enter a value greater than 0.\n");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            //TODO: falta implementar
            break;
        default:
            // TODO: Seria bueno poner un boton de cancelar
            printf("Please select an option");
            break;
        }

        xSemaphoreGive(xSemaphore);// se libera el semaforo
    }

}