#include "list.h"
#include <stdlib.h>

void list_append( GC_Node **head,GC_Node **tail,  void *ptr, size_t size)
{
	GC_Node *new_node = malloc(sizeof(GC_Node));
    	if(!new_node) return false;

    	new_node->ptr = ptr;
    	new_node->size = size;
    	new_node->next = NULL;

    	if(*head == NULL)
	{
        	*head = new_node;
		*tail = new_node; 
    	}else{
		(*tail)->next = new_node; 
		*tail = new_node; 
    	}

	return true; 
}
