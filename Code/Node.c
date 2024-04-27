#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Node.h"

struct Node* creatNode(struct Node* child, char* name, unsigned lineNum){
    struct Node* tmp = (struct Node*) malloc(sizeof(struct Node));
    tmp -> child = child;
    tmp -> brother = NULL;
    strcpy(tmp -> name, name);
    tmp -> lineNum = lineNum;
    tmp -> is_token = 0;
    tmp -> TYPE_INT = 1;
    tmp -> epsilon = 0;
    tmp -> is_type = 0;
    return tmp;
}

void printNode(struct Node* node){
    if(node -> is_token == 1){
    if(!strcmp(node -> name, "ID\0")){
        printf("ID: %s\n",node -> TYPE_ID);
    }
    else if(!strcmp(node -> name, "INT\0")){
        printf("INT: %u\n",node -> TYPE_INT);
    }
    else if(!strcmp(node -> name, "FLOAT\0")){
        printf("FLOAT: %f\n", node -> TYPE_FLOAT);
    }
    else if(node->is_type!=0){
	if(node->is_type==1)
	printf("TYPE: int\n");
	else
	printf("TYPE: float\n");
    }
    else{
    printf("%s\n",node -> name);
    }
    }
    else
    if(node -> epsilon == 0)
    printf("%s (%d)\n", node -> name, node -> lineNum);
}

void printSpace(unsigned depth){
    for(unsigned i = 0; i < depth; ++i) printf("  ");
}


void printTree(struct Node* rootNode, unsigned depth){
	if(rootNode != NULL){
		if(rootNode -> epsilon == 0)
		    printSpace(depth);
		printNode(rootNode);	
		if(rootNode -> child != NULL){
		    printTree(rootNode -> child, depth + 1);
		}
		struct Node* brNode = rootNode -> brother;
		if(brNode != NULL){
		    printTree(brNode,depth);
		}

	}
    
}

void printBrotherNum(struct Node *head){
    int BrotherNum = 0;
    struct Node* brNode = head -> brother;
    while (brNode != NULL)
    {
	    BrotherNum++;
        brNode = brNode -> brother;
    }
    printf("BrotherNum:%d\n",BrotherNum);

}


void printTreeFromRoot(struct Node *root){
    printTree(root,0);
}

struct Node *root;
unsigned LexError = 0;
