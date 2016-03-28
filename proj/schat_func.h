#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define max(A,B) ((A)>=(B)?(A):(B))

typedef enum{idle, busy} STATE;

void check_args(int argc, char**argv);

int check_dot(char*names);

void connect_(char**argv, int *afd, int fd_client,struct sockaddr_in* addr_client, socklen_t *addrlen_client, STATE *state, char*keyboard, int fd_udp,struct sockaddr_in addr_udp, socklen_t *addrlen_udp);

void disconnect(int*afd, STATE*s);

void find(char**buffer_udp, char*keyboard, int fd_udp, struct sockaddr_in addr_udp, socklen_t *addrlen_udp);

void free_strings(char**buffer, char**keyboard, char**command, char**names);

int get_answer_file(int afd, int line, char*name);

void join(char**argv,int fd_udp, int *leav, struct sockaddr_in addr_udp, socklen_t *addrlen_udp);

void leave(char**argv, int*leav, int fd_udp, struct sockaddr_in addr_udp, socklen_t *addrlen_udp );

void message(char*keyboard, STATE state, int* fd_client, int afd);

int send_challenge(int challenge_number, int newfd, int n, char*name);

void separate_delimiters_SRPL(char *str, char**names, char**tcp_ip, int* tcp_port);
