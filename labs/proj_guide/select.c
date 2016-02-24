#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
 #include <errno.h>
#include <arpa/inet.h>
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

#define max(A,B) ((A)>=(B)?(A):(B))

int main(void){
	int fd, newfd, afd;
	fd_set rfds;
	enum {idle,busy} state;
	int maxfd, counter;
	
	if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)exit(1);//error

	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(9000);

	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1) exit(1);//error

	if(listen(fd,5)==-1)exit(1);//error
	
	
	state=idle;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);maxfd=fd;
		if(state==busy){
			FD_SET(afd,&rfds);
			maxfd=max(maxfd,afd);
		}
		counter=select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);

		if(counter<=0)exit(1);//errror

		if(FD_ISSET(fd,&rfds)){
			addrlen=sizeof(addr);

			if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
			switch(state)
			{
				case idle: afd=newfd; state=busy; break;
				case busy: /* ... *///write “busy\n” in newfd
				close(newfd); break;
			}
		}

		if(FD_ISSET(afd,&rfds))
		{
			if((n=read(afd,buffer,128))!=0)
			{
				if(n==-1)exit(1);//error
				/* ... */// write buffer in afd
			}
			else{close(afd); state=idle;}//connection closed by peer
		}
	}//while(1)
	 close(fd); 
	 exit(0); 
}
