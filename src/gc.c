#include "../include/gc.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

static GC_Node *gc_head = NULL;
static GC_Node *gc_tail = NULL; 
static bool is_initialized = false; 
static size_t total_allocations = 0;
static size_t total_memory_allocated = 0;
static bool is_cleaned = false;

static void gc_atexit_handler(void) 
{
	gc_clean(true);
}

void gc_init(void) 
{
	if( is_initialized ) return; 
	atexit(gc_atexit_handler);
	is_initialized = true; 
}

void *gc_alloc(size_t size) 
{
	void *ptr = malloc(size);
    	if(!ptr) return NULL;

	if (!list_append(&gc_head, &gc_tail, ptr, size)) 
	{
        	free(ptr); 
        	return NULL;
    	}

    	total_allocations++;
    	total_memory_allocated += size;

    	return ptr;
}

void gc_stats(void)
{
    printf("Active tracked tracking blocks: %zu\n", total_allocations);
}

void gc_clean(bool print_report) 
{
    	if (is_cleaned) return;

    	size_t total_freed = 0;
    	size_t blocks_freed = 0;

    	if(print_report) 
	{
        	printf("\n=========================================\n");
        	printf("         GC CLEANUP REPORT\n");
        	printf("=========================================\n");
        	printf("Total Allocations : %zu\n\n", total_allocations);
        	printf("-----------------------------------------\n");
    	}

    	GC_Node *curr = gc_head;
    	size_t count = 1;

    	while(curr != NULL) 
	{
        	if (print_report) 
		{
            		printf("Block #%zu\n\n", count++);
            		printf("Address : %p\n\n", curr->ptr);
            		printf("Size    : %zu bytes\n\n", curr->size);
            		printf("Released Successfully\n");
            		printf("-----------------------------------------\n");
        	}

        	free(curr->ptr);
        	total_freed += curr->size;
        	blocks_freed++;

        	GC_Node *temp = curr;
		curr = curr->next;
        	free(temp);
    	}

    	gc_head = NULL;
	gc_tail = NULL; 

    	if (print_report) 
	{
        	printf("Total Memory Allocated : %zu bytes\n", total_memory_allocated);
        	printf("Total Memory Freed     : %zu bytes\n", total_freed);
        	printf("Blocks Released        : %zu\n", blocks_freed);
        	printf("Memory Remaining       : %zu bytes\n", total_memory_allocated - total_freed);
        	printf("Cleanup Status         : SUCCESS\n");
        	printf("=========================================\n");
    	}

    	is_cleaned = true;
}
