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

void check_args(int argc, char**argv, int*server_specified){
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
			*server_specified=0;
		}else *server_specified=1;
	}	
	/* ************************* */
	return;
}

void separate_delimiters_REG(char *str, char **name, char**surname, char**ip, char**scport){
	
	char *delimiter = " .;";
	char *token;
	// get the first token 
	token = strtok(str, delimiter);
   
	// walk through other tokens 
  
    *name = strtok(NULL, delimiter);
    *surname=strtok(NULL, delimiter);
    *ip=strtok(NULL, delimiter);
    *scport=strtok(NULL, delimiter);
   
	return;
}

void validate_user_command(char**buf, char **name, char**surname, char**ip, char**scport){
	char command[6];
	if(sscanf(*buf, "%s", command)!=1){
		printf("error in arguments\n");
	}else{
		printf("command= %s\n", command);
		if(strcmp(command, "REG")==0||strcmp(command, "UNR")==0){
			if(strcmp(command, "REG")==0)
				separate_delimiters_REG(*buf, &(*name), &(*surname), &(*ip), &(*scport));
			strcpy(*buf, "OK\0");
		}else{
			strcpy(*buf, "NOK\0");
		}
	}
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
	char *name, *surname, *ip, *scport;
	
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
	addr.sin_port=htons(9000);
	
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
						
						
					printf("typed: %s\n", buf);
					registe(&buffer, argv, fd, addr, "exit");
					close(fd);
					exit(0);
				}else{
					if(strcmp(buf, "list\n")==0){
						printf("imprime lista\n");
					}
				}
			}
			
		}

		if(FD_ISSET(fd,&rfds)){
			addrlen=sizeof(addr);
			nread=recvfrom(fd, buff,128,0,(struct sockaddr*)&addr, &addrlen);
			if(nread==-1)exit(1);//error
			write(1, "received: ",10);//stdout
			write(1, buff, nread);
			validate_user_command(&buff, &name, &surname, &ip, &scport);
			printf("%s\n", name);
  
			printf("%s\n", surname);
		 
			printf("%s\n", ip);
		  
			printf("%s\n", scport);
			ret=sendto(fd, buff, 128,0,(struct sockaddr*)&addr, addrlen);
			if(ret==-1)exit(1);
		}
		
	}
	/* ********************************************************** */
	
}
