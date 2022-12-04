#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <signal.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

// 통신 시 사용될 기본 크기
#define DEFAULT_BUF_LEN 512
// 기본 포트 번호
#define PORT 3428

int check; //어떤 바둑돌의 차례인지 결정하는 변수
int turn; //한번에 2개를 놓을 수 있으므로 이를 확인하기 위한 변수
int sock; // 소켓
int board[19][19] = {0};

void send_check() {
  char data[2] = {'c',(char) (check + '0')};
  send(sock, data, sizeof (data), 0);
}

void parse_check(const char data[2]) {
  check = data[1] - '0';
}

void send_turn() {
  char data[2] = {'t',(char) (turn + '0')};
  send(sock, data, sizeof (data), 0);
}

void parse_turn(const char data[2]) {
  turn = data[1] - '0';
}

void send_board_data(int board[][19]) {
    char data[19 * 19];
    int count = 0;
    for (int i = 0; i < 19; ++i) {
        for (int j = 0; j < 19; ++j) {
            data[count++] = (char) (board[i][j] + '0');
        }
    }
    send(sock, data, sizeof(data), 0);
}

void parse_board_data(const char data[MAXWORD]) {
    int count = 0;
    for (int i = 0; i < 19; ++i) {
        for (int j = 0; j < 19; ++j) {
            board[i][j] = data[count++] - '0';
        }
    }
}

//게임 재시작을 위해 콘솔에 있는 내용을 지우는 함수
void clearconsole() {
  system("cls");
  DWORD mode;
  GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
  SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode | ENABLE_MOUSE_INPUT);
}

//x, y 좌표를 받아 커서 위치를 이동시키는 함수
void gotoxy(int x, int y) {
  COORD m;
  m.X = x;
  m.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), m);
}

//바둑판을 그리는 함수
void draw_board(int array[][19]) {
    gotoxy(0, 0);
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

    for(i = 0; i < 19; i++) {
        for(j = 0; j < 19; j++) {
            if(array[i][j] == 1) {
                gotoxy(j * 2, i);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                printf("●");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            } else if(array[i][j] == 2) {
                gotoxy(j * 2, i);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                printf("●");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            } else if(array[i][j] == 3) {
                gotoxy(j * 2, i);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                printf("●");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
        }
    }
  gotoxy(0, 19);
}

//플레이어가 바둑판에 마우스를 클릭하면 해당 좌표를 얻는 함수
void get_pos(int *x, int *y, int board[][19]) {
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
              send_board_data(board);
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
  } else if (check == 2) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
    printf("●");
    if (turn == 2) {
      check = 3;
      turn = 0;
    }
  } else if (check == 3) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
    printf("●");
    if (turn == 2) {
      check = 1;
      turn = 0;
    }
  }
  send_turn();
  send_check();
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

