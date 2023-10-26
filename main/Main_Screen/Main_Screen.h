/*************************************************************************************************/
/*! @file	Main_Screen.h
 *	@brief	Introducir breve descripcion del fichero
 *
 *	\b Descripcion: Introducir aquï¿½ descripciï¿½n de las funcionalidades del fichero \n
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

#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__

#ifdef	__cplusplus
extern "C" {
#endif


/**********************************INCLUDES ******************************************************/
#include <time.h>
#include <freertos/FreeRTOS.h>
#include "freertos/semphr.h"

#define ALARM_NAMESPACE "alarm_type"

/*********************************** (2) PUBLIC VARS *********************************************/
extern SemaphoreHandle_t LlaveGlobal;
extern QueueHandle_t colaPulsador; // Cola para notificar a las tareas
extern TaskHandle_t xHandle1;
extern TaskHandle_t xHandle;
extern unsigned num;
//extern uint8_t u8_TimeConfigDone;


typedef struct tm tm_t;


//extern struct tm time_tc;
/********************************* (1) PUBLIC METHODS ********************************************/

/**
 * @brief Se visualiza la pantalla principal.
 *
 *
 * @param pvParameters no hacemos nada con el
 * @param[out] 
 * @return void
 */
void Main_Screen( void * pvParameters );

/**
 * @brief Se configura la hora por consola. Esto se tiene que configurar para que configure por teclado matricial.
 *
 *
 * @param _time estructura tipo tm que se le debe pasar a la funcion.
 * @param[out] _time Se modifica la estructura con el tiempo que haya puesto el usuario.
 * @return void
 */
void Time_config(tm_t *_time);

void convertTime2StringDisplay(tm_t *_time2Convert, char timeconverted[]);

extern void Time_config_touch(tm_t * const _time, uint8_t _hour,uint8_t _minute );
/******************************** (3) DEFINES & MACROS *******************************************/


/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/






#ifdef	__cplusplus
}
#endif

#endif  /* __MAIN_SCREEN_H__ */