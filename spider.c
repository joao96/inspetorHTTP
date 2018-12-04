#include "functions.h"

void spider(char *url, char *host) {
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
                node *temp = head_href;
                flag = 1;
                while(temp->prox != NULL && flag == 1){
                    printf("oi\n");
                    if(strcmp(temp->prox->href, buf) != 0){
                        temp = temp->prox;
                    }
                    else{
                        flag = 0; // ja teve essa ocorrencia de href na lista
                    }
                }
                if(flag == 1){ // nao tem essa ocorrencia de href na lista
                    node *novo_href = (node *)malloc(sizeof(node));
                    novo_href->prox = NULL;
                    strcpy(novo_href->href, buf);
                    if(head_href->prox == NULL){
                        head_href->prox = novo_href;
                    }
                    else {
                        node *temp = head_href->prox;
                        while(temp->prox != NULL)
                            temp = temp->prox;
                        temp->prox = novo_href;
                    }
                    fputs(buf, html_tree);
                }
            }
            bzero(buf, BUFFER_SIZE);
            j = 0;
        }
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
