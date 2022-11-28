#include <stdio.h>
#include <windows.h>
#define COLOR_RED	"\033[38;2;255;0;0m"

int check;

void clearconsole(){
    system("cls");
    DWORD CIN;
    DWORD mode;
    CIN = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(CIN, &mode);
    SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);
}

void draw_board(){
    int i;
    int j;
    printf("┌ ");
    for (i = 0 ; i< 17 ; i++)
        printf("┬ ");
    printf("┐\n");

    for(i = 0 ; i < 17 ; i++){
        printf("├ ");
        for (j = 0 ; j < 17 ; j++)
            printf("┼ ");
        printf("┤\n");
    }

    printf("└ ");
    for (i = 0 ; i < 17 ; i++)
        printf("┴ ");
    printf("┘");
}

void get_pos(int* x, int* y, int board[][19]) {
    INPUT_RECORD rec;
    DWORD dwNOER;
    HANDLE CIN = 0;

    while(1) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER);

        if(rec.EventType == MOUSE_EVENT) {
            if(rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                int xpos = rec.Event.MouseEvent.dwMousePosition.X;
                int ypos = rec.Event.MouseEvent.dwMousePosition.Y;
                if(xpos % 2 == 0 && xpos <= 36 && ypos <= 18) {
                    if(board[ypos][xpos/2] == 0) {
                        *x = xpos;
                        *y = ypos;
                        board[ypos][xpos/2] = check;
                        break;
                    }
                }
            }
        }
    }
}

void gotoxy(int x, int y) {
    COORD m;
    m.X = x;
    m.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), m);
}

void put_stone() {
    if (check == 1) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        printf("●");
        check = 2;
    } else if (check == 2) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        printf("●");
        check = 3;
    } else if (check == 3) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        printf("●");
        check = 1;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

int check_win(int board[][19]) {
    int i;
    int j;
    for (i = 2; i < 16; i++){
        for (j = 0; j < 19; j++){
            if (board[j][i-2] == 1 && board[j][i-1] == 1 && board[j][i] == 1 && board[j][i+1] == 1 && board[j][i+2] == 1 && board[j][i+3] == 1){
                gotoxy(0,23);
                printf("Player Red Win!\n");
                return 1;
            } else if(board[j][i-2] == 2 && board[j][i-1] == 2 && board[j][i] == 2 && board[j][i+1] == 2 && board[j][i+2] == 2 && board[j][i+3] == 2){
                gotoxy(0,23);
                printf("Player Green Win!\n");
                return 1;
            } else if(board[j][i-2] == 3 && board[j][i-1] == 3 && board[j][i] == 3 && board[j][i+1] == 3 && board[j][i+2] == 3 && board[j][i+3] == 3) {
                gotoxy(0, 23);
                printf("Player Blue Win!\n");
                return 1;
            } else if(board[i-2][j] == 1 && board[i-1][j] == 1 && board[i][j] == 1 && board[i+1][j] == 1 && board[i+2][j] == 1 && board[i+3][j] == 1) {
                gotoxy(0, 23);
                printf("Player RED Win!\n");
                return 1;
            } else if(board[i-2][j] == 2 && board[i-1][j] == 2 && board[i][j] == 2 && board[i+1][j] == 2 && board[i+2][j] == 2 && board[i+3][j] == 2) {
                gotoxy(0, 23);
                printf("Player Green Win\n");
                return 1;
            } else if(board[i-2][j] == 3 && board[i-1][j] == 3 && board[i][j] == 3 && board[i+1][j] == 3 && board[i+2][j] == 3 && board[i+3][j] == 3) {
                gotoxy(0, 23);
                printf("Player Blue Win\n");
                return 1;
            }
        }
    }
    for(i = 2; i < 16; ++i) {
        for(j =2; j < 16; ++j) {
            if (board[j-2][i-2] == 1 && board[j-1][i-1] == 1 && board[j][i] == 1 && board[j+1][i+1] == 1 && board[j+2][i+2] == 1 && board[j+3][i+3] == 1){
                gotoxy(0,23);
                printf("Player Red Win!\n");
                return 1;
            } else if (board[j-2][i-2] == 2 && board[j-1][i-1] == 2 && board[j][i] == 2 && board[j+1][i+1] == 2 && board[j+2][i+2] == 2 && board[j+3][i+3] == 2) {
                gotoxy(0,23);
                printf("Player Green Win!\n");
                return 1;
            } else if (board[j-2][i-2] == 3 && board[j-1][i-1] == 3 && board[j][i] == 3 && board[j+1][i+1] == 3 && board[j+2][i+2] == 3 && board[j+3][i+3] == 3) {
                gotoxy(0,23);
                printf("Player Blue Win!\n");
                return 1;
            } else if(board[j+2][i-2] == 1 && board[j+1][i-1] == 1 && board[j][i] == 1 && board[j-1][i+1] == 1 && board[j-2][i+2] == 1 && board[j-3][i+3] == 1) {
                gotoxy(0,23);
                printf("Player Red Win!\n");
                return 1;
            } else if(board[j+2][i-2] == 2 && board[j+1][i-1] == 2 && board[j][i] == 2 && board[j-1][i+1] == 2 && board[j-2][i+2] == 2 && board[j-3][i+3] == 2) {
                gotoxy(0,23);
                printf("Player Green Win!\n");
                return 1;
            } else if(board[j+2][i-2] == 3 && board[j+1][i-1] == 3 && board[j][i] == 3 && board[j-1][i+1] == 3 && board[j-2][i+2] == 3 && board[j-3][i+3] == 3) {
                gotoxy(0,23);
                printf("Player Blue Win!\n");
                return 1;
            }
        }
    }
    return 0;
}

void game() {
    int start = 1;
    char loop;
    while (start) {
        clearconsole();
        start = 0;
        loop = ' ';
        check = 1;
        int board[19][19] = {0};
        int x, y;
        draw_board();

        while (1) {
            get_pos(&x, &y, board);
            gotoxy(x, y);
            put_stone();

            gotoxy(0, 23);
            printf("%d %d", x/2, y);
            if(check_win(board)) {
                printf("Regame? y/n:");
                scanf("%c", &loop);
                if(loop == 'y') {
                    start = 1;
                    break;
                } else { break; }
            }
        }
    }
}



int main() {
    game();
    return 0;
}
