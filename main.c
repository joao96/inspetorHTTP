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

    do {

        recv_socket(buf, &new_socket);

        /* 'bye' message finishes the connection */
        if(strcmp(buf, "bye") == 0) {
            send_socket(new_socket, buf);
        } else {
            send_socket(new_socket, buf);
        }

    } while(strcmp(buf, "bye"));

    close(actual_socket);
    close(new_socket);
}
