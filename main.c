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
int BUFFER_SIZE = 4096;

int main(){

    int actual_socket, new_socket;
    int int_pointer = 1;
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;
    char *buf = malloc(BUFFER_SIZE);
    unsigned int addr_len;
    socket_create(&actual_socket, &servidor);
    config_socket(actual_socket, &int_pointer);

    addr_len = sizeof(struct sockaddr_in);

    bind_socket(actual_socket, servidor, addr_len);

    do {
        listen_socket(actual_socket);

        accept_socket(cliente, actual_socket, &new_socket);

        recv_socket(buf, new_socket);

        close(new_socket);

    } while(1);

}
