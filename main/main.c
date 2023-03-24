/*************************************************************************************************/
/*! @file	main.c
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

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************INCLUDES ******************************************************/
#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include <ds3231.h>
#include <string.h>
#include <time.h>

#include "ds3231.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/

struct tm time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

extern i2c_dev_t dev;

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/***************************** (6) PUBLIC METHODS IMPLEMENTATION *********************************/

// void app_main()
// {
//     ESP_ERROR_CHECK(i2cdev_init());
//      xTaskCreate(ds3231_test, "ds3231_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
// }


// void ds3231_test(void *pvParameters)
// {
   
// 	time_tc.tm_min=min;
//     time_tc.tm_hour=hora;
//     time_tc.tm_sec=0;
//     ESP_ERROR_CHECK(ds3231_set_time(&dev, &time_tc));

//     while (1)
//     {      

//         vTaskDelay(pdMS_TO_TICKS(250));
//         if (ds3231_get_time(&dev, &time_tc) != ESP_OK)
//         {
//             printf("Could not get time\n");
//             continue;
//         }

//         /* float is used in printf(). you need non-default configuration in
//          * sdkconfig for ESP8266, which is enabled by default for this
//          * example. see sdkconfig.defaults.esp8266
//          */
//         printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);
//     }
// }




void app_main(void)
{
	
	/*------------------Inicializar LCD y rtc---------------------------*/
	// Aqui tocara poner lo de la LCD caundo se prube el codigo
	// de momento utilizamos printf()
	//lcd_init();
	
	// La hora se configura en el main

	/*------------------------------------------------------------*/
	/*-----------Variables iniciales de hora y alarma-------------*/
	//uint8_t h=0, m=0, s=0;
	//uint8_t al_hour=0, al_min=0, al_second=0;
	RTC_init(); // Se inicializa el RTC
	/*------------------------------------------------------------*/
	
	/*------------------Establecer Hora y Alarma INICIALES----------------------------*/
	//rtc_setTime(dec2bcd(16), dec2bcd(47), dec2bcd(40)); //19:45:30 //0x13, 0x2D, 0x1E
	//rtc_setTime(dec2bcd(16), dec2bcd(m), dec2bcd(s));
	//set_alarm(dec2bcd(16), dec2bcd(47), dec2bcd(43)); //19:45:30 //0x13, 0x2D, 0x1E
	//set_alarm(dec2bcd(al_hour), dec2bcd(al_min), dec2bcd(al_second));
	/*-------------------------------------------------------------------------------*/
	
	//page= eeprom_read_byte(&page1); //Obtenemos valor de 5 y entramos directamente a config la hora
	
	//sei(); //Habilito interrupciones

	//EIMSK |= 1<<INT1; //Interrupcion de la hora
	//PCMSK0 |= ((1<<PCINT3)|(1<<PCINT4)); //Habilitamos interrupción en PINB3 y PINB4 para teclas arriba y abajo
	
	// seleccion=&pepe;
	// rtc_getTime(&h, &m, &s);
	// seg=bcd2dec(s);
	// min=bcd2dec(m);
	// while (1)
	// {
	// 	switch(page)
	// 	{
	// 		case 5:
	// 		{*seleccion=1; //Para que en la interrupción se dirija a modificar la variable minuto
	// 			//PCMSK0 |= ((1<<PCINT3)|(1<<PCINT4)); //Habilitamos interrupción en PINB3 y PINB4 para teclas arriba y abajo
	// 			imprimir(min, minut); //Para imprimir los minutos
	// 			lcd_write(LCD_INST, LINEA1 + 4);
	// 			imprimir(seg, segund); //Para imprimir los segundos
	// 			_delay_ms(500);
	// 			lcd_write(LCD_INST, CLEAR);}
	// 		case 6:
	// 		{
	// 			while(page==6)
	// 			{	
	// 				*seleccion=2; //Para que en la interrupción se dirija a modificar la variable segundo
	// 				//PCMSK0 |= ((1<<PCINT3)|(1<<PCINT4)); //Habilitamos interrupción en PINB3 y PINB4 para teclas arriba y abajo
	// 				imprimir(min, minut); //Para imprimir los minutos
	// 				lcd_write(LCD_INST, LINEA1 + 4);
	// 				imprimir(seg, segund); //Para imprimir los segundos
	// 				_delay_ms(500);
	// 				lcd_write(LCD_INST, CLEAR);
	// 			}
	// 			rtc_setTime(dec2bcd(16), dec2bcd(min), dec2bcd(seg)); //Se establece la hora que ya se seleccionó
	// 		break;
	// 		}
	// 	}
			
	// 	if(page==0)
	// 	{	
	// 		/*PARA QUE A LO QUE VAYA LA ENERGÍA*/
	// 		EIMSK |= (1<<INT0); // Interrupciones externas habilitadas para config ALARMA o la HORA
	// 		PCMSK1 |= (1<<PCINT8); //Interrupción para manual habilitada
	// 		/*--------------------IMPRIMIR LA HORA ACTUALIZADA-------------------*/
	// 		rtc_getTime(&h, &m, &s);
	// 		rtime=bcd2dec(s);
	// 		uint8_t rtime_m=bcd2dec(m);
	// 		imprimir(rtime_m, minuto); //Para imprimir los minutos reales (rtc)
	// 		lcd_write(LCD_INST, LINEA1 + 4);
	// 		imprimir(rtime, segundo); //Para imprimir los segundos reales (rtc)
	// 		_delay_ms(1000);
	// 		lcd_write(LCD_INST, CLEAR);
	// 		/*------------------------------------------------------------------*/	
	// 		variable=0; //para que no se dirija automaticamente a automatico
	// 		pin2d=&juan;
	// 		*pin2d=0;
	// 		lecturas_EEPROM();
	// 		cont=0; //Para que empiece en 0 el contador para PWM y se pueda apreciar todas las alarmas.
	// 		if(manual_ing==1)
	// 		{	
	// 			servomotor(2,al_manual+2,numero); //El '2' indica el 'for' de donde inicia y el 'al_manual+2' donde termina
	// 		}									  //'numero' indica que se activará el array de manual
	// 		if (nose1==1) //PARA DARLE DE COMER AUTOMATICAMENTE (ADULTO O CACHORRO)
	// 		{
	// 			if (automatico==2) //adulto 1
	// 			{
	// 				servomotor(0,1,alarma); //El '0' indica el 'for' de donde inicia y el '1' donde termina
	// 										//'alarma' indica que se activará el array de automatico
	// 			}
	// 			if (automatico==1) //cachorro 0
	// 			{
	// 				servomotor(2,5,alarma); //El '2' indica el 'for' de donde inicia y el '5' donde termina
	// 			}
	// 		}	
	// 	}
		
	// 	/*---------------PARA INGRESAR A LAS ALARMAS-------------------*/	
	// 	if(page==1)
	// 	{
	// 		EIMSK &=~ (1<<INT1); // Interrupcion externa INT1(HORA) deshabilitada
	// 		PCMSK1 &=~ (1<<PCINT8); //Interrupción para manual deshabilitada
	// 		//pin2d=&juan;
	// 		*pin2d=0;
	// 		char *A[15],*M[15]; //Se declara caracteres para la impresión en LCD
	// 		while(page==1)
	// 		{				
	// 			switch(*eleccion)
	// 			{
	// 				case 2:
	// 				{	A[11]="AUTOMATICO";
	// 					M[7]="MANUAL";
	// 					imprimir1(A[11],M[7]);
	// 					variable =1;
	// 					//_delay_ms(500);
	// 					break;}
	// 				case 1:
	// 				{	lcd_write(LCD_INST, LINEA1 + 0);
	// 					lcd_print("  ");
	// 					lcd_write(LCD_INST, LINEA2 + 0);
	// 					lcd_print("->");
	// 					variable =2;
	// 					//_delay_ms(500);
	// 					break;}
	// 			}
				
	// 			if((variable==1)&&((*pin2d != 0)))
	// 			{
	// 				lcd_write(LCD_INST, CLEAR);
	// 				nose=0; //Para entrar al ciclo WHILE
	// 				manual_ing=0; //Para que no entre a las alarmas manuales de page=0
	// 				eeprom_write_byte (( uint8_t *) 8, manual_ing); //Se almacena en la memoria EEPROM 1 para que no ingrese en modo manual
	// 				*pin2d=0;
	// 				while (nose== 0)
	// 				{	
	// 					switch(*eleccion)
	// 					{
	// 						//EIMSK |= (1<<INT0); // Interrupciones externas habilitadas
	// 						case 2:
	// 						{	A[9]="CACHORRO";
	// 							M[7]="ADULTO";
	// 							imprimir1(A[9],M[7]);
	// 							automatico=1;
	// 							//eeprom_write_byte (( uint8_t *) 12, automatico ); //Se almacena en la memoria EEPROM valor automatico para entrar en la siguiente condición
	// 							eeprom_update_byte(&automatico1, automatico); //Actualiza el dato de page para cuando se vaya la energía y no vaya a la config de la hora
	// 							//_delay_ms(500);
	// 						break;
	// 						}
	// 						case 1:
	// 						{	lcd_write(LCD_INST, LINEA1 + 0);
	// 							lcd_print("  ");
	// 							lcd_write(LCD_INST, LINEA2 + 0);
	// 							lcd_print("->");
	// 							automatico=2;
	// 							//eeprom_write_byte (( uint8_t *) 12, automatico ); //Se almacena en la memoria EEPROM valor automatico para entrar en la siguiente condición
	// 							eeprom_update_byte(&automatico1, automatico); //Actualiza el dato de page para cuando se vaya la energía y no vaya a la config de la hora
	// 							//_delay_ms(500);
	// 						break;
	// 						}
	// 					}
	// 					if(*pin2d != 0) //Además de cumplirse "automático", se debe presionar el "OK" de alarma
	// 					{	_delay_ms(500);
	// 						EIMSK |= (1<<INT1); // Interrupciones externas habilitadas
	// 						PCMSK1 |= (1<<PCINT8); //Interrupción para manual habilitada
	// 						nose=1; //Para salir del primer WHILE
	// 						page=0; //Para salir del segundo WHILE y vaya a PAGE=0
	// 						eeprom_update_byte(&page1, page); //Actualiza el dato de page para cuando se vaya la energía y no vaya a la config de la hora
	// 						nose1=1; // Para entrar en modo AUTOMATICO en PAGE=0
	// 						eeprom_write_byte (( uint8_t *) 15, nose1 ); //Se almacena en la memoria EEPROM valor '1'  en nose1 para entrar en AUTOMATICO en PAGE=0
	// 						lcd_write(LCD_INST, CLEAR);}
	// 				}
	// 			}
				
	// 			if((variable==2)&&((*pin2d != 0)))
	// 			{
	// 				lcd_write(LCD_INST, CLEAR);
	// 				*pin2d=0;
	// 				nose1=0; //Para que no dentre a las alarmas automaticas de page=0
	// 				eeprom_write_byte (( uint8_t *) 15, nose1 ); //Se almacena en la memoria EEPROM el valor '0' en nose1 para no entrar en AUTOMÁTICO en PAGE=0
	// 				//uint8_t manual=0; //Para que ingrese al WHILE
	// 				int contador=0; //Para volver a config las alarmas manuales desde cero
	// 				al_manual=0; //Para reiniciar a 0 el proceso de nuevo
	// 				*seleccion=3; //Para seleccionar el número máximo de alarmas
					
	// 				while (page== 1)
	// 				{	
	// 					if((*pin2d != 0)&&(al_manual!=0))  //Para que le den de comer al menos 2 veces al día
	// 					{
	// 						manual_ing=1; //Para que ingrese a modo MANUAL en PAGE=0
	// 						eeprom_write_byte (( uint8_t *) 8, manual_ing); //Se almacena en la memoria EEPROM 1 para que ingrese en modo manual
	// 						eeprom_write_byte (( uint8_t *) 10, al_manual); //Se almacena en la memoria EEPROM el valor de cuantas alarmas se quiere
	// 						t_seg=0; //Para reiniciar a 0 el proceso de selección de los segundos
	// 						*seleccion=4; //Para seleccionar en que segundos quiero que se ejecute la alarma en funcion de *seleccion=3
	// 						while(contador<=al_manual)
	// 						{		
	// 							//teclas(59, &t_seg); //Esto lo hacemos mediante la interrupción y seleccion 4	  
	// 							if((*pin2d != 0)&&(t_seg!=0)) // Aquí podemos restringir la hora a dar de comer de 6AM a 6PM
	// 							{   
	// 								*pin2d=0;
	// 								eeprom_write_byte (( uint8_t *) contador+2, t_seg ); //Almacena en cada celda de memoria EEPROM el dato del número de alarmas escritas
	// 								contador++;
	// 							}
	// 							if (contador>al_manual)
	// 							{
	// 								EIMSK &=~ 1<<INT0; //Deshabilito interrupción
	// 								page=0;
	// 								eeprom_update_byte(&page1, page); //Actualiza el dato de page para cuando se vaya la energía y no vaya a la config de la hora
	// 							}
	// 							lcd_write(LCD_INST, CLEAR);
	// 							lcd_write(LCD_INST, LINEA1 + 0);
	// 							itoa(t_seg,segund,10);
	// 							lcd_print(segund);
	// 							_delay_ms(200);
								
	// 							lcd_write(LCD_INST, LINEA1 + 6);
	// 							imprimir(contador, manual_c); //Se imprime el número de alarmas seleccionadas
	// 							_delay_ms(100);
	// 							lcd_write(LCD_INST, CLEAR);
	// 						}
	// 					}
	// 					imprimir(al_manual, manual_c); //Se imprime el número de alarmas seleccionadas
	// 					_delay_ms(100);
	// 					lcd_write(LCD_INST, CLEAR); 
	// 				}
	// 			}
	// 		}
	// 	}
	// }
}

/************************* (7)  STATIC METHODS IMPLEMENTATION ************************************/


/*HOLA*/



#ifdef __cplusplus
};
#endif