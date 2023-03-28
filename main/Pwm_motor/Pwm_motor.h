#ifndef __PWM_MOTOR_H__
#define __PWM_MOTOR_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "driver/ledc.h"
//#include "esp_err.h"
//#include "Pwm_motor.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz
/**********************************INCLUDES ******************************************************/


/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/


/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

/*INICIALIZAMOS EL PWM*/
void example_ledc_init(void);




#ifdef	__cplusplus
}
#endif

#endif  /* __PWM_MOTOR_H__ */