# Custom-GC Roadmap

## Current Version (v1.0)

### Implemented

- [x] gc_init()
- [x] gc_alloc()
- [x] gc_clean()
- [x] gc_stats()
- [x] Automatic cleanup using atexit()
- [x] Linked-list allocation tracking
- [x] Allocation size tracking
- [x] Cleanup report
- [x] Double cleanup prevention

---

## Corrections 

- [x] list_append() should return bool instead of void.
- [x] Free allocated memory if metadata allocation fails.
- [x] Prevent multiple gc_init() calls.
- [ ] Reset statistics after cleanup.
- [x] Add tail pointer for O(1) insertion.

---

## Version 2

### Memory APIs

- [ ] gc_calloc()
- [ ] gc_realloc()

### Statistics

- [ ] Peak memory usage
- [ ] Largest allocation
- [ ] Average allocation size
- [ ] Current live memory

### Report System

- [ ] Report flags

Example

```c
gc_clean(GC_REPORT_NONE);

gc_clean(GC_REPORT_SUMMARY);

gc_clean(GC_REPORT_FULL);

gc_clean(GC_SHOW_STATS | GC_SHOW_BLOCKS);
```

- [ ] Optional colored output

### Metadata

- [ ] Allocation ID

---

## Version 3

### Debugging

- [ ] Store __FILE__
- [ ] Store __LINE__
- [ ] GC_ALLOC() macro

### Performance

- [ ] Memory pool allocator
- [ ] Hash table instead of linked list

### Thread Safety

- [ ] pthread mutex protection

### Reports

- [ ] Sort allocations by size
- [ ] Allocation timestamps
- [ ] Memory timeline

---

## Version 4 (Future Ideas)

- [ ] Reference counting
- [ ] Mark-and-sweep collector
- [ ] Weak references
- [ ] JSON report export
- [ ] Interactive CLI
- [ ] Visual memory map

---

## Notes

