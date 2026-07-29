#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct GC_Node
{
    void *ptr;
    size_t size;
    size_t id;          /* allocation ID (v2)               */
    const char *file;   /* call-site file, or NULL (v3 dbg) */
    int line;            /* call-site line, or 0 (v3 dbg)     */
    struct GC_Node *next;
} GC_Node;

bool list_append(GC_Node **head, GC_Node **tail, void *ptr, size_t size,
                  size_t id, const char *file, int line);

/* Find the node tracking a given pointer, or NULL if not tracked. */
GC_Node *list_find(GC_Node *head, void *ptr);

#endif
