#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include "DebouncedInterrupt.h"
#include "string.h"
 
#define SDA_PIN PC_9
#define SCL_PIN PA_8
#define EEPROM_ADDR 0xA0 // macros
 
LCD_DISCO_F429ZI LCD;
 
I2C i2c(SDA_PIN, SCL_PIN); // i2c comm.
 
void WriteEEPROM(int address, unsigned int ep_address, char *data, int size);
void ReadEEPROM(int address, unsigned int ep_address, char *data, int size);
 
InterruptIn user_button(BUTTON1);
DebouncedInterrupt external_button(PA_6);
DebouncedInterrupt external_button_2(PA_7);
DebouncedInterrupt external_button_3(PA_5);
DigitalOut led(PG_14); // pinouts
DigitalOut green(PG_13); // pinouts
 
 
unsigned int eeprom_addr = 0; 
int state = 0; // control variables
tm t; // time struct
 
void state_control() { // FSM control
    if (state == 0) {
        state = 1;
    }
 
    else if (state == 1) {
        state = 2;
    }
 
    else if (state == 2){
        state = 3;
    }
 
    else if(state == 3) {
        state = 0;
    }
}
 
 
void increase_time(){ // change time behaviour
 
    if(state == 1){
        if(t.tm_hour <= 24){
            t.tm_hour ++;
        }
        else{
            t.tm_hour = 0;
        }
    }
 
    if(state == 2){
        if(t.tm_min <= 60){
            t.tm_min ++;
        }
        else{
            t.tm_min = 0;
        }
    }
 
    if(state == 3){
        if(t.tm_sec <= 60){
            t.tm_sec ++;
        }
        else{
            t.tm_sec = 0;
        }
    }
}
 
 
bool keep_time = false;
 
void log_time(){
    keep_time = true; // keep time pressed
}
 
 
bool show_time = false;
bool state_zero = true;
 
void display_time(){
    show_time = true;
    state = 5;
    state_zero = !state_zero;
    if (state_zero == true){
        state = 0;
        show_time = false; // control saved time display screen
    }
}
 
 
int main() {
 
    external_button.attach(&state_control, IRQ_FALL, 100, false);
    external_button_2.attach(&increase_time, IRQ_FALL, 100, false);
    external_button_3.attach(&display_time, IRQ_FALL, 100, false);
    user_button.rise(&log_time); // attach to buttons
 
    // set RTC to January 1, 2024, 00:00:00 (HH:MM:SS)
 
    t.tm_year = 124; // years since 1900
    t.tm_mon = 0;
    t.tm_mday = 0;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
 
    while (1) {
 
        // Read RTC and display on LCD    
        if (state == 0){
            time_t rawtime;
            time(&rawtime);
            char timebuf[20];
            strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));
   
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE);
            thread_sleep_for(1000); // display RTC
        }
 
        if (state == 1){
            set_time(mktime(&t));
 
            time_t rawtime;
            time(&rawtime);
            char timebuf[20];
            strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));
   
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Change Hour", CENTER_MODE);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE); // display change HH screen
   
            thread_sleep_for(1000);
        }
 
        if (state == 2){
            set_time(mktime(&t));
            time_t rawtime;
            time(&rawtime);
            char timebuf[20];
            strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));
 
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Change Min", CENTER_MODE);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE); // display change MM screen
 
            thread_sleep_for(1000);
        }
 
        if (state == 3){
            set_time(mktime(&t));
 
            time_t rawtime;
            time(&rawtime);
            char timebuf[20];
            strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));
 
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Change Sec", CENTER_MODE);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE); // display change SS screen
 
            thread_sleep_for(1000);
        }
 
        if (keep_time == true){
            time_t rawtime;
            time(&rawtime);
            char timebuf[20];
            strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));

            char data_write[10];
            sprintf((char *)data_write, "%s", timebuf); // write time to array
 
            WriteEEPROM(EEPROM_ADDR, eeprom_addr,data_write, 10);
            printf("Wrote value to EEPROM: %s\n", data_write);
            printf("eeprom addr %d\n",eeprom_addr); // test stuff
 
            eeprom_addr += 100; // iterate time
            keep_time = false;
 
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE); // display RTC
 
            thread_sleep_for(1000);
        }
 
        if (show_time == true ){
 
            char data_read[10];
            ReadEEPROM(EEPROM_ADDR, eeprom_addr -100, data_read, 10); // read data from eeprom
            printf("Previous value in EEPROM: %s\n", data_read);
            printf("eeprom addr %d\n",eeprom_addr -1);
 
            char data_read_2[10];
            ReadEEPROM(EEPROM_ADDR, eeprom_addr - 200, data_read_2, 10); // read second data from eeprom
            printf("Seccond Previous value in EEPROM: %s\n", data_read_2);
            printf("eeprom addr %d\n",eeprom_addr - 2); 
 
 
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_BLACK);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"Saved times:", CENTER_MODE); 
 
            uint8_t saved_time_1[11];
            sprintf((char *)saved_time_1, "1: %s", data_read);
            LCD.DisplayStringAt(0, 60, (uint8_t *)&saved_time_1, CENTER_MODE);
 
            uint8_t saved_time_2[11];
            sprintf((char *)saved_time_2, "2: %s", data_read_2);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&saved_time_2, CENTER_MODE); // display saved time screen
 
            show_time = false; 
        }
    }
}
 

void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[size + 2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB
 
  for (int i = 0; i < size; i++) {
    i2cBuffer[i + 2] = data[i];
  }
 
  int result = i2c.write(address, i2cBuffer, size + 2, false);
  thread_sleep_for(6);
}
 
void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB
 
  // Reset eeprom pointer address
  int result = i2c.write(address, i2cBuffer, 2, false);
  thread_sleep_for(6);
 
  // Read eeprom
  i2c.read(address, data, size);
  thread_sleep_for(6);
}
 
 