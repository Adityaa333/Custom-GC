#include "list.h"
#include <stdlib.h>

void list_append( GC_Node **head, void *ptr, size_t size)
{
	GC_Node *new_node = malloc(sizeof(GC_Node));
    	if(!new_node) return;

    	new_node->ptr = ptr;
    	new_node->size = size;
    	new_node->next = NULL;

    	if(*head == NULL)
	{
        	*head = new_node;
    	}else{
		GC_Node *curr = *head; 
		while(curr->next != NULL) curr = curr->next;
        	curr->next = new_node;
    }
}
