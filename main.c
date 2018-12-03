#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/stat.h>



#define porta 8228 //porta

void parsing(char*, char*, char*);
int get_host_by_name(char*, char*);
FILE *spider(char*, FILE*, char*);
void dump(FILE*, char*, char*, int);
FILE *create_html_txt(char*);

int main(){

    FILE *html_file = NULL, *html_tree = NULL;
    int actual_socket, new_socket; // sockets do servidor e cliente
    int BUFFER_SIZE = 200000000; // tamanho do buffer
    long int message_len;
    struct sockaddr_in servidor, cliente;
    char *buf = malloc(BUFFER_SIZE);
    char new_http[150] = "", new_host[150] = "";

    unsigned int addr_len;

    // cria o socket do servidor
    actual_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(actual_socket < 0){
        perror("Nao foi possivel criar o socket do servidor.\n");
    }
    printf("Socket criado com sucesso, %d.\n", actual_socket);

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta); // conecta-se a porta 8228

    addr_len = sizeof(struct sockaddr_in);

    // conecta o servidor ao socket criado
    if(bind(actual_socket, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Bindou errado.\n");
    }

    //do {
        // ouve a porta
        if(listen(actual_socket, 1) == -1) {
            perror("Escutou errado.\n");
        }
        printf("Porta: %d\n", porta);

        unsigned int cliente_lenght = sizeof(cliente);

        // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
        if ((new_socket = accept(actual_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
            perror("Erro ao aceitar o cliente.\n");
        }
        printf("Aceitou.\n");


        if((message_len = recv(new_socket, buf, BUFFER_SIZE, 0)) > 0) {  // recebe o request
            buf[message_len - 1] = '\0';
            //printf("Request: %s.\n", buf);
        }


        memset(new_http, 0x0, 150); // cria novo url para mandar ao cliente
        parsing(buf, new_http, new_host);

        // faz novo GET para o cliente
        int sock = get_host_by_name(new_http, new_host);


        memset(buf, 0x0, BUFFER_SIZE); //limpa o buffer antigo


        while(read(sock, buf, BUFFER_SIZE-1)!=0){
            html_file = create_html_txt(buf);
            bzero(buf, BUFFER_SIZE);
        }

        html_tree = spider(buf, html_file, new_host);
        dump(html_tree, new_host, new_http, sock);

        close(new_socket); // fecha o socket do cliente

   // } while(1);
}

void parsing(char* buf, char * new_http, char * new_host){
    char *get = strstr(buf, "GET");
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    char *user_agent = strstr(buf, "User-Agent");
    long int i, j=0;
    long int start_http = get - buf + 4;
    long int end_http = http - buf - 2;
    long int start_host = host - buf + 6;
    long int end_host = user_agent - buf - 2;
    for(i=start_http;i<end_http;i++){
        new_http[j] = buf[i];
        j++;
    }
    strcat(new_http, "/index.html");
    j = 0;
    for(i=start_host;i<end_host;i++){
        new_host[j] = buf[i];
        j++;
    }
}

int get_host_by_name(char *new_http, char *new_host){
    struct hostent *hp;
    struct sockaddr_in cliente;
    int on = 1, sock;
    if ((hp = gethostbyname(new_host)) == NULL){
        herror("gethostbyname");
    }
    bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
    cliente.sin_port = htons(80);
    cliente.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
        perror("conectou");
        exit(1);
    }
    char request[250] = "GET / HTTP/1.1\r\nHost: ";
    strcat(request, new_host);
    strcat(request, "\r\n\r\n\r\n");
    write(sock, request, strlen(request));

    return sock;
}

FILE * spider(char *html, FILE *html_file, char *host){
     FILE *html_tree;
     char href[256], buf[2000000], c;
     char *needle;
     long int i, j=0;

     html_file = fopen("html_file.txt", "r");
     html_tree = fopen("html_tree.txt", "a");

     if(html_file == NULL){
         printf("Erro ao abrir o arquivo\n");
         exit(0);
     }

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }

     while (fgets(href, sizeof(href), html_file)) {

         if((needle = strstr(href, "href=")) != NULL){
             i = needle - href + 6;
             while((c = href[i])!= '"'){
                 buf[j] = c;
                 i++;
                 j++;
             }
             strcat(buf, "\r\n");
             if(strstr(buf, host) != NULL){
                 fputs(buf, html_tree);
             }
             bzero(buf, 2000000);
             j = 0;
        }
     }
    fclose(html_file);
    fclose(html_tree);
    return html_tree;
}

void dump(FILE *html_tree, char *host, char *http, int sock){

    long int i, j = 0, k = 0;
    char *buf = malloc(200000000);
    char dir[150];
    char reverse_dir[150];
    char href[256], c;
    char *needle;
    bzero(dir, 150);
    bzero(reverse_dir, 150);
    dir[0] = '\0';
    reverse_dir[0] = '\0';

    html_tree = fopen("html_tree.txt", "r");

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }

    while (fgets(href, sizeof(href), html_tree)) {
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
        needle = strstr(href, host);
        i = needle - href + 0;
        k = 0;
        while(href[i] != '/')
            i++;
        while(href[i] != '\0'){
            href[k] = href[i];
            k++;
            i++;
        }
        href[k] = '\0';
        char request[1000] = "GET ";
        strcat(request, href);
        strcat(request, "HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\n\r\n");
        printf("%s\n", request);
        write(sock, request, strlen(request));
        memset(buf, 0x0, 200000000); //limpa o buffer antigo
        while(read(sock, buf, 200000000-1) != 0){
            printf("entrouuu");
            fprintf(stderr,"%s",buf);
            strcat(dir, "/");
            strcat(dir,buf);
            FILE *file = fopen(dir, "w");
            fputs(buf, file);
        }
    }

}

FILE * create_html_txt(char *html){

    FILE * html_file = fopen("html_file.txt", "a");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }

    fputs(html, html_file);
    return html_file;
}
