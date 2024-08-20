[12:58 PM] Simrin Leonard
// // Angular resolution = 360 degrees / Number of steps per revolution
// //                    = 360 degrees / 48 steps per revolution
// //                    ≈ 7.5 degrees per step
 
// // STUDENT 1
// // Full-step:
// // Time period for full-stepping = Period / Number of steps per revolution
// //                               = 54 seconds / 48 steps
// //                               ≈ 1.125 seconds per step
 
// // Half-step:
// // Time period for half-stepping = Period / Number of steps per revolution
// //                               = 54 seconds / 96 steps
// //                               ≈ 0.5625 seconds per step
 
// // STUDENT 2
// // Full-step sequence:
// // Time period for full-stepping = Period / Number of steps per revolution
// //                               = 48 seconds / 48 steps
// //                               ≈ 1.0 seconds per step
 
// // Half-step sequence:
// // Time period for half-stepping = Period / Number of steps per revolution
// //                               = 48 seconds / 96 steps
// //                               ≈ 0.5 seconds per step
 
 
#include "LCD_DISCO_F429ZI.h"
#include "DebouncedInterrupt.h"
#include "mbed.h"
#include "stm32f429i_discovery_lcd.h"
 
InterruptIn user_button(BUTTON1); // user button declaration
 
DebouncedInterrupt step_size(PE_2);
DebouncedInterrupt direction(PE_3);
DebouncedInterrupt speed_up(PE_4);
DebouncedInterrupt speed_down(PE_5); // external button declarations
 
DigitalOut RED(PC_3);
DigitalOut GRAY(PA_5);
DigitalOut YELLOW(PB_4);
DigitalOut BLACK(PB_7); // out motor connections
 
Ticker ticker1;
Timeout timeoutDelay1;
 
LCD_DISCO_F429ZI LCD; // declare LCD object
 
int period = 0;
bool step_type = 1; // 1 is full
bool direction_toggle = 1; //  1 is cw
int student = 2; // student state control; 0 is anna, 1 is simrin; 2 is clear
 
bool print_period = false;
bool test = false;
 
int stepper = 0;
 
void turn_off() { // motor off
    RED = 0;
    GRAY = 0;
    YELLOW = 0;
    BLACK = 0;
}
 
 
 
void step_1();
void step_2();
void step_3();
void step_4();
void step_1_half();
void step_2_half();
void step_3_half();
void step_4_half();
 
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
 
 
void (*full_state[])() = {step_1, step_2, step_3, step_4};
 
void stepper_full(){
    stepper = ++stepper % 4;
    if (direction_toggle){
        full_state[stepper]();
    }
    else{
        full_state[3 - stepper]();
    }
}
 
void (*half_state[])() = {step_1, step_1_half, step_2, step_2_half, step_3, step_3_half, step_4, step_4_half};
 
void stepper_state(){
    if (step_type){
        // stepper_full();
        stepper = ++stepper % 4;
        if (direction_toggle){
            full_state[stepper]();
        }
        else{
            full_state[3 - stepper]();
        }
    }
    else{
        // stepper_half();
        stepper = ++stepper % 8;
        if (direction_toggle){
            half_state[stepper]();
        }
        else{
            half_state[7 - stepper]();
        }
    }
}
 
 
void student_change(){ // function to control student
    LCD.Clear(LCD_COLOR_WHITE);
    direction_toggle = 1;
    step_type = 1;
 
    if (student == 2) {
        student = 1;
    }
 
    student = !student;
 
    if (student == 0) {
        period = 1.125*1000;
    }
    else {
        period = 1*1000;
    }
 
    ticker1.detach();
    ticker1.attach(&stepper_state, period);
 
    print_period = true;
 
}
 
void change_step(){ // step state control function (full or half)
   
    step_type = !step_type;
 
    if (step_type){
        if (!student){
            period = 1.125*1000;
        }
        else {
            period = 1*1000;
        }
    }
    else{
        if (!student){
            period = 0.5625*1000;
        }
        else {
            period = 0.5*1000;
        }
    }
}
 
void change_direction(){ // direction control function
    direction_toggle = !direction_toggle;
}
 
void increase_speed(){
    period -= 100;
}
 
void decrease_speed(){
    period += 100;
}
 
int main(){
    LCD.SetFont(&Font24);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE); // typesetting
 
    user_button.fall(&student_change); // change student on rising edge
 
    step_size.attach(&change_step, IRQ_FALL, 100, false);
    direction.attach(&change_direction, IRQ_FALL, 100, false);
    speed_up.attach(&increase_speed, IRQ_FALL, 100, false);
    speed_down.attach(&decrease_speed, IRQ_FALL, 100, false); // attach functions to external button
 
    while (1){
 
        //Anna Naumova
        if (student == 0){
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Anna", CENTER_MODE);
            LCD.DisplayStringAt(0, 20, (uint8_t *)"Naumova", CENTER_MODE);
            LCD.DisplayStringAt(0, 60, (uint8_t *)"400458754", CENTER_MODE);
            LCD.DisplayStringAt(0, 100, (uint8_t *)"54 Sec/Rev", CENTER_MODE); // display data
 
        }
 
        //Simrin Leonard
        if (student == 1){
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Simrin", CENTER_MODE);
            LCD.DisplayStringAt(0, 20, (uint8_t *)"Leonard", CENTER_MODE);
            LCD.DisplayStringAt(0, 60, (uint8_t *)"400444881", CENTER_MODE);
            LCD.DisplayStringAt(0, 100, (uint8_t *)"48 Sec/Rev", CENTER_MODE); // display data
        }
 
        if (print_period){
            printf("period %d", period);
        }
 
        thread_sleep_for(100);
 
    }
}
 