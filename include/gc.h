#ifndef GC_H
#define GC_H

#include <stddef.h>
#include <stdbool.h>

/* ==========================================
 * Report flags (v2)
 *
 * gc_clean() takes a bitmask of these flags to
 * control what gets printed at cleanup time.
 *
 * Example:
 *   gc_clean(GC_REPORT_NONE);
 *   gc_clean(GC_REPORT_SUMMARY);
 *   gc_clean(GC_REPORT_FULL);
 *   gc_clean(GC_SHOW_STATS | GC_SHOW_BLOCKS);
 * ========================================== */
typedef enum
{
    GC_REPORT_NONE    = 0,
    GC_SHOW_STATS     = 1 << 0, /* print the summary/stats footer   */
    GC_SHOW_BLOCKS    = 1 << 1, /* print a line per released block  */
    GC_REPORT_SUMMARY = GC_SHOW_STATS,
    GC_REPORT_FULL    = GC_SHOW_STATS | GC_SHOW_BLOCKS
} GC_ReportFlags;

/* ==========================================
 * Core API
 * ========================================== */
void gc_init(void);
void gc_clean(int report_flags);
void gc_stats(void);

/* ==========================================
 * Memory APIs (v1 / v2)
 * ========================================== */
void *gc_alloc(size_t size);
void *gc_calloc(size_t nmemb, size_t size);
void *gc_realloc(void *ptr, size_t size);

/* ==========================================
 * Debug-tracked memory APIs (v3 - Debugging)
 *
 * These record the call site (__FILE__/__LINE__)
 * for each allocation, shown in block reports.
 * Prefer the GC_ALLOC()/GC_CALLOC()/GC_REALLOC()
 * macros below over calling these directly.
 * ========================================== */
void *gc_alloc_dbg(size_t size, const char *file, int line);
void *gc_calloc_dbg(size_t nmemb, size_t size, const char *file, int line);
void *gc_realloc_dbg(void *ptr, size_t size, const char *file, int line);

#define GC_ALLOC(size)       gc_alloc_dbg((size), __FILE__, __LINE__)
#define GC_CALLOC(nmemb, sz) gc_calloc_dbg((nmemb), (sz), __FILE__, __LINE__)
#define GC_REALLOC(ptr, sz)  gc_realloc_dbg((ptr), (sz), __FILE__, __LINE__)

#endif
