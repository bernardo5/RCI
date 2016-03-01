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

typedef struct _user{
	char name[30];
	char ip[15];
	int scport;
	struct _user*left;
	struct _user*right;
}user;

int find_user(user*root, char*name, char**buf, char*surname, int qry){
	user*auxiliar=root;
	
	if(root!=NULL){
	
		if(strcmp(auxiliar->name, name)==0){
			if(qry){
				char answer[45];
				sprintf(answer, "%s %s%s%s%s%s%s%d", "RPL", auxiliar->name, ".", surname, ";", auxiliar->ip, ";", auxiliar->scport);
				strcpy(*buf, answer);
			}
			return 1;
		}else{
			if(strcmp(name, auxiliar->name)<0){
					if((auxiliar->left)==NULL){
						 return 0; /* name does not exist */
					 }
					return find_user(auxiliar->left, name, &(*buf), surname, qry);
			}else if(strcmp(name, auxiliar->name)>0){
					 if((auxiliar->right)==NULL){
						  return 0; /* name does not exist */
					  }
					return find_user(auxiliar->right, name, &(*buf), surname, qry);
				  }else{
					  printf("invalid name\n");
					 return 0; 
				  }
		}
	}else{
		 return 0; /* name does not exist */
	}
}


void DeleteUser(user**root, char*name, char**buf){
	user*auxiliar=(*root);
	user*successor, *parent;
	
	/*find user*/
	parent=auxiliar;
	if(strcmp(auxiliar->name, name)!=0){
		while(strcmp(auxiliar->name, name)!=0){
			if(strcmp(name, auxiliar->name)<0){
				parent=auxiliar;
				auxiliar=auxiliar->left;
			}else if(strcmp(name, auxiliar->name)>0){
				 parent=auxiliar;
				 auxiliar=auxiliar->right;
			 }
		}
	}
	/* **************************************** */
	
	if((strcmp(auxiliar->name, (*root)->name)==0)&&(!(((auxiliar->left)!=NULL)&&((auxiliar->right)!=NULL)))){
		if((auxiliar->left==NULL)&&(auxiliar->right==NULL)){
			 (*root)=NULL;
		 }else{
			if(auxiliar->left==NULL){
				(*root)=auxiliar->right;
			}else (*root)=auxiliar->left;
		 }
		strcpy(*buf, "OK\0");
		return;
	}
	
	if(((auxiliar->left)!=NULL)&&((auxiliar->right)!=NULL)){
		successor=auxiliar->right;
		//printf("%s\n", successor);
		/*parent=successor;	*/
		parent=auxiliar;
		/*if(successor->left!=NULL) successor=successor->left;*/
		while((successor->left)!=NULL){
			 parent=successor;
			 successor=successor->left;
		 }
		strcpy(auxiliar->name, successor->name);
		strcpy(auxiliar->ip, successor->ip);
		auxiliar->scport=auxiliar->scport;
		auxiliar=successor;			
	}
	//auxiliar now points to the node with max of one child to delete
	user*appropchild;
	
	if((auxiliar->left)!=NULL){
		appropchild=auxiliar->left;
	}else{
		appropchild=auxiliar->right;
	}
	
	if(parent==NULL){
		*root=appropchild;
	}else if((parent->left)==auxiliar){
		parent->left=appropchild;
	}else{
		parent->right=appropchild;
	}
	strcpy(*buf, "OK\0");
	return;
}

int min(int a, int b){
	if(a<b) return a;
	else return b;	
}

void put_to_null(user**root, char*name){
	user*auxiliar=(*root);
	if(strcmp(auxiliar->name, name)==0){
		auxiliar->left=NULL;
		auxiliar->right=NULL;
	}else{
		while(strcmp(auxiliar->name, name)!=0){
			if(strcmp(name, auxiliar->name)<0){
				auxiliar=auxiliar->left;
			}else if(strcmp(name, auxiliar->name)>0) auxiliar=auxiliar->right;
		}
		auxiliar->left=NULL;
		auxiliar->right=NULL;
	}
	return;
}

