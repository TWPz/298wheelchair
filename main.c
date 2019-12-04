#include "main.h"
#include "driverlib/driverlib.h"
#include "hal_LCD.h"
#include <stdbool.h>
#include <stdio.h>
/*
 * This project contains some code samples that may be useful.
 *
 */
// Motor1 - Left Motor
// Motor2 - Right Motor

#define STANDBY_PORT GPIO_PORT_P2
#define STANDBY_PIN GPIO_PIN7

#define MOTOR1_PWM_PORT GPIO_PORT_P8
#define MOTOR1_PWM_PIN GPIO_PIN1

#define MOTOR1_INPUT2_PORT GPIO_PORT_P1
#define MOTOR1_INPUT2_PIN GPIO_PIN1

#define MOTOR1_INPUT1_PORT GPIO_PORT_P1
#define MOTOR1_INPUT1_PIN GPIO_PIN0

#define MOTOR1_PHOTO_INT_PORT GPIO_PORT_P8
#define MOTOR1_PHOTO_INT_PIN GPIO_PIN2

#define MOTOR2_INPUT1_PORT GPIO_PORT_P8
#define MOTOR2_INPUT1_PIN GPIO_PIN0

#define MOTOR2_INPUT2_PORT GPIO_PORT_P5
#define MOTOR2_INPUT2_PIN GPIO_PIN1

#define MOTOR2_PWM_PORT GPIO_PORT_P2
#define MOTOR2_PWM_PIN GPIO_PIN5

#define MOTOR2_PHOTO_INT_PORT GPIO_PORT_P8
#define MOTOR2_PHOTO_INT_PIN GPIO_PIN3

int interrupt_occured = 0;
long interrupt_val = 0;
volatile int motor1_rpm_count = 0;
volatile int motor2_rpm_count = 0;
volatile long speed = 0;

char ADCState = 0; //Busy state of the ADC
int16_t ADCResult = 0; //Storage for the ADC conversion result
float count = 0;
int temp  = 2;
const int delay_time = 1450;
unsigned int a ,b ,c =0;
int row = 0;
int col = 0;
int result = -1;
int led_control = -1;
int test_result_c = -1;
int test_result_r = -1;
int depth = 0;
int prev = 0;
int mode_set = 0;

int selection_key = 0;

//
int i = 0;
// i as the count down

int os = 0;

int level_1_flag = -1;
int level_2_flag = -1;

int list_f[3];
int list_r[3];
int list_l[3];
int list_b[3];

int order[4];
int order_index = 0;


