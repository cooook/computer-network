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


class HttpResponse { 
private:
    char *url, *host, *resource;
public:
    void SetURL(const char* URL) {
        url = strdup(URL);
    }

    ~HttpResponse() {
        if (url)
            free(url);
        if (host)
            free(host);
        if (resource)
            free(resource);
        url = host = resource = NULL;
    }

    bool ParseURL() {
        char* pos = strstr(url, "http://");
        if (pos == NULL)
            pos = strstr(url, "https://");
        else 
            pos += strlen("http://");
        if (pos == NULL)
            return false; 
        else 
            pos += strlen("https://");
        if (!strstr(pos, "/"))
            return false;
        host = (char*)malloc(0x1000);
        resource = (char*)malloc(0x1000);
        sscanf(pos, "%[^/]%s", host, resource);
        return true; 
    }

    bool GetHttpResponse(char *&response, int &byte) {
        if (!ParseURL()) {
            printf("Error! Can't Parse url:%s!", url);
            return false; 
        }
        struct hostent *hp = gethostbyname(host);
        if (hp == NULL){
            printf("Error! Can't get ip by host url:%s\n", url);
            return false;
        }

        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == -1) {
            printf("Error! Can't create socket\n");
            return false; 
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(80);
        // serverAddr.sin_addr.s_addr = inet_addr("120.78.199.148");
        memcpy(&serverAddr.sin_addr, hp -> h_addr, 4); 

        if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            printf("Error! Can't connect to url:%s\n", url);
            return false; 
        }

        char *request = (char*)malloc(strlen(resource) + strlen(host) + 0x1000);
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nCookie: Hm_lvt_667639aad0d4654c92786a241a486361=1635247526; _ga=GA1.2.1480152237.1635247527; _gid=GA1.2.716570091.1635247527; likes=; Hm_lpvt_667639aad0d4654c92786a241a486361=1635257484; _gat_gtag_UA_61934506_5=1\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nConnection: close\r\n\r\n", resource, host);
        printf("%s", request);
        if (send(sock, request, strlen(request), 0) == -1) {
            printf("Error! Can't send\n");
            return false; 
        }
        int Content_Size = 0x1000;
        char *buffer = (char*)malloc(Content_Size);
        memset(buffer, 0, sizeof(buffer));
        int len; 
        byte = 0;
        do {
            len = recv(sock, buffer + byte, Content_Size - byte, 0);
            if (len > 0)
                byte += len;
            if (Content_Size - byte < 0x20) {
                Content_Size <<= 1;
                buffer = (char*)realloc(buffer, Content_Size);
            }

        }   while (len > 0);
        buffer[byte] = '\x00';
        response = buffer;
        close(sock);
        return true; 
    }
} ;

int main() {
    HttpResponse * res = new HttpResponse;
    char *response;
    int byte = 0;
    res -> SetURL("https://bing.ioliu.cn/");
    res -> GetHttpResponse(response, byte);
    printf("%s", response);
    free(response);
    response = NULL;
    return 0;
}