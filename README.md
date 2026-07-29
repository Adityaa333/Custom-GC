# Custom-GC

A small tracking garbage collector for C. It's not a real mark-and-sweep
collector (yet — see the roadmap) — it's closer to an arena allocator with
leak reporting: every allocation you make through the library is recorded
in a linked list, and `gc_clean()` walks that list and frees everything in
one shot, optionally printing a report as it goes. If you forget to call
`gc_clean()`, it runs automatically at program exit.

## Features

- Drop-in wrappers for `malloc`/`calloc`/`realloc` that are automatically
  tracked and cleaned up
- Automatic cleanup via `atexit()` — no leaks even if you forget to clean up
- Debug-tracked allocation macros that record the call site (`__FILE__` /
  `__LINE__`) of every allocation
- Flag-based report system (`GC_SHOW_STATS`, `GC_SHOW_BLOCKS`, or both) with
  colored terminal output
- Live stats: tracked block count, total/live/peak memory, largest
  allocation, average allocation size

## Project layout

```
.
├── Makefile
├── demo.c              # example usage of the library
├── include/
│   └── gc.h            # public API
└── src/
    ├── gc.c             # collector implementation
    ├── list.c           # internal tracking list (private)
    └── list.h
```

`gc.c` and `list.c` compile into a static library, `libgc.a`. Every other
`.c` file in the project root is automatically built into its own
executable, linked against that library — so you can drop in as many demo
or test programs as you like without touching the Makefile.

## Building and running

```sh
make          # builds libgc.a and every *.c program in the root dir
./demo        # run the example
make clean    # remove build artifacts
```

## Quick start

```c
#include "gc.h"

int main(void)
{
    gc_init();                       /* required before any allocation */

    int *nums = GC_ALLOC(sizeof(int) * 5);   /* tracked, records call site */
    for (int i = 0; i < 5; i++) nums[i] = i;

    char *s = GC_CALLOC(32, sizeof(char));   /* zero-initialized, tracked */
    s = GC_REALLOC(s, 64);                   /* grow a tracked block */

    gc_stats();                      /* print current stats */

    gc_clean(GC_REPORT_FULL);        /* free everything, print full report */
    return 0;
}
```

If you never call `gc_clean()` yourself, `gc_init()`'s `atexit()` handler
calls `gc_clean(GC_REPORT_FULL)` for you when the program exits.

See `demo.c` for a fuller example, including plain (non-debug) allocation
and the difference in report output between tracked and untracked call
sites.

## API

### Core

| Function | Description |
|---|---|
| `void gc_init(void)` | Enables tracking and registers the exit-time cleanup handler. Safe to call more than once — later calls are no-ops. Must be called before any allocation function. |
| `void gc_clean(int report_flags)` | Frees every tracked allocation. `report_flags` is a bitmask (see below). Safe to call more than once — later calls are no-ops. |
| `void gc_stats(void)` | Prints a snapshot of current tracked-memory statistics. |

### Memory (untracked call site)

| Function | Description |
|---|---|
| `void *gc_alloc(size_t size)` | Tracked `malloc`. |
| `void *gc_calloc(size_t nmemb, size_t size)` | Tracked `calloc`, with overflow checking on `nmemb * size`. |
| `void *gc_realloc(void *ptr, size_t size)` | Tracked `realloc`. `ptr` must have been returned by one of this library's allocation functions. |

### Memory (debug-tracked call site)

| Macro | Description |
|---|---|
| `GC_ALLOC(size)` | Same as `gc_alloc`, but records `__FILE__`/`__LINE__`. |
| `GC_CALLOC(nmemb, size)` | Same as `gc_calloc`, but records `__FILE__`/`__LINE__`. |
| `GC_REALLOC(ptr, size)` | Same as `gc_realloc`, but records `__FILE__`/`__LINE__`. |

Blocks allocated through the `GC_*` macros show an `Origin: file:line` line
in the block report; blocks allocated through the plain `gc_*` functions do
not.

### Report flags

`gc_clean()` takes a bitmask:

| Flag | Effect |
|---|---|
| `GC_REPORT_NONE` | Free everything silently. |
| `GC_SHOW_STATS` | Print the summary footer (totals, peak, largest, status). |
| `GC_SHOW_BLOCKS` | Print a line-item for every block as it's released. |
| `GC_REPORT_SUMMARY` | Alias for `GC_SHOW_STATS`. |
| `GC_REPORT_FULL` | `GC_SHOW_STATS \| GC_SHOW_BLOCKS`. |

```c
gc_clean(GC_REPORT_NONE);
gc_clean(GC_REPORT_SUMMARY);
gc_clean(GC_REPORT_FULL);
gc_clean(GC_SHOW_STATS | GC_SHOW_BLOCKS);   /* same as GC_REPORT_FULL */
```

## Known limitations

- **Single lifecycle.** Once `gc_clean()` has run, the collector is
  permanently inert for the rest of the process — there's no way to
  re-initialize and resume tracking. This is by design for the current
  "init → run → exit" model.
- **No individual free.** There's no `gc_free(ptr)` to release a single
  block early; everything is freed together by `gc_clean()`.
- **`gc_realloc`/`GC_REALLOC` require a tracked pointer.** Passing a
  pointer this library didn't allocate will fail with a warning rather
  than silently falling back to plain `realloc`.
- **Colors are not TTY-aware.** Report output always includes ANSI color
  codes, which will show as raw escape sequences if redirected to a file
  or a non-color terminal.

## Roadmap

See [`roadmap.md`](roadmap.md) for the full version plan. Implemented so
far:

- **v1** — core `gc_init`/`gc_alloc`/`gc_clean`/`gc_stats`, automatic
  `atexit()` cleanup, O(1) list append, double-cleanup and double-init
  guards, stats reset after cleanup
- **v2** — `gc_calloc`, `gc_realloc`, richer stats (peak/largest/average/
  live memory), flag-based report system, colored output, allocation IDs
- **v3 (partial — Debugging only)** — `__FILE__`/`__LINE__` call-site
  tracking via `GC_ALLOC()`/`GC_CALLOC()`/`GC_REALLOC()`

Still open: v3's performance work (memory pool allocator, hash table) and
thread safety (pthread mutex), plus v4's mark-and-sweep, reference
counting, and reporting ideas.
