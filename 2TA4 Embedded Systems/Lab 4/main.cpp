#include "LCD_DISCO_F429ZI.h"
#include "TS_DISCO_F429ZI.h"
#include "mbed.h"
// #include <cmath>


#define pRadius 5

LCD_DISCO_F429ZI LCD;
TS_DISCO_F429ZI TS;

AnalogIn aIn(PA_0);
PwmOut fan(PD_14);



DigitalOut led1(LED1);
DigitalOut led2(LED2);

float thresh_temp; 
 
void increaseTemp() {
    thresh_temp += 0.5;
}
 
void decreaseTemp() {
    thresh_temp -= 0.5;
}


Timeout timeoutDelay1;
Timeout timeoutDelay2;

int main() {
    TS_StateTypeDef tsState;
    uint16_t tsX, tsY;

    LCD.SetFont(&Font24);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);
    uint8_t text[30];

    aIn.set_reference_voltage(1);

    fan.period_us(256);

    int state = 0;

    float sensor_temp = aIn.read_voltage();
    thresh_temp = ((sensor_temp + 0.01)*100);

    while (true) {

        TS.GetState(&tsState);
        if (tsState.TouchDetected) {
            tsX = tsState.X;
            tsY = 320 - tsState .Y;
            if (tsX < 120 && tsY > 160) {
                timeoutDelay1.attach(&increaseTemp, 100ms);
                led1=1;
     
                // LCD.SelectLayer(1);
                // LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                // LCD.DrawRect(10,175,80,80);
                // LCD.SetLayerVisible(1,ENABLE);
                
            }
 
            else if(tsX > 120 && tsY > 160) {
                
                timeoutDelay2.attach(&decreaseTemp, 100ms);
                led2=1;
      
                // LCD.SelectLayer(0);
                // LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                // LCD.DrawRect(150,175,80,80);
                // LCD.SetLayerVisible(0,ENABLE);
            }
        }
        if (tsState.TouchDetected == false){
            // LCD.SetLayerVisible(1,DISABLE);
            // LCD.SetLayerVisible(0,DISABLE);


            // sensor_temp = aIn.read_voltage();

            // sprintf((char *)text, "Sensor: %.1fC", sensor_temp*100);
            // LCD.DisplayStringAt(0, 40, (uint8_t *)&text, LEFT_MODE);

            // sprintf((char *)text, "Thresh: %.1fC", thresh_temp);
            // LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);

            led1=0;
            led2=0;
        }

        if(state == 0){
            sensor_temp = aIn.read_voltage();

            sprintf((char *)text, "Sensor: %.1fC", sensor_temp*100);
            LCD.DisplayStringAt(0, 40, (uint8_t *)&text, LEFT_MODE);

            sprintf((char *)text, "Thresh: %.1fC", thresh_temp);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);

            //plus
            LCD.FillRect(45,175,10,80); //vert
            LCD.FillRect(10,210,80,10); //hor

            //minus
            LCD.FillRect(150,210,80,10);


            fan.pulsewidth_us(0);

            if (sensor_temp*100 > thresh_temp){
                state = 1;
            }

        }

        if (state == 1){
            for (int i = 0; i < 256; i++) {
                sensor_temp = aIn.read_voltage();

                sprintf((char *)text, "Sensor: %.1fC", sensor_temp*100);
                LCD.DisplayStringAt(0, 40, (uint8_t *)&text, LEFT_MODE);

                sprintf((char *)text, "Thresh: %.1fC", thresh_temp);
                LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);

                //plus
                LCD.FillRect(45,175,10,80); //vert
                LCD.FillRect(10,210,80,10); //hor

                //minus
                LCD.FillRect(150,210,80,10);

                fan.pulsewidth_us(i);
                thread_sleep_for(20);

                if(i == 255){
                    state = 2;
                }
            }
        }

        if (state == 2){
            sensor_temp = aIn.read_voltage();

            sprintf((char *)text, "Sensor: %.1fC", sensor_temp*100);
            LCD.DisplayStringAt(0, 40, (uint8_t *)&text, LEFT_MODE);

            sprintf((char *)text, "Thresh: %.1fC", std::round(thresh_temp));
            LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);
            fan.pulsewidth_us(255);

            //plus
            LCD.FillRect(45,175,10,80); //vert
            LCD.FillRect(10,210,80,10); //hor

            //minus
            LCD.FillRect(150,210,80,10);

            thread_sleep_for(20);
            
            if (sensor_temp*100 < thresh_temp){
                // LCD.Clear(LCD_COLOR_WHITE);
                state = 0;
                fan.pulsewidth_us(0);
            }

        }
    }
}

