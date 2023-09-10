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

// Define the GPIO pins for rows and columns
s_gpio_t colPins[] = {
    {GPIO_NUM_18, GPIO_MODE_OUTPUT},
    {GPIO_NUM_23, GPIO_MODE_OUTPUT},
    {GPIO_NUM_4, GPIO_MODE_OUTPUT},
    {GPIO_NUM_22, GPIO_MODE_OUTPUT}
};


s_gpio_t rowPins[] = {   
    {GPIO_NUM_27, GPIO_MODE_INPUT},
    {GPIO_NUM_26, GPIO_MODE_INPUT},
    {GPIO_NUM_25, GPIO_MODE_INPUT},
    {GPIO_NUM_33, GPIO_MODE_INPUT}
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
        //time_old_isr=0;
        time_old_isr = time_now_isr;
    }
    
    //time_old_isr = time_now_isr;
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

char readKeypad() {
    char key = 0;
    for (int i = 0; i < 4; i++) {

        gpio_set_level(colPins[(i+1)%4].e_gpioID, 0);
        gpio_set_level(colPins[(i+2)%4].e_gpioID, 0);
        gpio_set_level(colPins[(i+3)%4].e_gpioID, 0);
        gpio_set_level(colPins[i].e_gpioID, 1); // Set the current column to HIGH

        for (int j = 0; j < 4; j++) {
            if (gpio_get_level(rowPins[j].e_gpioID) == 1) {                               
                while (gpio_get_level(rowPins[j].e_gpioID) == 1);               
                key = keypad[i + j*4];
                vTaskDelay(10 / portTICK_PERIOD_MS); // Debounce delay
                
            }
        }
    }
    return key;
}

// Function to initialize the keypad
void keypadInit() {
    esp_err_t ret=ESP_FAIL;

    for (int i = 0; i < 4; i++) {
        gpio_set_direction(rowPins[i].e_gpioID, rowPins[i].e_gpioMode);
        ret=gpio_set_pull_mode(rowPins[i].e_gpioID,GPIO_PULLUP_PULLDOWN);
        if (ret !=ESP_OK){
            printf("ERROR");
        }
    }
    for (int i = 0; i < 4; i++) {
        gpio_set_direction(colPins[i].e_gpioID, colPins[i].e_gpioMode);
    }
}



