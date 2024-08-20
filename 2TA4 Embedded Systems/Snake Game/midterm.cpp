#include "LCD_DISCO_F429ZI.h"
#include "DebouncedInterrupt.h"
#include "mbed.h"
#include <time.h>
#include <type_traits>
#include <vector>
#include <chrono>

LCD_DISCO_F429ZI LCD;

InterruptIn user_button(BUTTON1);
DebouncedInterrupt UP(PC_3);
DebouncedInterrupt LEFT(PA_5);
DebouncedInterrupt RIGHT(PB_4);
DebouncedInterrupt DOWN(PA_7);

DigitalOut led1(PG_13); 
DigitalOut led2(PG_14); 

DigitalOut piezo(PH_0);
const int up_frequency = 400;   // Adjust these frequencies as needed
const int left_frequency = 600;
const int right_frequency = 800;
const int down_frequency = 1000;

Ticker ticker;

int score = 0;
int best_score;

bool game_over = false;

int x1 = 120;
int y1 = 160;
 
int x1_change = 0;       
int y1_change = 0;

int rand_x;
int rand_y;

int snake_length = 1;

bool move_up = false;
bool move_left = false;
bool move_right = false;
bool move_down = false;

bool difficulty_select = 1;

int difficulty = 3;


void piezo_ticker() { 
    piezo = !piezo; 
}

void playBuzzer(int freq) {
    ticker.attach(&piezo_ticker, 1.0 / freq);
}
 
void StopBuzzer() {
    ticker.detach(); // Corrected the reference to piezo_ticker
    piezo = 0;
}


void select_up(){
    if (difficulty == 3){
        difficulty = 0;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(1000);
    }

    else if (difficulty == 0){
        difficulty = 2;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(600);
    }
    
    else if (difficulty == 1){
        difficulty = 0;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(1000);
    }
    
    else if (difficulty == 2){
        difficulty = 1;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(800);
    }
}

void select_down(){
    if (difficulty == 3){
        difficulty = 0;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(1000);
    }

    else if (difficulty == 0){
        difficulty = 1;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(800);
    }
    
    else if (difficulty == 1){
        difficulty = 2;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(600);
    }
    
    else if (difficulty == 2){
        difficulty = 0;
        LCD.Clear(LCD_COLOR_WHITE);
        playBuzzer(1000);
    }
}

void set_choice(){
    difficulty_select = 0;
}


void rand_x_generator(){
    int r = rand( ) % 22;
    rand_x = (r+1)*10;
}

void rand_y_generator(){
    int r = rand( ) % 28;
    rand_y = (r+4)*10;
}

bool play_again = 0;

void restart_game(){
    game_over = false;

    move_up = false;
    move_left = false;
    move_right = false;
    move_down = false;

    play_again = true;
    
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);

    x1 = 120;
    y1 = 160;
    
    x1_change = 0;       
    y1_change = 0;

    snake_length = 1;

    if (score > best_score){
        best_score = score;
    }
    score = 0;
}

int snake_block = 10;
int snake_speed = 100;

void moving_up(){
    move_up = true;
    playBuzzer(up_frequency);

    move_left = false;
    move_right = false;
    move_down = false;
}

void moving_left(){
    move_left = true;
    playBuzzer(left_frequency);

    move_up = false;
    move_right = false;
    move_down = false;
}

void moving_right(){
    move_right = true; 
    playBuzzer(right_frequency);

    move_up = false;
    move_left = false;
    move_down = false;  
}

void moving_down(){
    move_down = true;  
    playBuzzer(down_frequency);

    move_up = false;
    move_right = false;
    move_left = false;
}

int snake_colour;


