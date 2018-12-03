#include "functions.h"

void spider(char *host) {
     FILE *html_tree, *html_file;
     char *href, buf[BUFFER_SIZE], c;
     char *needle;
     size_t href_size = 256;
     long int i = 0, j=0;
     href = (char *)malloc(href_size * sizeof(char));
     bzero(href, 256);
     bzero(buf, BUFFER_SIZE);

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
    while(getline(&href, &href_size, html_file) != -1) {
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
            bzero(buf, BUFFER_SIZE);
            j = 0;
        }
    }
    fclose(html_file);
    fclose(html_tree);
}
