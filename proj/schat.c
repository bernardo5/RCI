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

int main(int argc, char**argv){
	
	check_args(argc, argv);
	
	int fd, n, addrlen;
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	char buffer[128];
	
	/*get host IP*/
	if((h=gethostbyname("bernardo-HP-Pavilion-dv6-Notebook-PC"))==NULL)exit(1);//error
		
	a=(struct in_addr*)h->h_addr_list[0];
	
	/*send part*/
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(9000);
	
	n=sendto(fd, "REG teste.teste;ip;80\n", strlen("REG teste.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG pedro.teste;ip;80\n", strlen("REG pedro.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG zorro.teste;ip;80\n", strlen("REG zorro.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG bernardo.teste;ip;80\n", strlen("REG bernardo.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG diogo.teste;ip;80\n", strlen("REG diogo.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG ana.teste;ip;80\n", strlen("REG ana.teste;ip;80\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	
	n=sendto(fd, "UNR piça.teste\n", strlen("UNR piça.teste\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	close(fd);
		
	exit(0);
}
