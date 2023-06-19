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
#include "Main_Screen.h"
#include "ds3231.h"
#include "Pwm_motor.h"


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include <time.h>
#include "Button_Handler.h"

//#include "easyio.h" //error si incluyo esto de nuevo

/********************************* (1) PUBLIC METHODS ********************************************/

//---#define CONFIG_LED_PIN       (2)//2
/*********************************** (2) PUBLIC VARS *********************************************/

typedef enum{
    Manual=1,
    Automatico
}MENU_OPT;

typedef enum{
    Adulto=1,
    Cachorro
}AGE_t;

typedef enum{
    Manual_alarmas_3=1,
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

extern unsigned num;
// Esto se usa como flags para saber que se ha activado alguna de esas alarmas.
// mirar si se puede hacer con bitfields
// uint8_t manual_3     = 0;
// uint8_t automatico_1 = 0;
// uint8_t automatico_2 = 0;
// uint8_t automatico_3 = 0;
// uint8_t automatico_4 = 0;
// uint8_t automatico_5 = 0;
// uint8_t automatico_6 = 0;


extern tm_t time_tc;
extern i2c_dev_t s_dev; // Configurado en Main_screen.c 

extern TaskHandle_t xTask1;
extern TaskHandle_t xTask2;
//extern SemaphoreHandle_t xSemaphore;
//-------SemaphoreHandle_t LlaveGlobal;
QueueHandle_t colaPulsador; // Cola para notificar a las tareas
/******************************** (3) DEFINES & MACROS *******************************************/
#define ARRAY_SIZE(a) (sizeof(a)/ sizeof(a[0]))

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/
void Activacion_motor();
void init_manual_alarm_3();
void init_adulto_alarm();
void init_cachorro_alarm();
void select_option(void);

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

/**
 * @brief Menu alarma. Aqui se configura la hora y se monitorea las alarmas. Si alguna alarma salta
 *        se activa el motor.  
 * @param pvParameters 
 * @return void
 */
void Alarm_menu( void * pvParameters )
{
    int age_option=-1;
    uint8_t ret=0;
    int activar_alarma =0;
    static uint32_t alarm_state = Manual;
    uint8_t* manual_alarm_isr=(void*)0;
    num=-1; // Para que no entre al swithc

    //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",16,1);
    //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"1. Configurar hora",24,1);
    //LCD_ShowString(100-1,50-1,LGRAYBLUE,BLACK,"2 Configurar alarmas",32,1);

    vTaskDelay(pdMS_TO_TICKS(100)); // espera de x tiempo para que las otras tareas se inicialicen
    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) ==pdTRUE ) // Aqui se espera la interrupcion del boton PUSH_BUTTON_PIN_0 0
        {
           select_option();  
            switch (num) 
            {
                case '1':
                    Time_config(&time_tc); //Aqui se configura la hora. El usuario hace esto.
                    ESP_ERROR_CHECK(ds3231_set_time(&s_dev, &time_tc)); // Se envia la hora al modulo
                    break;
                case '2':
                    //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"Seleccione una opcion",16,1);
                    //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"1. Manual",24,1);
                    //LCD_ShowString(100-1,50-1,LGRAYBLUE,BLACK,"2. Automatico",32,1);
                    select_option(); 
                    switch (num)
                    {
                        case Manual:

                            //LCD_ShowString(120-1,20-1,LGRAYBLUE,BLACK,"Numero de Alarmas",16,1);
                            select_option(); // De momento solo se puede 3.TODO: hay que agregar mas valores
                            if (num > 0 && num <4 ) {
                                break;
                            }
                            
                            if (num == '1')
                            {
                                Time_config(&s_alarmas_manual[0]);
                                ds3231_set_alarm(&s_dev, DS3231_ALARM_1, &s_alarmas_manual[0], DS3231_ALARM1_MATCH_SECMINHOUR, 0, 0);
                                ds3231_enable_alarm_ints(&s_dev, DS3231_ALARM_1); // Se activa las interrupciones de alarma
                                activar_alarma=Manual_alarmas_3;
                                // Esto deberia activar la funcion motor: TODO: Hacer Edwin. Utiliza esta senal para activar el motor 
                            }
                            else if (num == '2')
                            {
                                Time_config(&s_alarmas_manual[0]);
                                Time_config(&s_alarmas_manual[1]);
                                ds3231_set_alarm(&s_dev, DS3231_ALARM_1, &s_alarmas_manual[0], DS3231_ALARM1_MATCH_SECMINHOUR, &s_alarmas_manual[1],
                                DS3231_ALARM2_MATCH_MINHOUR);
                                ds3231_enable_alarm_ints(&s_dev, DS3231_ALARM_BOTH); // Se activa las interrupciones de alarma
                                activar_alarma=Manual_alarmas_3;
                                // Esto deberia activar la funcion motor.  TODO: Hacer Edwin. Utiliza esta senal para activar el motor 
                            }
                            else if (num == '3')
                            {
                                Time_config(&s_alarmas_manual[2]);
                                activar_alarma=Manual_alarmas_3;
                            }
                            
                            break;
                        case Automatico:
                            printf("Select 1.Adulto o 2.Cachorro");
                            while (1) { // Esto se debe eliminar once we have the keyboard
                                printf("Age: ");
                                scanf("%d", &age_option);
                                if (age_option > 0 && age_option <3 ) {
                                    break;
                                }
                                printf("Invalid alarm value. Please enter a value greater than 0.\n");
                                vTaskDelay(pdMS_TO_TICKS(1000));
                            }

                            if (age_option ==Adulto)
                            {
                                Time_config(&s_alarmas_auto[0]);
                                Time_config(&s_alarmas_auto[1]);
                                Time_config(&s_alarmas_auto[2]);
                                activar_alarma=Adulto_alarmas;
                            }
                            else if (age_option ==Cachorro)
                            {
                                Time_config(&s_alarmas_auto[3]);
                                Time_config(&s_alarmas_auto[4]);
                                Time_config(&s_alarmas_auto[5]); 
                                activar_alarma=Cachorro_alarmas;
                            }
                             break;
                        default:
                        // TODO: Seria bueno poner un boton de cancelar
                        //printf("Please select an option");
                            break;  
                    }
                   
                default:
                    break;
            }
           

            /* Send a notification to prvTask1(), bringing it out of the 
            Blocked state. */
            xTaskNotifyGive( xTask1 );  
        }
        else
        {
            switch (activar_alarma){

            case Manual_alarmas_3:
                init_manual_alarm_3();
                ds3231_get_alarm_flags(&s_dev,(ds3231_alarm_t *)manual_alarm_isr);
                if (*manual_alarm_isr!=0){ // TODO: Comprobar si esto funciona. No estoy seguro si se guarda ahi el flag de la interrupcion
                    Activacion_motor();

                    ds3231_clear_alarm_flags(&s_dev,(ds3231_alarm_t)manual_alarm_isr);
                }
                break;
            case Adulto_alarmas:
                init_adulto_alarm();
                break;
            case Cachorro_alarmas:
                init_cachorro_alarm();
             default:
                break;

            }
            vTaskDelay(pdMS_TO_TICKS(100));       
        }          
    }
}

