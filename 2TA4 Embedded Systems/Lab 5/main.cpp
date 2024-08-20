/*
Calculation for angular resolution: 360°/48 steps = 7.5°/step
 
Braeden Marchant
400441243
Number: 43
Full Step Time: 0.895833.. sec
Half Step Time: 0.44791667 sec
 
Carter Sankoff
400476177
Number: 44
Full Step Time: 0.9166.. sec
Half Step Time: 0.45833.. sec
*/
 
 
#include "LCD_DISCO_F429ZI.h"
#include "DebouncedInterrupt.h"
#include "mbed.h"
 
 
InterruptIn user_button(BUTTON1); // user button declare
 
DebouncedInterrupt step_size(PE_2);
DebouncedInterrupt direction(PE_3);
DebouncedInterrupt speed_up(PE_4);
DebouncedInterrupt speed_down(PE_5); // external button declarations
 
DigitalOut RED(PC_3);
DigitalOut GRAY(PA_5);
DigitalOut YELLOW(PB_4);
DigitalOut BLACK(PB_7); // out moto connections
 
DigitalOut led1(LED1);
 
LCD_DISCO_F429ZI LCD; // declare LCD object
 
int student = 0; // student state control
int sleep_speed = 0; // increase or decrease speed
bool step = 0; // half or full step control
bool direction_toggle = 0; // direction control
 
 
void turn_off() {
    RED = 0;
    GRAY = 0;
    YELLOW = 0;
    BLACK = 0; // kill motor
}
 
// FUNCTIONS USED TO CONTROL MOTOR STEPS WITH OUT PINS
void step_1(){
    RED = 1;
    GRAY = 0;
    YELLOW = 1;
    BLACK = 0;
}
 
void step_1_half(){
    RED = 1;
    GRAY = 0;
    YELLOW = 0;
    BLACK = 0;
}
 
void step_2(){
    RED = 1;
    GRAY = 0;
    YELLOW = 0;
    BLACK = 1;
}
 
void step_2_half(){
    RED = 0;
    GRAY = 0;
    YELLOW = 0;
    BLACK = 1;
}
 
void step_3(){
    RED = 0;
    GRAY = 1;
    YELLOW = 0;
    BLACK = 1;
}
 
void step_3_half(){
    RED = 0;
    GRAY = 1;
    YELLOW = 0;
    BLACK = 0;
}
 
void step_4(){
    RED = 0;
    GRAY = 1;
    YELLOW = 1;
    BLACK = 0;
}
 
void step_4_half(){
    RED = 0;
    GRAY = 0;
    YELLOW = 1;
    BLACK = 0;
}
 
 
//FUNCTIONS FOR BRAEDEN STATE
void clockwise_full_B(){ // spin clockwise
    step_1();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*43/48 + sleep_speed);
}
 
void counter_clockwise_full_B(){ // spin counterclockwise
    step_4();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*43/48 + sleep_speed);
 
    step_1();
    thread_sleep_for(1000*43/48 + sleep_speed);
}
 
 
void clockwise_half_B(){ // spin clockwise with half steps
    step_1();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_1_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_2_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_3_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_4_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
}
 
void counter_clockwise_half_B(){ // spin counterclockwise with half steps
    step_4_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_3_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_2_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_1_half();
    thread_sleep_for(1000*43/96 + sleep_speed);
 
    step_1();
    thread_sleep_for(1000*43/96 + sleep_speed);
}
 
 
 
 
 
 
//FUNCTIONS TO CONTROL CARTER STATE
void clockwise_full_C(){ // spin clockwise full steps
    step_1();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*44/48 + sleep_speed);
}
 
void counter_clockwise_full_C(){ // spin counterclockwise full steps
    step_4();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*44/48 + sleep_speed);
 
    step_1();
    thread_sleep_for(1000*44/48 + sleep_speed);
}
 
 
void clockwise_half_C(){ // spin clockwise half steps
    step_1();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_1_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_2_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_3_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_4_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
}
 
