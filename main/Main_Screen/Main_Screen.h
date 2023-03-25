/*************************************************************************************************/
/*! @file	Main_Screen.h
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

#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__

#ifdef	__cplusplus
extern "C" {
#endif


/**********************************INCLUDES ******************************************************/


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
void Time_config(struct tm *_time);

/*********************************** (2) PUBLIC VARS *********************************************/


/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/






#ifdef	__cplusplus
}
#endif

#endif  /* __MAIN_SCREEN_H__ */