#include "snp_func.h"

void list(user*root, char*surname){
	if(root!=NULL){
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
	char* command=malloc(15*sizeof(char));
	sscanf(str, "%s %[^.].%[^;];%[^;];%d", command, *name, *surname, *ip, scport);
	free(command);
	return;
}

void separate_delimiters_UNR(char *str, char **name, char**surname){
	char* command=malloc(15*sizeof(char));
	sscanf(str, "%s %[^.].%s", command, *name, *surname);
    free(command);
	return;
	
}

void separate_delimiters_SRPL(char *str, char**surname, char**ip, int* scport){
	char* command=malloc(15*sizeof(char));
	sscanf(str, "%s %[^;];%[^;];%d", command, *surname, *ip, scport);
	free(command);
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
	fd_set rfds;
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
	addrlen=sizeof(addr);
	
	struct timeval tv = {10, 0}; /*waits 1m for an answer*/
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	int counter=select(fd + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

	if(counter<0){
		printf("Error in select\n");
		exit(1);//errror
	}
	
	if(FD_ISSET(fd,&rfds)){			
			n=recvfrom(fd, answer, 128,0, (struct sockaddr*)&addr, &addrlen);
			if(n==-1) return "error\n";//error
			if(n<128) answer[n]='\0';
			write(1, "echo: ",6);//stdout
			write(1, answer, strlen(answer));
			printf("\n");
	}
	/*************************************************************/	
	
	if(counter==0) return "NOK - can not reach server\n\0";
	
	return answer;	
}



char * get_user_location(char*server, char*name){
	char *surname=malloc(15*sizeof(char));
	char *snpip=malloc(15*sizeof(char));
	int snpport;
	
	separate_delimiters_SRPL(server, &surname, &snpip, &snpport);
	
	char* answer=malloc(128*sizeof(char));
	char query[45];
	fd_set rfds;
	int counter;
	
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
	
	addrlen=sizeof(addr);
	n=sendto(fd, query, 45, 0, (struct sockaddr*)&addr, sizeof(addr));
	if(n==-1) return "error\n";//error
	
	/*receive echo part*/
	addrlen=sizeof(addr);
	
	
	/***************************************************************/
	struct timeval tv = {10, 0}; /*waits 1m for an answer*/
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	counter=select(fd + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

	if(counter<0){
		printf("Error in select\n");
		exit(1);//errror
	}
	
	if(FD_ISSET(fd,&rfds)){
			n=recvfrom(fd, answer, 128,0, (struct sockaddr*)&addr, &addrlen);
			if(n==-1) return "error\n";//error
			if(n<128) answer[n]='\0';
			write(1, "echo: ",6);//stdout
			write(1, answer, strlen(answer));
			printf("\n");
	}
	/*************************************************************/	
	
	if(counter==0) return "NOK - can not reach server\n\0";
	
	return answer;
	
}

void validate_user_command(char**buf, char **name, char**surname, char**ip, int*scport, char*surname_program, user**root){
	char command[6];
	char query[30];
	if(sscanf(*buf, "%s", command)!=1){
		printf("error in arguments\n");
	}else{
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
						if(validate_surname(surname_program, *surname, &(*buf))!=0)	return;
						if(find_user((*root), *name, &(*buf), *surname, 0)){
							 DeleteUser(&(*root), *name, &(*buf));
						 }else  strcpy(*buf, "NOK - Invalid name in this server\0");
			}else if(strcmp(command, "QRY")==0){
				strcpy(query, *buf);
				separate_delimiters_UNR(query, &(*name), &(*surname));
				if(validate_surname(surname_program, *surname, &(*buf))!=0){
					/*enviar squery a perguntar qual o servidor cm quem tem de falar*/
					if(strcmp(ask_server(*surname), "SRPL")!=0){
						printf("%s\n", ask_server(*surname));
						strcpy(*buf, get_user_location(ask_server(*surname), *name));
						printf("%s", *buf);
					}else{
						strcpy(*buf, "NOK - Surname not registered");
					}
				}else{/*information in this server*/
					/*send RPL*/
					if(!find_user((*root), (*name), &(*buf), *surname, 1)){
						strcpy(*buf, "NOK - User not registered\n");
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
	fd_set rfds;
	struct timeval tv = {10, 0}; /*waits 1m for an answer*/
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	int counter=select(fd + 1,&rfds,(fd_set*)NULL,(fd_set*)NULL,&tv);

	if(counter<0){
		printf("Error in select\n");
		exit(1);//errror
	}
	
	if(FD_ISSET(fd,&rfds)){
			n=recvfrom(fd, *buff, 128,0, (struct sockaddr*)&addr, &addrlen);
			if(n==-1) exit(1);//error
			if(n<128) (*buff)[n]='\0';
			write(1, "echo: ",6);//stdout
			write(1, *buff, strlen(*buff));
			printf("\n");
			char*NOK=malloc(4*sizeof(char));
			if((sscanf(*buff, "%s", NOK))!=1) exit(-1);
			if((strcmp(place, "register")==0)&&(strcmp(NOK, "NOK")==0)){
				free(NOK);
				exit(0);
			}
			free(NOK);
	}
	/*************************************************************/	
	
	if(counter==0){
		printf("NOK - server not answering\n");
		exit(1);
	}
	return;
	
}
