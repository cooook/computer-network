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

/*
 * Hash Table for user.name 
 * Hash_Table[user.name] = struct pollfd ;

class Hash_Table {
private:
    static const int KEY = 76543; 
    const int P = 107;
    int first[KEY], e; 
    struct Entry{
        unsigned long long Hash;
        char *name;
        pollfd p_fd; 
        int next;
        bool Equ(const char * Identifier) { return strcmp(name, Identifier) == 0; }
        Entry() { name = NULL; p_fd.events = POLLIN | POLLOUT; }
        ~Entry() { ; }
    } a[KEY];

    unsigned long long Get_Hash(const char* Identifier) {
        unsigned long long Ans = 0;
        for (int i = 0; Identifier[i]; ++i)
            Ans = Ans * P + Identifier[i];
        return Ans; 
    }

    pollfd& New_Entry(const char* Identifier, unsigned long long Hash) {
        a[++e].Hash = Hash; 
        a[e].next = first[Hash % KEY];
        a[e].name = strdup(Identifier);
        return a[e].p_fd;
    }
public:
    pollfd& operator [] (const char * Identifier) {
        unsigned long long Hash = Get_Hash(Identifier);
        for (int i = first[Hash % KEY]; i; i = a[i].next)
            if (a[i].Equ(Identifier))
                return a[i].p_fd;
        return New_Entry(Identifier, Hash);
    }

    bool Count(const char* Identifier) {
        unsigned long long Hash = Get_Hash(Identifier);
        for (int i = first[Hash % KEY]; i; i = a[i].next)
            if (a[i].Equ(Identifier))
                return true; 
        return false; 
    }
} Symbol_Table;
*/