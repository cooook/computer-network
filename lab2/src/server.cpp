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
# include <map>
# include <thread>
# include <cstring>

# define PORT 8888 

static int MAXN = 0x1000;
int sock;
struct sockaddr_in serverAddr; 
socklen_t len; 
pollfd * p_fdArray;
int Size;

std::map<std::string, int> id_table; 
std::map<int, std::string> name_table;

void login(int sock) {
    static char name[0x100];
    memset(name, 0, sizeof(name));
    int size;
    std::string tmp_name; 
    while (true) {
        send(sock, "Plz Input your name:\n", sizeof("Plz Input your name:\n"), 0);
        size = recv(sock, name, 0xff, 0); 
        if (size <= 0)
            continue; 
        tmp_name = std::string(name); 
        if (!id_table.count(tmp_name)) {
            id_table[tmp_name] = sock;
            name_table[sock] = tmp_name;
            break; 
        }
        else {
            send(sock, "Plz Change your name!\n", sizeof("Plz Change your name!\n"), 0);
        }
    }
    send(sock, "Login success!\n", sizeof("Login success!\n"), 0);
}

void Set_fdArray(int fd, int events) {
    p_fdArray[fd].fd = fd;
    p_fdArray[fd].events = events;
    p_fdArray[fd].revents = 0;
}

void GetConnection() {
    while (true) {
        int connection_sock = accept(sock, (struct sockaddr*)&serverAddr, &len);
        
        Set_fdArray(connection_sock, POLLIN | POLLOUT | POLLHUP);
        
        Size = std::max(Size, connection_sock); 
        if (MAXN - Size < 0x50) {
            MAXN <<= 1;
            p_fdArray = (pollfd *)realloc(p_fdArray, sizeof(pollfd) * MAXN);
        }
        std::thread login_thread(login, connection_sock);
        login_thread.detach();
    }
}

void Remove_Sock(int fd) {
    id_table.erase(name_table[fd]);
    name_table.erase(fd); 
    p_fdArray[fd].fd = 0;
    p_fdArray[fd].events = 0;
    p_fdArray[fd].revents = 0;
    if (Size == fd) 
        while (!p_fdArray[Size].fd == 0) --Size; 
}

void RecvFromSock(int fd) {
    static char message[0x1000], name[0x100];
    memset(message, 0, sizeof(message));
    memset(name, 0, sizeof(name));
    int len = recv(fd, message, sizeof(message) - 0x20, 0);
    if (len <= 0)
        Remove_Sock(fd);
    while (true) {
        send(fd, "Which one do you want to send?\n", sizeof("Which one do you want to send?\n"), 0);
        int len1 = recv(fd, name, sizeof(name) - 1, 0);
        if (len1 <= 0)
            Remove_Sock(fd); 
        std::string tmp_name = std::string(name);
        if (!id_table.count(tmp_name)) {
            send(fd, "No such people!\n", sizeof("No such people!\n"), 0);
            continue;
        }
        else {
            sprintf(message, "Send from %s:%s\n", name_table[fd], message);
            send(id_table[tmp_name], message, len, 0);
            send(fd, "Success!\n", sizeof("Success!\n"), 0);
            break; 
        }
    }    
}

void DealSock() {
    int fd;
    for (int i = 2; i < MAXN; ++i) {
        if (!p_fdArray[i].fd)
            continue;
        fd = p_fdArray[i].fd;
        if (fd == sock)
            continue; 
        if (p_fdArray[i].revents & POLLHUP) 
            Remove_Sock(fd);
        else if (p_fdArray[i].revents & POLLIN) {
            std::thread deal_fd(RecvFromSock, fd);
            deal_fd.detach();
        }
    }
}

void broadcast() {
    static char buf[0x1000];
    while (true) {
        fgets(buf, sizeof(buf) - 0x20, stdin);
        sprintf(buf, "Send from server:%s\n", buf);
        for (int i = 2; i <= Size; ++i) {
            if (!p_fdArray[i].fd)
                continue;
            send(i, buf, strlen(buf), 0);
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
    p_fdArray = (pollfd *)malloc(sizeof(pollfd) * MAXN);
    memset(p_fdArray, 0, sizeof(p_fdArray));
    
    std::thread get_Connection(GetConnection);
    get_Connection.detach();

    std::thread broadcast_thread(broadcast);
    broadcast_thread.detach();

    while (1) {
        int n = poll(p_fdArray, MAXN, -1);
        if (n <= 0) {
            puts("Error!");
            continue; 
        }
        DealSock();
    }


    free(p_fdArray);
    p_fdArray = NULL;


    return 0;
}