#include "functions.h"

int BUFFER_SIZE = 4096; // tamanho do buffer
int porta = 8228;

int main( int argc, char *argv[] ){

    FILE *html_file = NULL, *html_tree = NULL, *request_file = NULL;
    int actual_socket, new_socket; // sockets do servidor e cliente
    long int message_len;
    unsigned int addr_len;
    struct sockaddr_in servidor, cliente;
    char *buf = malloc(BUFFER_SIZE);
    char new_url[150] = "\0", new_host[150] = "\0";
    int c, i, opcao = 0;
    unsigned int cliente_lenght = sizeof(cliente);
    int tr=1;

    if(argv[1]){ // tratamento para o argumento do número da porta
        if(strcmp(argv[1], "-p") == 0)
            if(argv[2] != NULL)
                porta = atoi(argv[2]);
    }

    do{
        // cria o socket do servidor
        actual_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(actual_socket < 0){
            perror("Nao foi possivel criar o socket do servidor.\n");
        }
        printf("Socket criado com sucesso.\n");

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta); // conecta-se a porta 8228 ou a porta passada por argv

        addr_len = sizeof(struct sockaddr_in);


    if (setsockopt(actual_socket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // conecta o servidor ao socket criado
    if(bind(actual_socket, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Erro Bind.\n");
    }
    //do {
    if(listen(actual_socket, 1) == -1) { // ouve a porta
        perror("Erro ao Escutar.\n");
    }
    printf("Porta: %d\n", porta);

    // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
    if ((new_socket = accept(actual_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
        perror("Erro ao aceitar o cliente.\n");
    }
    printf("Cliente aceito.\n");

    if((message_len = recv(new_socket, buf, BUFFER_SIZE, 0)) > 0) {  // recebe o request
        buf[message_len - 1] = '\0';

        if(strstr(buf, "POST") != NULL) { // se o tipo do request for POST, encerra o programa
            printf("Requisicao do tipo POST.\n");
            exit(0);
        }

        request_file = fopen("request.txt", "w");

        if(request_file == NULL){
            printf("Arquivo de request nao foi aberto.\n");
            exit(1);
        }
        fputs(buf, request_file);
        fclose(request_file);
        system("nano request.txt");
        request_file = fopen("request.txt", "r");

        i = 0;
        bzero(buf, BUFFER_SIZE);
        if (request_file) {
            while ((c = getc(request_file)) != EOF){
                buf[i] = c;
                i++;
            }
            printf("Request: %s.\n", buf);
            fputs(buf, request_file);
            fclose(request_file);
            system("nano request.txt");
            request_file = fopen("request.txt", "r");
            i = 0;
            bzero(buf, BUFFER_SIZE);
            if (request_file) {
                while ((c = getc(request_file)) != EOF){
                    buf[i] = c;
                    i++;
                }
                fclose(request_file);
            }
            //printf("NEW BUFFER: %s\n", buf);
        }
    }

    printf("Request: %s.\n", buf);
    parsing(buf, new_url, new_host);

        // faz novo GET para o cliente
        int sock = get_host_by_name(new_url, new_host);

    bzero(buf, BUFFER_SIZE); //limpa o buffer antigo

    html_file = fopen("html_file.txt", "w");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(2);
    }

    while(read(sock, buf, BUFFER_SIZE - 1) != 0){
        fputs(buf, html_file);
        bzero(buf, BUFFER_SIZE);
    }
    fclose(html_file);
    system("nano html_file.txt");
    html_file = fopen("html_file.txt", "r");
    bzero(buf, BUFFER_SIZE);
    while(fread(buf, 1, BUFFER_SIZE, html_file) == BUFFER_SIZE){
        send(new_socket, buf, BUFFER_SIZE, 0);
    }


    // do{
    //     printf("*************************************************************\n");
    //     printf("*                       Inspetor HTTP                       *\n");
    //     printf("*************************************************************\n");
    //
    //     printf("1 - SPIDER.\n");
    //     printf("2 - DUMP.\n");
    //     printf("3 - SAIR.\n");
    //     printf("Digite a opcao: ");
    //     scanf("%d", &opcao);
    //
    //     switch (opcao) {
    //         case 1:
    //             spider(new_url, new_host);
    //             break;
    //
    //
    //         case 2:
    //             html_tree = fopen("html_tree.txt", "a");
    //             if(html_tree != NULL){
    //                 fseek (html_tree, 0, SEEK_END);
    //                 if (ftell(html_tree) == 0) {
    //
    //                 printf("Arquivo vazio!\n");
    //                 }else{
    //                     dump(new_url, new_host);
    //                 }
    //             }else{
    //                 printf("Nao foi possivel abrir o html tree.\n");
    //             }
    //             break;
    //         case 3:
    //             printf("Fim do INSPETOR HTTP.\n");
    //             exit(3);
    //         default:
    //             printf("Opcao Invalida.\n");
    //     }
    //
    // }while(opcao == 1 || opcao == 2 || opcao == 3);


    fclose(html_file);
    close(new_socket); // fecha o socket do cliente

    return 0;
    // } while(1);
}

void parsing(char* buf, char *new_url, char *new_host){
    char *get = strstr(buf, "GET");
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    long int i, j = 0;
    char c = '\0';
    long int start_url = get - buf + 4;
    long int end_url = http - buf - 1;
    long int start_host = host - buf + 6;

    for(i=start_url;i<end_url;i++){
        new_url[j] = buf[i];
        j++;
    }
    j = 0;
    while(buf[start_host + j]!= '\n' && buf[start_host + j] != '\r'){
        new_host[j] = buf[start_host + j];
        j++;
    }
}

int get_host_by_name(char *new_url, char *new_host){

    struct hostent *hp;
    char request[250];
    struct sockaddr_in cliente;
    bzero(request, 250);
    int on = 1, sock;
    printf("NEW host:%s\n", new_host);
    if ((hp = gethostbyname(new_host)) == NULL){
        herror("gethostbyname");
    }
    bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
    cliente.sin_port = htons(80);
    cliente.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
        perror("Nao foi possivel conectar.");
        exit(4);
    }
    if(strcmp(new_url, new_host) == 0) {
        strcat(request, "GET / HTTP/1.1\r\nHost: ");
        strcat(request, new_host);
        strcat(request, "\r\n\r\n");
    }
    else {
        strcat(request, "GET ");
        strcat(request, new_url);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, new_host);
        strcat(request, "\r\n\r\n");
    }
    write(sock, request, strlen(request));

    return sock;
}