void init_manual_alarm_3(){


    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

    if( time_tc.tm_hour ==s_alarmas_manual[2].tm_hour && time_tc.tm_min ==s_alarmas_manual[2].tm_min)
    {
        Activacion_motor();
    }
     
}

void init_adulto_alarm(){

    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

    if( time_tc.tm_hour ==s_alarmas_auto[0].tm_hour && time_tc.tm_min ==s_alarmas_auto[0].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[1].tm_hour && time_tc.tm_min ==s_alarmas_auto[1].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[2].tm_hour && time_tc.tm_min ==s_alarmas_auto[2].tm_min)
    {
        Activacion_motor();
    }
     
}

void init_cachorro_alarm(){

    if (ds3231_get_time(&s_dev, &time_tc) != ESP_OK)
        {
            printf("Could not get time\n");
        }

   if( time_tc.tm_hour ==s_alarmas_auto[3].tm_hour && time_tc.tm_min ==s_alarmas_auto[3].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[4].tm_hour && time_tc.tm_min ==s_alarmas_auto[4].tm_min)
    {
        Activacion_motor();
    }
    if( time_tc.tm_hour ==s_alarmas_auto[5].tm_hour && time_tc.tm_min ==s_alarmas_auto[5].tm_min)
    {
        Activacion_motor();
    }
     
}

void Activacion_motor()
{
    // Esto es para alarma de manual 1 y 2 
    // if (ds3231_get_alarm_flags(&s_dev,(ds3231_alarm_t *)DS3231_ALARM_BOTH)
    // &&manual_3 && automatico_1 && automatico_2 && automatico_3 && automatico_4
    // && automatico_5 && automatico_6)  // Hay un warnig. tengo que mirarlo bien. creo que ahi tengo q
    // //poner las alarmas que configure
    
        // Hay que poner mutex
        WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_50);
        vTaskDelay(pdMS_TO_TICKS(2000));
        // Hay que poner mutex
        WPWM_motor(LEDC_CHANNEL, LEDC_DUTY_0);
    
}

void select_option(void)
{
    uint8_t ret=0;

    while(ret)
    {
        num = keypad_getkey();
        if (num =='1' || num =='2' || num =='3' )
        {
            break;
        }
        
        //hay que poner un timeout. De momento probarlo asi, aunque creo q por el watchdog interno no se puede.
        //Si es asi, hay que desactivar ese WD
        vTaskDelay(pdMS_TO_TICKS(100)); // Esto evitario que salte el WD ya que da tiempo a que se ejecute la tarea que refresca el WD
    }
                          
}
