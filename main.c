#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ServidorSocket.c"

int actual_socket = 0;
int new_socket = 0;

//porta
#define porta 8228

// tamanho do buffer
#define BUFFER_LENGTH 4096

char buf[BUFFER_LENGTH];

int main(){

    int actual_socket, new_socket;
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;
    char buf[] = "Hello World";
    unsigned int addr_len;
    socket_create(&actual_socket, servidor);
    //config_socket(actual_socket);
    addr_len = sizeof(struct sockaddr_in);
    printf("actual socket = %d\n", actual_socket);
    int int_pointer = 1;

    if(setsockopt(actual_socket, SOL_SOCKET, SO_REUSEADDR, &int_pointer, sizeof(int)) == -1){
        printf("Erro no socket\n");
        exit(-1);
    }

    if(bind(actual_socket, (struct sockaddr*)&servidor, sizeof(servidor)) == -1 ) {
        printf("Bindou errado\n");
        exit(-1);
    }

    //bind_socket(actual_socket, servidor, addr_len);

    listen_socket(actual_socket);

    do {
        accept_socket(cliente, actual_socket, &new_socket);

        send_socket(new_socket, buf);

        close(new_socket);
    } while(1);

}
