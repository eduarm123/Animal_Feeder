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


/**********************************INCLUDES ****************************************************/
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


/********************************* (1) PUBLIC METHODS ********************************************/
//#define CONFIG_LED_PIN       (2)//2

/*********************************** (2) PUBLIC VARS *********************************************/
extern TaskHandle_t MainScreen_Handle;
extern TaskHandle_t AlarmaMenu_Handle;
extern bool is_alarm_set;
extern QueueHandle_t commandQueue;

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


typedef enum{
    Manual=1,
    Automatico
}MENU_OPT;

typedef enum{
    Adulto=1,
    Cachorro
}AGE_t;

typedef enum{
    Manual_alarmas_1=1,
    Manual_alarmas_2,
    Manual_alarmas_3,
    Adulto_alarmas,
    Cachorro_alarmas
}ACTIVAR_ALARM;

tm_t s_alarmas_auto[]={
    {.tm_hour=6,.tm_min=0,.tm_sec=0,}, // adulto
    {.tm_hour=12,.tm_min=0,.tm_sec=0,}, // adulto
    {.tm_hour=18,.tm_min=0,.tm_sec=0,}, // adulto 
    {.tm_hour=7,.tm_min=0,.tm_sec=0,}, // cachorro 
    {.tm_hour=12,.tm_min=0,.tm_sec=0,}, // cachorro 
    {.tm_hour=17,.tm_min=0,.tm_sec=0,}, // cachorro 
}; // Esta la implementamos por polling

tm_t s_alarmas_manual[]={
    {0}, // Esta tiene interrupcion
    {0}, // Esta tiene interrupcion
    {0}, // Esta de aqui no tendra
};


i2c_dev_t s_dev; // necessary for RTC_init()
unsigned num;

char m[1]; //Sirve para almacenar el número ingresado por teclado tipo char
int n=0;

uint8_t n_alarms;

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/
static void select_option(void);
static void Alarma_menu( void);
/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
//void Titilar(int indice);
Resultados obtenerHora();
void Titilar(int indice);
void convertTime2StringDisplay(tm_t *_time2Convert, char hour_car[], char min_car[], char seg_car[]);


void Main_Screen( void * pvParameters )
{
    char  seg_car[3];
    char  min_car[3];
    char  hour_car[3];
    /*------INICIALIZAR FTF-----*/
    spi_master_init(SPI3_HOST, LCD_DEF_DMA_CHAN, LCD_DMA_MAX_SIZE, SPI3_DEF_PIN_NUM_MISO, SPI3_DEF_PIN_NUM_MOSI, SPI3_DEF_PIN_NUM_CLK);
    spi_lcd_init(SPI3_HOST, 40*1000*1000, LCD_SPI3_DEF_PIN_NUM_CS0);
    LCD_Display_Resolution(horizontal);
    LCD_Clear(LGRAYBLUE);
    /*----------------------------------------------*/

    gpio_num_t keypad[8] = {27, 26, 25, 33, 32, 14, 12, 13}; //Pines a ocupar para teclado matricial

    keypad_initalize(keypad); /// Inicializa keyboard

    RTC_init(&s_dev); // Inicializa el i2c

    
    for (;;)
    {
        LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"*************",24,1);
        LCD_ShowString(20-1,60-1,LGRAYBLUE,BLACK,"CAT Feeder",24,1);
        LCD_ShowString(60-1,100-1,LGRAYBLUE,BLACK,"Welcome!",32,1);
        LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
        LCD_ShowPicture_16b(250-1, 50-1, 40, 40, gImage_qq);

        Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto. TODO: hay que reemplazar por teclado.
        ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
    
        for(;;)
        {  
            
            LCD_ShowString(1-1,20-1,LGRAYBLUE,BLACK,"*************",24,1);
            LCD_ShowString(50-1,50-1,LGRAYBLUE,BLACK,"CAT FEEDER",24,1);
            LCD_ShowString(1-1,80-1,LGRAYBLUE,BLACK,"*************",24,1);       

            if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
            {
                printf("Could not get time\n");
            }

            printf("--- main screen ---\n");
            printf("%02d:%02d:%02d\n", time_tc.tm_hour, time_tc.tm_min, time_tc.tm_sec);

            /*---Conversión entero a caracter para imprimir en TFT sin problema---*/
            LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
            LCD_ShowChar(80,180,LGRAYBLUE,BLACK,':',32,1);

            convertTime2StringDisplay(&time_tc,hour_car,min_car,seg_car);
           
            LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,hour_car,32,1);           
            LCD_ShowString(100-1,180-1,LGRAYBLUE,BLACK,min_car,32,1);
            LCD_ShowString(180-1,180-1,LGRAYBLUE,BLACK,seg_car,32,1);
            /*--------------------------------------------------------------------*/
            
            num = keypad_getkey();
            if (num=='C')
            {               
                Alarma_menu();
                LCD_Clear(LGRAYBLUE);
            }


            vTaskDelay(pdMS_TO_TICKS(10));
        }
            
        
    }
}

