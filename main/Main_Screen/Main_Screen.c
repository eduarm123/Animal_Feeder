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

/*#include "esp_system.h"
#include "driver/spi_master.h"
#include "pretty_effect.h"
#include "spi_lcd.h"*/


/********************************* (1) PUBLIC METHODS ********************************************/

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
    uint8_t u8_hour;
    uint8_t u8_min;
} time_result_t;


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

uint8_t n_alarms; // Se guarda la configuracion las alarmas que estan declaradas en ACTIVAR_ALARM

/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/
static uint8_t select_option(void);
static void Alarma_menu(void);
/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/
time_result_t obtenerHora();
void Titilar(int indice, int n);
void convertTime2StringDisplay(tm_t *_time2Convert, char timeconverted[]);



void Main_Screen( void * pvParameters )
{
    esp_err_t ret;
    //spi_device_handle_t spi;
    uint8_t u8_key=0;
    char u8_timeconverted[9];
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
        //LCD_ShowPicture_16b(250-1, 50-1, 40, 40, gImage_qq);
        //---LCD_ShowPicture_16b(0, 0, 40, 40, gImage_qq);
        //LCD_ShowPicture_16b(0, 0, 240, 320, gImage_qq);
        
        vTaskDelay(pdMS_TO_TICKS(5000));
        //Initialize the effect displayed
        ret=pretty_effect_init();
        ESP_ERROR_CHECK(ret);

        //Go do nice stuff.
        display_pretty_colors(LCD_SPI);

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
            convertTime2StringDisplay(&time_tc,u8_timeconverted);
            LCD_ShowString(25-1,180-1,LGRAYBLUE,BLACK,u8_timeconverted,32,1);           
            /*--------------------------------------------------------------------*/
            
            u8_key = keypad_getkey();
            if (u8_key=='C')
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
    
    uint8_t hour_total; //Para ingresar el 2do dígito de la hora
    uint8_t min_total;
    time_result_t time2send;

    time2send = obtenerHora();
    hour_total=time2send.u8_hour;
    min_total=time2send.u8_min;

    _time->tm_sec=0;
    _time->tm_hour=hour_total;
    _time->tm_min=min_total;
}

time_result_t obtenerHora() 
{
    char numeroStr[5];
    uint8_t u8_key=0;
    int indice = 0;
    uint8_t num_dec[4]={0};
    time_result_t time_set;
    
    for (size_t i = 0; i <= 3; i++)
    {
        numeroStr[0]='_';
        numeroStr[i+1]=' ';
    }

    while (indice <= 3) {

        //Titilar(indice);
        LCD_ShowChar(100-1,180-1,LGRAYBLUE,BLACK,numeroStr[0],32,1); // Mejorar
        LCD_ShowChar(130-1,180-1,LGRAYBLUE,BLACK,numeroStr[1],32,1); // Mejorar
        LCD_ShowChar(180-1,180-1,LGRAYBLUE,BLACK,numeroStr[2],32,1); // Mejorar
        LCD_ShowChar(210-1,180-1,LGRAYBLUE,BLACK,numeroStr[3],32,1); // Mejorar
        vTaskDelay(pdMS_TO_TICKS(10));
        u8_key = keypad_getkey();

        if (u8_key != '\0') {

            if (u8_key =='A'){
                u8_key ='_';
                indice++; 
                numeroStr[indice] = u8_key;            
            }
            else if (u8_key == 'B') {
                indice=0;
                for (size_t i = 0; i <= 3; i++)
                {                                     
                    numeroStr[0]='_';
                    numeroStr[i+1]=' ';
                }               
            }
            else{

                num_dec[indice]= u8_key - '0';

                if(num_dec[0]>2 || (num_dec[0] == 2 && num_dec[1] > 3) || num_dec[2]>5 || num_dec[3]>9 ){
                    numeroStr[indice] = '0';
                }
                else{
                    numeroStr[indice] = u8_key;
                }   

            }            
            vTaskDelay(pdMS_TO_TICKS(10));
            printf("Número actual: %s\n", numeroStr);
        }
              
    }

    time_set.u8_hour = (numeroStr[0]-'0') * 10 + (numeroStr[1]-'0');
    time_set.u8_min = (numeroStr[2]-'0') * 10 + (numeroStr[3]-'0');

    LCD_Clear(LGRAYBLUE);

    return (time_set);
}

void Titilar(int indice, int n)
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
            
            break;
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
            break;
            
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
            break;
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
            break;
        default:
            break;
    }
}

