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
 *	Log: Button_Handler.c
 * Revision [Date // Author]:  Modifications \n
 * -----------------------------------------------------------\n
 * V1.0 [ 28 oct. 2020 // earmijos ]: Original \n
 *
 */
/*************************************************************************************************/

#ifndef __BUTTON_HANDLER_H__
#define __BUTTON_HANDLER_H__

#ifdef	__cplusplus
extern "C" {
#endif


/**********************************INCLUDES ******************************************************/
#include <stdbool.h>
#include <driver/gpio.h>

#define KEYPAD_DEBOUNCING 100   ///< time in ms
#define KEYPAD_STACKSIZE  5

/********************************* (1) PUBLIC METHODS ********************************************/
/**
  * @brief Esto se encarga de todas las pulsaciones. 
  * De momento solo configura la interrupcion para el pin 0 (boot) del esp32.
  * TODO: falta implemtnar las demas teclas/pulsadores
  *
  * 
  * @param pvParameters no se usa
  *
  * @return void
  *     
  */
void Button_Handler(void);

bool ReadKey( const char _c_key);

void HAL_GPIO_test_Init(void);

int Read2digits(int _buffer);

char keypad_getkey();

esp_err_t keypad_initalize(gpio_num_t keypad_pins[8]);

/*********************************** (2) PUBLIC VARS *********************************************/


/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/






#ifdef	__cplusplus
}
#endif

#endif  /* __BUTTON_HANDLER_H__ */