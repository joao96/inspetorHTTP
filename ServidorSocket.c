#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int socket_create(int *, struct sockaddr_in *);
int config_socket(int, int *);
int bind_socket(int , struct sockaddr_in, socklen_t);
int listen_socket(int);
int accept_socket(struct sockaddr_in, int, int *);
void recv_socket();
void send_socket();

//porta
#define porta 8228

// tamanho do buffer
#define BUFFER_LENGTH 4096

int socket_create(int *actual_socket, struct sockaddr_in *servidor){
    if((*actual_socket = socket(AF_INET, SOCK_STREAM, 0))==-1){
        printf("Nao foi possivel criar o socket do servidor.\n");
        exit(-1);
    }
    printf("Socket criado com sucesso, %d.\n", *actual_socket);
    servidor->sin_family = AF_INET;
    servidor->sin_port = htons(porta);
    return 1;
}

int config_socket(int actual_socket, int *int_pointer){
    if(setsockopt(actual_socket, SOL_SOCKET, SO_REUSEADDR, &int_pointer, sizeof(int)) == -1){
        printf("Erro no socket.\n");
        exit(-1);
    }
    return 1;
}

int bind_socket(int actual_socket, struct sockaddr_in servidor, socklen_t addr_len){
    if(bind(actual_socket, (struct sockaddr*)&servidor, sizeof(servidor)) == -1 ) {
        printf("Bindou errado.\n");
        exit(-1);
    }
    return 1;
}
int listen_socket(int actual_socket){
    if(listen(actual_socket, 3) == -1) {
        printf("Escutou errado.\n");
        exit(-1);
    }
    printf("Porta: %d\n", porta);
    return 1;
}

int accept_socket(struct sockaddr_in cliente, int actual_socket, int *new_socket){
    socklen_t cliente_lenght = sizeof(cliente);
    if ((*new_socket = accept(actual_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
        printf("Erro ao aceitar o cliente.\n");
        exit (-1);
    }
    printf("Aceitou.\n");
    return 1;
}

void send_socket(int actual_socket, char *buf){
    if (send(actual_socket, buf, strlen(buf), 0)) {
        printf("Conexao com o cliente esta estabelecida.\n");
    }
    printf("Sendou.\n");
}

void recv_socket(char *buf, int new_socket){
    memset(buf, 0x0, BUFFER_LENGTH);

    /* Receives client message */
    int message_len;
    if((message_len = recv(new_socket, buf, BUFFER_LENGTH, 0)) > 0) {
        buf[message_len - 1] = '\0';
        printf("Cliente fala: %s.\n", buf);
    }

}
