

#ifndef LIST_H
#define LIST_H

#include "memory.h"
#include <string.h>

struct list_node 
{
	void * data;
	struct list_node * next;
};

typedef struct list_node Node;

typedef struct List
{
	int size;
	Node * head;
}List;


List * 	initList		(void);
int    	addFront		(List *, void * data);
void 	removeNode		(List * L, Node * prev, Node * curr);
float * listToFloatArray(List * list);
void   	freeList		(List *);
void   	printList  		(List *, char * data_type);

#endif
