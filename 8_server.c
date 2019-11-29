#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <errno.h>

#ifndef MODE
#define MODE 1// 0: printf (incompatible with client), 1: shared memory, 2: message queue, 3: mmap
#endif

#define SOCK_PATH "server_sock"
#define BUFFSIZE 250

pid_t pid;
uid_t uid;
gid_t gid;
time_t start_time;

char sendBuff[BUFFSIZE];
struct sockaddr_un serv_addr;
int listenfd = 0, connfd = 0, rc = 0;
char cont = 1;


void handle_pipe() {
    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
}

int init() {
    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("socket() failed");
        return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(sendBuff, 0, sizeof(sendBuff));

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH);

    unlink(SOCK_PATH);
    rc = bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (rc < 0)
    {
        perror("bind() failed");
        return -1;
    }

    rc = listen(listenfd, 10);
    if (rc< 0)
    {
        perror("listen() failed");
        return -1;
    }

    return 0;
}

int init_data() {
    pid = getpid();
    uid = getuid();
    gid = getgid();
    start_time = time(0);
    printf("Started server, pid: %d, uid: %d, gid: %d\n", pid, uid, gid);
    return 0;
}

int send_to_client(char* s) {
    snprintf(sendBuff, sizeof(sendBuff), "%s", s);
    rc = send(connfd, sendBuff, sizeof(sendBuff), 0);
    if (rc < 0)
    {
        char err[100];
        if (errno != 32) {
            perror("send() failed");
            return -1;
        }
    }
    return 0;
}

void terminate() {
    cont = 0;
    if (connfd != -1) close(connfd);
    if (listenfd != -1) close(listenfd);
}

int main() {
    signal(SIGPIPE, handle_pipe);
    signal(SIGTERM, terminate);
    double load[3];
    time_t uptime = 0;

    if (init() == -1) exit(1);
    init_data();


    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    if (connfd < 0)
    {
        perror("accept() failed");
        exit(1);
    }

    while (cont) {
        if (uptime != time(0) - start_time) {
            uptime = time(0) - start_time;
            getloadavg(load, 3);
            char s[100];
            sprintf(s, "%lu: %f %f %f\n", uptime, load[0], load[1], load[2]);
            send_to_client(s);
            if (uptime == 200) cont = 0;
        }
    }
    terminate();
    return 0;
}

