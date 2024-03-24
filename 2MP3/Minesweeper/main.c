#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIZE 11 //not 10 because i am including the labels in the array
#define BOMBS 15

int prev_coord_arr[85];
int prev_coord_counter = 0;

bool endgame = false;
bool winner = false;


void printBoard(char board[SIZE][SIZE]){
    char value;
    for (int j = 0; j < SIZE; j++){
        for (int i = 0; i < SIZE; i++){
            printf(" %c", board[i][j]);
        }
        printf("\n");
    }
}


void placeBomb (char board[SIZE][SIZE], int minefield[BOMBS]){
     for (int i= 0;i<BOMBS; i++){
            int x_bomb,y_bomb;
            
            x_bomb = minefield[i]/100;
            y_bomb = minefield[i]%10;

            if (y_bomb == 0){
                y_bomb =10;
            }
            if (x_bomb == 0){
                x_bomb =10;
            } 
            board[x_bomb][y_bomb] = 'X';
        }    
        
        printBoard(board);
}

int coordInput(char board[SIZE][SIZE], int minefield[BOMBS]){
    int x,y;
    bool loop_condition = true;
    printf("Enter the coordinates (x,y) to reveal a cell\n");

    int coords_entered;

    while (loop_condition == true){
        int input_value;
        bool valid_input = true;
        input_value = scanf("%d,%d",&x,&y);
        coords_entered = x*100+y;

        if (input_value == 2 && x>=0 && x<=9 && y>=0 && y<=9 && prev_coord_counter<=85){

            if (prev_coord_counter > 0){
                for (int i = 0; i < prev_coord_counter; i++){
                    if (prev_coord_arr[i] == coords_entered){
                    printf("Coordinate already revealed. Please try a new coordinate \n");
                    valid_input = false;
                    }
                }
            }
                
            if (valid_input == true){
                prev_coord_arr[prev_coord_counter] = coords_entered;
                prev_coord_counter++;
                
                if (prev_coord_counter == 85){
                    printf("YOU WIN!\n");
                    winner = true;
                }
                return coords_entered; 
            }
        }

        else{
            printf("Not a valid input. Please try again with the format: x,y \n");
            //If input is not valid, clear buffer to prevent infinite loop when input is not an integer 
            scanf("%*[^\n]");
        }
    }
} 

bool bomb_hit(int coord,int minefield[],char board[SIZE][SIZE]){
    int x,y;

    y = (coord % 10) +1;
    x = (coord+100) / 100;

    int new_coord = (x*100)+y;

    bool bombHit = false;

    for (int i= 0;i<BOMBS; i++){
        if (minefield[i] == new_coord){          
            bombHit=true;            
        }
    }
    
    if (bombHit == true) {
        printf("Game Over! You hit a bomb.\n");
    }

    return bombHit;
}

void countAdjacentBombs(int coord,int minefield[],char board[SIZE][SIZE]){
    int x,y;

    y = (coord % 10) +1;
    x = (coord+100) / 100;

    int new_coord = (x*100)+y;

    int counter = 0;

    int top,bottom,right,left,top_right,bottom_right,top_left,bottom_left;

    top = new_coord-1;
    bottom = new_coord +1;
    right = new_coord +100;
    left = new_coord - 100;
    top_right = new_coord +99;
    bottom_right = new_coord +101;
    top_left = new_coord -101;
    bottom_left = new_coord - 99;

    for (int i= 0;i<BOMBS; i++){
        
        if (minefield[i] == top){
            counter++;
        } else if(minefield[i] == bottom){
            counter++;
        } else if(minefield[i] == right){
            counter++;
        } else if(minefield[i] == left){
            counter++;
        } else if(minefield[i] == top_right){
            counter++;
        } else if(minefield[i] == top_left){
            counter++;
        } else if(minefield[i] == bottom_right){
            counter++;
        } else if(minefield[i] == bottom_left){
            counter++;
        }
            
    }
    board[x][y] = counter +'0';

    if(winner == false){
        printBoard(board);
    }
  
}




int main(){

    char board[SIZE][SIZE];

    char value;
    for (int j = 0; j < SIZE; j++){
        for (int i = 0; i < SIZE; i++){
            if (i==0){
                if (j==0){
                    value = ' ';
                } else {
                    value = (j-1)+'0';
                }
            } else{
                if (j==0){
                    value = (i-1)+'0';
                } else{
                    value = '-';
                }
                
            }
            board[i][j]= value;
        }
    }


    srand(time(NULL));
    int minefield[BOMBS];

    int bomb_count = 0;

    while(bomb_count<BOMBS){
        int x = (rand() % 10) +1;
        int y = (rand() % 10) +1;

        int bomb_coord = (x*100)+y;

        bool unique = true;
        for (int i = 0 ;i<bomb_count;i++){
            //printf("minefield: %d, bomb_coord: %d\n",minefield[i],bomb_coord);
            if (minefield[i] == bomb_coord){
                unique = false;
                break;
            }
        }

        if (unique==true){
            minefield[bomb_count]=bomb_coord;
            bomb_count++;
        }
    }


    int input,counter;
    printf("Welcome to Minesweeper!\n");
    printBoard(board);


    while (endgame == false && winner == false){
        input = coordInput(board,minefield);
        endgame = bomb_hit(input,minefield,board);

        if(endgame == false) {
            countAdjacentBombs(input,minefield,board);
            if(winner == true) {
                endgame = true;
            } 
        }
    }

    if (endgame == true){
        placeBomb(board,minefield);
    } 

    return 0;
}
