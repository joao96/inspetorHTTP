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


//porta
#define porta 8228
void parsing(char*, char*, char*);
int get_host_by_name(char*, char*, int);
void spider(char *);
int main(){

    // sockets do servidor e cliente
    int actual_socket, new_socket;
    // tamanho do buffer
    int BUFFER_SIZE = 200000000;
    int message_len;
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
    // conecta-se a porta 8228
    servidor.sin_port = htons(porta);

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

        // recebe o request
        if((message_len = recv(new_socket, buf, BUFFER_SIZE, 0)) > 0) {
            buf[message_len - 1] = '\0';
            printf("Request: %s.\n", buf);
        }

        // cria novo url para mandar ao cliente
        memset(new_http, 0x0, 150);
        parsing(buf, new_http, new_host);

        int sock=get_host_by_name(new_http, new_host, actual_socket);

        /*
         // envia novo url
         if(send(actual_socket, new_http, strlen(new_http), 0)){
         printf("Novo pedido por: %s", new_http);
         }
         */
        //limpa o buffer antigo
        memset(buf, 0x0, BUFFER_SIZE);

        // espera resposta do cliente
        /*if((message_len = recv(sock, buf, BUFFER_SIZE, 0)) > 0) {
            buf[message_len - 1] = '\0';
            printf("Resposta: %s.\n", buf);
        }*/

        while(read(sock,buf,BUFFER_SIZE-1)!=0){
            fprintf(stderr,"%s",buf);
            spider(buf);
            bzero(buf, BUFFER_SIZE);
        }


        // fecha o socket do cliente
        close(new_socket);

   // } while(1);
}

void parsing(char* buf, char * new_http, char * new_host){
    char *get = strstr(buf, "GET");
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    char *user_agent = strstr(buf, "User-Agent");
    int i, j=0;
    int start_http = get - buf + 4;
    int end_http = http - buf - 2;
    int start_host = host - buf + 6;
    int end_host = user_agent - buf - 2;
    for(i=start_http;i<end_http;i++){
        new_http[j] = buf[i];
        j++;
    }
    strcat(new_http, "/index.html");
    printf("concat: %s", new_http);
    j = 0;
    for(i=start_host;i<end_host;i++){
        new_host[j] = buf[i];
        j++;
    }
}

int get_host_by_name(char *new_http, char *new_host, int actual_socket){
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

void spider(char *html){
    FILE *html_tree, *html_page;
    char needle[1] = "";

    html_tree = fopen("html_tree.txt", "w");
    html_page = fopen("html_page.txt", "w");

    if (html_tree == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }
    if (html_page == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }

    fputs(html, html_page);

    while(!feof(html_page)){
        char *href = strstr(html, "<a href=");
        href = href + 7;
        while(*href!= '>'){
            fprintf(html_tree, "%s", href);
        }

    }

}
