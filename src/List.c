
#include "List.h"

#include <stdlib.h>
#include <stdio.h>


List * initList(void)
{
	List * list = (List *) malloc(sizeof(List));
	
	if(!list)
		return NULL;
	
	list->head = NULL;
	list->size = 0;
	
	return list;
}
/**
 * Adds a node to the front of the list.
 */
int addFront(List * list, void * data)
{
	Node * new = (Node *) malloc(sizeof(Node));
		if(!new) return 0;
	
	Node ** head = &(list->head);
	
	new->data = data;
	new->next = *head;
	
	*head = new;
	list->size++;
	
	return 1;
}

void removeNode(List * L, Node * prev, Node * curr)
{
	if(prev == NULL)
		L->head = curr->next;
	else
		prev->next = curr->next;
	free(curr->data);
	free(curr);
	
	L->size--;
}

float * listToFloatArray(List * list)
{
	int idx = 0, size = list->size;
	Node * curr = list->head;
	
	float * farray = allocf1d(size);
		if(!farray)return NULL;
	
	while(curr != NULL)
	{
		float * fval  = (float *)(curr->data);
		farray[idx++] = *fval;
		
		curr = curr->next;
	}
	if(idx != size){
		printf("idx = %d\n", idx);
		return NULL;
	}
	
	return farray;
}

void freeList(List * list)
{
	Node * curr = list->head;
	Node * temp;
	
	while(curr != NULL)
	{
		temp = curr->next;
		if(curr->data != NULL)
			free(curr->data);
		free(curr);
		curr = temp;
	}
	free(list);
}

void printList(List * list, char * data_type)
{
	printf("Size : %d\n", list->size);
	
	Node * curr = list->head;
	
	while(curr != NULL)
	{
		if(strcmp(data_type, "float") == 0)
		{
			float * fd = (float *)curr->data;
			printf("%f\n", *fd);
		}
		
		curr = curr->next;
	}
	
	printf("Size : %d\n", list->size);
}