void AddUser(user**root, char*name, char*ip, int scport, char**buf){
		if((*root)==NULL){
			(*root)=malloc(sizeof(user));
			strcpy((*root)->name, name);
			strcpy((*root)->ip, ip);
			(*root)->scport=scport;
			strcpy(*buf, "OK\0");
		}else{
			if(strcmp(name, (*root)->name)<0){
				if(((*root)->left)==NULL){
					((*root)->left)=malloc(sizeof(user));
					strcpy(((*root)->left)->name, name);
					strcpy(((*root)->left)->ip, ip);
					(*root)->left->scport=scport;
					strcpy(*buf, "OK\0");
				}else AddUser(&((*root)->left), name, ip, scport, &(*buf));
			}else{
				if(strcmp(name, (*root)->name)>0){
					if(((*root)->right)==NULL){
						((*root)->right)=malloc(sizeof(user));
						strcpy(((*root)->right)->name, name);
						strcpy(((*root)->right)->ip, ip);
						(*root)->right->scport=scport;
						strcpy(*buf, "OK\0");
					}else AddUser(&((*root)->right), name, ip, scport, &(*buf));
				}else{ /*already exists*/
					printf("invalid username\n");
					strcpy(*buf, "NOK - Name already exists!\0");
				}
			}
		}
	return;
}

void list(user*root, char*surname){
	if(root!=NULL){
		/*printf("yupi!!!\n");*/
		list(root->left, surname);
		printf("%s\t\t%s\t\t%s\t\t%d\n", root->name, surname, root->ip, root->scport);
		list(root->right, surname);
	}
	return;
}

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

void separate_delimiters_REG(char *str, char **name, char**surname, char**ip, int* scport){
	char*port;
	char *ip1, *ip2, *ip3, *ip4;
	char *delimiter = " .;";
	char *token;
	// get the first token 
	token = strtok(str, delimiter);
   
	// walk through other tokens 
  
    *name = strtok(NULL, delimiter);
    *surname=strtok(NULL, delimiter);
    ip1=strtok(NULL, delimiter);
    ip2=strtok(NULL, delimiter);
    ip3=strtok(NULL, delimiter);
    ip4=strtok(NULL, delimiter);
    /*strcpy(*ip, ip1);
    strcat(*ip, ".");
    strcat(*ip, ip2);
    strcat(*ip, ".");
    strcat(*ip, ip3);
    strcat(*ip, ".");
    strcat(*ip, ip4);*/
    
    sprintf(*ip, "%s%s%s%s%s%s%s", ip1, ".", ip2, ".", ip3, ".", ip4);
    port=strtok(NULL, delimiter);
    sscanf(port, "%d", scport);
   
	return;
}

void separate_delimiters_UNR(char *str, char **name, char**surname){
	char *delimiter = " .";
	char *token;
	//char*surname_aux;
	// get the first token 
	token = strtok(str, delimiter);
   
	// walk through other tokens 
  
    *name = strtok(NULL, delimiter);
    *surname=strtok(NULL, delimiter);
    sscanf(*surname, "%s", *surname);
   
	return;
	
}

int validate_surname(char*surname_program, char*surname, char**buf){
	if(strcmp(surname_program, surname)!=0){
		printf("invalid surname\n");
		strcpy(*buf, "NOK - Invalid Surname for this server\0");
	}
	return strcmp(surname_program, surname);
}

char* ask_server(char*surname){
	char* answer=malloc(128*sizeof(char));
	char query[45];
	
	int fd, n;
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	socklen_t addrlen;
	
	if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL){
		return "error\n";//error
	}
	a=(struct in_addr*)h->h_addr_list[0];
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(58000);
	
	sprintf(query, "%s %s", "SQRY", surname);
	
	addrlen=sizeof(addr);
	n=sendto(fd, query, 45, 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) return "error\n";//error
	
	/*receive echo part*/
	addrlen=sizeof(addr);
	n=recvfrom(fd, answer, 128,0, (struct sockaddr*)&addr, &addrlen);
	if(n==-1) return "error\n";//error
	if(n<128) answer[n]='\0';
	printf("answer to echo\n");
	write(1, "echo: ",6);//stdout
	write(1, answer, n);
	printf("\n");
	
	return answer;	
}

void separate_delimiters_SRPL(char *str, char**surname, char**ip, int* scport){
	char*port;
	char *ip1, *ip2, *ip3, *ip4;
	char *delimiter = " .;";
	char *token;
	// get the first token 
	token = strtok(str, delimiter);
	// walk through other tokens 
    *surname=strtok(NULL, delimiter);
    ip1=strtok(NULL, delimiter);
    ip2=strtok(NULL, delimiter);
    ip3=strtok(NULL, delimiter);
    ip4=strtok(NULL, delimiter);
    sprintf(*ip, "%s%s%s%s%s%s%s", ip1, ".", ip2, ".", ip3, ".", ip4);
    port=strtok(NULL, delimiter);
    sscanf(port, "%d", scport);
	return;
}

