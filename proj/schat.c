#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define max(A,B) ((A)>=(B)?(A):(B))

int binary_to_int(int num){
	int binary_num, decimal_num = 0, base = 1, rem;
    binary_num = num;
    while (num > 0)
    {
        rem = num % 10;
        decimal_num = decimal_num + rem * base;
        num = num / 10 ;
        base = base * 2;
    }
    return decimal_num;
}

int convert_to_binary(int n){
	int rem, i = 1, binary = 0;
    while(n != 0){
		rem = n%2;
        n /= 2;
        binary += rem*i;
        i *= 10;
    }
    return binary;
}

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

int send_challenge(int challenge_number, int newfd, int n, char*name){
	int nw;
	int binary;
	char buffer[10];
	printf("challenge number: %d\n", challenge_number);
	binary=convert_to_binary(challenge_number);
	printf("binary: %d\n", binary);
	sprintf(buffer, "%08d\n", binary);
	printf("buffer:%s\n", buffer);
	if((nw=write(newfd,buffer,n))<=0)exit(1);
	printf("buffer:%s\n", buffer);
	/*get answer to compare*/
	char answer[10];
	FILE *fp;
	
	fp = fopen(name,"r");
		if(fp == NULL){
			printf("Erro na abertura do ficheiro para escrita\n");
			return 1;
		}
		n=1;
	while((fgets(answer,sizeof(answer),fp)!=NULL) && n!=challenge_number) n++;	
	printf("answer: %s in line %d\n", answer, n);
	fclose(fp);	
	
	if((n=read(newfd,buffer,128))!=0)
	{if(n==-1)exit(1);//error
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
			printf("Erro na abertura do ficheiro para escrita\n");
			return 1;
		}
	while((fgets(buffer,sizeof(buffer),fp)!=NULL) && n!=line) n++;	
	printf("answer: %s in line %d\n", buffer, n);
	fclose(fp);	
	
	if((n=write(afd,buffer,strlen(buffer)+1))<=0){
									printf("error sending message\n");
									exit(1);
								}else printf("sent: %s\n", buffer);
	
	return 0;
}

int main(int argc, char**argv)
{
	check_args(argc, argv);
	/*udp socket*/
	int fd_udp, n_udp, leav=1, n_client; 
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
	int fd, newfd, afd, fd_client;
	socklen_t addrlen, addrlen_client;
	fd_set rfds;
	enum {idle, busy} state;
	//int c=0;
	int maxfd,counter;
	struct sockaddr_in addr, addr_client;
	int n, nw;
	char buffer[128];
	
	char keyboard[45];
	char *command=malloc(15*sizeof(char));
	char*names=malloc(15*sizeof(char));
	char*key=malloc(15*sizeof(char));
	
	char *tcp_ip;
	char allen[30];
	int tcp_port;
	

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
	//c=0;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);maxfd=fd;
		//FD_SET(fd_client,&rfds);
		FD_SET(fileno(stdin), &rfds);
		if(state==busy){FD_SET(afd,&rfds);maxfd=max(maxfd,afd);}
		
		counter=select(fileno(stdin)+fd_client+maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
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
						
					}else if((strcmp(command, "connect")==0)/*&&(connected==false)*/){
						if(sscanf(keyboard, "%s %s %s", command, names, key)!=3){
							printf("not enough arguments\n");
						}else{
							printf("%s %s %s\n", command, names, key);
							/*find part*/
							if(check_dot(names)&&(state==idle)){
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
								
								/* *****************************************************/
								/*separate arguments*/
								sscanf(buffer_udp, "%s %[^;];%s", command, names, allen);
								printf("tcp_ip: %s\n", allen);
								char *token;
								tcp_ip = strtok(allen, ";");
								token=strtok(NULL, ";");
								tcp_port=atoi(token);
								
								printf("%s %s %s %d\n", command, names, tcp_ip, tcp_port);
								
								/* **************************************************** */
								/*tcp connect*/
								if((fd_client=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error in socket client\n");exit(1);}//error
								memset((void*)&addr_client,(int)'\0',sizeof(addr_client));
								addr_client.sin_family=AF_INET;
								inet_aton(tcp_ip, &addr_client.sin_addr);
								addr_client.sin_port=htons(tcp_port);
								n_client=connect(fd_client,(struct sockaddr*)&addr_client, sizeof(addr_client));
								if(n_client==-1){printf("erro no connect\n"); exit(1);}else{
									printf("connected\n");afd=fd_client;state=busy;/*connected=true;*/
									sprintf(allen, "%s %s\n", "NAME", argv[2]);
									if((nw=write(fd_client,allen,strlen(allen)+1))<=0){
										printf("error sending message\n");
										exit(1);
									}else{
										 printf("sent: %s\n", allen);
										if((n=read(fd_client,buffer,128))!=0){
											if(n==-1)exit(1);
											printf("line:%d\n", atoi(buffer));
											if(get_answer_file(fd_client, binary_to_int(atoi(buffer)), strcat(key, ".txt"))){
												close(afd);
												state=idle;
											}
										}
									 }
									
								}
								
								
							}
							
						}
					}else if((strcmp(command, "message")==0)/*&&(connected==true)*/){
						printf("send message\n");
						bzero(allen, strlen(allen));
						if(sscanf(keyboard, "%s %[^\n]s", command, allen)!=2){
							printf("not enough arguments\n");
						}else{
							if(state==busy){
								printf("input: %s\n", keyboard);
								printf("message to send: %s\n", allen);
								fd_client=afd;
								if((nw=write(fd_client,allen,strlen(allen)+1))<=0){
									printf("error sending message\n");
									exit(1);
								}else printf("sent: %s\n", allen);
							}
							
						}
					
					}else if((strcmp(command, "disconnect")==0)){
						if(state==busy){
							printf("wants to disconnect\n");
							close(afd);
							state=idle;
						}
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
				printf("message received: %s\n", buffer);//error
				sscanf(buffer, "%s %s", command, names);
				if(strcmp(command, "NAME")==0){
					printf("entrou no nome\n");
					printf("Attempt to connect by %s\n", names);
					srand(time(NULL));
					if(send_challenge(rand()%256, afd, n, strcat(names, ".txt"))){close(afd);
							state=idle;}
					
					bzero(command, strlen(command));
				}
				
			}
			else{close(afd); state=idle;}//connection closed by peer
		}
	}//while(1)
	 close(fd);
	  exit(0);
}		
