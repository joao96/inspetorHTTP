#include "functions.h"

int BUFFER_SIZE = 4096; // tamanho do buffer
int porta = 8228;
int nivel = 0;

int main(int argc, char *argv[] ){

    FILE *html_file = NULL, *html_tree = NULL, *request_file = NULL;
    int actual_socket, new_socket; // sockets do servidor e cliente
    long int message_len;
    unsigned int addr_len;
    struct sockaddr_in servidor, cliente;
    char *buf = malloc(BUFFER_SIZE);
    char dir[150] = "\0", nano[200] = "\0";
    char new_url[150] = "\0", new_host[150] = "\0", aux_url[150] = "\0";
    int c, i, opcao = 0;
    unsigned int cliente_lenght = sizeof(cliente);
    char *href_list[HREF_LIST_SIZE];
    bzero(href_list, 4096);
    int tr=1;
    i=0;

    if(argv[1]){ // tratamento para o argumento do número da porta
        if(strcmp(argv[1], "-p") == 0)
            if(argv[2] != NULL)
                porta = atoi(argv[2]);
    }

    do{
        arvore *head_href = (arvore *)malloc(sizeof(arvore));
        initialize_node(head_href);
        bzero(buf, BUFFER_SIZE);
        bzero(new_url, 150);
        bzero(new_host, 150);
        actual_socket = socket(AF_INET, SOCK_STREAM, 0); // cria o socket do servidor
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

        printf("Digite enter para comecar a escutar a porta\n");
        getchar();

        if(listen(actual_socket, 1) == -1) { // ouve a porta
            perror("Erro ao Escutar.\n");
        }
        printf("Porta: %d\n", porta);

        // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
        if ((new_socket = accept(actual_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
            perror("Erro ao aceitar o cliente.\n");
        }
        printf("Cliente aceito.\n");

        close(actual_socket);

        if((message_len = recv(new_socket, buf, BUFFER_SIZE, 0)) > 0) {  // recebe o request
            buf[message_len - 1] = '\0';

            if(strstr(buf, "POST") != NULL) { // se o tipo do request for POST, fecha o socket e volta para o inicio
                printf("Requisicao do tipo POST.\n");
                close(new_socket);
                break;
            }

            request_file = fopen("request.txt", "w");

            if(request_file == NULL){
                printf("Arquivo de request nao foi aberto.\n");
                exit(1);
            }
            fputs(buf, request_file);
            fclose(request_file);
            system("nano request.txt"); // abre a opção para o usuário editar o request recebido
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
        }

        printf("Request: %s.\n", buf);
        parsing(buf, new_url, new_host); // realiza o parsing para retirar informações úteis do request

        // faz novo GET para o cliente
        int sock = get_host_by_name(new_url, new_host);

        bzero(buf, BUFFER_SIZE); //limpa o buffer antigo

        bzero(dir, 150);
        strcpy(aux_url, new_url);
        for(i=0;i<strlen(aux_url);i++){
            if(aux_url[i] == '/' || aux_url[i] == ':')
                aux_url[i] = '-';
        }

        strcpy(dir, aux_url);
        strcat(dir, "/index.txt");
        mkdir(aux_url, S_IRUSR | S_IWUSR | S_IXUSR);
        html_file = fopen(dir, "w");

        if(html_file == NULL){
            printf("Erro ao abrir o arquivo. MAIN 1\n");
            exit(2);
        }

        long int timeout = 0, header_size, q = 0, file_size = 0;
        char *content_length, *header, size_content[50];
        while(read(sock, buf, BUFFER_SIZE - 1) != 0){
            if( (header = strstr(buf, "\r\n")) != NULL){
                if( (content_length = strstr(buf, "Content-Length:")) != NULL){
                    timeout = content_length - buf + 15;
                    header_size = header - buf + 4;
                    bzero(size_content, 50);
                    while(buf[timeout+q] != '\r'){
                        size_content[q] = buf[timeout+q];
                        q++;
                    }
                    timeout = atoi(size_content);
                    file_size = BUFFER_SIZE - header_size;
                    timeout-=file_size;
                }
            }
            timeout-=BUFFER_SIZE;
            if(timeout == 0)
                break;
            fputs(buf, html_file);
            bzero(buf, BUFFER_SIZE);
        }

        fclose(html_file);
        strcat(nano, "nano ");
        strcat(nano, dir);
        system(nano);
        html_file = fopen(dir, "r");
        bzero(buf, BUFFER_SIZE);
        while(fread(buf, 1, BUFFER_SIZE, html_file) == BUFFER_SIZE){ // envia de volta para o browser a página html
            send(new_socket, buf, BUFFER_SIZE, 0);
        }


        do{
            printf("*************************************************************\n");
            printf("*                       Inspetor HTTP                       *\n");
            printf("*************************************************************\n");

            printf("1 - SPIDER.\n");
            printf("2 - DUMP.\n");
            printf("3 - SAIR.\n");
            printf("Digite a opcao: ");
            scanf("%d", &opcao);
            getchar();

            switch (opcao) {
                case 1:
                    strcpy(head_href->href, new_url);
                    spider(new_url, new_host, aux_url, head_href, aux_url);
                    imprime_arvore(head_href, 0);
                    break;

                case 2:
                    html_tree = fopen(dir, "a");
                    if(html_tree != NULL){
                        fseek (html_tree, 0, SEEK_END);
                        if (ftell(html_tree) == 0) {

                            printf("Arquivo vazio!\n");
                        }else{
                            dump(new_url, new_host);
                        }
                    }else{
                        printf("Nao foi possivel abrir o html tree.\n");
                    }
                    break;
                case 3:
                    printf("Fim do INSPETOR HTTP.\n");
                    exit(3);
                default:
                    printf("Opcao Invalida.\n");
            }

        }while(opcao == 1 || opcao == 2 || opcao == 3);

        FILE *fp = fopen("tree.txt", "r");
        if(fp != NULL) {
            int contador_href = 0;
            size_t href_size = 256;
            char *aux_href = (char *)malloc(href_size * sizeof(char));
            while(getline(&aux_href, &href_size, fp) != -1) {
                    contador_href++;
            }
            fclose(fp);
            zera_arvore(head_href, contador_href - 2);
            remove("tree.txt");
            remove("tree_tab.txt");
        }
        fclose(html_file);
        close(new_socket); // fecha o socket do cliente

    }while(1);

    return 0;
}

// irá retirar o url e o host do request
void parsing(char* buf, char *new_url, char *new_host){
    char *get = strstr(buf, "GET");
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    long int i, j = 0;
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
// usará o IP do host e criará um novo request HTTP
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

// percorre a estrura de dados de árvore para imprimir a árvore hipertextual de hrefs da página
void imprime_arvore(struct Arvore *node_href, int n_tab){
    int i, j, control = 0;
    char buf[500], buf_tab[500];
    bzero(buf, 500);
    bzero(buf_tab, 500);
    int flag = 0;
    if(node_href != NULL){
        for(i=0;i<N;i++){
            FILE *fp = fopen("tree.txt", "a");
            FILE *fp_notab = fopen("tree_tab.txt", "a");
            for(j = control;j < n_tab;j++){
                printf("\t");
                strcat(buf, "\t");
            }
            if(node_href != NULL){
                if(flag == 0){
                    printf("%s\n", node_href->href);
                    flag = 1;
                    strcat(buf, node_href->href);
                    strcat(buf_tab, node_href->href);
                    if(strstr(buf, "\r\n") == NULL)
                        strcat(buf, "\r\n");
                    //strcat(buf, "\n");
                    fprintf(fp,"%s", buf);
                    fprintf(fp_notab,"%s", buf_tab);
                    fclose(fp);
                    fclose(fp_notab);
                }
                bzero(buf, 300);
                if(node_href->filhos[i] != NULL){
                    imprime_arvore(node_href->filhos[i], n_tab+1);
                    if(flag == 1)
                        control = n_tab;
                        j = control;
                }
                else{
                    i = N;
                    j = n_tab;
                }
            }
            else{
                i = N;
                j = n_tab;
            }
        }
    }


}

// uma vez fechada a conexão com esse cliente, apaga-se o conteúdo da árvore hipertextual
void zera_arvore(struct Arvore *head_href, int contador_href){
    int i, contador = 0;
    arvore *temp = head_href;
    while (strcmp(temp->href, "\0") != 0) {
        for(i=0;i<N;i++){
            if(temp->filhos[i] == NULL)
                contador++;
            else{
                zera_arvore(temp->filhos[i], contador_href);
                contador++;
            }
        }
        if(contador == contador_href || contador == N)
            bzero(temp->href, 256);

    }
}
