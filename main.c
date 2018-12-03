#include "functions.h"

int BUFFER_SIZE = 4096; // tamanho do buffer

int main(){

    FILE *html_file = NULL, *html_tree = NULL;
    int actual_socket, new_socket; // sockets do servidor e cliente
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

    html_file = fopen("html_file.txt", "a");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(6);
    }

    while(read(sock, buf, BUFFER_SIZE-1)!=0){
        //fprintf(stderr, "%s", buf);
        fputs(buf, html_file);
        bzero(buf, BUFFER_SIZE);
    }

    spider(new_host);
    dump(new_host);

    fclose(html_tree);
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
        perror("nao foi possivel conectar.");
        exit(1);
    }
    char request[250] = "GET / HTTP/1.1\r\nHost: ";
    strcat(request, new_host);
    strcat(request, "\r\n\r\n\r\n");
    write(sock, request, strlen(request));

    return sock;
}

char *delchar(char *x, int start) {
    char *buf = malloc(BUFFER_SIZE);
    bzero(buf, BUFFER_SIZE);
    int i = 0;
    while(x[start] != '\0'){
        buf[i] = x[start+i];
        // printf("%c", buf[i]);
    }
    return buf;
}
