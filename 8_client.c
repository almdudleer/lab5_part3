#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define SOCK_PATH "server_sock"
#define BUFFSIZE 250

int sock;
int conn;
struct sockaddr saddr;
char BUFF[BUFFSIZE];

int init() {
    saddr.sa_family = AF_UNIX;
    strcpy(saddr.sa_data, SOCK_PATH);
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    conn = connect(sock, &saddr, sizeof(struct sockaddr) + 6);
    memset(BUFF, 0, sizeof(BUFF));
    return 0;
}

char* receive() {
    recv(sock, BUFF, BUFFSIZE, 0);
    return BUFF;
}

int terminate() { return 0; }

int main() {
    if (init() == -1) exit(1);
    printf("%s", receive());
    if (terminate() == -1) return 1;
    return 0;
}