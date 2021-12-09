# include <iostream>
# include <cstdio>
# include <cstdlib>
# include <cerrno>
# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <pthread.h> 

# define PORT 8888 

static int MAXN = 0x1000;
int sock;
struct sockaddr_in serverAddr; 
socklen_t len; 
pollfd ** p_fdArray;
int Size;

void GetConnection() {
    while (true) {
        int connection_sock = accept(sock, (struct sockaddr*)&serverAddr, &len);
        pollfd tmp_fd;
        tmp_fd.events = POLLIN | POLLOUT;
        tmp_fd.fd = connection_sock; 
        p_fdArray[connection_sock] = &tmp_fd;
        Size = std::max(Size, connection_sock); 
        if (MAXN - Size < 0x50) {
            MAXN <<= 1;
            p_fdArray = (pollfd **)realloc(p_fdArray, sizeof(pollfd*) * MAXN);
        }
    }
}

int main() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sock, 20) == -1) {
        perror("listen");
        exit(1);
    }

    len = sizeof(serverAddr);
    p_fdArray = (pollfd **)malloc(sizeof(pollfd*) * MAXN);


    free(p_fdArray);
    p_fdArray = NULL;


    return 0;
}