#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

class SocketCliente{
public:
    SocketCliente(std::string host, int porta);
    int cliente_socket;

private:
    struct cliente;
};
