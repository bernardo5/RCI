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

int main(int argc, char**argv){
	
	check_args(argc, argv);
	char keyboard[45];
	char command[15], key[15];
	char*names=malloc(15*sizeof(char));
	
	
	
	int fd, n, leav=1; 
	socklen_t addrlen;
	struct sockaddr_in addr;
	char* buffer=malloc(128*sizeof(char));
	
	fd_set rfds;
	int counter;
	
	int fd_tcp, n_tcp, newfd, afd;
	enum {idle,busy} state=idle;
	
	socklen_t addrlen_tcp;
	struct sockaddr_in addr_tcp;
	char*ptr;
	char* buffer_tcp=malloc(128*sizeof(char));
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	inet_aton(argv[8], & addr.sin_addr);
	addr.sin_port=htons(atoi(argv[10]));
	
	
	if((fd_tcp=socket(AF_INET,SOCK_STREAM,0))==-1)exit(1);//error

	memset((void*)&addr_tcp,(int)'\0',sizeof(addr_tcp));
	addr_tcp.sin_family=AF_INET;
	addr_tcp.sin_addr.s_addr=htonl(INADDR_ANY);
	addr_tcp.sin_port=htons(atoi(argv[6]));

	if(bind(fd_tcp,(struct sockaddr*)&addr_tcp,sizeof(addr_tcp))==-1) exit(1);//error
	
	
	
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fileno(stdin), &rfds);
		
		counter=select(fileno(stdin)+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
		if(counter<=0){printf("error in select\n");exit(1);}

		if(FD_ISSET(fileno(stdin),&rfds)){
			if(fgets(keyboard, 45, stdin)){
				if(sscanf(keyboard, "%s", command)==1){
					if(strcmp(command, "join")==0){
						join(&buffer, argv);
						n=sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
						if(n==-1) exit(1);//error
						/*receive echo part*/
						addrlen=sizeof(addr);				
						printf("going to rcvfrom\n");
						/***************************************************************/
						
						n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
						if(n==-1) exit(1);//error
						printf("answer to echo\n");
						write(1, "echo: ",6);//stdout
						buffer[n]='\0';
						printf("%s\n", buffer);
						leav=0;
						
			/*************************************************************/				
					}else if(strcmp(command, "find")==0){
						if(sscanf(keyboard, "%s %s", command, names)!=2){
							printf("not enough arguments\n");
						}else{
							if(check_dot(names)){
								printf("name and surname: %s\n", names);
								find(&buffer, names);
								
								n=sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
								if(n==-1) exit(1);//error
								
								/*receive echo part*/
								
								addrlen=sizeof(addr);
								printf("going to rcvfrom\n");
								n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
								if(n==-1) exit(1);//error
								printf("answer to echo\n");
								write(1, "echo: ",6);//stdout
								buffer[n]='\0';
								printf("%s\n", buffer);
							}
						}
						
					}else if(strcmp(command, "leave")==0){
						leave(&buffer, argv);
						leav=1;
						n=sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
						if(n==-1) exit(1);//error
						
						/*receive echo part*/
						
						addrlen=sizeof(addr);
						printf("going to rcvfrom\n");
						n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
						if(n==-1) exit(1);//error
						printf("answer to echo\n");
						write(1, "echo: ",6);//stdout
						buffer[n]='\0';
						printf("%s\n", buffer);
						
					}else if(strcmp(command, "exit")==0){
						if(!leav){
							leave(&buffer, argv);
							n=sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
							if(n==-1) exit(1);//error
							
							/*receive echo part*/
							
							addrlen=sizeof(addr);
							printf("going to rcvfrom\n");
							n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
							if(n==-1) exit(1);//error
							printf("answer to echo\n");
							write(1, "echo: ",6);//stdout
							buffer[n]='\0';
							printf("%s\n", buffer);
						}				
							close(fd);
							exit(0);
							//}else printf("please leave before exit\n");
					}
				}
			}
		}
		if(FD_ISSET(fd_tcp, &rfds)){
			addrlen_tcp=sizeof(addr_tcp);

			if((newfd=accept(fd_tcp,(struct sockaddr*)&addr_tcp,&addrlen_tcp))==-1)exit(1);//error
			switch(state){
				case idle: afd=newfd; state=busy; break;
				case busy: strcpy(buffer_tcp,"busy\n");ptr=&buffer_tcp[0]; if(write(newfd,ptr,n_tcp)<=0)exit(1);//error
				close(newfd); break;
			}
		}
		
		if(FD_ISSET(afd,&rfds)){
			if((n_tcp=read(afd,buffer_tcp,128))!=0){
				if(n_tcp==-1)exit(1);//error
				ptr=&buffer_tcp[0];
				if(write(afd,ptr,n_tcp)<=0)exit(1);
			}else{
				close(afd);
				state=idle;
			}//connection closed by peer
		}
	
	}
}
