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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/

typedef enum{
    Manual,
    Automatico
}MENU_OPT;

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

void Alarm_menu( void * pvParameters )
{
    // Aqui hay que a;adir la parte de las alarmas

    // if(manual_ing==1)
	//{	
	// 	servomotor(2,al_manual+2,numero); //El '2' indica el 'for' de donde inicia y el 'al_manual+2' donde termina
	//}									  //'numero' indica que se activará el array de manual
    // if (nose1==1) //PARA DARLE DE COMER AUTOMATICAMENTE (ADULTO O CACHORRO)
    // {
    //     if (automatico==2) //adulto 1
    //     {
    //         servomotor(0,1,alarma); //El '0' indica el 'for' de donde inicia y el '1' donde termina
    //                                 //'alarma' indica que se activará el array de automatico
    //     }
    //     if (automatico==1) //cachorro 0
    //     {
    //         servomotor(2,5,alarma); //El '2' indica el 'for' de donde inicia y el '5' donde termina
    //     }
    // }	
    
    
    for (;;)
    {      


    }

}