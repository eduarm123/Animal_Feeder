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
#include <string.h>


/********************************* (1) PUBLIC METHODS ********************************************/
typedef volatile struct
{
    gpio_num_t e_gpioID;
    gpio_mode_t e_gpioMode;

}s_gpio_t;

/*********************************** (2) PUBLIC VARS *********************************************/
TaskHandle_t ISR = NULL;
SemaphoreHandle_t xSemaphore;

/******************************** (3) DEFINES & MACROS *******************************************/
#define CONFIG_LED_PIN       (1)//2
#define ESP_INR_FLAG_DEFAULT (0)
#define PUSH_BUTTON_PIN_0    (2)//0 // Boot button in the esp32

// #define PUSH_BUTTON_PIN_1   (15)
// #define PUSH_BUTTON_PIN_2   (14)
// #define PUSH_BUTTON_PIN_3   (13)
// #define PUSH_BUTTON_PIN_4   (12)

/*-----------------PINES CON ERROR----------------*/
/*#define PUSH_BUTTON_PIN_1   (12)
#define PUSH_BUTTON_PIN_2   (13)
#define PUSH_BUTTON_PIN_3   (14)
#define PUSH_BUTTON_PIN_4   (15)

#define PUSH_BUTTON_PIN_5   (35)
#define PUSH_BUTTON_PIN_6   (34)
#define PUSH_BUTTON_PIN_7   (33)
#define PUSH_BUTTON_PIN_8   (32)*/
/*-------------------------------------------------*/

#define PUSH_BUTTON_PIN_1   (32)
#define PUSH_BUTTON_PIN_2   (33)
#define PUSH_BUTTON_PIN_3   (25)
#define PUSH_BUTTON_PIN_4   (26)

#define PUSH_BUTTON_PIN_5   (27)
#define PUSH_BUTTON_PIN_6   (14)
#define PUSH_BUTTON_PIN_7   (12)
#define PUSH_BUTTON_PIN_8   (13)


#define FILA                  (4)
#define COLUMNA               (4)


/*********************************** (4) PRIVATE VARS ********************************************/
static s_gpio_t vs_as_keyboard_t[8] =
{
 {
  .e_gpioID = PUSH_BUTTON_PIN_1,
  .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_2,
    .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_3,
  .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_4,
    .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_5,
  .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_6,
    .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_7,
  .e_gpioMode = GPIO_MODE_INPUT,
 },
 {
  .e_gpioID = PUSH_BUTTON_PIN_8,
    .e_gpioMode = GPIO_MODE_INPUT,
 },
};


static const char* const MATRIX[FILA][COLUMNA]={{"1","2","3","A"},
                                                {"4","5","6","B"},
                                                {"7","8","9","C"},
                                                {"Esc","0","#","D"}};


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

static bool TEST_GPIO_Debounce_button(s_gpio_t* _p_s_gpio, bool _b_Value_pressed){
    bool b_ret = true;
    while(gpio_get_level(_p_s_gpio->e_gpioID)== _b_Value_pressed)
    {
        b_ret = false;
    }
    return (b_ret);

}

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

bool ReadKey(const char *const _c_key){

    uint32_t u32_itcol, u32_itrow, u32_it;
    bool b_KeyPressed=false;


    for(u32_it = 0; u32_it<8; u32_it++){
        
        gpio_set_direction(vs_as_keyboard_t[u32_it].e_gpioID, vs_as_keyboard_t[u32_it].e_gpioMode);
        gpio_set_pull_mode(vs_as_keyboard_t[u32_it].e_gpioID, GPIO_PULLUP_ONLY);
    }

     for(u32_itrow = 4; u32_itrow<8; u32_itrow ++){
        
        /* Se configutra las salidas asi para evitar cortos  */
        vs_as_keyboard_t[((u32_itrow +1)%4)+4].e_gpioMode  = GPIO_MODE_INPUT;
        vs_as_keyboard_t[((u32_itrow +2)%4)+4].e_gpioMode  = GPIO_MODE_INPUT;
        vs_as_keyboard_t[((u32_itrow +3)%4)+4].e_gpioMode  = GPIO_MODE_INPUT;
        vs_as_keyboard_t[u32_itrow].e_gpioMode         = GPIO_MODE_OUTPUT;

        /* Aqui se envia a la funcion gpio_set_direction   */
        gpio_set_direction(vs_as_keyboard_t[((u32_itrow +1)%4)+4].e_gpioID, vs_as_keyboard_t[u32_it].e_gpioMode);
        gpio_set_direction(vs_as_keyboard_t[((u32_itrow +2)%4)+4].e_gpioID, vs_as_keyboard_t[u32_it].e_gpioMode);
        gpio_set_direction(vs_as_keyboard_t[((u32_itrow +3)%4)+4].e_gpioID, vs_as_keyboard_t[u32_it].e_gpioMode);
        gpio_set_direction(vs_as_keyboard_t[u32_itrow].e_gpioID, vs_as_keyboard_t[u32_it].e_gpioMode);

        gpio_set_level(vs_as_keyboard_t[u32_itrow].e_gpioID,0); // Se activa salida uno por uno en cada ciclo


         for(u32_itcol = 0; u32_itcol <4; u32_itcol ++){
             
             if(!(TEST_GPIO_Debounce_button(&vs_as_keyboard_t[u32_itcol],0))){
               vTaskDelay(pdMS_TO_TICKS(100));
               if(!strcmp(_c_key, MATRIX[u32_itrow-4][u32_itcol])){
                   b_KeyPressed=true;
               }
               else {

                   b_KeyPressed = false;
               }

             }
         }
     }

     return (b_KeyPressed);
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
        gpio_set_level(CONFIG_LED_PIN,toggle^=1); // Para probar en debug
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

    gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT); // Lo tengo para debugear
    
    //Pulsadores
    gpio_set_direction(PUSH_BUTTON_PIN_0, GPIO_MODE_INPUT);

    gpio_set_intr_type(PUSH_BUTTON_PIN_0, GPIO_INTR_NEGEDGE); // falling edge

    xSemaphore = xSemaphoreCreateBinary(); // esto es para activar las alarmas

    // Install the driver's GPIO ISR handler service, which allows per-pin GPIO interrupt handler
    //Install ISR service with defautl configuration
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    gpio_isr_handler_add(PUSH_BUTTON_PIN_0, button_isr_handler, NULL);

    xTaskCreate( button_task, "button_task", 4096, NULL , 10,&ISR );
    
    vTaskDelay(pdMS_TO_TICKS(1000)); // espera de x tiempo para que las otras tareas se inicialicen

}

