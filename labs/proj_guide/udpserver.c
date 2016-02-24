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
#include <string.h>

int main(){
	int fd, n, addrlen, ret, nread;
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	char buffer[128];
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(9000);
	
	ret=bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret==-1)exit(1);//error
	
	while(1){
			addrlen=sizeof(addr);
			nread=recvfrom(fd, buffer,128,0,(struct sockaddr*)&addr, &addrlen);
			if(nread==-1)exit(1);//error
			ret=sendto(fd, buffer, nread,0,(struct sockaddr*)&addr, addrlen);
			if(ret==-1)exit(1);
	}
	close(fd);	
	exit(0);
}