void counter_clockwise_half_C(){ // spin counterclockwise half steps
    step_4_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_4();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_3_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_3();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_2_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_2();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_1_half();
    thread_sleep_for(1000*44/96 + sleep_speed);
 
    step_1();
    thread_sleep_for(1000*44/96 + sleep_speed);
}
 
 
 
void student_change(){ // function to control student state
    LCD.Clear(LCD_COLOR_WHITE);
    sleep_speed = 0;
    step = 0;
    direction_toggle = 0; // reset other states to initial
 
    if (student == 0){
        student = 1;
    }
 
    else if (student == 1){
        student = 2 ;
    }
   
    else if (student == 2){
        student = 1;
    }
}
 
 
void change_step(){ // step state control function
    if (step == 0){
        step = 1;
    }
 
    else if (step == 1){
        step = 0;
    }
}
 
 
void change_direction(){ // direction state control function
    if (direction_toggle == 0){
        direction_toggle = 1;
    }
 
    else if (direction_toggle == 1){
        direction_toggle = 0;
    }
}
 
 
void increase_speed(){ // increase time to spin by 100 ms
    sleep_speed -= 100;
}
 
 
void decrease_speed(){ // decrease time to spin by 100 ms
    sleep_speed += 100;
}
 
 
 
int main(){
    LCD.SetFont(&Font24);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE); // typesetting
 
    user_button.rise(&student_change); // change student on rising edge
 
    step_size.attach(&change_step, IRQ_FALL, 100, false);
    direction.attach(&change_direction, IRQ_FALL, 100, false);
    speed_up.attach(&increase_speed, IRQ_FALL, 100, false);
    speed_down.attach(&decrease_speed, IRQ_FALL, 100, false); // attach functions to external button
 
 
    while (true){
 
        //BRAEDEN MARCHANT STATE
        if (student == 1){
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Braeden", CENTER_MODE);
            LCD.DisplayStringAt(0, 20, (uint8_t *)"Marchant", CENTER_MODE);
            LCD.DisplayStringAt(0, 60, (uint8_t *)"400441243", CENTER_MODE);
            LCD.DisplayStringAt(0, 100, (uint8_t *)"43 Sec/Rev", CENTER_MODE); // display data
           
           
            //full step cw
            if (step == 0 && direction_toggle == 0){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Full Step CW", CENTER_MODE);
                clockwise_full_B();
            }
 
            //full step ccw
            if (step == 0 && direction_toggle == 1){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Full Step CCW", CENTER_MODE);
                counter_clockwise_full_B();
            }
 
 
            //half step cw
            if (step == 1 && direction_toggle == 0){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Half Step CW", CENTER_MODE);
                clockwise_half_B();
            }
 
            //half step ccw
            if (step == 1 && direction_toggle == 1){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Half Step CCW", CENTER_MODE);
                counter_clockwise_half_B();
            }
        }
 
        //CARTER SANKOFF STATE
        if (student == 2){
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Carter", CENTER_MODE);
            LCD.DisplayStringAt(0, 20, (uint8_t *)"Sankoff", CENTER_MODE);
            LCD.DisplayStringAt(0, 60, (uint8_t *)"400476177", CENTER_MODE);
            LCD.DisplayStringAt(0, 100, (uint8_t *)"44 Sec/Rev", CENTER_MODE); // display data
           
           
            //full step cw
            if (step == 0 && direction_toggle == 0){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Full Step CW", CENTER_MODE);
                clockwise_full_C();
            }
 
            //full step ccw
            if (step == 0 && direction_toggle == 1){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Full Step CCW", CENTER_MODE);
                counter_clockwise_full_C();
            }
 
 
            //half step cw
            if (step == 1 && direction_toggle == 0){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Half Step CW", CENTER_MODE);
                clockwise_half_C();
            }
 
            //half step ccw
            if (step == 1 && direction_toggle == 1){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Half Step CCW", CENTER_MODE);
                counter_clockwise_half_C();
            }
        }
 
    }
}