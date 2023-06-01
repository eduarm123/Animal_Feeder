/*************************************************************************************************/
/*! @file	Main_Screen.c
 *	@brief	Introducir breve descripcion del fichero
 *
 *	\b Descripcionn: Introducir aquï¿½ descripciï¿½n de las funcionalidades del fichero \n
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
#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include <string.h>
#include <time.h>
#include "ds3231.h"
#include "Main_Screen.h"
#include "Button_Handler.h"
#include "freertos/semphr.h"
#include "easyio.h"

//SemaphoreHandle_t LlaveGlobal;
//int8_t num1;
char m[1]; //Sirve para almacenar el número ingresado por teclado tipo char
/********************************* (1) PUBLIC METHODS ********************************************/
//#define CONFIG_LED_PIN       (2)//2

/*********************************** (2) PUBLIC VARS *********************************************/

/// keypad pinout
///                     R1  R2  R3  R4  C1  C2  C3  C4 
//gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};

tm_t time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

i2c_dev_t s_dev; // necessary for RTC_init()

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/


void Main_Screen( void * pvParameters )
{
    spi_master_init(SPI3_HOST, LCD_DEF_DMA_CHAN, LCD_DMA_MAX_SIZE, SPI3_DEF_PIN_NUM_MISO, SPI3_DEF_PIN_NUM_MOSI, SPI3_DEF_PIN_NUM_CLK);
    spi_lcd_init(SPI3_HOST, 40*1000*1000, LCD_SPI3_DEF_PIN_NUM_CS0);
    LCD_Display_Resolution(horizontal);
    LCD_Clear(LGRAYBLUE);

    LCD_ShowString(90-1,20-1,LGRAYBLUE,BLACK,"HOLA",16,1);
    LCD_ShowString(120-1,80-1,LGRAYBLUE,BLACK,"HELLO",24,1);
    LCD_ShowString(60-1,130-1,LGRAYBLUE,BLACK,"--HI---!",32,1);
    LCD_ShowPicture_16b(250, 140-1, 40, 40, gImage_qq);

    //xSemaphoreTake(LlaveGlobal, portMAX_DELAY);
<<<<<<< HEAD
    //gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};
    gpio_num_t keypad[8] = {27, 26, 25, 33, 32, 35, 34, 39};
=======
    ///                     R1  R2  R3  R4  C1  C2  C3  C4 
    //gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};
    gpio_num_t keypad[8] = {16, 17, 5, 27, 26, 25, 33, 32};
>>>>>>> d6d38e4ffadafe6906e2a295044da57d36344996
    keypad_initalize(keypad); /// Inicializa keyboard
    RTC_init(&s_dev); // Inizializa el i2c
    //Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto. TODO: hay que reemplazar por teclado.
    for (;;)
    {
        xSemaphoreTake(LlaveGlobal, portMAX_DELAY);
        
        //gpio_num_t keypad[8] = {13, 12, 14, 27, 26, 25, 33, 32};
        //keypad_initalize(keypad); /// Inicializa keyboard
        //RTC_init(&s_dev); // Inizializa el i2c
        Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto. TODO: hay que reemplazar por teclado.
        ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
        //gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT);
        vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen 
        //gpio_set_level(CONFIG_LED_PIN,1); // Para probar en debug
        //if (xSemaphoreTake(LlaveGlobal, portMAX_DELAY))
        //{
            for(int i=0; i<=4; i++)
            {    
                printf("Presionar la tecla x para continuar\n");
                //while(!ReadKey("2"));
                printf("Well done\n"); 

                vTaskDelay(pdMS_TO_TICKS(250));
                if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
                {
                    printf("Could not get time\n");
                    continue;
                }

                printf("1.--- Configurar alarmas ---\n");
                printf("2.--- Configurar hora ------\n");

                printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);
                LCD_ShowNum(60-1,180-1,LGRAYBLUE,BLACK,time_tc.tm_hour,6,32,1); /* code */
                LCD_ShowNum(75-1,180-1,LGRAYBLUE,BLACK,time_tc.tm_min,6,32,1); /* code */
                LCD_ShowNum(90-1,180-1,LGRAYBLUE,BLACK,time_tc.tm_sec,6,32,1); /* code */
                
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            printf("FIIIIIIIN\n");
            //vTaskDelay(pdMS_TO_TICKS(2000));
            xSemaphoreGive(LlaveGlobal);
            vTaskDelay(pdMS_TO_TICKS(500));
        //}
    }
}

