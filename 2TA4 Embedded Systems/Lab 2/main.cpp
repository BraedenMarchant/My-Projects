#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include "stm32f429i_discovery_lcd.h"
#include <time.h>
 
LCD_DISCO_F429ZI LCD; // initialize LCD display
 
InterruptIn user_button(BUTTON1);
InterruptIn external_button(PA_6, PullUp);
DigitalOut led3(PG_13); // pinouts
 
Timer timer1;
Timer timer2;
Ticker ticker1;
Timeout timeoutDelay;
Timeout timeoutDelay2; // initialize interrupts
 
int time_track = 0;
int best_time = 0;
int num_games = 0; // initialize scorekeeping variables
 
int state = 0; // initialzie state variable
 
int rand_wait_time = 0; // initialize random delay variable
 
void rand_time(){
    int r = 1000 + rand( ) % 5001; //
    rand_wait_time = r; // random LED delay in milleseconds (between 1-5 s)
}
 
 
void timer_start(){
    timer1.start();
}
 
void green_on(){
    led3=1;
    timer_start(); // start reaction timer as soon as LED turns on
}
 
void random_led_on(){
    timeoutDelay.attach(&green_on,std::chrono::milliseconds(rand_wait_time)); // turn on LED after delay
    timer2.start(); // start reaction time for cheat condition
}
 
 
void stop_timer(){
    timer1.stop();
    int elapsed_time =
        chrono::duration_cast<chrono::milliseconds>(timer1.elapsed_time())
            .count(); // call for reaction time
    timer1.reset();
 
    led3 = 0; // turn off LED
    time_track = elapsed_time;
 
    if (num_games == 0){
        best_time = elapsed_time;
        num_games = 1; // set best time on first attempt
    }
    else{
        if (time_track < best_time){
        best_time = time_track; // update fastest time if recent is fastest
        }
    }
}
 
 
void display_screen(){
    LCD.SetFont(&Font24);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE); // declare typesetting
 
    LCD.Clear(LCD_COLOR_WHITE);
    uint8_t text[30];
    LCD.DisplayStringAt(0, 0, (uint8_t *)"Reaction Tests", CENTER_MODE);
 
    LCD.DisplayStringAt(0, 40, (uint8_t *)"Latest: ", LEFT_MODE);
    sprintf((char *)text, "%d ms", time_track);
    LCD.DisplayStringAt(0, 40, (uint8_t *)&text, RIGHT_MODE); // output recent time to LCD
 
    LCD.DisplayStringAt(0, 80, (uint8_t *)"Best:", LEFT_MODE);
    sprintf((char *)text, "%d ms", best_time);
    LCD.DisplayStringAt(0, 80, (uint8_t *)&text, RIGHT_MODE); // output best time to LCD
 
 
}
 
 
void toggle_green(){
    led3 = !led3;
}
 
 
void tick_green(){
    ticker1.attach(&toggle_green,100ms); // flash green LED at ~10 Hz
}
 
 
void timer_stop(){
    timer1.stop();
    timer1.reset();
}
 
 
void zero_state(){ // behaviour of external pushbutton
    tick_green(); // start flashing LED
    num_games = 0;
    state = 1; // reset control variables
    LCD.Clear(LCD_COLOR_WHITE); // reset LCD
    timeoutDelay.detach();
    timer_stop();  // ready timers
}
 
 
void set_state() {
    if (state == 0){
        tick_green();
        state = 1; // initial state to 1 on first press
    }
   
    if (state == 1) {
        rand_time(); // call for random delay
       
        ticker1.detach();
        led3 = 0; // turn LED off to start
        random_led_on(); // control random behaviour
   
        state = 2; // update state
    }
 
    else if (state == 2) {
        timer2.stop();
        int cheat_timer =
            chrono::duration_cast<chrono::milliseconds>(timer2.elapsed_time())
                .count(); // check time to track cheat condition of t = 0
        timer2.reset();
       
        if (cheat_timer > rand_wait_time){ // cheating wasn't detected
            stop_timer();
            display_screen();
            state = 1; // reset state and display scorekeeping
        }
 
        else{ // detected cheating
            timeoutDelay.detach();
            timer_stop();
 
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_DARKBLUE);
            LCD.Clear(LCD_COLOR_WHITE); // typesetting
            uint8_t text[30];
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Too Fast", LEFT_MODE);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"Try again", LEFT_MODE); // initialize and display error message
           
            led3=0;
            tick_green();
            state = 1; // revert to error behaviour
        }
 
    }
}
 
int main() {
    srand(time(NULL)); // seed random generator with current time
    user_button.rise(&set_state); // update state and behaviour on internal button rising edge
    external_button.fall(&zero_state); // revert to default behaviour on external press
     __enable_irq();
    tick_green(); // initial green LED flashing
    while(true);
}