#include "functions.h"

void spider(char *url, char *host) {
     printf("SPIDER ON\n");
     FILE *html_tree, *html_file;
     char *href, buf[BUFFER_SIZE], c;
     char *needle;
     char final_url[250];
     size_t href_size = 256;
     long int i = 0, j=0, flag;
     href = (char *)malloc(href_size * sizeof(char));
     bzero(href, 256);
     bzero(buf, BUFFER_SIZE);
     node *head_href = (node *)malloc(sizeof(node));
     if(!head_href){
         printf("Sem memoria disponivel!\n");
         exit(0);
     }
     head_href->prox = NULL;

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
                make_list(buf, head_href);
                printf("1-HREF = %s\n", buf);
                getchar();
            }else if(buf[0] == '/' && buf[1]!='/'){ //pega hrefs que sejam do tipo "/...."
                make_list(buf,head_href);
                printf("2-HREF = %s\n", buf);
                getchar();
            }else if(buf[0] != 'h' && buf[0]!='#'){ //pega hrefs que sejam do tipo "files/...."
                make_list(buf,head_href);
                printf("3-HREF = %s\n", buf);
                getchar();
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
    node *atual = head_href->prox, *proxNode;
    while(atual != NULL){
       proxNode = atual->prox;
       free(atual);
       atual = proxNode;
    }
    fclose(html_file);
    fclose(html_tree);
}

void make_list(char *href, struct Node *head_href){
    node *temp = head_href;
    long int flag = 1;
    FILE *html_tree;

    while(temp->prox != NULL && flag == 1){
        //printf("oi\n");
        if(strcmp(temp->prox->href, href) != 0){
            temp = temp->prox;
        }
        else{
            flag = 0; // ja teve essa ocorrencia de href na lista
        }
    }
    if(flag == 1){ // nao tem essa ocorrencia de href na lista
        node *novo_href = (node *)malloc(sizeof(node));
        novo_href->prox = NULL;
        strcpy(novo_href->href, href);
        if(head_href->prox == NULL){
            head_href->prox = novo_href;
        }
        else {
            node *temp = head_href->prox;
            while(temp->prox != NULL)
                temp = temp->prox;
            temp->prox = novo_href;
        }
        html_tree = fopen("html_tree.txt", "a");
        fputs(href, html_tree);
        fclose(html_tree);
    }
}
