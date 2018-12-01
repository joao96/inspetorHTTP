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


//porta
#define porta 8228


int main(){

    // sockets do servidor e cliente
    int actual_socket, new_socket;
    // tamanho do buffer
    int BUFFER_SIZE = 4096;
    int int_pointer = 1;
    int message_len;
    struct sockaddr_in servidor, cliente;
    char *buf = malloc(BUFFER_SIZE);
    unsigned int addr_len;

    // cria o socket do servidor
    actual_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(actual_socket < 0){
        perror("Nao foi possivel criar o socket do servidor.\n");
    }
    printf("Socket criado com sucesso, %d.\n", actual_socket);
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    // conecta-se a porta 8228
    servidor.sin_port = htons(porta);

    addr_len = sizeof(struct sockaddr_in);

    // conecta o servidor ao socket criado
    if(bind(actual_socket, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Bindou errado.\n");
    }

    do {
      // ouve a porta
      if(listen(actual_socket, 1) == -1) {
        perror("Escutou errado.\n");
      }
      printf("Porta: %d\n", porta);

      int cliente_lenght = sizeof(cliente);
      // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
      if ((new_socket = accept(actual_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
          perror("Erro ao aceitar o cliente.\n");
      }
      printf("Aceitou.\n");
      // recebe o que o cliente quer transmitir
      if((message_len = recv(new_socket, buf, BUFFER_LENGTH, 0)) > 0) {
          buf[message_len - 1] = '\0';
          printf("Cliente fala: %s.\n", buf);
      }
      // fecha o socket do cliente
      close(new_socket);

    } while(1);

}
