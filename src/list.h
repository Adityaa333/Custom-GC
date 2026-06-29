#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct GC_Node 
{
    void *ptr;
    size_t size;
    struct GC_Node *next;
}GC_Node;

//void list_append(GC_Node **head, void *ptr, size_t size); 

#endif
