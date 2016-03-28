#include "schat_func.h"

int main(int argc, char**argv)
{
	check_args(argc, argv); /*checks if the correct sintax was used in the command line*/
	/*udp socket*/
	int fd_udp, leav=1; 
	socklen_t addrlen_udp;
	struct sockaddr_in addr_udp;
	char* buffer_udp=malloc(128*sizeof(char));
	
	
	fd_udp=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd_udp==-1){
	 exit(-1);/*error*/
	}
	
	memset((void*)&addr_udp, (int)'\0', sizeof(addr_udp));
	addr_udp.sin_family=AF_INET;
	inet_aton(argv[8], & addr_udp.sin_addr);
	addr_udp.sin_port=htons(atoi(argv[10]));
	
	
	
	/*tcp socket and select*/
	int fd, newfd, afd, fd_client;
	socklen_t addrlen, addrlen_client;
	fd_set rfds;
	STATE state;
	int maxfd,counter;
	struct sockaddr_in addr, addr_client;
	int n, nw;
	char *buffer=malloc(128*sizeof(char));
	
	char *keyboard=malloc(45*sizeof(char));
	char *command=malloc(15*sizeof(char));
	char*names=malloc(15*sizeof(char));
	
	void (*old_handler)(int);//interupt handler
	if((old_handler=signal(SIGPIPE, SIG_IGN))==SIG_ERR)exit(1);

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
		
		counter=select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(counter<=0)exit(1);//errror
		
		if(FD_ISSET(fileno(stdin), &rfds)){ /*checks for keyboard input*/
			fgets(keyboard, 45, stdin);
			if(sscanf(keyboard, "%s", command)==1){
				if(strcmp(command, "join")==0){
					join(argv, fd_udp, &leav, addr_udp, &addrlen_udp);
					
					}else if(strcmp(command, "find")==0){
						find(&buffer_udp, keyboard, fd_udp, addr_udp, &addrlen_udp);
						
					}else if(strcmp(command, "leave")==0){
						leave(argv, &leav, fd_udp, addr_udp, &addrlen_udp );						
					}else if((strcmp(command, "connect")==0)){
						connect_(argv,	&afd, fd_client,& addr_client,
							 &addrlen_client, &state, keyboard,fd_udp, addr_udp, &addrlen_udp);
					}else if((strcmp(command, "message")==0)){
						message(keyboard, state, & fd_client, afd);
					
					}else if((strcmp(command, "disconnect")==0)){
						if(state==busy){
							printf("wants to disconnect\n");
							disconnect(&afd, &state);
						}
					}else if(strcmp(command, "exit")==0){
						if(!leav){
							leave(argv, &leav, fd_udp, addr_udp, &addrlen_udp);
						}			
							if(state==busy) disconnect(&afd, &state);
							close(fd_udp);
							free_strings(&buffer, &keyboard, &command, &names);
							exit(0);
							//}else printf("please leave before exit\n");
					}
			}
			/****************************************/
		}
		
		if(FD_ISSET(fd,&rfds)){ /*checks if someone is trying to connect*/
			addrlen=sizeof(addr);
			if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
			switch(state)
			{
				case idle: afd=newfd; state=busy; break;
				case busy: if((nw=write(newfd,"BUSY\n",n))<=0)exit(1);//error
				close(newfd); break;
			}
		}
		if(state==busy){	
			if(FD_ISSET(afd,&rfds)) /*checks if has a new message from a connected peers*/
			{
				if((n=read(afd,buffer,128))!=0)
				{if(n==-1)exit(1);//error
					printf("message received: %s\n", buffer);//error
					sscanf(buffer, "%s %s", command, names);
					if(strcmp(command, "NAME")==0){
						printf("Attempt to connect by %s\n", names);
						srand(time(NULL));
						sprintf(buffer,"%s%s", names, ".txt");
						if(send_challenge(rand()%256, afd, n, buffer)){
							disconnect(&afd, &state);
						}else{
							bzero(command, strlen(command));
							if((n=read(afd,buffer,128))!=0){
								if(n==-1)exit(1);
								if(n==0){close(afd); state=idle;}else{
									unsigned char b;
									sscanf(buffer, "%s %c", command ,&b);
									sprintf(buffer, "%s%s", argv[2], ".txt");
									if(get_answer_file(afd, (int)b, buffer)){
										disconnect(&afd, &state);
									}
								}
							}
						}
						
					}
					
				}
				else{disconnect(&afd, &state);}//connection closed by peer
			}
		}
	}//while(1)
	 close(fd);
	  exit(0);
}		
