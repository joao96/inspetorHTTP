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

extern int porta;
#define N 20
extern int BUFFER_SIZE;
#define TRUE 1
#define FALSE 0

void spider(char *, char *, char *);
void parsing(char*, char*, char*);
int get_host_by_name(char*, char*);
void dump(char*, char*);

struct Arvore{
 char href[256];
 struct Arvore *filhos[N];
};
typedef struct Arvore arvore;

void make_tree(char*, struct Arvore *, char *, char *, char *);
void initialize_node(struct Arvore *);
int walk_tree(char*, struct Arvore *);
