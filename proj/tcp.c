#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(){
	int fd, nbytes, nleft, nwritten, nread, n;
	char*ptr, buffer[128];
	struct sockaddr_in addr;
	int addrlen;
	
	fd=socket(AF_INET, SOCK_STREAM,0);
	if(fd==-1){printf("error opening socket\n");exit(-1);}

	memset((void*)&addr, (int)'\0', sizeof(addr));

	addr.sin_family=AF_INET;
	inet_aton("192.168.1.78", &addr.sin_addr);
	addr.sin_port=htons(58000);
	
	n=connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n==-1){printf("erro no connect\n"); exit(1);}

	ptr=strcpy(buffer,"Hello!\n");
	nbytes=7;

	nleft=nbytes;
	while(nleft>0){nwritten=write(fd,ptr,nleft);
		if(nwritten<=0)exit(1);//error
		nleft-=nwritten;
		ptr+=nwritten;}
	nleft=nbytes; ptr=buffer;
	while(nleft>0){nread=read(fd,ptr,nleft);
		if(nread==-1)exit(1);//error
		else if(nread==0)break;//closed by peer
		nleft-=nread;
		ptr+=nread;}
	nread=nbytes-nleft;
	close(fd);
	write(1,"echo: ",6);//stdout
	write(1,buffer,nread);
	

	exit(0);
}
