/*************************************************************************************************/
/*! @file	Button_Handler.c
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
#include "Button_Handler.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "freertos/semphr.h"


/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/
TaskHandle_t ISR = NULL;
SemaphoreHandle_t xSemaphore;

/******************************** (3) DEFINES & MACROS *******************************************/
#define CONFIG_LED_PIN 2
#define ESP_INR_FLAG_DEFAULT 0
#define PUSH_BUTTON_PIN 0 // Boot button in the esp32

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
// interrupt service routine, called when the button is pressed
void IRAM_ATTR button_isr_handler(void* arg) {
    
    xTaskResumeFromISR(ISR); 

}

//task that will react to button clicks
 void button_task(void *arg)
{
    bool toggle=false;

    while(1){  
        vTaskSuspend(NULL);
        gpio_set_level(CONFIG_LED_PIN,toggle^=1); // Para probar en debug
        //BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Set the binary semaphore to unblock the waiting task
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
        // If a higher priority task is woken up by the semaphore give, yield
        //portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void Button_Handler( void * pvParameters )
{

    gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(PUSH_BUTTON_PIN, GPIO_MODE_INPUT);

    gpio_set_intr_type(PUSH_BUTTON_PIN, GPIO_INTR_NEGEDGE); // falling edge

    xSemaphore = xSemaphoreCreateBinary(); // esto es para activar las alarmas

    // Install the driver's GPIO ISR handler service, which allows per-pin GPIO interrupt handler
    //Install ISR service with defautl configuration
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    gpio_isr_handler_add(PUSH_BUTTON_PIN, button_isr_handler, NULL);

    xTaskCreate( button_task, "button_task", 4096, NULL , 10,&ISR );
    
    vTaskDelay(pdMS_TO_TICKS(1000)); // espera de x tiempo para que las otras tareas se inicialicen


    for (;;)
    {      
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}