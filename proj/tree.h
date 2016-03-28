#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/time.h>


typedef struct _user{
	char name[30];
	char ip[15];
	int scport;
	struct _user*left;
	struct _user*right;
}user;

int find_user(user*root, char*name, char**buf, char*surname, int qry);

void AddUser(user**root, char*name, char*ip, int scport, char**buf);

void DeleteUser(user**root, char*name, char**buf);

int min(int a, int b);

void put_to_null(user**root, char*name);
