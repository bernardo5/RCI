//test1.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
	struct hostent *h;
	struct in_addr *a;
	
	if((h=gethostbyname("bernardo"))==NULL)exit(1);//error
	
	printf("official host name:%s\n", h->h_name);
		
	a=(struct in_addr*)h->h_addr_list[0];
	
	printf("internet address: %s (%081X)\n", inet_ntoa(*a), (long unsigned int)ntohl(a->s_addr));
	
	exit(0);
}