int threshold = 0;
int distance_f = 0;
int distance_r = 0;
int distance_l = 0;
int distance_b = 0;
//
void main(void)
{
    //char buttonState = 0; //Current button press state (to allow edge detection)

    /*
     * Functions with two underscores in front are called compiler intrinsics.
     * They are documented in the compiler user guide, not the IDE or MCU guides.
     * They are a shortcut to insert some assembly code that is not really
     * expressible in plain C/C++. Google "MSP430 Optimizing C/C++ Compiler
     * v18.12.0.LTS" and search for the word "intrinsic" if you want to know
     * more.
     * */
    //Turn off interrupts during initialization
/''''/    __disable_interrupt();

    //Stop watchdog timer unless you plan on using it
    WDT_A_hold(WDT_A_BASE);

    // Initializations - see functions for more detail
    Init_GPIO();    //Sets all pins to output low as a default
    Init_PWM();     //Sets up a PWM output
    Init_Clock();   //Sets up the necessary system clocks
    Init_LCD();     //Sets up the LaunchPad LCD display
    Init_RTC();
     /*
     * The MSP430 MCUs have a variety of low power modes. They can be almost
     * completely off and turn back on only when an interrupt occurs. You can
     * look up the power modes in the Family User Guide under the Power Management
     * Module (PMM) section. You can see the available API calls in the DriverLib
     * user guide, or see "pmm.h" in the driverlib directory. Unless you
     * purposefully want to play with the power modes, just leave this command in.
     */
    PMM_unlockLPM5(); //Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    displayScrollText("LS5 G19");



                    showChar('M',pos1);
                    showChar('E',pos2);
                    showChar('N',pos3);
                    showChar('U',pos4);
                    showChar(' ',pos5);
                    showChar(' ',pos6);
    //                executeDirection_f(0, 50);
        while(os < 1)
        {WheelChairOS();}

    //All done initializations - turn interrupts back on.
    __enable_interrupt();

    //Output PWM
       Timer_A_outputPWM(TIMER_A0_BASE, &param);
       // Set 2nd pwm pin to input so that both pwm can run at the same speed (we are shorting them)
       GPIO_setAsInputPin(MOTOR2_PWM_PORT, MOTOR2_PWM_PIN);

       //Start Timer
       RTC_start(RTC_BASE, RTC_CLOCKSOURCE_XT1CLK);

       //photo-interrupter
       GPIO_setAsInputPin(MOTOR1_PHOTO_INT_PORT, MOTOR1_PHOTO_INT_PIN);
       GPIO_setAsInputPin(MOTOR2_PHOTO_INT_PORT, MOTOR2_PHOTO_INT_PIN);

       GPIO_setOutputHighOnPin(STANDBY_PORT, STANDBY_PIN);

       //executeDirection(1, 10);


    //
//       executeDirection_f(0, 12);
//       executeDirection(1,(int)20/(3.14*1.3));
//       executeDirection(2, 3);
//       executeDirection(3, 4);
   Run_with_order();
////
    while(1);
}



void executeDirection_f(int motorDirection, int revolution){
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4|GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);

    int distance_travelled = 0;


    // Motor 1 and Motor 2 goes forward
    GPIO_setOutputLowOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
    GPIO_setOutputHighOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
    GPIO_setOutputLowOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
    GPIO_setOutputHighOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
    bool in_gap = false;
    while(distance_travelled<=revolution){
        if(!in_gap && GPIO_getInputPinValue(MOTOR2_PHOTO_INT_PORT, MOTOR2_PHOTO_INT_PIN)==1)
            in_gap = true;

        if(in_gap && GPIO_getInputPinValue(MOTOR2_PHOTO_INT_PORT, MOTOR2_PHOTO_INT_PIN)==0){
            in_gap = false;
            distance_travelled++;
            motor1_rpm_count++;

        }

        if(distance_travelled > (revolution/4)*3){
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);
        }
        else if(distance_travelled > (revolution/4)*2){
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);
        }
        else if(distance_travelled > revolution/4){
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);
        }


    }

    GPIO_setOutputLowOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
    GPIO_setOutputLowOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
    GPIO_setOutputLowOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
    GPIO_setOutputLowOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
}

void Run_with_order(void){

    int motor_o = 0;
    distance_f = list_f[0]*100 + list_f[1]*10 + list_f[2];
    distance_l = list_l[0]*100 + list_l[1]*10 + list_l[2];
    distance_r = list_r[0]*100 + list_r[1]*10 + list_r[2];
    distance_b = list_b[0]*100 + list_b[1]*10 + list_b[2];
    for(motor_o = 0 ; motor_o < 4; motor_o++){

        if(order[motor_o] == 2){
            showInt(distance_f);
            //run forward + led threshold
            executeDirection_f(0, distance_f);
            __delay_cycles(200000);
        }
        if(order[motor_o] == 4){
            showInt(distance_l);
            //run left
            executeDirection(2, distance_l);
            __delay_cycles(200000);
        }
        if(order[motor_o] == 6){
            showInt(distance_r);
            //run right
            executeDirection(3, distance_r);
            __delay_cycles(200000);
        }
        if(order[motor_o] == 8){
            showInt(distance_b);
            //run back
            executeDirection(1, distance_b);
            __delay_cycles(200000);
        }
    }

}

void WheelChairOS(void){
    /*
     * OS for Wheel Chair
     * LS5_G19
     * KeyPad layout =
     *                  * for up menu
     *                  # for enter and confirm
     *
     *                  2 for forward mode
     *                  4 for left turn
     *                  6 for right turn
     *                  8 for backward
     *
     *                  7 for left swipe on menu
     *                  9 for right swipe on menu
     *
     * Logic
     *                  {{
     *                      main menu = [mode selection, run]
     *                          1st level = [forward, backward, right, left]-- direction control
     *                              2nd level = [distance threshold] --> forward & backward would indicate distance, right & left only do square-angle turn
     *                  }}
     *
     *                      we then put each movement commands into the waiting list to be done as FIFO
     *
     *                  power indicators are associated with the distance moved
     *                  distance = revo/min * actual time
     *
     *
     *
     */

//1st level

    KeyPad_with_Mux();
    if(selection_key == 9 && mode_set == 4){
        level_1_flag = 0;

                showChar('R',pos1);
                showChar('U',pos2);
                showChar('N',pos3);
                showChar(' ',pos4);
                showChar(' ',pos5);
                showChar(' ',pos6);

        __delay_cycles(2000000);

                showChar('O',pos1);
                showChar('R',pos2);
                showChar('D',pos3);
                showChar('E',pos4);
                showChar('R',pos5);
                showChar(' ',pos6);

        while(mode_set == 4 ){

            if(order_index == 4)
            {break;}



            KeyPad_with_Mux();//1st
            i = 9000000;
            do i--;
            while(i != 0);
            i = 3000000;
            do i--;
            while(i != 0);
            if(selection_key == 2 || selection_key == 4 || selection_key == 6 || selection_key == 8){


                showChar(' ',pos1);
                showChar(' ',pos2);
                showChar(selection_key + '0',pos3);
                showChar(' ',pos4);
                showChar(order_index + '0',pos5);
                showChar(' ',pos6);

            order[order_index] = selection_key;
            order_index++;
            }

        }
        while(order_index == 4){

                showChar('L',pos1);
                showChar('A',pos2);
                showChar('U',pos3);
                showChar('N',pos4);
                showChar('C',pos5);
                showChar('H',pos6);

            _delay_cycles(2000000);
            os = 1;
            break;
        }
    }
    while(mode_set < 4){
    KeyPad_with_Mux();
    if(selection_key == 7 && depth == 0){
        level_1_flag = 1;

                showChar('M',pos1);
                showChar('O',pos2);
                showChar('D',pos3);
                showChar('E',pos4);
                showChar(' ',pos5);
                showChar(' ',pos6);

    }
    //2nd level
    KeyPad_with_Mux();
    if((selection_key == 13 && depth == 0 && level_1_flag == 1)){
        depth = 1;
        level_2_flag = 1;
        //enter 2nd level
        //initialize as forward

               showChar('2',pos1);
               showChar('4',pos2);
               showChar('6',pos3);
               showChar('8',pos4);
               showChar(' ',pos5);
               showChar(' ',pos6);


    }

    KeyPad_with_Mux();
    if((selection_key == 2) && (depth == 1)){
        level_2_flag = 1;
        prev = 2;

                showChar('F',pos1);
                showChar('R',pos2);
                showChar('O',pos3);
                showChar('N',pos4);
                showChar('T',pos5);
                showChar(' ',pos6);


    }
            //back

      KeyPad_with_Mux();
        if((selection_key == 8) && (depth == 1)){
            level_2_flag = 1;
            prev = 8;

                    showChar('B',pos1);
                    showChar('A',pos2);
                    showChar('C',pos3);
                    showChar('K',pos4);
                    showChar(' ',pos5);
                    showChar(' ',pos6);


        }




        //right
        KeyPad_with_Mux();
        if(selection_key == 6 && depth == 1){
            level_2_flag = 1;
            prev = 6;

                    showChar('R',pos1);
                    showChar('I',pos2);
                    showChar('G',pos3);
                    showChar('H',pos4);
                    showChar('T',pos5);
                    showChar(' ',pos6);

        }


        //left
        KeyPad_with_Mux();
        if(selection_key == 4 && depth == 1 ){
            level_2_flag = 1;
            prev = 4;

                    showChar('L',pos1);
                    showChar('E',pos2);
                    showChar('F',pos3);
                    showChar('T',pos4);
                    showChar(' ',pos5);
                    showChar(' ',pos6);

        }


        KeyPad_with_Mux();
        if((selection_key == 13) && (level_2_flag ==1) && prev != 0){


                    showChar('E',pos1);
                    showChar('N',pos2);
                    showChar('T',pos3);
                    showChar('E',pos4);
                    showChar('R',pos5);
                    showChar(' ',pos6);


            i = 10000;
            do i--;
            while(i != 0);


            while((1)){

                KeyPad_with_Mux();
                if(selection_key == 12){break;}
                if(selection_key != 13){


                    if(prev == 2){
                        list_f[b] = selection_key;
                        showChar('F',pos1);
                    }
                    if(prev == 4){
                        list_l[b] = selection_key;
                        showChar('L',pos1);
                    }
                    if(prev == 6){
                        list_r[b] = selection_key;
                        showChar('R',pos1);
                    }
                    if(prev == 8){
                        list_b[b] = selection_key;
                        showChar('B',pos1);
                    }



                                   showChar(' ',pos2);
                                   showChar(selection_key+'0',pos3);
                                   showChar(' ',pos4);
                                   showChar(' ',pos5);
                                   showChar(b+'0',pos6);

                                    b++;
                i = 9000000;
                do i--;
                while(i != 0);
                i = 3000000;
                do i--;
                while(i != 0);
                }
            }


               showChar(' ',pos1);
               showChar('D',pos2);
               showChar('O',pos3);
               showChar('N',pos4);
               showChar('E',pos5);
               showChar(' ',pos6);


            i = 9000000;
            do i--;
            while(i != 0);
            i = 3000000;
            do i--;
            while(i != 0);
            KeyPad_with_Mux();
            while(selection_key != 0 ){
                i = 9000000;
                do i--;
                while(i != 0);

                KeyPad_with_Mux();
            };

               char temp_array[3] = {0};
                            if(prev == 2){
                                temp_array[0] = list_f[0];
                                temp_array[1] = list_f[1];
                                temp_array[2] = list_f[2];

                            }
                            if(prev == 4){
                                temp_array[0] = list_l[0];
                                temp_array[1] = list_l[1];
                                temp_array[2] = list_l[2];
                            }
                            if(prev == 6){
                                temp_array[0] = list_r[0];
                                temp_array[1] = list_r[1];
                                temp_array[2] = list_r[2];
                            }
                            if(prev == 8){
                                temp_array[0] = list_b[0];
                                temp_array[1] = list_b[1];
                                temp_array[2] = list_b[2];
                            }

                            //__delay_cycles(200);
                            showChar(' ',pos1);
                            showChar(' ',pos2);
                            showChar(temp_array[0]+'0',pos3);
                            showChar(temp_array[1]+'0',pos4);
                            showChar(temp_array[2]+'0',pos5);
                            showChar(' ',pos6);

                                //showHex((int)count); //Put the previous result on the LCD display

            i = 9000000;
            do i--;
            while(i != 0);
            KeyPad_with_Mux();
            while(selection_key != 12){
                i = 9000000;
                do i--;
                while(i != 0);
                KeyPad_with_Mux();
            }
            mode_set++;

                           showChar(' ',pos1);
                           showChar('C',pos2);
                           showChar('O',pos3);
                           showChar('N',pos4);
                           showChar('T',pos5);
                           showChar(mode_set+'0',pos6);



            b = 0;
            //registered!
        }}



}
void showInt(long val){
    showChar((val/100000)%10 + '0', pos1);
    showChar((val/10000)%10 + '0', pos2);
    showChar((val/1000)%10 + '0', pos3);
    showChar((val/100)%10 + '0', pos4);
    showChar((val/10)%10 + '0', pos5);
    showChar(val%10 + '0', pos6);
}



void KeyPad_with_Mux(void){
    //led
           /*
            * 1.6 col 1
            * 5.2 col 3
            * 5.3 col 2
            *
            * 1 & 1 = 3  row 4 #4
            * 1 & 0 = 2  row 3 #6
            * 0 & 1 = 1  row 2 #7
            * 0 & 0 = 0  row 1 #1
            */
         //KeyPad();
           GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN6|GPIO_PIN7);
           GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN0);

   //        GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN2|GPIO_PIN3);
   //        GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN6);
           // col

           GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2|GPIO_PIN3);

           GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN3|GPIO_PIN2);
           // row

           GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2); //high Z
           GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN3); //Enable

           while(test_result_c == -1){
           for (led_control = 1; led_control <= 4; led_control ++){
               //led_control = 3;
           switch(led_control){
           case 1:
               GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
               GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
               if ((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0)) == 1)
                   test_result_c = 1;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7)) == 1)
                   test_result_c = 2;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6)) == 1)
                   test_result_c = 3;
               //__delay_cycles(2000000);
               //GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN2);
               break;

           case 2:
               GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
               GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
               if ((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0)) == 1)
                   test_result_c = 7;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7)) == 1)
                   test_result_c = 8;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6)) == 1)
                   test_result_c = 9;
               //__delay_cycles(2000000);
               //GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN2);
               break;
           case 3:
               GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
               GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
               if ((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0)) == 1)
                   test_result_c = 4;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7)) == 1)
                   test_result_c = 5;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6)) == 1)
                   test_result_c = 6;
               //__delay_cycles(2000000);
               //GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN2);
               break;

           default:
               GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
               GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
               if ((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0)) == 1)
                   test_result_c = 12;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7)) == 1)
                   test_result_c = 0;
               if ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6)) == 1)
                   test_result_c = 13;
               //__delay_cycles(2000000);
               //GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN2);
               break;

           }
           }
           }
           selection_key = test_result_c;
//           if (ADCState == 0)
//           {
//               showChar(test_result_c+'0',pos3);
//               ADCState = 1; //Set flag to indicate ADC is busy - ADC ISR (interrupt) will clear it
//               ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
//           }

           //__delay_cycles(2000000);


           test_result_c = -1;
           //end led


}

// 0 is forward, 1 is backwards, 2 is turn left, 3 is turn right
// We are assuming that one revolution = one meter
void executeDirection(int motorDirection, int revolution){
    int distance_travelled = 0;
    switch(motorDirection){
    case 0:
        // Motor 1 and Motor 2 goes forward
        GPIO_setOutputLowOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
        GPIO_setOutputHighOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
        GPIO_setOutputLowOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
        GPIO_setOutputHighOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
        break;
    case 1:
        // Motor 1 and Motor 2 goes backward
        GPIO_setOutputHighOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
        GPIO_setOutputLowOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
        GPIO_setOutputHighOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
        GPIO_setOutputLowOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
        break;
    case 2:
        // Motor 1 goes backwards
        GPIO_setOutputHighOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
        GPIO_setOutputLowOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
        // Motor 2 goes forward
        GPIO_setOutputLowOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
        GPIO_setOutputHighOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
        break;
    case 3:
        // Motor 1 goes forward
        GPIO_setOutputLowOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
        GPIO_setOutputHighOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
        // Motor 2 goes backward
        GPIO_setOutputHighOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
        GPIO_setOutputLowOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
        break;
    default:
        // Set both Motors to off
        GPIO_setOutputLowOnPin(STANDBY_PORT, STANDBY_PIN);
        return;
    }

    bool in_gap = false;
    while(distance_travelled<revolution){
        speed = 1.3 * 3.1415926 * motor1_rpm_count;
        //showInt(speed);
        if(!in_gap && GPIO_getInputPinValue(MOTOR2_PHOTO_INT_PORT, MOTOR2_PHOTO_INT_PIN)==1)
            in_gap = true;

        if(in_gap && GPIO_getInputPinValue(MOTOR2_PHOTO_INT_PORT, MOTOR2_PHOTO_INT_PIN)==0){
            in_gap = false;
            distance_travelled++;
            motor1_rpm_count++;

        }
    }

    GPIO_setOutputLowOnPin(MOTOR1_INPUT2_PORT, MOTOR1_INPUT2_PIN);
    GPIO_setOutputLowOnPin(MOTOR2_INPUT1_PORT, MOTOR2_INPUT1_PIN);
    GPIO_setOutputLowOnPin(MOTOR1_INPUT1_PORT, MOTOR1_INPUT1_PIN);
    GPIO_setOutputLowOnPin(MOTOR2_INPUT2_PORT, MOTOR2_INPUT2_PIN);
}

/*
 * Real Time Clock counter Initialization
 */
void Init_RTC()
{
    // RTC runs at a speed of 32768 Hz, so
    // Set RTC modulo to 32764 to trigger interrupt every ~1s
//    RTC_setModulo(RTC_BASE, 32768);
//    trigger interrupt every 10 s
    RTC_init(RTC_BASE, 16384, RTC_CLOCKPREDIVIDER_10);
    // trigger interrupt every 10s
//    RTC_init(RTC_BASE, 32768, RTC_CLOCKPREDIVIDER_10);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
}


/*
 * RTC Interrupt Service Routine
 * Wakes up every ~10 milliseconds to update stowatch
 */
#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    interrupt_occured = 1;
//    interrupt_val++;
//    if(interrupt_ready)
        //showInt(motor1_rpm_count);
        showInt((int) motor1_rpm_count *1.3 * 3.14);
    //showInt(motor1_rpm_count*6);
    motor1_rpm_count = 0;
    motor2_rpm_count = 0;

    GPIO_toggleOutputOnPin(LED2_PORT, LED2_PIN);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
}

void KeyPad(void){
    //key pad


    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN0);//led out
            // col as output first

            GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
            GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2|GPIO_PIN3);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2|GPIO_PIN3);

            GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN3|GPIO_PIN4|GPIO_PIN5);
            GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN0);
            while(row == 0){
            if((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == 1))
            {
                row = 2;
            }

            if((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 1))
            {
                row = 3;
            }

            if((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == 1))
            {
                row = 4;
            }

            if((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0) == 1))
            {
                row = 1;
            }
            }


             //set row as output

            GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN6);
            GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN2|GPIO_PIN3);
            GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3|GPIO_PIN4|GPIO_PIN5);
            GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3|GPIO_PIN4|GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);


            while(col == 0){
            if((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6) == 1))
            {
                col = 1;

            }

            if((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN2) == 1))
            {
                col = 3;

            }

            if((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN3) == 1))
            {
                col = 2;

            }
            }

            while(result == -1){
                if(col == 1 && row == 1)
                    result = 1;
                else if (col == 1 && row == 2)
                    result = 4;
                else if (col == 1 && row == 3)
                    result = 7;
                else if (col == 1 && row == 4)
                    result = -1;
                else if (col == 2 && row == 1)
                    result = 2;
                else if (col == 2 && row == 2)
                    result = 5;
                else if (col == 2 && row == 3)
                    result = 8;
                else if (col == 2 && row == 4)
                    result = 0;
                else if (col == 3 && row == 1)
                    result = 3;
                else if (col == 3 && row == 2)
                    result = 6;
                else if (col == 3 && row == 3)
                    result = 9;
                else if (col == 3 && row == 4)
                    result = -1;
            }

            led_control = result;
            if (ADCState == 0)
            {
             showChar(result+'0',pos3);
             ADCState = 1; //Set flag to indicate ADC is busy - ADC ISR (interrupt) will clear it
             ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);

            }
            __delay_cycles(200);
            row = 0;
            col = 0;
            result = -1;


    //end key pad
}

void Init_GPIO(void)
{
    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    //Set LaunchPad switches as inputs - they are active low, meaning '1' until pressed
    GPIO_setAsInputPinWithPullUpResistor(SW1_PORT, SW1_PIN);
    GPIO_setAsInputPinWithPullUpResistor(SW2_PORT, SW2_PIN);

    //Set LED1 and LED2 as outputs
    //GPIO_setAsOutputPin(LED1_PORT, LED1_PIN); //Comment if using UART
    GPIO_setAsOutputPin(LED2_PORT, LED2_PIN);
}

/* Clock System Initialization */
void Init_Clock(void)
{
    /*
     * The MSP430 has a number of different on-chip clocks. You can read about it in
     * the section of the Family User Guide regarding the Clock System ('cs.h' in the
     * driverlib).
     */

    /*
     * On the LaunchPad, there is a 32.768 kHz crystal oscillator used as a
     * Real Time Clock (RTC). It is a quartz crystal connected to a circuit that
     * resonates it. Since the frequency is a power of two, you can use the signal
     * to drive a counter, and you know that the bits represent binary fractions
     * of one second. You can then have the RTC module throw an interrupt based
     * on a 'real time'. E.g., you could have your system sleep until every
     * 100 ms when it wakes up and checks the status of a sensor. Or, you could
     * sample the ADC once per second.
     */
    //Set P4.1 and P4.2 as Primary Module Function Input, XT_LF
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1 + GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Set external clock frequency to 32.768 KHz
    CS_setExternalClockSource(32768);
    // Set ACLK = XT1
    CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
    // Initializes the XT1 crystal oscillator
    CS_turnOnXT1LF(CS_XT1_DRIVE_1);
    // Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    // Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
}

/* PWM Initialization */
void Init_PWM(void)
{
    /*
     * The internal timers (TIMER_A) can auto-generate a PWM signal without needing to
     * flip an output bit every cycle in software. The catch is that it limits which
     * pins you can use to output the signal, whereas manually flipping an output bit
     * means it can be on any GPIO. This function populates a data structure that tells
     * the API to use the timer as a hardware-generated PWM source.
     *
     */
    //Generate PWM - Timer runs in Up-Down mode
    param.clockSource           = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider    = TIMER_A_CLOCKSOURCE_DIVIDER_10;
    param.timerPeriod           = TIMER_A_PERIOD; //Defined in main.h
    param.compareRegister       = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    param.compareOutputMode     = TIMER_A_OUTPUTMODE_RESET_SET;
    param.dutyCycle             = HIGH_COUNT; //Defined in main.h

    //PWM_PORT PWM_PIN (defined in main.h) as PWM output
    GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR1_PWM_PORT, MOTOR1_PWM_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    //GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR2_PWM_PORT, MOTOR2_PWM_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
//    GPIO_setAsPeripheralModuleFunctionOutputPin(PWM_PORT, PWM_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
}


