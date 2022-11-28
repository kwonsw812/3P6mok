#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

#define MAX_SIZE 19

void gotoxy(int x, int y);

void draw_game(void)
{
//    int i = 0;
//    int j = 0;
//
//    gotoxy(0, 0);
//
//    for(i = 0; i < MAX_SIZE; i++){
//        for(j = 0; j < MAX_SIZE; j++){
//            gotoxy(i,j);
//            if(i == 0){
//                if(j == 0)
//                    printf("��");
//
//                else if(j == MAX_SIZE - 1)
//                    printf("��");
//
//                else
//                    printf("��");
//            }
//
//            else if (j == 0){
//                if(i == MAX_SIZE - 1)
//                    printf("��");
//
//                else
//                    printf("��");
//            }
//
//            else if(j == MAX_SIZE - 1) {
//                if (i == MAX_SIZE - 1)
//                    printf("��");
//                else
//                    printf("��");
//            }
//
//            else if (i == MAX_SIZE - 1){
//                printf("��");
//            }
//
//            else
//                printf("��");
//        }
//        printf("\n");
//    }

    printf("\u250C");

}

int main() {
    printf("\u250C\u252C\u2510\n");
    return 0;
}

void gotoxy(int x, int y)
{
    COORD pos={x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}