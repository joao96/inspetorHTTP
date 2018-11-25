#include "SocketServidor.h"
#include "SocketException.h"
#include <string>
using namespace std;
int main ( int argc, char *argv[] )
{
  //std::cout << "running....\n";

  try
    {
      // Create the socket
      SocketServidor server ( 8228 );

      while ( true )
  {

    SocketServidor new_sock;
    server.aceita ( new_sock );

    try
      {
        while ( true )
    {
    int slen;
      char buf [4096];
      std::string data;
      new_sock = ::send(new_sock.servidor_socket, data.c_str(), data.size(), 0);
      slen = ::recv(new_sock.servidor_socket, buf, 4096, 0);
      buf[slen - 1] = '\0';
      printf("Client says: %s\n", buf);
    }
      }
    catch ( SocketException& ) {}

  }
    }
  catch ( SocketException& e )
    {
      //std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

  return 0;
}
