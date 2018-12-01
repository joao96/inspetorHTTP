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
    int int_pointer = 1;
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;
    char buf[] = "Hello World";
    unsigned int addr_len;
    socket_create(&actual_socket, &servidor);
    //config_socket(actual_socket, &int_pointer);

    addr_len = sizeof(struct sockaddr_in);

    bind_socket(actual_socket, servidor, addr_len);

    do {
        //listen_socket(actual_socket);

        if(listen(actual_socket, 3) == -1) {
            printf("Escutou errado\n");
            exit(-1);
        }
        printf("Porta: %d\n", porta);

        accept_socket(cliente, actual_socket, &new_socket);

        send_socket(new_socket, buf);

        close(new_socket);
    } while(1);

}
