#include "schat_func.h"

int check_dot(char*names){ /*checks correct sintax for <name.surname>*/
	char*pointer_to_dot=strchr(names, '.');
	if(pointer_to_dot==names){
		return 0;
	}else if(pointer_to_dot==NULL){
			return 0;
		}else if(names[strlen(names)-1]=='.'){
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

void join(char**argv,int fd_udp, int *leav, struct sockaddr_in addr_udp, socklen_t *addrlen_udp){
	char*buffer_udp=malloc(128*sizeof(char));
	int n_udp;
	
	
	sprintf(buffer_udp, "%s %s%s%s%s%s\n","REG", argv[2], ";", argv[4], ";", argv[6]);
	
	n_udp=sendto(fd_udp, buffer_udp, strlen(buffer_udp), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
	if(n_udp==-1) exit(1);//error
	/*receive echo part*/
	(*addrlen_udp)=sizeof(addr_udp);	
	
	
	fd_set rfds;
			
	struct timeval tv = {10, 0}; /*waits 10s for an answer*/
	FD_ZERO(&rfds);
	FD_SET(fd_udp,&rfds);
	int counter=select(fd_udp + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

	if(counter<0){
		printf("Error in select\n");
		exit(1);//errror
	}
			
	if(FD_ISSET(fd_udp,&rfds)){
		n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &(*addrlen_udp));
		if(n_udp==-1) exit(1);//error
		write(1, "echo: ",6);//stdout
		buffer_udp[n_udp]='\0';
		printf("%s\n", buffer_udp);
		(*leav)=0;
	}
	/*************************************************************/	
		
	if(counter==0){
		printf("NOK - server not answering\n");
		exit(1);
	}
	
	free(buffer_udp);
	return;
}

void leave(char**argv, int*leav, int fd_udp, struct sockaddr_in addr_udp, socklen_t *addrlen_udp ){
	char buffer_udp[128];
	int n_udp;
	
	sprintf(buffer_udp, "%s %s\n","UNR", argv[2]);
	(*leav)=1;
	n_udp=sendto(fd_udp, buffer_udp, strlen((buffer_udp)), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
	if(n_udp==-1) exit(1);//error
	
	/*receive echo part*/				
	(*addrlen_udp)=sizeof(addr_udp);
	
	
	fd_set rfds;
			
	struct timeval tv = {10, 0}; /*waits 10s for an answer*/
	FD_ZERO(&rfds);
	FD_SET(fd_udp,&rfds);
	int counter=select(fd_udp + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

	if(counter<0){
		printf("Error in select\n");
		exit(1);//errror
	}
			
	if(FD_ISSET(fd_udp,&rfds)){
		n_udp=recvfrom(fd_udp, buffer_udp, 128,0, (struct sockaddr*)&addr_udp, &(*addrlen_udp));
		if(n_udp==-1) exit(1);//error
		write(1, "echo: ",6);//stdout
		(buffer_udp)[n_udp]='\0';
		printf("%s\n", (buffer_udp));
	}
	/*************************************************************/	
		
	if(counter==0){
		printf("NOK - server not answering\n");
		exit(1);
	}
	return;
}

void find(char**buffer_udp, char*keyboard, int fd_udp, struct sockaddr_in addr_udp, socklen_t *addrlen_udp){
	int n_udp;
	char*names=malloc(15*sizeof(char));
	char*command=malloc(15*sizeof(char));
	
	if(sscanf(keyboard, "%s %s", command, names)!=2){
		printf("not enough arguments\n");
	}else{
		if(check_dot(names)){
			sprintf((*buffer_udp), "%s %s\n","QRY", names);
							
			n_udp=sendto(fd_udp, (*buffer_udp), strlen((*buffer_udp)), 0, (struct sockaddr*)&addr_udp, sizeof(addr_udp));
			if(n_udp==-1) exit(1);//error
							
			/*receive echo part*/
							
			(*addrlen_udp)=sizeof(addr_udp);
			
			fd_set rfds;
			
			struct timeval tv = {30, 0}; /*waits 30s for an answer*/
			FD_ZERO(&rfds);
			FD_SET(fd_udp,&rfds);
			int counter=select(fd_udp + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

			if(counter<0){
				printf("Error in select\n");
				exit(1);//errror
			}
			
			if(FD_ISSET(fd_udp,&rfds)){
					n_udp=recvfrom(fd_udp, (*buffer_udp), 128,0, (struct sockaddr*)&addr_udp, &(*addrlen_udp));
					if(n_udp==-1) exit(1);//error
					write(1, "echo: ",6);//stdout
					(*buffer_udp)[n_udp]='\0';
					printf("%s\n", (*buffer_udp));
			}
			/*************************************************************/	
			
			if(counter==0){
				printf("NOK - server not answering\n");
			}
		}
	}
	free(names);
	free(command);
	return;
}

void separate_delimiters_SRPL(char *str, char**names, char**tcp_ip, int* tcp_port){
	char* command=malloc(15*sizeof(char));
	sscanf(str, "%s %[^;];%[^;];%d", command, *names, *tcp_ip, tcp_port);
	free(command);
	return;
}

int send_challenge(int challenge_number, int newfd, int n, char*name){
	int nw;
	char buffer[10];
	unsigned char number=challenge_number; /*converts to ASCII*/
	sprintf(buffer, "%s %c\n", "AUTH", number);
	printf("Sending authentication: %s\n", buffer); 
	if((nw=write(newfd,buffer,n))<=0){
		return 1;
	}
	/*get answer to compare*/
	char answer[10];
	FILE *fp;
	
	fp = fopen(name,"r");
		if(fp == NULL){
			printf("Error opening file\n");
			printf("Cannot open file %s\n", name);
			return 1;
		}
		n=1;
	while((fgets(answer,sizeof(answer),fp)!=NULL) && n!=challenge_number) n++;	
	fclose(fp);	
	
	if((n=read(newfd,buffer,128))!=0)
	{if(n==-1){
		printf("read\n");
		exit(1);//error
		}
				if(n==0) return 1;
				if(strcmp(answer, buffer)!=0) return 1;	
	}
	
	
	return 0;
}

int get_answer_file(int afd, int line, char*name){
	int n=1;
	char buffer[128];
	FILE *fp;
	
	fp = fopen(name,"r");
		if(fp == NULL){
			printf("Error opening file\n");
			printf("Doesn't exist file %s\n", name);
			return 1;
		}
	while((fgets(buffer,sizeof(buffer),fp)!=NULL) && n!=line) n++;	
	fclose(fp);	
	
	if((n=write(afd,buffer,strlen(buffer)+1))<=0){
									printf("error sending message\n");
									exit(1);
								}else printf("sent: %s\n", buffer);
	
	return 0;
}

void disconnect(int*afd, STATE*s){
	close((*afd));
	*s=idle;
	return;
}

void message(char*keyboard, STATE state, int* fd_client, int afd){
	int nw;
	char *buf=malloc(128*sizeof(char));
	char *command=malloc(15*sizeof(char));
	
	if(sscanf(keyboard, "%s %[^\n]s", command, buf)!=2){
		printf("not enough arguments\n");
	}else{
		if(state==busy){
			(*fd_client)=afd;
			if((nw=write((*fd_client),buf,strlen(buf)+1))<=0){
				printf("error sending message\n");
				exit(1);
			}else printf("sent: %s\n", buf);
		}
							
	}
	free(buf);
	free(command);
	return;
}

void connect_(char**argv, int *afd, int fd_client,struct sockaddr_in* addr_client, socklen_t *addrlen_client, STATE *state, char*keyboard, int fd_udp,struct sockaddr_in addr_udp, socklen_t *addrlen_udp){
	char* command=malloc(15*sizeof(char));
	char* names=malloc(15*sizeof(char));
	char* key=malloc(15*sizeof(char));
	char*buf=malloc(30*sizeof(char));
	int n, nw, n_client, tcp_port;
	char*tcp_ip;
	char*buffer=malloc(128*sizeof(char));
	char*buffer_udp=malloc(128*sizeof(char));
	
	if(sscanf(keyboard, "%s %s %s", command, names, key)!=3){
		 printf("wrong arguments\n");
		 return;
	 }
	if(strcmp(names, argv[2])!=0){	
			if((*state)==idle){
				find(&buffer_udp, keyboard, fd_udp, addr_udp, &(*addrlen_udp));/*gets user location*/
				if((strcmp(buffer_udp, "NOK - Surname not registered")!=0)&&(strcmp(buffer_udp, "NOK - User not registered\n")!=0)){
				/* *****************************************************/
					
				/*separate arguments*/
				separate_delimiters_SRPL(buffer_udp, &names, &tcp_ip, &tcp_port);
					
				/* **************************************************** */
				/*tcp connect*/
				if((fd_client=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error in socket client\n");exit(1);}//error
				memset((void*)&(*addr_client),(int)'\0',sizeof(*addr_client));
				(*addr_client).sin_family=AF_INET;
				inet_aton(tcp_ip, &(*addr_client).sin_addr);
				(*addr_client).sin_port=htons(tcp_port);
				n_client=connect(fd_client,(struct sockaddr*)&(*addr_client), sizeof((*addr_client)));
				if(n_client==-1){printf("error in connect\n"); exit(1);}else{
					printf("connected\n");(*afd)=fd_client;(*state)=busy;/*connected=true;*/
					sprintf(buf, "%s %s\n", "NAME", argv[2]);
					if((nw=write(fd_client,buf,strlen(buf)+1))<=0){ /*sends <name.surname> to other guy*/
						printf("error sending message\n");
						exit(1);
					}else{
						printf("sent: %s\n", buf);
						if((n=read(fd_client,buffer,128))!=0){/*gets challenge*/
							if(n==-1)exit(1);
							if(n!=0){
								buffer[n]='\0';
								printf("buffer:%s\n", buffer);
								if(strcmp(buffer, "BUSY\n")!=0){
									sprintf(buf, "%s%s",key, ".txt");
									
									unsigned char b;
									sscanf(buffer, "%s %c", command, &b);
									int a=(int)b;/*convert ASCII to integer*/
									if(get_answer_file((*afd), a, buf)){
										disconnect(&(*afd), &(*state));
									}else{
									/*reverse authentication*/
									srand(time(NULL));
									if(send_challenge(rand()%255, fd_client, n, buf)){disconnect(&(*afd), &(*state));}
									}
								}else{
									 printf("User is busy\n");
									 disconnect(&(*afd), &(*state));
								 }
							}else disconnect(&(*afd), &(*state));
						}
					}					
				}
			}else{
				printf("NOK - Connection refused...\n");
			}
		}
	}else{
		printf("NOK - You can't connect with yourself \n");
	}	
	free(names);
	free(key);
	free(command);	
	free(buf);
	free(buffer_udp);
	free(buffer);
	return;
}

void free_strings(char**buffer, char**keyboard, char**command, char**names){
	free(*buffer);
	free(*keyboard);
	free(*command);
	free(*names);
	return;
}
