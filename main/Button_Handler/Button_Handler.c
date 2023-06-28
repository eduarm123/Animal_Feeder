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
#include <memory.h>
#include <time.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <driver/gpio.h>
//#include <freertos/queue.h>
#include "freertos/semphr.h"
#include <string.h>
#include "easyio.h"

/********************************* (1) PUBLIC METHODS ********************************************/
typedef volatile struct
{
    gpio_num_t e_gpioID;
    gpio_mode_t e_gpioMode;

}s_gpio_t;

/*********************************** (2) PUBLIC VARS *********************************************/
TaskHandle_t ISR = NULL;
SemaphoreHandle_t xSemaphore;
bool keypad_processing = false;
/******************************** (3) DEFINES & MACROS *******************************************/
//#define CONFIG_LED_PIN       (2)//2
//#define ESP_INR_FLAG_DEFAULT (0)
//#define PUSH_BUTTON_PIN_0    (4)//0 // Boot button in the esp32

/** \brief Keypad mapping array*/
const char keypad[] = { 
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};  

/** \brief Keypad configuration pions*/
static gpio_num_t _keypad_pins[8];

/** \brief Last isr time*/
time_t time_old_isr = 0;

/** \brief Pressed keys queue*/
QueueHandle_t keypad_queue;

/**
 * @brief Handle keypad click
 * @param [in]args row number
 */
void intr_click_handler(void *args);

/**
 * @brief Enable rows'pin pullup resistor, and isr. Prepares
 * keypad to read pressed row number.
 */
void turnon_rows()
{
    for(int i = 4; i < 8; i++) /// Columns
    {
        gpio_set_pull_mode(_keypad_pins[i], GPIO_PULLDOWN_ONLY);
    }
    for(int i = 0; i < 4; i++) /// Rows
    {
        gpio_set_pull_mode(_keypad_pins[i], GPIO_PULLUP_ONLY);
        gpio_intr_enable(_keypad_pins[i]);
    }
}

/**
 * @brief Enable columns'pin pullup resistor, and disable rows isr and pullup resistor.
 * Prepares keypad to read pressed column number. 
 */
void turnon_cols()
{
    for(int i = 0; i < 4; i++) /// Rows
    {
        gpio_intr_disable(_keypad_pins[i]);
        gpio_set_pull_mode(_keypad_pins[i], GPIO_PULLDOWN_ONLY);
    }
    for(int i = 4; i < 8; i++) /// Columns
    {
        gpio_set_pull_mode(_keypad_pins[i], GPIO_PULLUP_ONLY);
    }
}

esp_err_t keypad_initalize(gpio_num_t keypad_pins[8])
{
    memcpy(_keypad_pins, keypad_pins, 8*sizeof(gpio_num_t));

    /** Maybe cause issues if try to desinstall this flag because it's global allocated 
     * to all pins try to use gpio_isr_register instrad of gpio_install_isr_service **/
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_install_isr_service(ESP_INTR_FLAG_EDGE));
    for(int i = 0; i < 4; i++) /// Rows
    {
        gpio_intr_disable(keypad_pins[i]);
        gpio_set_direction(keypad_pins[i], GPIO_MODE_INPUT);
        gpio_set_intr_type(keypad_pins[i], GPIO_INTR_NEGEDGE);
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_isr_handler_add(_keypad_pins[i], (void*)intr_click_handler, (void*)i));
        
    }
    for(int i = 4; i < 8; i++)
    {
        gpio_set_direction(keypad_pins[i], GPIO_MODE_INPUT);
    }

    keypad_queue = xQueueCreate(5, sizeof(char));
    if(keypad_queue == NULL)
        return ESP_ERR_NO_MEM;

    turnon_rows();

    return ESP_OK;
}

void intr_click_handler(void* args)
{
    /*int index = (int)(args);
    time_t time_now_isr = time(NULL);
    time_t time_isr = (time_now_isr - time_old_isr) * 1000L;
    
    if (time_isr >= KEYPAD_DEBOUNCING && !keypad_processing)
    {
        keypad_processing = true;
        
        turnon_cols();
        for (int j = 4; j < 8; j++)
        {
            if (!gpio_get_level(_keypad_pins[j]))
            {
                xQueueSendFromISR(keypad_queue, &keypad[index * 4 + j - 4], NULL);
                break;
            }
        }
        turnon_rows();
        
        keypad_processing = false;
    }
    time_old_isr = time_now_isr;*/


    int index = (int)(args);
    //unsigned num;
    time_t time_now_isr = time(NULL);
    time_t time_isr = (time_now_isr - time_old_isr)*1000L;
    //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"HOLA111",16,1);
    if(time_isr >= KEYPAD_DEBOUNCING)
    {
        turnon_cols();
        for(int j = 4; j < 8; j++)
        {
            if(!gpio_get_level(_keypad_pins[j]))
            {
                xQueueSendFromISR(keypad_queue, &keypad[index*4 + j - 4], NULL);
                break;
            }
        }
        turnon_rows();
    }
    
    time_old_isr = time_now_isr;
}

char keypad_getkey()
{
    char key;
    if(!uxQueueMessagesWaiting(keypad_queue)) /// if is empty, return teminator character
        return '\0';
    xQueueReceive(keypad_queue, &key, portMAX_DELAY);    
    return key;
}

void keypad_delete()
{
    for(int i = 0; i < 8; i++)
    {   
        gpio_isr_handler_remove(_keypad_pins[i]);
        gpio_set_direction(_keypad_pins[i], GPIO_MODE_DISABLE);
    }
    vQueueDelete(keypad_queue);
}




// interrupt service routine, called when the button is pressed
void IRAM_ATTR button_isr_handler(void* arg) {    
    xTaskResumeFromISR(ISR); 
}


/**
  * @brief Esta tarea se activa cuando se pulsa el pulsador boot
  * TODO: falta implemtnar las demas teclas/pulsadores
  *
  * 
  * @param arg no se usa
  *
  * @return void
  *     
  */
 void button_task(void *arg)
{
    bool toggle=false;

    while(1){  
        vTaskSuspend(NULL); // Se suspende la tarea
        //----gpio_set_level(CONFIG_LED_PIN,toggle^=1); // Para probar en debug
        //BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Set the binary semaphore to unblock the waiting task
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken); // Desbloquea la tarea de Alarma
        // If a higher priority task is woken up by the semaphore give, yield
        //portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void Button_Handler()
{

    //----gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT); // Lo tengo para debugear
    
    //Pulsadores
    //----gpio_set_direction(PUSH_BUTTON_PIN_0, GPIO_MODE_INPUT);

    //----gpio_set_intr_type(PUSH_BUTTON_PIN_0, GPIO_INTR_NEGEDGE); // falling edge

    xSemaphore = xSemaphoreCreateBinary(); // esto es para activar las alarmas

    // Install the driver's GPIO ISR handler service, which allows per-pin GPIO interrupt handler
    //Install ISR service with defautl configuration
    //----gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    //---gpio_isr_handler_add(PUSH_BUTTON_PIN_0, button_isr_handler, NULL);

    xTaskCreate( button_task, "button_task", 4096, NULL , 10,&ISR );
    
    vTaskDelay(pdMS_TO_TICKS(1000)); // espera de x tiempo para que las otras tareas se inicialicen

}

