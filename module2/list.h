

#ifndef LIST_H
#define LIST_H


struct list_node 
{
	float value;
	struct list_node * next;
};

typedef struct list_node Node;


int  addFront	(Node ** head, float value);
void freeList	(Node * head);

#endif
