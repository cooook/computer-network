#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
# include <thread>

#define MYPORT 8888
#define BUFFER_SIZE 1024
int sock_cli;


void RecvHandler(); 
void SendHandler();

void login() {
    static char buffer[BUFFER_SIZE], name[BUFFER_SIZE];
    while (true) {
        int len = recv(sock_cli, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "Login success!\n")) {
            printf("%s", buffer);
            // fgets(name, sizeof(name) - 1, stdin);
            scanf("%s", name);
            send(sock_cli, name, sizeof(name), 0);
        }
        else {
            printf("%s", buffer);
            break; 
        }
    }

    std::thread recvThread(RecvHandler);
    recvThread.detach();

    std::thread sendThread(SendHandler);
    sendThread.detach();
}

void RecvHandler() {
    char buf[0x1000];
    while (true) {
        if (recv(sock_cli, buf, sizeof(buf), 0) <= 0) 
            break; 
        printf("%s", buf);
    }
}

void SendHandler() {
    static char name[BUFFER_SIZE];
    static char sendbuf[BUFFER_SIZE];
    static char buffer[BUFFER_SIZE << 1]; 
    while (1)
    {
        printf("您想给谁发消息?\n");
        // fgets(name, sizeof(name), stdin);
        scanf("%s", name);
        if (strcmp(name, "quit") == 0)
            break; 
        printf("您想发送什么消息?\n");
        scanf("%s", sendbuf);
        // fgets(sendbuf, sizeof(sendbuf), stdin);
        sprintf(buffer, "{__id}={%s}:{__message}={%s}", name, sendbuf);
        printf("%s\n len = %d\n", buffer, strlen(buffer));
        send(sock_cli, buffer, strlen(buffer), 0); //发送
        memset(buffer, 0, sizeof(buffer));
    }
}

int main()
{

    ///定义sockfd
    sock_cli = socket(AF_INET, SOCK_STREAM, 0);
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);                 ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); ///服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    login();



    while (1);
    close(sock_cli);
    return 0;
}