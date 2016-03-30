#include "tree.h"

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

void PosFixed_delete_tree(user** base_node){
  if((*base_node)==NULL) return;
  PosFixed_delete_tree(&(*base_node)->left);
  PosFixed_delete_tree(&(*base_node)->right);
  free(*base_node);
  return;
}

