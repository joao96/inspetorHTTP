#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

//Interface da classe Servidor

class SocketServidor{
public:

    SocketServidor(int porta);

    void aceita (SocketServidor&);

private:
    struct servidor;
    socklen_t addr_len;

}
