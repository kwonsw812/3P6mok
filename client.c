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
        if (len > 0)
          printf("%s\n", recv_message);
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
  char server_ip[256] = "127.0.0.1";
  char name[] = "client";
  unsigned int tid;
  int sock;
  char input[MAXWORD];
  // char message[DEFAULT_BUF_LEN];
  char *pexit = NULL;
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

  for (;;) {
    gets(input);
    if (strcmp(input, "exit") == 0) {
      break;
    }
    send(sock, input, sizeof(input), 0);
  }

  closesocket(sock);
  WSACleanup();
  CloseHandle(main_thread);

  return 0;
}