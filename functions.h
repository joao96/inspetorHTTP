#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/stat.h>

#define porta 8228 //porta

extern int BUFFER_SIZE;

void spider(char *);
void parsing(char*, char*, char*);
int get_host_by_name(char*, char*);
void dump(char*);
FILE *create_html_txt(char*);
void remove_portion(char*, int, int);

struct Node{
 char href[256];
 struct Node *prox;
};
typedef struct Node node;
