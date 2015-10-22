
#include "list.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * Adds a node to the front of the list.
 */
int addFront( Node ** head, float value)
{
	Node * new;
	
	if((new = (Node *) malloc(sizeof(Node))) == NULL)
		return 0;
	
	new->value = value;
	new->next  = *head;
	
	*head = new;
	
	return 1;
}

void freeList(Node * curr)
{
	Node * temp;
	
	while(curr != NULL)
	{
		temp = curr->next;
		free(curr);
		curr = temp;
	}
}
