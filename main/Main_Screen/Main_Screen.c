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
#include "picture.h"
#include <stdlib.h>


char m[1]; //Sirve para almacenar el número ingresado por teclado tipo char
char seg_car[2],min_car[2];
int n=0;

extern TaskHandle_t xTask1;
extern TaskHandle_t xTask2;
/********************************* (1) PUBLIC METHODS ********************************************/
//#define CONFIG_LED_PIN       (2)//2

/*********************************** (2) PUBLIC VARS *********************************************/

tm_t time_tc=
{
    .tm_hour=0,
    .tm_min=0,
    .tm_sec=0,
};

typedef struct {
    int resultado1;
    int resultado2;
} Resultados;


i2c_dev_t s_dev; // necessary for RTC_init()
unsigned num;
/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
//void Titilar(int indice);
void Titilar(int indice, char numeroStr[]);

void Main_Screen( void * pvParameters )
{
    /*------INICIALIZAR E IMPRIMIR EN LCD FTF-----*/
    spi_master_init(SPI3_HOST, LCD_DEF_DMA_CHAN, LCD_DMA_MAX_SIZE, SPI3_DEF_PIN_NUM_MISO, SPI3_DEF_PIN_NUM_MOSI, SPI3_DEF_PIN_NUM_CLK);
    spi_lcd_init(SPI3_HOST, 40*1000*1000, LCD_SPI3_DEF_PIN_NUM_CS0);
    LCD_Display_Resolution(horizontal);
    LCD_Clear(LGRAYBLUE);
    
    LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"HOLA",16,1);
    LCD_ShowString(100-1,50-1,LGRAYBLUE,BLACK,"EDWIN",24,1);
    LCD_ShowString(60-1,100-1,LGRAYBLUE,BLACK,"ORENSE!",32,1);
    LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
    LCD_ShowPicture_16b(250-1, 50-1, 40, 40, gImage_qq);
    /*----------------------------------------------*/

    gpio_num_t keypad[8] = {27, 26, 25, 33, 32, 2, 16, 17}; //Pines a ocupar para teclado matricial

    keypad_initalize(keypad); /// Inicializa keyboard

    RTC_init(&s_dev); // Inicializa el i2c
    
    for (;;)
    {

        Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto. TODO: hay que reemplazar por teclado.
        ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
    
        for(;;)
        {    
            LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"HOLA",16,1);
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

            /*---Conversión entero a caracter para imprimir en TFT sin problema---*/
            itoa(time_tc.tm_min, min_car, 10);
            itoa(time_tc.tm_sec, seg_car, 10);

            LCD_ShowString(100-1,180-1,LGRAYBLUE,BLACK,min_car,32,1);
            LCD_ShowString(180-1,180-1,LGRAYBLUE,BLACK,seg_car,32,1);
            /*--------------------------------------------------------------------*/

            vTaskDelay(pdMS_TO_TICKS(500));
            num = keypad_getkey();
            if (num=='C')
            {               
                /* Send notification to prvTask2(), bringing it out of the 
                Blocked state. */
                xTaskNotifyGiveIndexed( xTask2, 0 );

                /* Block to wait for prvTask2() to notify this task. */
                ulTaskNotifyTakeIndexed( 0, pdTRUE, portMAX_DELAY );
            }
        }
            vTaskDelay(pdMS_TO_TICKS(500));
        
    }
}

//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){

    int hour_total; //Para ingresar el 2do dígito de la hora

    int min_total;

    Resultados obtenerHora() 
    {
        //LCD_Clear(LGRAYBLUE);
        char numeroStr[5] = "";
        int indice = 0;
        
        for (size_t i = 0; i <= 3; i++)
        {
            indice=0;
            numeroStr[i]=' ';
        }

        while (indice <= 3) {
            //Titilar(indice);
            Titilar(indice, numeroStr);
            LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,numeroStr[0],32,1); // Mejorar
            LCD_ShowChar(130-1,180-1,LGRAYBLUE,BLACK,numeroStr[1],32,1); // Mejorar
            LCD_ShowChar(180-1,180-1,LGRAYBLUE,BLACK,numeroStr[2],32,1); // Mejorar
            LCD_ShowChar(210-1,180-1,LGRAYBLUE,BLACK,numeroStr[3],32,1); // Mejorar
            vTaskDelay(pdMS_TO_TICKS(500));
            num = keypad_getkey();
            if (num != '\0') {
                if (num == 'A') {
                    indice++;
                    // Si se presiona "A", ignorar y continuar esperando el siguiente número
                    continue;
                }
                if (num == 'B') {
                    for (size_t i = 0; i <= 3; i++)
                    {
                        indice=0;
                        numeroStr[i]=' ';
                    }
                    continue;
                }
                numeroStr[indice] = num;
                printf("Número actual: %s\n", numeroStr);
                n=1;
            }
            //vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        char primerNumeroStr[2] = {numeroStr[0], '\0'};
        char segundoNumeroStr[2] = {numeroStr[1], '\0'};
        char tercerNumeroStr[2] = {numeroStr[2], '\0'};
        char cuartoNumeroStr[2] = {numeroStr[3], '\0'};

        int primerNumero = atoi(primerNumeroStr);
        int segundoNumero = atoi(segundoNumeroStr);
        int tercerNumero = atoi(tercerNumeroStr);
        int cuartoNumero = atoi(cuartoNumeroStr);

        int resultado1 = primerNumero * 10 + segundoNumero;
        int resultado2 = tercerNumero * 10 + cuartoNumero;

        Resultados resultados;
        resultados.resultado1 = resultado1;
        resultados.resultado2 = resultado2;
        LCD_Clear(LGRAYBLUE);
        return resultados;
    }

        Resultados resultados = obtenerHora();
        hour_total=resultados.resultado1;
        min_total=resultados.resultado2;

    _time->tm_sec=0;
    _time->tm_hour=hour_total;
    _time->tm_min=min_total;
}

void Titilar(int indice, char numeroStr[])
{

    switch (indice)
    {
        case 0:
            if (n==0)
            {
                LCD_ShowChar(130,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(100,180,LGRAYBLUE,BLACK,'_',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                LCD_ShowChar(100,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            if (n==1)
            {
                LCD_ShowChar(130,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
                //LCD_ShowChar(100,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,numeroStr[0],32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            
            break;
        case 1:
            LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
            LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
            LCD_ShowChar(130,180,LGRAYBLUE,BLACK,'_',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            LCD_ShowChar(130,180,LGRAYBLUE,BLACK,' ',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            break;
        case 2:
            LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
            LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            LCD_ShowChar(180,180,LGRAYBLUE,BLACK,' ',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            break;
        case 3:
            LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            LCD_ShowChar(210,180,LGRAYBLUE,BLACK,' ',32,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            break;
        //default:
            //break;
    }
}