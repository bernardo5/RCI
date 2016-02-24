#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
 #include <errno.h>
#include <arpa/inet.h>
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
	int sock_fd;
	char buffer[128];
	
	struct sockaddr_in server_addr, client_addr;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if(sock_fd == -1){
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);		/* nmero de porto */
	inet_aton("194.210.228.93", & server_addr.sin_addr);	/* endereo IP */

	if( connect(sock_fd, ( struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect ");
		exit(-1);
	}
	
	strcpy(buffer, "hello bitches");
	send(sock_fd, buffer, strlen(buffer) +1, 0);
	perror("send");
	printf("message sent: %s\n", buffer);

	printf("press enter to continue: ");
	getchar();


	read(sock_fd, buffer, 128);
	printf("read message %s\n", buffer);
	
	
	strcpy(buffer, "222222222222");
	send(sock_fd, buffer, strlen(buffer) +1, 0);
	perror("sendto");
	printf("message sent: %s\n", buffer);
	
	printf("press enter to continue: ");
	getchar();


	read(sock_fd, buffer, 128);
	printf("read message %s\n", buffer);
					
	exit(0);
	
}