//Intentar meter esto en otra funcion o tarea para que cada modulo sea independiente.

void Time_config(tm_t * const _time){

    int hour_total; //Para ingresar el 2do dígito de la hora
    int min_total;
    Resultados resultados;

    resultados = obtenerHora();
    hour_total=resultados.resultado1;
    min_total=resultados.resultado2;

    _time->tm_sec=0;
    _time->tm_hour=hour_total;
    _time->tm_min=min_total;
}

Resultados obtenerHora() 
{
    char numeroStr[5];
    int indice = 0;
    Resultados resultados;
    
    for (size_t i = 0; i <= 3; i++)
    {
        indice=0;
        numeroStr[i]=' ';
    }

    while (indice <= 3) {

        Titilar(indice);
        LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,numeroStr[0],32,1); // Mejorar
        LCD_ShowChar(130-1,180-1,LGRAYBLUE,BLACK,numeroStr[1],32,1); // Mejorar
        LCD_ShowChar(180-1,180-1,LGRAYBLUE,BLACK,numeroStr[2],32,1); // Mejorar
        LCD_ShowChar(210-1,180-1,LGRAYBLUE,BLACK,numeroStr[3],32,1); // Mejorar
        vTaskDelay(pdMS_TO_TICKS(100));
        num = keypad_getkey();
        if (num != '\0') {
            if (num == 'A') {
                indice++;                
                // Si se presiona "A", ignorar y continuar esperando el siguiente número
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
            
        }          
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

    
    resultados.resultado1 = resultado1;
    resultados.resultado2 = resultado2;
    LCD_Clear(LGRAYBLUE);

    return resultados;
}

void Titilar(int indice)
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
                vTaskDelay(pdMS_TO_TICKS(250));
                LCD_ShowChar(100,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            }
            if (n==1)
            {
                LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,'_',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            
            //break;
        case 1:
            if (n==0)
            {
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(130,180,LGRAYBLUE,BLACK,'_',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                LCD_ShowChar(130,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                break;
            }
            if (n==1)
            {
                LCD_ShowChar(130,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            
        case 2:
            if (n==0)
            {
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,'_',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                break;
            }
            if (n==1)
            {
                LCD_ShowChar(180,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            
        case 3:
            if (n==0)
            {
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,'_',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(250));
                break;
            }
            if (n==1)
            {
                LCD_ShowChar(210,180,LGRAYBLUE,BLACK,' ',32,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
            }
            
        default:
            break;
    }
}

static void Alarma_menu( void)
{
    int age_option=-1;
    uint8_t ret=0;
    int activar_alarma =0;
    uint8_t q_AlarmMenu[10]={0};
    //int q_automatico=1;
    num=50; // Para que no entre al switch

    LCD_Clear(LGRAYBLUE);
    LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
    LCD_ShowString(10-1,60-1,LGRAYBLUE,BLACK,"1.Configurar hora",16,1);
    LCD_ShowString(10-1,100-1,LGRAYBLUE,BLACK,"2.Configurar alarmas",16,1);
    LCD_ShowString(10-1,140-1,LGRAYBLUE,BLACK,"3.Mirar alarmas",16,1);
    LCD_ShowString(10-1,180-1,LGRAYBLUE,BLACK,"4.Ir a main screen",16,1);
    select_option();  
    switch (num) 
    {
        case '1':
            LCD_Clear(LGRAYBLUE);
            LCD_ShowString(20,70,LGRAYBLUE,BLACK,"CONFIG HORA",24,1);
            LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',32,1);
            Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto.
            ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
            break;
        case '2':
            LCD_Clear(LGRAYBLUE);
            LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
            LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"1. Manual",16,1);
            LCD_ShowString(20-1,120-1,LGRAYBLUE,BLACK,"2. Automatico",16,1);
            select_option(); 
            switch (num)
            {
                case '1': //MANUAL
                    LCD_Clear(LGRAYBLUE);
                    printf("ESTAS EN MANUAL.\n");
                    LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"Numero de Alarmas",16,1);
                    select_option(); // De momento solo se puede 3.TODO: hay que agregar mas valores
                    if (num > 0 && num <4 ) {
                        break;
                    }
                    
                    if (num == '1')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_1;
                        //q_AlarmMenu[1]=1;
/*                         q_AlarmMenu[2]=s_alarmas_manual[0].tm_hour;
                        q_AlarmMenu[3]=s_alarmas_manual[0].tm_min;
                        q_AlarmMenu[4]=s_alarmas_manual[0].tm_sec; */
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);                       
                        
                        
                    }
                    else if (num == '2')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        Time_config(&s_alarmas_manual[1]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        printf("Config alarma 2.\n");
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_2;
                        //q_AlarmMenu[1]=2;
   /*                      q_AlarmMenu[2]=s_alarmas_manual[0].tm_hour;
                        q_AlarmMenu[3]=s_alarmas_manual[0].tm_min;
                        q_AlarmMenu[4]=s_alarmas_manual[0].tm_sec;
                        q_AlarmMenu[5]=s_alarmas_manual[1].tm_hour;
                        q_AlarmMenu[6]=s_alarmas_manual[1].tm_min;
                        q_AlarmMenu[7]=s_alarmas_manual[1].tm_sec; */
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);                   
                    }
                    else if (num == '3')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        Time_config(&s_alarmas_manual[1]);
                        Time_config(&s_alarmas_manual[2]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_3;
                        //q_AlarmMenu[1]=3;
/*                      q_AlarmMenu[2]=s_alarmas_manual[0].tm_hour;
                        q_AlarmMenu[3]=s_alarmas_manual[0].tm_min;
                        q_AlarmMenu[4]=s_alarmas_manual[0].tm_sec;
                        q_AlarmMenu[5]=s_alarmas_manual[1].tm_hour;
                        q_AlarmMenu[6]=s_alarmas_manual[1].tm_min;
                        q_AlarmMenu[7]=s_alarmas_manual[1].tm_sec;
                        q_AlarmMenu[8]=s_alarmas_manual[2].tm_hour;
                        q_AlarmMenu[9]=s_alarmas_manual[2].tm_min;
                        q_AlarmMenu[10]=s_alarmas_manual[2].tm_sec; */
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);
                    }
                    
                    break;
                case '2': //AUTOMATICO
                    LCD_Clear(LGRAYBLUE);
                    LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
                    LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"1. Adulto",16,1);
                    LCD_ShowString(20-1,120-1,LGRAYBLUE,BLACK,"2. Cachorro",16,1);
                    select_option();
                    if (num =='1')
                    {
/*                         Time_config(&s_alarmas_auto[0]);
                        Time_config(&s_alarmas_auto[1]);
                        Time_config(&s_alarmas_auto[2]); */
                        q_AlarmMenu[0]=Automatico;
                        n_alarms=Adulto_alarmas;
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);             
                    }
                    else if (num =='2')
                    {
/*                         Time_config(&s_alarmas_auto[3]);
                        Time_config(&s_alarmas_auto[4]);
                        Time_config(&s_alarmas_auto[5]);  */
                        q_AlarmMenu[0]=Automatico;
                        n_alarms=Cachorro_alarmas; 
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY); 
                    }                   
                    break;               
                default:
                // TODO: Seria bueno poner un boton de cancelar
                    LCD_Clear(LGRAYBLUE);
                    break;  
            }
        case '3': //Ver alarmas
            printf("------------------Mirar alarmas------------------------------.\n");
            printf("----------------------valor %d.\n",n_alarms);
            switch (n_alarms)
            {                                                     
                case Manual_alarmas_1:
                    while(num != '1' )
                    {
                        char  seg_car_1[3];
                        char  min_car_1[3];
                        char  hour_car_1[3];                      
                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Presione 1 para volver",16,1); 
                        LCD_ShowChar(155,180,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,180,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[0],hour_car_1,min_car_1,seg_car_1);                       
                        LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,hour_car_1,24,1);           
                        LCD_ShowString(100-1,180-1,LGRAYBLUE,BLACK,min_car_1,24,1);
                        LCD_ShowString(180-1,180-1,LGRAYBLUE,BLACK,seg_car_1,24,1);

                        select_option();
                    }
                    break;
                case Manual_alarmas_2:
                    while(num != '1' )
                    {
                        char  seg_car_1[3];
                        char  min_car_1[3];
                        char  hour_car_1[3];
                        char  seg_car_2[3];
                        char  min_car_2[3];
                        char  hour_car_2[3];

                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Presione 1 para volver",16,1); 

                        LCD_ShowChar(155,90,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,90,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[0],hour_car_1,min_car_1,seg_car_1);                       
                        LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,hour_car_1,24,1);           
                        LCD_ShowString(100-1,90-1,LGRAYBLUE,BLACK,min_car_1,24,1);
                        LCD_ShowString(180-1,90-1,LGRAYBLUE,BLACK,seg_car_1,24,1);

    
                        LCD_ShowChar(155,120,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,120,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[1],hour_car_2,min_car_2,seg_car_2);                       
                        LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,hour_car_2,24,1);           
                        LCD_ShowString(100-1,120-1,LGRAYBLUE,BLACK,min_car_2,24,1);
                        LCD_ShowString(180-1,120-1,LGRAYBLUE,BLACK,seg_car_2,24,1);

                        select_option();
                    }
                    break;
                case Manual_alarmas_3:
                    while(num != '1' )
                    {
                        char  seg_car_1[3];
                        char  min_car_1[3];
                        char  hour_car_1[3];
                        char  seg_car_2[3];
                        char  min_car_2[3];
                        char  hour_car_2[3];
                        char  seg_car_3[3];
                        char  min_car_3[3];
                        char  hour_car_3[3];

                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Presione 1 para volver",16,1); 

                        LCD_ShowChar(155,90,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,90,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[0],hour_car_1,min_car_1,seg_car_1);                       
                        LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,hour_car_1,24,1);           
                        LCD_ShowString(100-1,90-1,LGRAYBLUE,BLACK,min_car_1,24,1);
                        LCD_ShowString(180-1,90-1,LGRAYBLUE,BLACK,seg_car_1,24,1);

    
                        LCD_ShowChar(155,120,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,120,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[1],hour_car_2,min_car_2,seg_car_2);                       
                        LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,hour_car_2,24,1);           
                        LCD_ShowString(100-1,120-1,LGRAYBLUE,BLACK,min_car_2,24,1);
                        LCD_ShowString(180-1,120-1,LGRAYBLUE,BLACK,seg_car_2,24,1);

                        LCD_ShowChar(155,150,LGRAYBLUE,BLACK,':',24,1);
                        LCD_ShowChar(80,150,LGRAYBLUE,BLACK,':',24,1);
                        convertTime2StringDisplay(&s_alarmas_manual[1],hour_car_3,min_car_3,seg_car_3);                       
                        LCD_ShowString(25-1,150-1,LGRAYBLUE,BLACK,hour_car_3,24,1);           
                        LCD_ShowString(100-1,150-1,LGRAYBLUE,BLACK,min_car_3,24,1);
                        LCD_ShowString(180-1,150-1,LGRAYBLUE,BLACK,seg_car_3,24,1);

                        select_option();
                    }
                    break;
                default:
                    printf("---------default de Mirar alarmas-------.\n");
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;
            }                  
            break;    
        default:
            break;
    }
     
         
}

