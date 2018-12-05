#include "functions.h"

void spider(char *url, char *host) {
    printf("SPIDER ON\n");
    FILE *html_tree, *html_file;
    char *href, buf[BUFFER_SIZE], c;
    char *needle;
    char final_url[250];
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

    html_file = fopen("html_file.txt", "r");
    html_tree = fopen("html_tree.txt", "a");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo. 1\n");
        exit(1);
    }

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo. 2\n");
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
                make_tree(buf, head_href);
                printf("1-HREF = %s\n", buf);
                //getchar();
            }else if(buf[0] == '/' && buf[1]!='/'){ //pega hrefs que sejam do tipo "/...."
                make_tree(buf, head_href);
                printf("2-HREF = %s\n", buf);
                //getchar();
            }else if(buf[0] != 'h' && buf[0]!='#'){ //pega hrefs que sejam do tipo "files/...."
                make_tree(buf, head_href);
                printf("3-HREF = %s\n", buf);
                //getchar();
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

void make_tree(char *href, struct Arvore *head_href){
    FILE *html_tree;
    arvore *temp = head_href;
    int i;
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
        html_tree = fopen("html_tree.txt", "a");
        fputs(href, html_tree);
        fclose(html_tree);
    }
}

void initialize_node(struct Arvore *head_href){
    int i;
    for(i=0;i<N;i++)
        head_href->filhos[i] = NULL;
}
