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

int main(){
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
	
	n=sendto(fd, "REG teste.teste;ip;scport\n", strlen("REG teste.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG pedro.teste;ip;scport\n", strlen("REG pedro.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG zorro.teste;ip;scport\n", strlen("REG zorro.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG bernardo.teste;ip;scport\n", strlen("REG bernardo.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG diogo.teste;ip;scport\n", strlen("REG diogo.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	n=sendto(fd, "REG anacleto.teste;ip;scport\n", strlen("REG anacleto.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
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
	
	
	/*n=sendto(fd, "REG a.teste;ip;scport\n", strlen("REG a.teste;ip;scport\n"), 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) exit(1);//error
	
	/*receive echo part*/
	
	/*addrlen=sizeof(addr);
	printf("going to rcvfrom\n");
	n=recvfrom(fd, buffer, 128,0, (struct sockaddr*)&addr, &addrlen);
	if(n==-1) exit(1);//error
	printf("answer to echo\n");
	write(1, "echo: ",6);//stdout
	buffer[n]='\0';
	printf("%s\n", buffer);*/
	close(fd);
		
	exit(0);
}
