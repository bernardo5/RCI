#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define max(A,B) ((A)>=(B)?(A):(B))


int check_dot(char*names){
	char*pointer_to_dot=strchr(names, '.');
	if(pointer_to_dot==names){
		printf("ponto no inicio\n");
		return 0;
	}else if(pointer_to_dot==NULL){
		printf("nao ha ponto\n");
			return 0;
		}else if(names[strlen(names)-1]=='.'){
			printf("ponto no final\n");
			return 0;
		}else return 1;
}

void check_args(int argc, char**argv){
	/*check if program was called correctly for mandatory parameters*/
	if(argc<11){
		printf("too few arguments\n\n");
		printf("Please use correct call: schat -n name.surname -i ip -p scport -s snpip -q snpport\n");
		exit(-1);
	}
	if(!((strcmp(argv[1], "-n")==0)&&(strcmp(argv[3], "-i")==0)&&(strcmp(argv[5], "-p")==0)&&(strcmp(argv[7], "-s")==0)&&(strcmp(argv[9], "-q")==0))){
		printf("Incorrect use of program call\n");
		printf("Please use correct call: schat -n name.surname -i ip -p scport -s snpip -q snpport\n");
		exit(-1);
	}else{ /* - parameters are ok */
		if(!check_dot(argv[2])){
			 printf("Please use correct call: schat -n name.surname -i ip -p scport -s snpip -q snpport\nSpecify name and a surname\n");
			exit(-1);
		}
	}	
	/* ************************* */
	return;
}

void join(char**buf, char**argv){
	sprintf(*buf, "%s %s%s%s%s%s\n","REG", argv[2], ";", argv[4], ";", argv[6]);
	/*strcpy(*buf, "REG ");
	strcat(*buf, argv[2]);
	strcat(*buf, ";");
	strcat(*buf, argv[4]);
	strcat(*buf, ";");
	strcat(*buf, argv[6]);*/	
	return;
}

void leave(char**buf, char**argv){
	sprintf(*buf, "%s %s\n","UNR", argv[2]);	
	return;
}

void find(char**buf, char*names){
	sprintf(*buf, "%s %s\n","QRY", names);	
	return;
}


int main(int argc, char**argv)
{
	check_args(argc, argv);
	
	/*udp socket*/
	
	int fd_udp, n_udp, leav=1; 
	socklen_t addrlen_udp;
	struct sockaddr_in addr_udp;
	char* buffer_udp=malloc(128*sizeof(char));
	
	
	fd_udp=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd_udp==-1) exit(-1);/*error*/
	
	memset((void*)&addr_udp, (int)'\0', sizeof(addr_udp));
	addr_udp.sin_family=AF_INET;
	inet_aton(argv[8], & addr_udp.sin_addr);
	addr_udp.sin_port=htons(atoi(argv[10]));
	
	
	
	/*tcp socket and select*/
	int fd, newfd, afd;
	socklen_t addrlen;
	fd_set rfds;
	enum {idle, busy} state;
	int maxfd,counter;
	struct sockaddr_in addr;
	int n, nw;
	char buffer[128];
	
	char keyboard[45];
	char command[15], key[15];
	char*names=malloc(15*sizeof(char));
	

	if((fd=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error in socket\n");exit(1);}//error

	memset((void*)&addr,(int)'\0',sizeof(addr));
	

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(atoi(argv[6]));

	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){
		printf("bind\n");
		perror("socket error");
		exit(1);//error
	}
	if(listen(fd,5)==-1){printf("listen\n");exit(1);}//error
	state=idle;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);maxfd=fd;
		FD_SET(fileno(stdin), &rfds);
		if(state==busy){FD_SET(afd,&rfds);maxfd=max(maxfd,afd);}
		
		counter=select(fileno(stdin)+maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(counter<=0)exit(1);//errror
		
		if(FD_ISSET(fileno(stdin), &rfds)){
			fgets(keyboard, 45, stdin);
			/****************************************/
			if(sscanf(keyboard, "%s", command)==1){
				if(strcmp(command, "join")==0){
					join(&buffer_udp, argv);
					n_udp=sendto(fd_udp, buffer_udp, strlen(buffer_udp), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
					if(n_udp==-1) exit(1);//error
					/*receive echo part*/
					addrlen_udp=sizeof(addr_udp);				
					printf("going to rcvfrom\n");
					/***************************************************************/
					
					n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &addrlen_udp);
					if(n_udp==-1) exit(1);//error
					printf("answer to echo\n");
					write(1, "echo: ",6);//stdout
					buffer_udp[n_udp]='\0';
					printf("%s\n", buffer_udp);
					leav=0;
					
		/*************************************************************/
					
					}else if(strcmp(command, "find")==0){
						if(sscanf(keyboard, "%s %s", command, names)!=2){
							printf("not enough arguments\n");
						}else{
							if(check_dot(names)){
								printf("name and surname: %s\n", names);
								find(&buffer_udp, names);
								
								n_udp=sendto(fd_udp, buffer_udp, strlen(buffer_udp), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
								if(n_udp==-1) exit(1);//error
								
								/*receive echo part*/
								
								addrlen_udp=sizeof(addr_udp);
								printf("going to rcvfrom\n");
								n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &addrlen_udp);
								if(n_udp==-1) exit(1);//error
								printf("answer to echo\n");
								write(1, "echo: ",6);//stdout
								buffer_udp[n_udp]='\0';
								printf("%s\n", buffer_udp);
							}
						}
						
					}else if(strcmp(command, "leave")==0){
						leave(&buffer_udp, argv);
						leav=1;
						n_udp=sendto(fd_udp, buffer_udp, strlen(buffer_udp), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
						if(n_udp==-1) exit(1);//error
						
						/*receive echo part*/
						
						addrlen_udp=sizeof(addr_udp);
						printf("going to rcvfrom\n");
						n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &addrlen_udp);
						if(n_udp==-1) exit(1);//error
						printf("answer to echo\n");
						write(1, "echo: ",6);//stdout
						buffer_udp[n_udp]='\0';
						printf("%s\n", buffer_udp);
						
					}else if(strcmp(command, "exit")==0){
						if(!leav){
							leave(&buffer_udp, argv);
							n_udp=sendto(fd_udp, buffer_udp, strlen(buffer_udp), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
							if(n_udp==-1) exit(1);//error
							
							/*receive echo part*/
							
							addrlen_udp=sizeof(addr_udp);
							printf("going to rcvfrom\n");
							n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &addrlen_udp);
							if(n_udp==-1) exit(1);//error
							printf("answer to echo\n");
							write(1, "echo: ",6);//stdout
							buffer_udp[n_udp]='\0';
							printf("%s\n", buffer_udp);
						}				
							close(fd_udp);
							exit(0);
							//}else printf("please leave before exit\n");
					}
			}
			/****************************************/
		}
		
		if(FD_ISSET(fd,&rfds)){
			addrlen=sizeof(addr);
			if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
			switch(state)
			{
				case idle: afd=newfd; state=busy; break;
				case busy: if((nw=write(newfd,"BUSY\n",n))<=0)exit(1);//error
				close(newfd); break;
			}
		}
		if(FD_ISSET(afd,&rfds))
		{
			if((n=read(afd,buffer,128))!=0)
			{if(n==-1)exit(1);//error
				if((nw=write(afd,buffer,n))<=0)exit(1);//error
			}
			else{close(afd); state=idle;}//connection closed by peer
		}
	}//while(1)
	 close(fd);
	  exit(0);
}		
