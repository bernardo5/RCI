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

#define max(A,B) ((A)>=(B)?(A):(B))

void empty_buffer(char**buff){
	*buff[0] = '\0';
	return;
}

void create_surname_register_buffer(char**buff, char**argv){
	sprintf(*buff, "%s%s%s%s%s%s", "SREG ", argv[2], ";", argv[4], ";", argv[6]);
	return;
}

void destroy_surname_buffer(char**buff, char**argv){
	sprintf(*buff, "%s %s", "SUNR ", argv[2]);
	return;
}

void registe(char**buff, char**argv, int fd, struct sockaddr_in addr, char*place){
	int n;
	socklen_t addrlen;
	
	if(strcmp(place, "register")==0){
		create_surname_register_buffer(&(*buff), argv);
	}else{
		destroy_surname_buffer(&(*buff), argv);
	}
	addrlen=sizeof(addr);
	n=sendto(fd, *buff, 128, 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) exit(1);//error
	
	/*receive echo part*/
	empty_buffer(&(*buff));
	addrlen=sizeof(addr);
	n=recvfrom(fd, *buff, 128,0, (struct sockaddr*)&addr, &addrlen);
	if(n==-1) exit(1);//error
	printf("answer to echo\n");
	write(1, "echo: ",6);//stdout
	write(1, *buff, n);
	printf("\n");
	return;
}

int main(int argc, char**argv){
	int server_specified=0;
	
	/*check if program was called correctly for mandatory parameters*/
	if(argc<7){
		printf("too few arguments\n\n");
		printf("Please use correct call: snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
		exit(-1);
	}
	
	if(!((strcmp(argv[1], "-n")==0)&&(strcmp(argv[3], "-s")==0)&&(strcmp(argv[5], "-q")==0))){
		printf("Incorrect use of program call\n");
		printf("Please use correct call: snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
		exit(-1);
	}	
	/* ************************* */
	
	/* check opcional parameters */

	if(argc==11){
		if(!((strcmp(argv[7], "-i")==0)&&(strcmp(argv[9], "-p")==0))){
			printf("Incorrect use of program call, will use predefined parameters\n");
			server_specified=0;
		}else server_specified=1;
	}	
	/* ************************* */
	
	
	int port, fd, afd, newfd, ret;
	struct sockaddr_in addr, addr_server;
	struct hostent *h;
	struct in_addr *a;
	char *buffer=malloc(128*sizeof(char));
	fd_set rfds;
	enum {idle,busy} state;
	int maxfd, counter;
	int n, nw;
	char *ptr;
	/*empty_buffer(&buffer);*/
	
	if(server_specified==1){
		a=(struct in_addr*)argv[8];
		port=atoi(argv[10]);
	}else{
		/*get host IP*/
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL)exit(1);//error
		a=(struct in_addr*)h->h_addr_list[0];
		port=58000;
	}
	
	/*send part*/
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(port);
	
	memset((void*)&addr_server, (int)'\0', sizeof(addr_server));
	addr_server.sin_family=AF_INET;
	addr_server.sin_addr.s_addr=htonl(INADDR_ANY);
	addr_server.sin_port=htons(9000);
	
	registe(&buffer, argv, fd, addr, "register");
	
	ret=bind(fd, (struct sockaddr*)&addr_server, sizeof(addr_server));
	if(ret==-1)exit(1);//error
	
	state=idle;
	
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);maxfd=fd;
		FD_SET(STDIN_FILENO, &rfds);
		
		if(state==busy){
			FD_SET(afd,&rfds);
			maxfd=max(maxfd,afd);
		}
		counter=select(maxfd + STDIN_FILENO + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
//-------------------------------------------------------------------------------------------------------------------AQUI
		if(counter<=0)exit(1);//errror

		if(FD_ISSET(fd,&rfds)){
			addrlen=sizeof(addr);

			if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
			switch(state)
			{
				case idle: afd=newfd; state=busy; break;
				case busy: strcpy(buffer,"busy\n");ptr=&buffer[0]; if(write(newfd,ptr,n)<=0)exit(1);//error
				close(newfd); break;
			}
		}

		if(FD_ISSET(afd,&rfds))
		{
			if((n=read(afd,buffer,128))!=0)
			{
				if(n==-1)exit(1);//error
				ptr=&buffer[0];
				if(write(afd,ptr,n)<=0)exit(1);
			}
			else{close(afd); state=idle;}//connection closed by peer
		}
		
	}
	
	registe(&buffer, argv, fd, addr, "exit");
	close(fd);
	exit(0);
}
