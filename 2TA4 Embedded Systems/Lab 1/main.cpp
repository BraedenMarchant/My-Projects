#include "mbed.h"

InterruptIn userButton(BUTTON1);

DigitalOut led3(PG_13);
DigitalOut led4(PG_14);

Timeout timeoutDelay;
Timeout timeoutDelay2;
Timeout timeoutDelay3;
Ticker ticker1;
Ticker ticker2;
Ticker ticker3;
Ticker ticker4;

int state = 0; // default state on startup

void red_on(){
    led4=1;
}

void red_off(){
    led4=0;
}

void green_on(){
    led3=1;
}

void green_off(){
    led3=0;
}



void toggle_red(){
    led4 = !led4;
}

void toggle_green(){
    led3 = !led3;
}



void tick_red(){
    ticker1.attach(&toggle_red,1s);
}

void tick_green(){
    ticker2.attach(&toggle_green,1s);
}



void tick_red_off_4(){
    ticker2.attach(&red_off,4s);
}

void tick_green_off_4(){
    ticker4.attach(&green_off,4s);
}

void green_delay(){
    ticker3.attach(&green_on,4s);
    timeoutDelay2.attach(&tick_green_off_4,1s);
}


void SetState() {
    
    if (state != 1) {
        if (led3 == 0 && led4 == 0){
            led4=0;
            led3=0;

            ticker1.detach();
            ticker2.detach();

            timeoutDelay.attach(&tick_red,2s);
            timeoutDelay2.attach(&tick_green,1s);

            state = 1;
        }
        else{
            led4=0;
            led3=0;

            ticker1.detach();
            ticker2.detach();

            timeoutDelay.attach(&tick_red,1s);
            tick_green();

            state = 1;
        }

        
    } 
  
    else {
        led4=0;
        led3=0;

        ticker1.detach();
        ticker2.detach();
        ticker3.detach();
        ticker4.detach();

        ticker1.attach(&red_on,4s);
        timeoutDelay.attach(&tick_red_off_4,1s);
        timeoutDelay3.attach(&green_delay,2s);

        state = 2;
    }
}


int main() {
    userButton.fall(&SetState);
    __enable_irq();
    
    tick_red();

    while(true);
}
