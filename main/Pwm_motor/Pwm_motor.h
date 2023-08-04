#ifndef __PWM_MOTOR_H__
#define __PWM_MOTOR_H__

#ifdef	__cplusplus
extern "C" {
#endif


/**********************************INCLUDES ******************************************************/
#include <stdio.h>
#include "driver/ledc.h"

#include "ds3231.h"
#include <time.h>
//#include "esp_err.h"
//#include "Pwm_motor.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/



/******************************** (3) DEFINES & MACROS *******************************************/
/*#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (2) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_50               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_DUTY_0               (0) // Set duty to 0%.
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz*/

#define LEDC_TIMER            LEDC_TIMER_0
#define LEDC_MODE             LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO        (2) // Define the output GPIO
#define LEDC_CHANNEL          LEDC_CHANNEL_0
#define LEDC_DUTY_RES         LEDC_TIMER_15_BIT // Set duty resolution to 15 bits for smoother motion
#define LEDC_DUTY_MIN         (2000) // Set duty for minimum position (1 ms pulse)
#define LEDC_DUTY_MAX         (4000) // Set duty for maximum position (2 ms pulse)
#define LEDC_DUTY_50  ((LEDC_DUTY_MAX + LEDC_DUTY_MIN) / 2) // Set duty for 50% position (1.5 ms pulse)
#define LEDC_DUTY_0               (0) // Set duty to 0%.
#define LEDC_FREQUENCY        (50) // Frequency in Hertz. Set frequency at 50 Hz for servomotors

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

/*INICIALIZAMOS EL PWM*/
void example_ledc_init(void);

void WPWM_motor(const ledc_channel_t _channel, const uint32_t _dutyCycle);



#ifdef	__cplusplus
}
#endif

#endif  /* __PWM_MOTOR_H__ */