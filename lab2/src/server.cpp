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

std::map<std::string, int> id_table; 
std::map<int, std::string> name_table;

void RecvFromSock(int fd);

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
    std::thread sockHandler(RecvFromSock, sock);
    sockHandler.detach();
}

void GetConnection() {
    while (true) {
        int connection_sock = accept(sock, (struct sockaddr*)&serverAddr, &len);
        
        printf("sock=%d\n", connection_sock);
        
        std::thread login_thread(login, connection_sock);
        login_thread.detach();
    }
}

void Remove_Sock(int fd) {
    id_table.erase(name_table[fd]);
    name_table.erase(fd); 
}

bool parse(char *buffer, char *message, char *name) {
    
}

void RecvFromSock(int fd) {
    static char message[0x1000], name[0x100], buffer[0x2000];
    while (true) {
        memset(message, 0, sizeof(message));
        memset(name, 0, sizeof(name));
        memset(buffer, 0, sizeof(buffer));
        int len = recv(fd, buffer, sizeof(buffer), 0);
        printf("fd:%d buffer:%s\n", fd, buffer);
        
        if (len <= 0) {
            printf("Error\n");
            Remove_Sock(fd);
            break; 
        }
        sscanf(buffer, "{__id}={%s}:{__message}={%s}", name, message);
        // if (!)
        printf("name:%s\nmessage:%s\n", name, message);


        std::string tmp_name = std::string(name);

        if (!id_table.count(tmp_name)) {
            send(fd, "No such people!\n", sizeof("No such people!\n"), 0);
        }
        else {
            sprintf(buffer, "Send from %s:%s\n", name_table[fd].c_str(), message);
            send(id_table[tmp_name], buffer, strlen(buffer), 0);
            send(fd, "Success!\n", sizeof("Success!\n"), 0);
        }
    }
}

void broadcast() {
    static char buf[0x1000], buffer[0x1000];
    while (true) {
        fgets(buf, sizeof(buf) - 0x20, stdin);
        sprintf(buffer, "Send from server:%s\n", buf);
        printf("%s", buffer);
        for (auto i:id_table) {
            printf("id:%d\n", i.second);
            send(i.second, buffer, strlen(buffer), 0);
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
    
    std::thread get_Connection(GetConnection);
    get_Connection.detach();

    std::thread broadcast_thread(broadcast);
    broadcast_thread.detach();


    while (1); 
    return 0;
}