//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){

    int hour_1; //Para ingresar el 1er dígito de la hora
    //int hour_2; //Para ingresar el 2do dígito de la hora
    int hour_total; //Para ingresar el 2do dígito de la hora
    unsigned num;
    //unsigned num1;
    int num1;
    int min_1=0; //Para ingresaar el 1er dígito del minuto
    int min_total;
    //int min_2;  //Para ingresaar el 1er dígito del minuto
    int cont=0;

    int obtenerHora() 
    {
        char numeroStr[2+1] = "";
        //int cont=0;

        while (cont < 2) 
        {
            num = keypad_getkey();

            if (num != '\0') {
                numeroStr[cont] = num;
                cont++;
                printf("Número actual: %s\n", numeroStr);
            }
            //printf("Nada impreso.\n");
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        num1 = atoi(numeroStr);
        return num1;
    }

    int concatenarHoras(int numero1, float mult) 
    {
        printf("Concatenado.\n");
        vTaskDelay(pdMS_TO_TICKS(100));
        if (numero1 < 0*mult || numero1 > 24*mult) {
            printf("Error.\n");
            return -1;
        }
        int numeroConcatenado = numero1;
        //cont=0;
        return numeroConcatenado;
    }

    while (1)
    {
        printf("Hora1\n");
        //hour_1= obtenerHora();
        hour_1=3;
        printf("Hora2\n");
        cont=0;
        //min_1= obtenerHora();
        min_1=2;
        printf("Hora3\n");
        hour_total= concatenarHoras(hour_1, 1);
        min_total= concatenarHoras(min_1, 2.5);
        if (hour_total != -1) {
            printf("La hora es: %c\n", hour_total);
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        }
        if (min_total != -1) {
            printf("La hora es: %c\n", min_total);
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        }
    }

    /*while (1) {
        num = keypad_getkey();  /// gets from key queue
        //char m[]={keypressed}; //es otra manera de almacenar el caracter del teclado
        *m = num; //Lo convertimos a puntero al caracter del teclado para usar el atoi
        hour_1= atoi(m);

        //Concatenar dichos números para formar un valor y compararlo si es mayor a 0 y menor a 24.
        if (hour_1 > 0 && hour_1 < 24) //&&(Boton_OK)
        {
            printf("Presionado: %c\n", hour_1);
            vTaskDelay(pdMS_TO_TICKS(2000));
            break;
        }
        printf("Invalid hour value. Please enter a value between 0 and 23.\n");
        printf("Pressed key: %c\n", hour_1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (1) {
        num1 = keypad_getkey();  /// gets from key queue
        //char m[]={keypressed}; //es otra manera de almacenar el caracter del teclado
        *m = num1; //Lo convertimos a puntero al caracter del teclado para usar el atoi
        //fgets(keypressed, sizeof(keypressed),stdin);
        min_1= atoi(m);
        if (min_1 > 0 && min_1 < 60) {
            printf("Presionado: %c\n", min_1);
            vTaskDelay(pdMS_TO_TICKS(2000));
            break;
        }
        printf("Invalid minute value. Please enter a value between 0 and 59.\n");
        printf("Pressed key: %c\n", min_1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }*/

    _time->tm_sec=0;
    _time->tm_hour=hour_total;
    _time->tm_min=min_total;
}