#include "SocketCliente.h"
#include "SocketException.h"

SocketCliente::SocketCliente (std::string host, int porta)
{
    cliente_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(cliente_socket == -1) {
        perror("Nao foi possivel criar o socket");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Socket servidor criado: %d\n", cliente_socket);

    /* pode dar merda */
    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(PORT);
    memset(cliente.sin_zero, 0x0, 8);

  if ((setsockopt(cliente_socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) == -1) )
    {
      throw SocketException ( "Could not create client socket." );
    }

  if (::connect(cliente_socket, (struct sockaddr*) &cliente, sizeof(cliente)) == -1)
    {
      throw SocketException ( "Could not bind to port." );
    }

}
