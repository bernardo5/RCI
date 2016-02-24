#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void separate_delimiters_REG(char str[80], char **name, char**surname, char**ip, char**scport){
	
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


int main(){
	
	char *str=malloc(80*sizeof(char));
	strcpy(str, "REG name.surname;ip;scport");
	char *name, *surname, *ip, *scport;
	
	separate_delimiters_REG(str, &name, &surname, &ip, &scport);
	
	printf("%s\n", name);
  
	printf("%s\n", surname);
 
	printf("%s\n", ip);
  
	printf("%s\n", scport);
	
	exit(0);
}


/*int main(){
   char str[80] = "REG name.surname;ip;scport";
   char *delimiter = " .;";
   char *token;
   char *array[8];
   char *name, *surname, *ip, *scport;
   int i, j = 0;
   // get the first token 
   token = strtok(str, delimiter);
   
   // walk through other tokens 
  
      name = strtok(NULL, delimiter);
     surname=strtok(NULL, delimiter);
     ip=strtok(NULL, delimiter);
     scport=strtok(NULL, delimiter);
   
  
   printf("%s\n", name);
  
   printf("%s\n", surname);
 
   printf("%s\n", ip);
  
   printf("%s\n", scport);
   
   exit(0);
}*/
