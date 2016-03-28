#include "snp_func.h"

int main(int argc, char**argv){
	int server_specified=0;
	
	check_args(argc, argv, &server_specified);
	
	
	int port, fd, ret;
	struct sockaddr_in addr;
	/*struct sockaddr_in addr_server;*/
	struct hostent *h;
	struct in_addr *a;
	char *buffer=malloc(128*sizeof(char));
	char *buff=malloc(128*sizeof(char));
	fd_set rfds;
	int counter;
	int nread;
	char buf[15];
	socklen_t addrlen;
	char *name=malloc(35*sizeof(char));
	char *surname=malloc(35*sizeof(char));
	char*ip=malloc(30*sizeof(char));
	int scport;
	user*root=NULL; /*pointer to binary tree to store users*/
	
	if(server_specified==1){
		a=(struct in_addr*)argv[8];
		port=atoi(argv[10]);
	}else{
		/*get host IP*/
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL)exit(1);//error
		a=(struct in_addr*)h->h_addr_list[0];
		port=58000;
	}
	
	/*register in tejo*/
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(port);
	
	registe(&buffer, argv, fd, addr, "register");
	close(fd);
	/* ******************************************** */
	
	/*listen to clients*/
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(atoi(argv[6]));
	
	ret=bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret==-1){
		printf("Error in bind\n");
		exit(1);//error
	}
	
	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		FD_SET(fileno(stdin), &rfds);
		counter=select(fd + fileno(stdin) + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
//-------------------------------------------------------------------------------------------------------------------AQUI
		if(counter<=0){
			printf("Error in select\n");
			exit(1);//errror
		}
		
		if(FD_ISSET(fileno(stdin), &rfds)){ /* user interface */
			if(fgets(buf, 15, stdin)){
				if(strcmp(buf, "exit\n")==0){
					close(fd);
					fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
					if(fd==-1) exit(-1);/*error*/
						
					memset((void*)&addr, (int)'\0', sizeof(addr));
					addr.sin_family=AF_INET;
					addr.sin_addr=*a;
					addr.sin_port=htons(port);
						
					registe(&buffer, argv, fd, addr, "exit");
					close(fd);
					exit(0);
				}else{
					if(strcmp(buf, "list\n")==0){
						printf("name\t\tsurname\t\tip\t\tscport\n");
						list(root, argv[2]);
					}
				}
			}
			
		}

		if(FD_ISSET(fd,&rfds)){
			addrlen=sizeof(addr);
			nread=recvfrom(fd, buff,128,0,(struct sockaddr*)&addr, &addrlen);
			if(nread==-1)exit(1);//error
			if(nread<128) buff[nread]='\0';
			write(1, "received: ",10);//stdout
			write(1, buff, strlen(buff));
			printf("\n");
			validate_user_command(&buff, &name, &surname, &ip, &scport, argv[2], &root);
			ret=sendto(fd, buff, 128,0,(struct sockaddr*)&addr, addrlen);
			if(ret==-1)exit(1);
		}
		
	}
	/* ********************************************************** */
	
}
