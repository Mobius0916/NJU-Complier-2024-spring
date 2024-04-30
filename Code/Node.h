#ifndef NODE
#define NODE
struct Node
{
    struct Node* child;
    struct Node* brother;
    char name[32];
    unsigned is_token;
    unsigned lineNum;
    unsigned epsilon;
    unsigned is_type;//0 is not TYPE,1 is int,2 is float
    union 
    {
        int TYPE_INT;
        float TYPE_FLOAT;
        char TYPE_ID[32];
        char RELOP_[5];
    };
};


struct Node* creatNode(struct Node* child, char* name, unsigned lineNum);

void printNode(struct Node*node);
void printSpace(unsigned depth);


void printTree(struct Node*rootNode,unsigned depth);
void addBrother(struct Node *head,struct Node*toAdd);

void printTreeFromRoot(struct Node *root);
extern struct Node *root;
extern unsigned LexError;
#endif
