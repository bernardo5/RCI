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

void empty_buffer(char**buff){
	*buff[0] = '\0';
	return;
}

void create_surname_register(char**buff, char**argv){
	strcpy(*buff, "SREG ");
	strcat(*buff, argv[2]);
	strcat(*buff, ";");
	strcat(*buff, argv[4]);
	strcat(*buff, ";");
	strcat(*buff, argv[6]);
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
	
	
	int fd, n, addrlen, port;
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	char *buffer=malloc(128*sizeof(char));
	empty_buffer(&buffer);
	
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
	create_surname_register(&buffer, argv);
	printf("sent to server: %s\n", buffer);
	n=sendto(fd, buffer, 7, 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) exit(1);//error
	/*receive echo part*/
	empty_buffer(&buffer);
	addrlen=sizeof(addr);
	printf("going to rcvfrom\n");
	n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
	if(n==-1) exit(1);//error
	printf("answer to echo\n");
	write(1, "echo: ",6);//stdout
	write(1, buffer, n);
	printf("\n");
	close(fd);
		
	exit(0);
}
