#include "functions.h"

void spider(char *url, char *host, char *dir) {
    printf("SPIDER ON\n");
    FILE *html_tree, *html_file;
    char *href, buf[BUFFER_SIZE], c;
    char *needle;
    char final_url[250], href_tree[150], href_dir[150];
    size_t href_size = 256;
    long int i = 0, j=0;
    href = (char *)malloc(href_size * sizeof(char));
    bzero(href, 256);
    bzero(buf, BUFFER_SIZE);
    arvore *head_href = (arvore *)malloc(sizeof(arvore));
    if(!head_href){
        printf("Sem memoria disponivel!\n");
        exit(0);
    }
    initialize_node(head_href);

    strcpy(href_dir, dir);
    strcat(href_dir, "/index.txt");
    html_file = fopen(href_dir, "r");
    strcpy(href_tree, dir);
    strcat(href_tree, "/html_tree.txt");
    html_tree = fopen(href_tree, "a");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo. SPIDER\n");
        exit(1);
    }

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo. SPIDER\n");
        exit(2);
    }

    if(strcmp(url, host) == 0)
        strcpy(final_url, host);
    else
        strcpy(final_url, url);

    while(getline(&href, &href_size, html_file) != -1) {
        if((needle = strstr(href, "href=")) != NULL){
            i = needle - href + 6;
            while((c = href[i])!= '"'){
                buf[j] = c;
                i++;
                j++;
            }
            strcat(buf, "\r\n");
            if(strstr(buf, final_url) != NULL){
                make_tree(buf, head_href, href_tree, host, dir);
            }else if(buf[0] == '/' && buf[1]!='/'){ //pega hrefs que sejam do tipo "/...."
                make_tree(buf, head_href, href_tree, host, dir);
            }else if(buf[0] != 'h' && buf[0]!='#'){ //pega hrefs que sejam do tipo "files/...."
                make_tree(buf, head_href, href_tree, host, dir);
            }
            bzero(buf, BUFFER_SIZE);
            j = 0;
        }//else if((needle = strstr(href, "src=")) != NULL){
        //     i = needle - href + 5;
        //     while((c = href[i])!= '"'){
        //         buf[j] = c;
        //         i++;
        //         j++;
        //     }
        //     strcat(buf, "\r\n");
        //}
    }
    // arvore *atual = head_href->prox, *proxNode;
    // while(atual != NULL){
    //    proxNode = atual->prox;
    //    free(atual);
    //    atual = proxNode;
    // }
    fclose(html_file);
    fclose(html_tree);
}

int walk_tree(char *href, struct Arvore *head_href){ // checa a existencia do href na arvore
    int i;
    arvore *temp = head_href;
    for (i=0; i<N; i++){
        if(temp->filhos[i] != NULL){
            if(strcmp(temp->filhos[i]->href, href) != 0){
                walk_tree(href, temp->filhos[i]);
            }
            else
                return TRUE;
        }
    }
    return FALSE;
}

void make_tree(char *href, struct Arvore *head_href, char *href_tree, char*host, char*dir){
    struct hostent *hp;
    struct sockaddr_in cliente;
    int on = 1, sock, i;
    char buf[BUFFER_SIZE];
    char request[500];
    char new_dir[150];
    FILE *html_tree;
    arvore *temp = head_href;
    if(walk_tree(href, head_href) == FALSE){ // nao tem essa ocorrencia de href na arvore
        arvore *novo_href = (arvore *)malloc(sizeof(arvore));
        initialize_node(novo_href);
        strcpy(novo_href->href, href);
        for(i=0;i<N;i++){
            if(temp->filhos[i] == NULL){
                temp->filhos[i] = novo_href;
                i = N;
            }
        }
        html_tree = fopen(href_tree, "a");
        if(html_tree == NULL){
            printf("Erro ao abrir o arquivo. SPIDER make_tree.\n");
            exit(1);
        }
        fputs(href, html_tree);
        fclose(html_tree);
        if ((hp = gethostbyname(host)) == NULL){
            herror("gethostbyname");
        }
        bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
        cliente.sin_port = htons(80);
        cliente.sin_family = AF_INET;
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
            perror("nao foi possivel conectar. SPIDER");
            exit(4);
        }
        strcpy(request, "GET ");
        strcat(request, href);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\n\r\n");
        write(sock, request, strlen(request));
        strcpy(new_dir, "/");
        strcat(new_dir, dir);
        strcat(new_dir, "/");
        strcat(new_dir, href);
        mkdir(new_dir, S_IRUSR | S_IWUSR | S_IXUSR);
        FILE *file = fopen(new_dir, "w");
        if(file != NULL){
            while(read(sock, buf, BUFFER_SIZE-1) != 0){
                fwrite(buf, 1, sizeof(buf), file);
            }
        }
        fclose(file);
        spider(href, host, new_dir);
    }
}

void initialize_node(struct Arvore *head_href){
    int i;
    for(i=0;i<N;i++)
        head_href->filhos[i] = NULL;
}
