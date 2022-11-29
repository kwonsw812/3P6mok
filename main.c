#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

int check; //어떤 바둑돌의 차례인지 결정하는 변수
int turn; //한번에 2개를 놓을 수 있으므로 이를 확인하기 위한 변수

//게임 재시작을 위해 콘솔에 있는 내용을 지우는 함수
void clearconsole() {
    system("cls");
    DWORD CIN;
    DWORD mode;
    CIN = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(CIN, &mode);
    SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);
}

//x, y 좌표를 받아 커서 위치를 이동시키는 함수
void gotoxy(int x, int y) {
    COORD m;
    m.X = x;
    m.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), m);
}

//바둑판을 그리는 함수
void draw_board() {
    int i;
    int j;
    printf("┌ ");
    for (i = 0; i < 17; i++)
        printf("┬ ");
    printf("┐\n");

    for (i = 0; i < 17; i++) {
        printf("├ ");
        for (j = 0; j < 17; j++)
            printf("┼ ");
        printf("┤\n");
    }

    printf("└ ");
    for (i = 0; i < 17; i++)
        printf("┴ ");
    printf("┘");
    gotoxy(0, 19);
}

//플레이어가 바둑판에 마우스를 클릭하면 해당 좌표를 얻는 함수
void get_pos(int* x, int* y, int board[][19]) {
    INPUT_RECORD rec;
    DWORD dwNOER;
    HANDLE CIN = 0;

    while (1) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER);

        if (rec.EventType == MOUSE_EVENT) {
            if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                int xpos = rec.Event.MouseEvent.dwMousePosition.X;
                int ypos = rec.Event.MouseEvent.dwMousePosition.Y;
                if (xpos % 2 == 0 && xpos <= 36 && ypos <= 18) {
                    if (board[ypos][xpos / 2] == 0) {
                        *x = xpos;
                        *y = ypos;
                        board[ypos][xpos / 2] = check;
                        break;
                    }
                }
            }
        }
    }
}

//콘솔창에 바둑돌을 출력하는 함수
void put_stone() {
    ++turn;
    if (check == 1) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        printf("●");
        if (turn == 2) {
            check = 2;
            turn = 0;
        }
    }
    else if (check == 2) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        printf("●");
        if (turn == 2) {
            check = 3;
            turn = 0;
        }
    }
    else if (check == 3) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        printf("●");
        if (turn == 2) {
            check = 1;
            turn = 0;
        }
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

//배열을 검사하여 승리 조건을 만족했는지 검사하는 함수
int check_win(int board[][19]) {
    int i;
    int j;
    for (i = 2; i < 16; i++) {
        for (j = 0; j < 19; j++) {
            if (board[j][i - 2] == 1 && board[j][i - 1] == 1 && board[j][i] == 1 && board[j][i + 1] == 1 && board[j][i + 2] == 1 && board[j][i + 3] == 1) {
                gotoxy(60, 8);
                printf("*****Player Red Win!*****\n");
                return 1;
            }
            else if (board[j][i - 2] == 2 && board[j][i - 1] == 2 && board[j][i] == 2 && board[j][i + 1] == 2 && board[j][i + 2] == 2 && board[j][i + 3] == 2) {
                gotoxy(60, 8);
                printf("*****Player Green Win!*****\n");
                return 1;
            }
            else if (board[j][i - 2] == 3 && board[j][i - 1] == 3 && board[j][i] == 3 && board[j][i + 1] == 3 && board[j][i + 2] == 3 && board[j][i + 3] == 3) {
                gotoxy(60, 8);
                printf("*****Player Blue Win!*****\n");
                return 1;
            }
            else if (board[i - 2][j] == 1 && board[i - 1][j] == 1 && board[i][j] == 1 && board[i + 1][j] == 1 && board[i + 2][j] == 1 && board[i + 3][j] == 1) {
                gotoxy(60, 8);
                printf("*****Player RED Win!*****\n");
                return 1;
            }
            else if (board[i - 2][j] == 2 && board[i - 1][j] == 2 && board[i][j] == 2 && board[i + 1][j] == 2 && board[i + 2][j] == 2 && board[i + 3][j] == 2) {
                gotoxy(60, 8);
                printf("*****Player Green Win*****\n");
                return 1;
            }
            else if (board[i - 2][j] == 3 && board[i - 1][j] == 3 && board[i][j] == 3 && board[i + 1][j] == 3 && board[i + 2][j] == 3 && board[i + 3][j] == 3) {
                gotoxy(60, 8);
                printf("*****Player Blue Win*****\n");
                return 1;
            }
        }
    }
    for (i = 2; i < 16; ++i) {
        for (j = 2; j < 16; ++j) {
            if (board[j - 2][i - 2] == 1 && board[j - 1][i - 1] == 1 && board[j][i] == 1 && board[j + 1][i + 1] == 1 && board[j + 2][i + 2] == 1 && board[j + 3][i + 3] == 1) {
                gotoxy(60, 8);
                printf("*****Player Red Win!*****\n");
                return 1;
            }
            else if (board[j - 2][i - 2] == 2 && board[j - 1][i - 1] == 2 && board[j][i] == 2 && board[j + 1][i + 1] == 2 && board[j + 2][i + 2] == 2 && board[j + 3][i + 3] == 2) {
                gotoxy(60, 8);
                printf("*****Player Green Win!*****\n");
                return 1;
            }
            else if (board[j - 2][i - 2] == 3 && board[j - 1][i - 1] == 3 && board[j][i] == 3 && board[j + 1][i + 1] == 3 && board[j + 2][i + 2] == 3 && board[j + 3][i + 3] == 3) {
                gotoxy(60, 8);
                printf("*****Player Blue Win!*****\n");
                return 1;
            }
            else if (board[j + 3][i - 2] == 1 && board[j + 2][i - 1] == 1 && board[j + 1][i] == 1 && board[j][i + 1] == 1 && board[j - 1][i + 2] == 1 && board[j - 2][i + 3] == 1) {
                gotoxy(60, 8);
                printf("*****Player Red Win!*****\n");
                return 1;
            }
            else if (board[j + 3][i - 2] == 2 && board[j + 2][i - 1] == 2 && board[j + 1][i] == 2 && board[j][i + 1] == 2 && board[j - 1][i + 2] == 2 && board[j - 2][i + 3] == 2) {
                gotoxy(60, 8);
                printf("*****Player Green Win!*****\n");
                return 1;
            }
            else if (board[j + 3][i - 2] == 3 && board[j + 2][i - 1] == 3 && board[j + 1][i] == 3 && board[j][i + 1] == 3 && board[j - 1][i + 2] == 3 && board[j - 2][i + 3] == 3) {
                gotoxy(60, 8);
                printf("*****Player Blue Win!*****\n");
                return 1;
            }
        }
    }
    return 0;
}

//실질적으로 게임을 실행하고 관리하는 함수
void game() {
    char loop = 'y';
    int x, y;
    do {
        clearconsole();
        check = 1;
        turn = 1;
        int board[19][19] = { 0 };
        draw_board();
        while (1) {
            get_pos(&x, &y, board);
            gotoxy(x, y);
            put_stone();
            gotoxy(0, 23);
            if (check_win(board) == 1) { //y를 입력하면 재시작 이외에는 프로그램 종료
                gotoxy(65, 10);
                printf("Regame? y/n: ");
                scanf(" %c", &loop);
                break;
            }
        }
    } while (loop == 'y');
}

int main() {
    game();
    return 0;
}
