#include "tree.h"

char* ask_server(char*surname);

void check_args(int argc, char**argv, int*server_specified);

void create_surname_register_buffer(char**buff, char**argv);

void destroy_surname_buffer(char**buff, char**argv);

void empty_buffer(char**buff);

char * get_user_location(char*server, char*name);

void list(user*root, char*surname);

void registe(char**buff, char**argv, int fd, struct sockaddr_in addr, char*place);

void separate_delimiters_REG(char *str, char **name, char**surname, char**ip, int* scport);

void separate_delimiters_SRPL(char *str, char**surname, char**ip, int* scport);

void separate_delimiters_UNR(char *str, char **name, char**surname);

int validate_surname(char*surname_program, char*surname, char**buf);

void validate_user_command(char**buf, char **name, char**surname, char**ip, int*scport, char*surname_program, user**root);