char * get_user_location(char*server, char*name){
	char *surname=malloc(15*sizeof(char));
	char *snpip=malloc(15*sizeof(char));
	int snpport;
	
	separate_delimiters_SRPL(server, &surname, &snpip, &snpport);
	
	printf(" jdkhcjdncjadcjdlvncdjvnidvndjv %s %s %d\n", surname, snpip, snpport);
	printf("%s\n", name);
	
	char* answer=malloc(128*sizeof(char));
	char query[45];
	
	int fd, n;
	struct sockaddr_in addr;
	socklen_t addrlen;
	
	fd=socket(AF_INET, SOCK_DGRAM, 0); /*UDP socket*/
	if(fd==-1) exit(-1);/*error*/
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	inet_aton(snpip, & addr.sin_addr);
	addr.sin_port=htons(snpport);
	
	sprintf(query, "%s %s%s%s", "QRY", name, ".", surname);
	printf("query: %s\n", query);
	
	addrlen=sizeof(addr);
	n=sendto(fd, query, 45, 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) return "error\n";//error
	
	/*receive echo part*/
	addrlen=sizeof(addr);
	printf("dsj\n");
	n=recvfrom(fd, answer, 128,0, (struct sockaddr*)&addr, &addrlen);
	if(n==-1) return "error\n";//error
	if(n<128) answer[n]='\0';
	printf("bring it\n");
	printf("answer to echo\n");
	write(1, "echo: ",6);//stdout
	write(1, answer, n);
	printf("\n");
	
	
	return answer;
	
}

void validate_user_command(char**buf, char **name, char**surname, char**ip, int*scport, char*surname_program, user**root){
	char command[6];
	char query[30];
	if(sscanf(*buf, "%s", command)!=1){
		printf("error in arguments\n");
	}else{
		printf("command= %s\n", command);
		if((strcmp(command, "REG")==0)||(strcmp(command, "UNR")==0)||(strcmp(command, "QRY")==0)){
			if(strcmp(command, "REG")==0){ /* registo de um user */
				separate_delimiters_REG(*buf, &(*name), &(*surname), &(*ip), &(*scport));
				if(validate_surname(surname_program, *surname, &(*buf))!=0)return;
				if(!find_user((*root), *name, &(*buf), *surname, 0)){
					AddUser(&(*root), *name, *ip, *scport,  &(*buf));
					put_to_null(&(*root), (*name));
				}
			}else if(strcmp(command, "UNR")==0){ /* apagar a sessao de um user */
						separate_delimiters_UNR(*buf, &(*name), &(*surname));
						printf("%s %s\n", *name, *surname);
						if(validate_surname(surname_program, *surname, &(*buf))!=0)	return;
						if(find_user((*root), *name, &(*buf), *surname, 0)){
							printf("encontrou e nao devia\n");
							 DeleteUser(&(*root), *name, &(*buf));
						 }else  strcpy(*buf, "NOK - Invalid name in this server\0");
			}else if(strcmp(command, "QRY")==0){
				strcpy(query, *buf);
				separate_delimiters_UNR(query, &(*name), &(*surname));
				printf("%s %s\n", *name, *surname);
				if(validate_surname(surname_program, *surname, &(*buf))!=0){
					printf("QRY de apelido diferente: %s\n", *surname);
					/*enviar squery a perguntar qual o servidor cm quem tem de falar*/
					printf("%s\n", ask_server(*surname));
					strcpy(*buf, get_user_location(ask_server(*surname), *name));
					printf("%s", *buf);
				}else{/*information in this server*/
					/*send RPL*/
					printf("enviar RPL\n");
					if(!find_user((*root), (*name), &(*buf), *surname, 1)){
						strcpy(*buf, "User not registered\n");
					}
				}
			}
		}else{
			strcpy(*buf, "NOK - invalid command\0");
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
	if(n<128) *buff[n]='\0';
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
			write(1, buff, nread);
			validate_user_command(&buff, &name, &surname, &ip, &scport, argv[2], &root);
			ret=sendto(fd, buff, 128,0,(struct sockaddr*)&addr, addrlen);
			if(ret==-1)exit(1);
		}
		
	}
	/* ********************************************************** */
	
}
