#ifndef GC_H
#define GC_H

#include <stddef.h>
#include <stdbool.h>

void gc_init(void);
void *gc_alloc(size_t size);
void gc_stats(void);
void gc_clean(bool print_report);

#endif
