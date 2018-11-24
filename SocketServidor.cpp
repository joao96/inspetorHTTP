#include "servidor.h"

SocketServidor::SocketServidor(int porta)
{
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(servidor_socket == -1) {
       perror("Nao foi possivel criar o socket");
       return EXIT_FAILURE;
    }
    fprintf(stdout, "Socket servidor criado: %d\n", servidor_socket);

    /* pode dar merda */
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORT);
    memset(servidor.sin_zero, 0x0, 8);

    //pode dar ruim
    if (setsockopt(servidor_socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) == -1)
    {
      throw SocketException ( "Nao foi possivel criar o servidor." );
    }

  if (::bind(servidor_socket, (struct sockaddr*)&servidor, sizeof(servidor)) == -1 )
    {
      throw SocketException ( "Nao foi possivel conectar a porta." );
    }

  if ((::listen(servidor_socket, 1) == -1))
    {
      throw SocketException ( "Impossivel ouvir o socket." );
    }

}

SocketServidor::aceita(SocketServidor& socket){
    if ((clientfd=accept(servidor_socket,(struct sockaddr *) &servidor, &addr_len )) == -1) {perror("Accept error:");
       return EXIT_FAILURE;
   }

}
