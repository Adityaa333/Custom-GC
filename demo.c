#include "gc.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
	/* gc_init() registers the atexit handler and enables tracking.
	 * Nothing can be allocated before this runs. */
	gc_init();

	printf("== Plain allocation (no call-site tracking) ==\n");
	int *nums = gc_alloc(sizeof(int) * 5);
	for (int i = 0; i < 5; i++) nums[i] = i * i;
	printf("nums: ");
	for (int i = 0; i < 5; i++) printf("%d ", nums[i]);
	printf("\n\n");

	printf("== Debug-tracked allocation (records file:line) ==\n");
	char *greeting = GC_ALLOC(32);
	strcpy(greeting, "hello, gc");
	printf("greeting: %s\n\n", greeting);

	printf("== GC_CALLOC: zero-initialized block ==\n");
	int *zeros = GC_CALLOC(10, sizeof(int));
	printf("zeros[3] = %d (should be 0)\n\n", zeros[3]);

	printf("== GC_REALLOC: grow a tracked block ==\n");
	greeting = GC_REALLOC(greeting, 64);
	strcat(greeting, " world");
	printf("greeting after realloc: %s\n\n", greeting);

	printf("== gc_stats(): live snapshot before cleanup ==\n");
	gc_stats();
	printf("\n");

	/* Explicit cleanup with the full report (stats + per-block detail).
	 * If you skip this call, gc_init()'s atexit handler will still run
	 * gc_clean(GC_REPORT_FULL) automatically when main() returns. */
	printf("== gc_clean(GC_REPORT_FULL) ==\n");
	gc_clean(GC_REPORT_FULL);

	return 0;
}
