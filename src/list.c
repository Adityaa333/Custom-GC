#include "list.h"
#include <stdlib.h>

bool list_append(GC_Node **head, GC_Node **tail, void *ptr, size_t size,
                  size_t id, const char *file, int line)
{
	GC_Node *new_node = malloc(sizeof(GC_Node));
	if (!new_node) return false;

	new_node->ptr = ptr;
	new_node->size = size;
	new_node->id = id;
	new_node->file = file;
	new_node->line = line;
	new_node->next = NULL;

	if (*head == NULL)
	{
		*head = new_node;
		*tail = new_node;
	} else {
		(*tail)->next = new_node;
		*tail = new_node;
	}

	return true;
}

GC_Node *list_find(GC_Node *head, void *ptr)
{
	for (GC_Node *curr = head; curr != NULL; curr = curr->next)
	{
		if (curr->ptr == ptr) return curr;
	}
	return NULL;
}
