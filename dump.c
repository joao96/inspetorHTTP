#include "functions.h"

void dump(char *url, char *host) {
    FILE *html_tree;
    struct hostent *hp;
    struct sockaddr_in cliente;
    int on = 1, sock;
    long int i, j = 0, k = 0;
    char l;
    char buf[BUFFER_SIZE], aux_buf[BUFFER_SIZE];
    char dir[150];
    char reverse_dir[150];
    char href[256], c;
    char request[500];
    char final_url[250];
    char *needle;
    bzero(dir, 150);
    bzero(reverse_dir, 150);
    dir[0] = '\0';
    reverse_dir[0] = '\0';

    html_tree = fopen("html_tree.txt", "r");

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo. 3\n");
        exit(3);
    }

    while (fgets(href, sizeof(href), html_tree)) {
        j = 0, k = 0, i = 0;
        if ((hp = gethostbyname(host)) == NULL){
            herror("gethostbyname");
        }
        bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
        cliente.sin_port = htons(80);
        cliente.sin_family = AF_INET;
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
            perror("nao foi possivel conectar.");
            exit(4);
        }
        memset(dir, '\0', 150);
        memset(reverse_dir, '\0', 150);
        if((needle = strstr(href, "\r\n")) != NULL){
            i = needle - href - 1;
            while((c = href[i]) != '/')
                i--;
            while(((c = href[i-1]) != '/')){
                reverse_dir[j] = c;
                j++;
                i--;
            }
            j = strlen(reverse_dir);
            while(j > 0){
                dir[k] = reverse_dir[j-1];
                j--;
                k++;
            }
            k = 0;
            mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR);
        }
        bzero(reverse_dir, 150);
        if((needle = strstr(href, "\r\n")) != NULL){
            i = needle - href - 1;
            j = 0;
            while((c = href[i]) != '/')
                i--;
            while(((c = href[i+1]) != '\r')){

                reverse_dir[j] = c;
                i++;
                j++;
            }
        }
        needle = strstr(href, url);
        i = needle - href + 0;
        k = 0;
        while(href[i] != '/')
            i++;
        while(href[i] != '\0'){
            href[k] = href[i];
            k++;
            i++;
        }
        href[k-2] = '\0';
        strcat(request, "GET ");
        strcat(request, href);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\n\r\n");
        printf("%s\n", request);
        write(sock, request, strlen(request));
        memset(buf, 0x0, BUFFER_SIZE); //limpa o buffer antigo
        if(strlen(dir) != 0){
            strcat(dir, "/");
            strcat(dir, reverse_dir);
        }else{
            strcpy(dir, reverse_dir);
        }
        FILE *file = fopen(dir, "w");
        if(file != NULL){
            j = 0;
            while(read(sock, buf, BUFFER_SIZE-1) != 0){
                if(needle = strstr(buf, "\r\n\r\n")){
                    i = needle - buf + 4;
                    while(buf[i] != '\0'){
                        aux_buf[j] = buf[i];
                        i++;
                        j++;
                    }
                    fwrite(aux_buf, 1, sizeof(aux_buf), file);
                }
                else
                    fwrite(buf, 1 , sizeof(buf), file);
                bzero(aux_buf, BUFFER_SIZE);
                bzero(buf, BUFFER_SIZE);
            }
            fclose(file);
        }
        else{
            printf("Erro ao abrir o arquivo 5.\n");
            exit(5);
        }
        close(sock);
    }
}