int main()
{
    srand(time(NULL));
    LCD.SetFont(&Font24);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);

    rand_y_generator();
    rand_x_generator();

    std::vector<std::vector<int>> snake_list;
    std::vector<int> snake_head;


    while (true) {
        StopBuzzer();
        UP.attach(&select_up, IRQ_FALL, 50, false);
        DOWN.attach(&select_down, IRQ_FALL, 50, false);

        while (difficulty_select){
            StopBuzzer();

            //home_screen
            if (difficulty == 3){
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Welcome to:", CENTER_MODE);
                LCD.DisplayStringAt(0, 160, (uint8_t *)"Snake Game!", CENTER_MODE);

                LCD.DisplayStringAt(0, 200, (uint8_t *)"Choose Your", CENTER_MODE);
                LCD.DisplayStringAt(0, 220, (uint8_t *)"Difficulty", CENTER_MODE);

                LCD.FillRect(30, 30, 130, 10);
                LCD.FillRect(30, 40, 10, 20);
                LCD.FillRect(30, 60, 180, 10);
                LCD.FillRect(200, 70, 10, 20);
                LCD.FillRect(70, 90, 140, 10);


                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(210, 30, 10, 10);
                LCD.DrawLine(160, 35, 170, 35);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                LCD.SetTextColor(LCD_COLOR_WHITE);
                LCD.DrawLine(150, 32, 152, 32);
                LCD.DrawLine(150, 38, 152, 38);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                thread_sleep_for(100);
            }
            
            
            if (difficulty == 0){
                LCD.DisplayStringAt(0, 100, (uint8_t *)"Difficulty:", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_ORANGE);
                LCD.DisplayStringAt(0, 160, (uint8_t *)"Easy", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_WHITE);
                LCD.DisplayStringAt(0, 180, (uint8_t *)"Medium", CENTER_MODE);
                LCD.DisplayStringAt(0, 200, (uint8_t *)"Hard", CENTER_MODE);
                
                snake_speed = 150;
                snake_colour = 1;
                user_button.rise(&set_choice);
                thread_sleep_for(100);
            }
            
            if (difficulty == 1){
                LCD.DisplayStringAt(0, 100, (uint8_t *)"Difficulty:", CENTER_MODE);
                LCD.DisplayStringAt(0, 160, (uint8_t *)"Easy", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_ORANGE);
                LCD.DisplayStringAt(0, 180, (uint8_t *)"Medium", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_WHITE);
                LCD.DisplayStringAt(0, 200, (uint8_t *)"Hard", CENTER_MODE);
                
                snake_speed = 100;
                snake_colour = 2;
                user_button.rise(&set_choice);
                thread_sleep_for(100);
            }
            
            if (difficulty == 2){
                LCD.DisplayStringAt(0, 100, (uint8_t *)"Difficulty:", CENTER_MODE);
                LCD.DisplayStringAt(0, 160, (uint8_t *)"Easy", CENTER_MODE);
                LCD.DisplayStringAt(0, 180, (uint8_t *)"Medium", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_ORANGE);
                LCD.DisplayStringAt(0, 200, (uint8_t *)"Hard", CENTER_MODE);
                LCD.SetBackColor(LCD_COLOR_WHITE);
                
                snake_speed = 50;
                snake_colour = 3;
                user_button.rise(&set_choice);
                thread_sleep_for(100);
            }
        }

        while (!game_over){
            StopBuzzer();
                        
            UP.attach(&moving_up, IRQ_FALL, 50, false);
            LEFT.attach(&moving_left, IRQ_FALL, 50, false);
            RIGHT.attach(&moving_right, IRQ_FALL, 50, false);
            DOWN.attach(&moving_down, IRQ_FALL, 50, false);
            
            snake_head.clear();
            snake_head.push_back(x1);
            snake_head.push_back(y1);
            snake_list.push_back(snake_head);

            if (play_again){
                LCD.Clear(LCD_COLOR_WHITE);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                LCD.DisplayStringAt(0, 140, (uint8_t *)"Play again!", CENTER_MODE);

                LCD.FillRect(30, 30, 130, 10);
                LCD.FillRect(30, 40, 10, 20);
                LCD.FillRect(30, 60, 180, 10);
                LCD.FillRect(200, 70, 10, 20);
                LCD.FillRect(70, 90, 140, 10);


                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(210, 30, 10, 10);
                LCD.DrawLine(160, 35, 170, 35);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                LCD.SetTextColor(LCD_COLOR_WHITE);
                LCD.DrawLine(150, 32, 152, 32);
                LCD.DrawLine(150, 38, 152, 38);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                play_again = 0;
            }

            if (snake_list.size() > snake_length){
                snake_list.erase(snake_list.begin());
            }

            for (int i = 0; i < snake_list.size() - 1; ++i) {
                if (snake_list[i] == snake_head) {
                    LCD.Clear(LCD_COLOR_WHITE);
                    game_over = true;
                    snake_list.clear();
                    break; // Exiting loop early since collision detected
                }
            }

            printf("x: %d \n", rand_x);
            printf("y: %d \n", rand_y);


            if (move_up){
                x1_change = 0;
                y1_change = -snake_block;

                x1 += x1_change;
                y1 += y1_change;
                LCD.Clear(LCD_COLOR_WHITE);

                for (const auto& x : snake_list) {
                    int x_coord = x[0];
                    int y_coord = x[1];
                    if (snake_colour == 1){
                        LCD.SetTextColor(LCD_COLOR_GREEN);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 2){
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 3){
                        LCD.SetTextColor(LCD_COLOR_ORANGE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }
                }


                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(rand_x,rand_y,snake_block,snake_block);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                uint8_t text[30];
                LCD.DrawLine(0,30,240,30);
                LCD.DisplayStringAt(0, 0, (uint8_t *)"Score:", LEFT_MODE);
                sprintf((char *)text, "%d", score);
                LCD.DisplayStringAt(100, 0, (uint8_t *)&text, LEFT_MODE);

                LCD.DisplayStringAt(140, 0, (uint8_t *)"Best:", LEFT_MODE);
                sprintf((char *)text, "%d", best_score);
                LCD.DisplayStringAt(0, 0, (uint8_t *)&text, RIGHT_MODE);

                thread_sleep_for(snake_speed); 
            }

            if (move_left){
                x1_change = -snake_block;
                y1_change = 0;

                x1 += x1_change;
                y1 += y1_change;
                LCD.Clear(LCD_COLOR_WHITE);

                for (const auto& x : snake_list) {
                    int x_coord = x[0];
                    int y_coord = x[1];
                    if (snake_colour == 1){
                        LCD.SetTextColor(LCD_COLOR_GREEN);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 2){
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 3){
                        LCD.SetTextColor(LCD_COLOR_ORANGE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }
                }
                
                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(rand_x,rand_y,snake_block,snake_block);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                uint8_t text[30];
                LCD.DrawLine(0,30,240,30);
                LCD.DisplayStringAt(0, 0, (uint8_t *)"Score:", LEFT_MODE);
                sprintf((char *)text, "%d", score);
                LCD.DisplayStringAt(100, 0, (uint8_t *)&text, LEFT_MODE);

                LCD.DisplayStringAt(140, 0, (uint8_t *)"Best:", LEFT_MODE);
                sprintf((char *)text, "%d", best_score);
                LCD.DisplayStringAt(0, 0, (uint8_t *)&text, RIGHT_MODE);

                thread_sleep_for(snake_speed);  
            }

            if (move_right){
                x1_change = snake_block;
                y1_change = 0;

                x1 += x1_change;
                y1 += y1_change;
                LCD.Clear(LCD_COLOR_WHITE);

                for (const auto& x : snake_list) {
                    int x_coord = x[0];
                    int y_coord = x[1];
                    if (snake_colour == 1){
                        LCD.SetTextColor(LCD_COLOR_GREEN);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 2){
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 3){
                        LCD.SetTextColor(LCD_COLOR_ORANGE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }
                }

                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(rand_x,rand_y,snake_block,snake_block);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                uint8_t text[30];
                LCD.DrawLine(0,30,240,30);
                LCD.DisplayStringAt(0, 0, (uint8_t *)"Score:", LEFT_MODE);
                sprintf((char *)text, "%d", score);
                LCD.DisplayStringAt(100, 0, (uint8_t *)&text, LEFT_MODE);

                LCD.DisplayStringAt(140, 0, (uint8_t *)"Best:", LEFT_MODE);
                sprintf((char *)text, "%d", best_score);
                LCD.DisplayStringAt(0, 0, (uint8_t *)&text, RIGHT_MODE);

                thread_sleep_for(snake_speed); 
            }

            if (move_down){
                x1_change = 0;
                y1_change = snake_block;

                x1 += x1_change;
                y1 += y1_change;
                LCD.Clear(LCD_COLOR_WHITE);

                for (const auto& x : snake_list) {
                    int x_coord = x[0];
                    int y_coord = x[1];
                    if (snake_colour == 1){
                        LCD.SetTextColor(LCD_COLOR_GREEN);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 2){
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }

                    if (snake_colour == 3){
                        LCD.SetTextColor(LCD_COLOR_ORANGE);
                        LCD.FillRect(x_coord, y_coord, snake_block, snake_block);
                        LCD.SetTextColor(LCD_COLOR_DARKBLUE);
                    }
                }
                
                LCD.SetTextColor(LCD_COLOR_RED);
                LCD.DrawRect(rand_x,rand_y,snake_block,snake_block);
                LCD.SetTextColor(LCD_COLOR_DARKBLUE);

                uint8_t text[30];
                LCD.DrawLine(0,30,240,30);
                LCD.DisplayStringAt(0, 0, (uint8_t *)"Score:", LEFT_MODE);
                sprintf((char *)text, "%d", score);
                LCD.DisplayStringAt(100, 0, (uint8_t *)&text, LEFT_MODE);

                LCD.DisplayStringAt(140, 0, (uint8_t *)"Best:", LEFT_MODE);
                sprintf((char *)text, "%d", best_score);
                LCD.DisplayStringAt(0, 0, (uint8_t *)&text, RIGHT_MODE);

                thread_sleep_for(snake_speed);  
            }

            if (x1 >= 240 || x1 < 0 || y1 >= 320 || y1 < 30){
                game_over = true;
                LCD.Clear(LCD_COLOR_WHITE);
                snake_list.clear();
            }

            if (x1 == rand_x && y1 == rand_y){
                rand_x_generator();
                rand_y_generator();
                score += 1;
                snake_length += 1;
                playBuzzer(1200);
                thread_sleep_for(100);

            }
        }

        while(game_over){
            StopBuzzer();
            LCD.SetFont(&Font24);
            LCD.SetTextColor(LCD_COLOR_RED);
            LCD.DisplayStringAt(0, 100, (uint8_t *)"You Lost", CENTER_MODE);
            LCD.DisplayStringAt(0, 140, (uint8_t *)"Press The", CENTER_MODE);
            LCD.DisplayStringAt(0, 160, (uint8_t *)"User Button", CENTER_MODE);
            LCD.DisplayStringAt(0, 180, (uint8_t *)"To Play Again", CENTER_MODE);

            user_button.rise(&restart_game); 
        }
        
        
    }
}