//배열을 검사하여 승리 조건을 만족했는지 검사하는 함수
int check_win(int board[][19]) {
  int i;
  int j;
  for (i = 2; i < 16; i++) {
    for (j = 0; j < 19; j++) {
      if (board[j][i - 2] == 1 && board[j][i - 1] == 1 && board[j][i] == 1 && board[j][i + 1] == 1 &&
          board[j][i + 2] == 1 && board[j][i + 3] == 1) {
        gotoxy(60, 8);
        printf("*****Player Red Win!*****\n");
        return 1;
      } else if (board[j][i - 2] == 2 && board[j][i - 1] == 2 && board[j][i] == 2 && board[j][i + 1] == 2 &&
                 board[j][i + 2] == 2 && board[j][i + 3] == 2) {
        gotoxy(60, 8);
        printf("*****Player Green Win!*****\n");
        return 1;
      } else if (board[j][i - 2] == 3 && board[j][i - 1] == 3 && board[j][i] == 3 && board[j][i + 1] == 3 &&
                 board[j][i + 2] == 3 && board[j][i + 3] == 3) {
        gotoxy(60, 8);
        printf("*****Player Blue Win!*****\n");
        return 1;
      } else if (board[i - 2][j] == 1 && board[i - 1][j] == 1 && board[i][j] == 1 && board[i + 1][j] == 1 &&
                 board[i + 2][j] == 1 && board[i + 3][j] == 1) {
        gotoxy(60, 8);
        printf("*****Player RED Win!*****\n");
        return 1;
      } else if (board[i - 2][j] == 2 && board[i - 1][j] == 2 && board[i][j] == 2 && board[i + 1][j] == 2 &&
                 board[i + 2][j] == 2 && board[i + 3][j] == 2) {
        gotoxy(60, 8);
        printf("*****Player Green Win*****\n");
        return 1;
      } else if (board[i - 2][j] == 3 && board[i - 1][j] == 3 && board[i][j] == 3 && board[i + 1][j] == 3 &&
                 board[i + 2][j] == 3 && board[i + 3][j] == 3) {
        gotoxy(60, 8);
        printf("*****Player Blue Win*****\n");
        return 1;
      }
    }
  }
  for (i = 2; i < 16; ++i) {
    for (j = 2; j < 16; ++j) {
      if (board[j - 2][i - 2] == 1 && board[j - 1][i - 1] == 1 && board[j][i] == 1 && board[j + 1][i + 1] == 1 &&
          board[j + 2][i + 2] == 1 && board[j + 3][i + 3] == 1) {
        gotoxy(60, 8);
        printf("*****Player Red Win!*****\n");
        return 1;
      } else if (board[j - 2][i - 2] == 2 && board[j - 1][i - 1] == 2 && board[j][i] == 2 && board[j + 1][i + 1] == 2 &&
                 board[j + 2][i + 2] == 2 && board[j + 3][i + 3] == 2) {
        gotoxy(60, 8);
        printf("*****Player Green Win!*****\n");
        return 1;
      } else if (board[j - 2][i - 2] == 3 && board[j - 1][i - 1] == 3 && board[j][i] == 3 && board[j + 1][i + 1] == 3 &&
                 board[j + 2][i + 2] == 3 && board[j + 3][i + 3] == 3) {
        gotoxy(60, 8);
        printf("*****Player Blue Win!*****\n");
        return 1;
      } else if (board[j + 3][i - 2] == 1 && board[j + 2][i - 1] == 1 && board[j + 1][i] == 1 && board[j][i + 1] == 1 &&
                 board[j - 1][i + 2] == 1 && board[j - 2][i + 3] == 1) {
        gotoxy(60, 8);
        printf("*****Player Red Win!*****\n");
        return 1;
      } else if (board[j + 3][i - 2] == 2 && board[j + 2][i - 1] == 2 && board[j + 1][i] == 2 && board[j][i + 1] == 2 &&
                 board[j - 1][i + 2] == 2 && board[j - 2][i + 3] == 2) {
        gotoxy(60, 8);
        printf("*****Player Green Win!*****\n");
        return 1;
      } else if (board[j + 3][i - 2] == 3 && board[j + 2][i - 1] == 3 && board[j + 1][i] == 3 && board[j][i + 1] == 3 &&
                 board[j - 1][i + 2] == 3 && board[j - 2][i + 3] == 3) {
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
    draw_board(board);
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

int client_init(char *ip, int port) {
  SOCKET server_socket;
  WSADATA wsadata;
  SOCKADDR_IN server_address = {0};

  if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
    printf("Failed WSAStartup\n");
    return -1;
  }

  if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("Failed to open socket\n");
    return -1;
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(ip);
  server_address.sin_port = htons(port);

  if ((connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0) {
    printf("Failed to connect to the server\n");
    return -1;
  }

  return server_socket;
}


unsigned int WINAPI service(void *params) {
  SOCKET s = (SOCKET) params;
  char recv_message[MAXWORD];
  DWORD index = 0;
  WSANETWORKEVENTS ev;
  HANDLE event = WSACreateEvent();

  WSAEventSelect(s, event, FD_READ | FD_CLOSE);
  while (1) {
    index = WSAWaitForMultipleEvents(1, &event, false, INFINITE, false);
    if ((index != WSA_WAIT_FAILED) && (index != WSA_WAIT_TIMEOUT)) {
      WSAEnumNetworkEvents(s, event, &ev);
      if (ev.lNetworkEvents == FD_READ) {
        int len = recv(s, recv_message, MAXWORD, 0);
        if (*recv_message == 't') {
          parse_turn(recv_message);
        } else if (*recv_message=='c') {
          parse_check(recv_message);
        }else {
          parse_board_data(recv_message);
          clearconsole();
          draw_board(board);
        }
      } else if (ev.lNetworkEvents == FD_CLOSE) {
        printf("[i] Server has closed\n");
        closesocket(s);
        break;
      }
    }
  }
  WSACleanup();
  _endthreadex(0);
}


int main(void) {
    SetConsoleOutputCP(65001);
  char server_ip[256] = "127.0.0.1";
  char name[] = "client";
  unsigned int tid;
  char input[MAXWORD];
  HANDLE main_thread;

  sock = client_init(server_ip, PORT);
  if (sock < 0) {
    printf("Failed to initialize socket\n");
    return 1;
  }

  main_thread = (HANDLE) _beginthreadex(NULL, 0, service, (void *) sock, 0, &tid);
  if (!main_thread) {
    printf("Failed to start the main thread\n");
    return 1;
  }
  game();

  closesocket(sock);
  WSACleanup();
  CloseHandle(main_thread);

  return 0;
}
