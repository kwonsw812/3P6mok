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
// 클라이언트 개수
#define CLIENT_COUNT 3

// 소켓 구조체 타입
typedef struct sock_info {
    SOCKET s;
    HANDLE ev;
    char name[DEFAULT_BUF_LEN];
    char ip[DEFAULT_BUF_LEN];
} SOCK_INFO;
SOCK_INFO sock_arr[CLIENT_COUNT + 1];

// 전체 소켓 개수 카운터
int total_socket_count;

// 인터럽트 시그널 시 스택 해제 후 종료를 위한 변수
volatile bool running = true;

// 서버 초기화
SOCKET server_init();

// 서버 종료
void server_close();

// 서버 서비스
unsigned int WINAPI server_thread(void *);

// TODO
unsigned int WINAPI recv_and_forward(void *);

// 클라이언트 추가
int add_client(int);

// TODO
int read_client(int);

// 클라이언트 제거
void remove_client(int);

// 클라이언트에 메세지 배포
int notify_client(char *);

// 클라이언트 아이피 얻기
char *get_client_ip(int);

void interrupt(int) {
    running = false;
}


SOCKET server_init() {
    WSADATA wsadata;
    SOCKET sock;
    SOCKADDR_IN server_addr;

    memset(&sock_arr, 0, sizeof(sock_arr));
    total_socket_count = 0;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("Failed WSAStartup\n");
        exit(0);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to open socket\n");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to bind socket\n");
        exit(0);
    }

    if (listen(sock, SOMAXCONN) < 0) {
        printf("Failed to listen socket\n");
        exit(0);
    }

    return sock;
}

void server_close() {
    for (int i = 0; i < total_socket_count; ++i) {
        closesocket(sock_arr[i].s);
        WSACloseEvent(sock_arr[i].ev);
    }
}

unsigned int WINAPI server_thread(void *arg) {
    SOCKET sock;
    WSANETWORKEVENTS events;
    int index;
    WSAEVENT handle_arr[CLIENT_COUNT + 1];

    sock = server_init();
    printf("Server initialized at port %d\n", PORT);

    HANDLE event = WSACreateEvent();
    sock_arr[total_socket_count].ev = event;
    sock_arr[total_socket_count].s = sock;
    strcpy(sock_arr[total_socket_count].name, "server");
    strcpy(sock_arr[total_socket_count].ip, "0.0.0.0");
    WSAEventSelect(sock, event, FD_ACCEPT);
    ++total_socket_count;

    while (running) {
        memset(&handle_arr, 0, sizeof(handle_arr));
        for (int i = 0; i < total_socket_count; ++i) {
            handle_arr[i] = sock_arr[i].ev;
        }

        printf(".");

        index = WSAWaitForMultipleEvents(
                total_socket_count, handle_arr, false, INFINITE, false);
        if ((index != WSA_WAIT_FAILED) && (index != WSA_WAIT_TIMEOUT)) {
            WSAEnumNetworkEvents(sock_arr[index].s, sock_arr[index].ev, &events);
            if (events.lNetworkEvents == FD_ACCEPT) { add_client(index); }
            else if (events.lNetworkEvents == FD_READ) { read_client(index); }
            else if (events.lNetworkEvents == FD_CLOSE) { remove_client(index); }
        } else {
            printf("ERR %d\n", index);
        }
    }
    closesocket(sock);

    WSACleanup();
    _endthreadex(0);
}

int add_client(int index) {
    SOCKADDR_IN addr;
    int len = 0;
    SOCKET accept_sock;
    if (total_socket_count == FD_SETSIZE) {
        return -1;
    }
    len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    accept_sock = accept(sock_arr[0].s, (SOCKADDR *) &addr, &len);

    HANDLE event = WSACreateEvent();
    sock_arr[total_socket_count].ev = event;
    sock_arr[total_socket_count].s = accept_sock;
    strcpy(sock_arr[total_socket_count].ip, inet_ntoa(addr.sin_addr));

    WSAEventSelect(accept_sock, event, FD_READ | FD_CLOSE);

    ++total_socket_count;
    printf("[i] A new client added: %s\n", inet_ntoa(addr.sin_addr));

    return 0;
}

int read_client(int index) {
    unsigned int tid;
    HANDLE main_thread = (HANDLE) _beginthreadex(NULL, 0, recv_and_forward, (void *) index, 0, &tid);
    WaitForSingleObject(main_thread, INFINITE);
    CloseHandle(main_thread);
    return 0;
}

unsigned int WINAPI recv_and_forward(void *arg) {
    int index = (int) arg;
    char message[MAXWORD], share_message[MAXWORD];
    SOCKADDR_IN client_address;
    int addr_len = 0;
    char *tok1 = NULL;
    char *nxt_tok = NULL;

    memset(&client_address, 0, sizeof(client_address));

    if (recv(sock_arr[index].s, message, MAXWORD, 0) > 0) {
        addr_len = sizeof(client_address);
        getpeername(sock_arr[index].s, (SOCKADDR *) &client_address, &addr_len);
        strcpy(share_message, message);
        if (strlen(sock_arr[index].name) <= 0) {
            tok1 = strtok_s(message, "]", &nxt_tok);
            strcpy(sock_arr[index].name, tok1 + 1);
        }

        for (int i = 0; i < total_socket_count; ++i) {
            send(sock_arr[i].s, share_message, MAXWORD, 0);
        }
    }

    _endthreadex(0);
}

void remove_client(int index) {
    char remove_ip[256];
    char message[MAXWORD];

    strcpy(remove_ip, get_client_ip(index));
    printf("[i] Lose connection: %s\n", remove_ip);

    closesocket(sock_arr[index].s);
    WSACloseEvent(sock_arr[index].ev);

    --total_socket_count;
    sock_arr[index].s = sock_arr[total_socket_count].s;
    sock_arr[index].ev = sock_arr[total_socket_count].ev;

    strcpy(sock_arr[index].ip, sock_arr[total_socket_count].ip);
    strcpy(sock_arr[index].name, sock_arr[total_socket_count].name);
}

char *get_client_ip(int index) {
    static char ipaddr[256];
    int addr_len;
    struct sockaddr_in sock;

    addr_len = sizeof(sock);
    if (getpeername(sock_arr[index].s, (struct sockaddr *) &sock, &addr_len) < 0)
        return NULL;

    strcpy(ipaddr, inet_ntoa(sock.sin_addr));
    return ipaddr;
}

int notify_client(char *message) {
    for (int i = 1; i < total_socket_count; ++i) {
        send(sock_arr[i].s, message, DEFAULT_BUF_LEN, 0);
    }

    return 0;
}

int main(void) {
    signal(SIGINT, interrupt);
    unsigned int tid;
    char message[MAXWORD] = "";
    HANDLE main_thread;

    main_thread = (HANDLE) _beginthreadex(
            NULL, 0, &server_thread, NULL, 0, &tid);

    if (!main_thread) {
        printf("Error: _beginthreadex() failed\n");
        return 1;
    } else {
        printf("Server thread created successfully\n");
    }

    while (running) {
        gets(message);
        if (strcmp(message, "exit") == 0) {
            break;
        }

        notify_client(message);
    }

    server_close();
    WSACleanup();
    CloseHandle(main_thread);

    return 0;
}