static void select_option(void)
{
    uint8_t ret=1;
    while(ret)
    {
        num = keypad_getkey();
        if (num =='1' || num =='2' || num =='3' || num =='4' )
        {
            break;
        }
        printf("Seleccione una opcion\n");
        //hay que poner un timeout. De momento probarlo asi, aunque creo q por el watchdog interno no se puede.
        //Si es asi, hay que desactivar ese WD
        vTaskDelay(pdMS_TO_TICKS(100)); // Esto evitario que salte el WD ya que da tiempo a que se ejecute la tarea que refresca el WD
    }
                          
}

void convertTime2StringDisplay(tm_t *_time2Convert, char hour_car[], char min_car[], char seg_car[]){


    itoa(_time2Convert->tm_hour, hour_car, 10);
    itoa(_time2Convert->tm_min, min_car, 10);
    itoa(_time2Convert->tm_sec, seg_car, 10);

    if (_time2Convert->tm_sec < 10) {
        seg_car[1] = seg_car[0]; // Move the single digit to the second position
        seg_car[0] = '0';        // Place '0' at the first position as the leading zero
        seg_car[2] = '\0';       // Add null terminator at the end
    }
    if (_time2Convert->tm_min < 10) {
        min_car[1] = min_car[0]; // Move the single digit to the second position
        min_car[0] = '0';        // Place '0' at the first position as the leading zero
        min_car[2] = '\0';       // Add null terminator at the end
    }
    if (_time2Convert->tm_hour < 10) {
        hour_car[1] = hour_car[0]; // Move the single digit to the second position
        hour_car[0] = '0';         // Place '0' at the first position as the leading zero
        hour_car[2] = '\0';        // Add null terminator at the end
    }
}
