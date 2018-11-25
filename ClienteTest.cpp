#include "SocketCliente.h"
#include "SocketException.h"
#include <iostream>
#include <string>
using namespace std;
int main ( int argc, int argv[] )
{
  try
    {

      SocketCliente client_socket ( "localhost", 8228);

      std::string reply;

      try
      {
          int slen;
          char buf [4096];
          std::string data = "This message";
          client_socket = ::send(client_socket.cliente_socket, data.c_str(), data.size(), MSG_NOSIGNAL);
          slen = ::recv(client_socket.cliente_socket, buf, MAXRECV, 0);
          buf[slen +  1] = "\0";
          std::out << "recebemos isso : \n\ ""<<buf<<"\"\n";;

      }

      catch ( SocketException& ) {}

      std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;

    }
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\n";
    }

  return 0;
}
