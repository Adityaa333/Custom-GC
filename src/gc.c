#include "gc.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

/* ==========================================
 * ANSI colors for report output (v2)
 * ========================================== */
#define CLR_RESET  "\x1b[0m"
#define CLR_BOLD   "\x1b[1m"
#define CLR_GREEN  "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_CYAN   "\x1b[36m"

static GC_Node *gc_head = NULL;
static GC_Node *gc_tail = NULL;
static bool is_initialized = false;
static bool is_cleaned = false;

static size_t total_allocations = 0;
static size_t total_memory_allocated = 0;
static size_t next_alloc_id = 1;

/* v2 stats */
static size_t peak_memory_usage = 0;
static size_t largest_allocation = 0;
static size_t current_live_memory = 0;

static void gc_atexit_handler(void)
{
	gc_clean(GC_REPORT_FULL);
}

void gc_init(void)
{
	if (is_initialized) return;
	atexit(gc_atexit_handler);
	is_initialized = true;
}

static void record_new_allocation(size_t size)
{
	total_allocations++;
	total_memory_allocated += size;
	current_live_memory += size;

	if (size > largest_allocation) largest_allocation = size;
	if (current_live_memory > peak_memory_usage) peak_memory_usage = current_live_memory;
}

/* ==========================================
 * gc_alloc
 * ========================================== */
void *gc_alloc_dbg(size_t size, const char *file, int line)
{
	if (!is_initialized)
	{
		fprintf(stderr, "gc_alloc: gc_init() was not called\n");
		return NULL;
	}

	void *ptr = malloc(size);
	if (!ptr) return NULL;

	if (!list_append(&gc_head, &gc_tail, ptr, size, next_alloc_id, file, line))
	{
		free(ptr);
		return NULL;
	}

	next_alloc_id++;
	record_new_allocation(size);

	return ptr;
}

void *gc_alloc(size_t size)
{
	return gc_alloc_dbg(size, NULL, 0);
}

/* ==========================================
 * gc_calloc
 * ========================================== */
void *gc_calloc_dbg(size_t nmemb, size_t size, const char *file, int line)
{
	if (!is_initialized)
	{
		fprintf(stderr, "gc_calloc: gc_init() was not called\n");
		return NULL;
	}

	/* Guard against nmemb * size overflow before it happens. */
	if (nmemb != 0 && size > (size_t)-1 / nmemb)
	{
		fprintf(stderr, "gc_calloc: requested size overflows size_t\n");
		return NULL;
	}

	void *ptr = calloc(nmemb, size);
	if (!ptr) return NULL;

	size_t total_size = nmemb * size;

	if (!list_append(&gc_head, &gc_tail, ptr, total_size, next_alloc_id, file, line))
	{
		free(ptr);
		return NULL;
	}

	next_alloc_id++;
	record_new_allocation(total_size);

	return ptr;
}

void *gc_calloc(size_t nmemb, size_t size)
{
	return gc_calloc_dbg(nmemb, size, NULL, 0);
}

/* ==========================================
 * gc_realloc
 * ========================================== */
void *gc_realloc_dbg(void *ptr, size_t size, const char *file, int line)
{
	if (!is_initialized)
	{
		fprintf(stderr, "gc_realloc: gc_init() was not called\n");
		return NULL;
	}

	if (ptr == NULL)
	{
		return gc_alloc_dbg(size, file, line);
	}

	GC_Node *node = list_find(gc_head, ptr);
	if (!node)
	{
		fprintf(stderr, "gc_realloc: pointer %p is not tracked by gc\n", ptr);
		return NULL;
	}

	void *new_ptr = realloc(ptr, size);
	if (!new_ptr) return NULL;

	size_t old_size = node->size;

	node->ptr = new_ptr;
	node->size = size;
	node->file = file;
	node->line = line;

	/* Only count growth toward the historical "total allocated" figure. */
	if (size > old_size) total_memory_allocated += (size - old_size);

	current_live_memory = current_live_memory - old_size + size;

	if (size > largest_allocation) largest_allocation = size;
	if (current_live_memory > peak_memory_usage) peak_memory_usage = current_live_memory;

	return new_ptr;
}

void *gc_realloc(void *ptr, size_t size)
{
	return gc_realloc_dbg(ptr, size, NULL, 0);
}

/* ==========================================
 * gc_stats (v2: peak / largest / average / live)
 * ========================================== */
void gc_stats(void)
{
	double avg = total_allocations
		? (double)total_memory_allocated / (double)total_allocations
		: 0.0;

	printf(CLR_CYAN "----------- GC STATS ------------\n" CLR_RESET);
	printf("Tracked blocks         : %zu\n", total_allocations);
	printf("Total memory allocated : %zu bytes\n", total_memory_allocated);
	printf("Current live memory    : %zu bytes\n", current_live_memory);
	printf("Peak memory usage      : %zu bytes\n", peak_memory_usage);
	printf("Largest allocation     : %zu bytes\n", largest_allocation);
	printf("Average allocation     : %.2f bytes\n", avg);
	printf(CLR_CYAN "----------------------------------\n" CLR_RESET);
}

/* ==========================================
 * gc_clean (v2: flag-based report system)
 * ========================================== */
void gc_clean(int report_flags)
{
	if (is_cleaned) return;

	bool show_stats  = (report_flags & GC_SHOW_STATS)  != 0;
	bool show_blocks = (report_flags & GC_SHOW_BLOCKS) != 0;

	size_t total_freed = 0;
	size_t blocks_freed = 0;

	if (show_stats || show_blocks)
	{
		printf(CLR_BOLD CLR_GREEN "\n=========================================\n");
		printf("         GC CLEANUP REPORT\n");
		printf("=========================================\n" CLR_RESET);
	}

	GC_Node *curr = gc_head;

	while (curr != NULL)
	{
		if (show_blocks)
		{
			printf(CLR_YELLOW "Block #%zu\n" CLR_RESET, curr->id);
			printf("  Address : %p\n", curr->ptr);
			printf("  Size    : %zu bytes\n", curr->size);
			if (curr->file)
			{
				printf("  Origin  : %s:%d\n", curr->file, curr->line);
			}
			printf("  Status  : Released\n");
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

	if (show_stats)
	{
		size_t remaining = (total_memory_allocated >= total_freed)
			? total_memory_allocated - total_freed : 0;

		printf(CLR_CYAN "Total Allocations       : %zu\n" CLR_RESET, total_allocations);
		printf("Total Memory Allocated  : %zu bytes\n", total_memory_allocated);
		printf("Total Memory Freed      : %zu bytes\n", total_freed);
		printf("Blocks Released         : %zu\n", blocks_freed);
		printf("Memory Remaining        : %zu bytes\n", remaining);
		printf("Peak Memory Usage       : %zu bytes\n", peak_memory_usage);
		printf("Largest Allocation      : %zu bytes\n", largest_allocation);
		printf(CLR_GREEN "Cleanup Status          : SUCCESS\n" CLR_RESET);
		printf(CLR_BOLD CLR_GREEN "=========================================\n\n" CLR_RESET);
	}

	/* Reset statistics after cleanup (roadmap item). Note: is_cleaned stays
	 * true below, so the collector still won't track new allocations after
	 * this point in the current single-lifecycle design. */
	total_allocations = 0;
	total_memory_allocated = 0;
	current_live_memory = 0;
	peak_memory_usage = 0;
	largest_allocation = 0;
	next_alloc_id = 1;

	is_cleaned = true;
}
