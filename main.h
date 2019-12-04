#ifndef MAIN_H_
#define MAIN_H_

#include "driverlib/driverlib.h"

#define TIMER_A_PERIOD  100000 //T = 1/f = (TIMER_A_PERIOD * 1 us)
#define HIGH_COUNT      5000  //Number of cycles signal is high (Duty Cycle = HIGH_COUNT / TIMER_A_PERIOD)

//Output pin to buzzer
#define PWM_PORT        GPIO_PORT_P1
#define PWM_PIN         GPIO_PIN7
//LaunchPad LED1 - note unavailable if UART is used
#define LED1_PORT       GPIO_PORT_P1
#define LED1_PIN        GPIO_PIN0
//LaunchPad LED2
#define LED2_PORT       GPIO_PORT_P4
#define LED2_PIN        GPIO_PIN0
//LaunchPad Pushbutton Switch 1
#define SW1_PORT        GPIO_PORT_P1
#define SW1_PIN         GPIO_PIN2
//LaunchPad Pushbutton Switch 2
#define SW2_PORT        GPIO_PORT_P2
#define SW2_PIN         GPIO_PIN6
//Input to ADC - in this case input A9 maps to pin P8.1
#define ADC_IN_PORT     GPIO_PORT_P8
#define ADC_IN_PIN      GPIO_PIN1
#define ADC_IN_CHANNEL  ADC_INPUT_A9


void Init_RTC();
void showInt(long val);
void executeDirection(int motorDirection, int revolution);
void executeDirection_f(int motorDirection, int revolution);
Timer_A_outputPWMParam param; //Timer configuration data structure for PWM
void Init_GPIO(void);
void Init_Clock(void);
void Init_UART(void);
void Init_PWM(void);
void Init_ADC(void);
void KeyPad(void);
void KeyPad_with_Mux(void);
void Photo_Interrupter(void);
void WheelChairOS(void);
void Run_with_order(void);
//Timer_A_outputPWMParam param; //Timer configuration data structure for PWM

#endif /* MAIN_H_ */