static void Alarma_menu( void)
{
    uint8_t u8_key=10;
    uint8_t q_AlarmMenu[10]={0};

    LCD_Clear(LGRAYBLUE);
    LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
    LCD_ShowString(10-1,60-1,LGRAYBLUE,BLACK,"1.Configurar hora",16,1);
    LCD_ShowString(10-1,100-1,LGRAYBLUE,BLACK,"2.Configurar alarmas",16,1);
    LCD_ShowString(10-1,140-1,LGRAYBLUE,BLACK,"3.Mirar alarmas",16,1);
    LCD_ShowString(10-1,180-1,LGRAYBLUE,BLACK,"4.Ir a main screen",16,1);
    u8_key=select_option();
    switch (u8_key) 
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
            u8_key=select_option(); 
            switch (u8_key)
            {
                case '1': //MANUAL
                    LCD_Clear(LGRAYBLUE);
                    printf("ESTAS EN MANUAL.\n");
                    LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"Numero de Alarmas",16,1);
                    u8_key=select_option(); // De momento solo se puede 3.TODO: hay que agregar mas valores
                    if (u8_key > 0 && u8_key <4 ) {
                        break;
                    }
                    
                    if (u8_key == '1')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_1;
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);                       
                                               
                    }
                    else if (u8_key == '2')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        Time_config(&s_alarmas_manual[1]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        printf("Config alarma 2.\n");
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_2;
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);                   
                    }
                    else if (u8_key == '3')
                    {
                        Time_config(&s_alarmas_manual[0]);
                        Time_config(&s_alarmas_manual[1]);
                        Time_config(&s_alarmas_manual[2]);
                        is_alarm_set=false;  // Break out of the inner loop, so the alarmTask waits for the next alarm setting
                        q_AlarmMenu[0]=Manual;
                        n_alarms=Manual_alarmas_3;
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);
                    }
                    
                    break;
                case '2': //AUTOMATICO
                    LCD_Clear(LGRAYBLUE);
                    LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",24,1);
                    LCD_ShowString(20-1,70-1,LGRAYBLUE,BLACK,"1. Adulto",16,1);
                    LCD_ShowString(20-1,120-1,LGRAYBLUE,BLACK,"2. Cachorro",16,1);
                    u8_key=select_option();
                    if (u8_key =='1')
                    {
                        q_AlarmMenu[0]=Automatico;
                        n_alarms=Adulto_alarmas;
                        xQueueSendToBack(commandQueue, &q_AlarmMenu, portMAX_DELAY);             
                    }
                    else if (u8_key =='2')
                    {
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
            break;    
        case '3': //Ver alarmas
            switch (n_alarms)
            {                                                     
                case Manual_alarmas_1:
                    while(u8_key != '1' )
                    {             
                        char u8_timeconverted[9];                      
                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Presione 1 para volver",16,1);                        
                        convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted);            
                        LCD_ShowString(180-1,180-1,LGRAYBLUE,BLACK,u8_timeconverted,24,1);

                        u8_key=select_option();
                    }
                    break;
                case Manual_alarmas_2:
                    while(u8_key != '1' )
                    {
                        char u8_timeconverted_1[9];  
                        char u8_timeconverted_2[9];  

                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Press 1 para volver",16,1); 

                        convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
                        LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);                       
   
                        convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
                        LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
                        u8_key=select_option();
                    }
                    break;
                case Manual_alarmas_3:
                    while(u8_key != '1' )
                    {
                        char u8_timeconverted_1[9];  
                        char u8_timeconverted_2[9];
                        char u8_timeconverted_3[9];

                        LCD_Clear(LGRAYBLUE);
                        LCD_ShowString(50-1,20-1,LGRAYBLUE,BLACK,"--Alarma set--",16,1); 
                        LCD_ShowString(1-1,60-1,LGRAYBLUE,BLACK,"Presione 1 para volver",16,1); 

                        convertTime2StringDisplay(&s_alarmas_manual[0],u8_timeconverted_1);                       
                        LCD_ShowString(25-1,90-1,LGRAYBLUE,BLACK,u8_timeconverted_1,24,1);           
                             
                        convertTime2StringDisplay(&s_alarmas_manual[1],u8_timeconverted_2);                       
                        LCD_ShowString(25-1,120-1,LGRAYBLUE,BLACK,u8_timeconverted_2,24,1);           
 
                        convertTime2StringDisplay(&s_alarmas_manual[2],u8_timeconverted_3);                       
                        LCD_ShowString(25-1,150-1,LGRAYBLUE,BLACK,u8_timeconverted_3,24,1);           

                        u8_key=select_option();
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

static uint8_t select_option(void)
{
    uint8_t ret=1;
    uint8_t _u8_key;

    while(ret)
    {
        _u8_key = keypad_getkey();
        if (_u8_key =='1' || _u8_key =='2' || _u8_key =='3' || _u8_key =='4' )
        {
            break;
        }
        printf("Seleccione una opcion\n");
        vTaskDelay(pdMS_TO_TICKS(100)); // Esto evitario que salte el WD ya que da tiempo a que se ejecute la tarea que refresca el WD
    }

    return (_u8_key);                         
}

/**
 * @brief Convert time values to a character string in "HH:MM:SS" format with leading zeros.
 *
 * This function takes a pointer to a tm_t structure _time2Convert containing the time values to convert,
 * and stores the resulting character string in the timeconverted array. The resulting string will have the
 * format "HH:MM:SS" with leading zeros for hours, minutes, and seconds if necessary.
 * 
 * @param[in] _time2Convert Pointer to the tm_t structure containing the time values to convert.
 * @param[out] timeconverted Pointer to the character array where the resulting time string will be stored.
 *
 * @note The character arrays 'hour_car', 'min_car', and 'seg_car' must have enough memory to store
 * the converted time components as strings. Make sure to allocate at least 3 characters for each
 * array (including the null terminator '\0').
 *
 * @see tm_t
 * 
 * @note The timeconverted array must have enough space to store the entire time string "HH:MM:SS" along with the null-terminator.
 */
void convertTime2StringDisplay(tm_t *_time2Convert, char timeconverted[]){

   timeconverted[0]='0' + (_time2Convert->tm_hour)/10;
   timeconverted[1]='0' + (_time2Convert->tm_hour)%10;
   timeconverted[2]=':';
   timeconverted[3]='0' + (_time2Convert->tm_min)/10;
   timeconverted[4]='0' + (_time2Convert->tm_min)%10;
   timeconverted[5]=':'; 
   timeconverted[6]='0' + (_time2Convert->tm_sec)/10;
   timeconverted[7]='0' + (_time2Convert->tm_sec)%10;
   timeconverted[8]='\0';

}