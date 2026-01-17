#ifndef BUNUELIB_H
#define BUNUELIB_H

#define BUNUEL_LEN(array) (sizeof(array)/sizeof((array)[0]))

// TODO:2 Consider renaming fixed set to capped set or capacity set
///////// the set is not fixed but it does have a capped capacity
///////// its pre allocated

// TODO:1 document usage
#define BUNUEL_FIXED_SET(prefix, T, capacity) \
	typedef struct { T items[capacity]; int count; } T##Set; \
	static inline bool prefix##_eq(T* a, T* b); \
	\
	static inline int prefix##_index(T##Set* set, T* item) { \
		for (int i = 0; i < set->count; i++) { \
			if (prefix##_eq(&set->items[i], item)) return i; \
		} \
		return -1; \
	} \
	\
	static inline T* prefix##_add(T##Set* set, T item) { \
		int i = prefix##_index(set, &item); \
		if (i >= 0) return &set->items[i]; \
		if (set->count >= capacity) return NULL; \
		i = set->count; \
		set->count += 1; \
		set->items[i] = item; \
		return &set->items[i]; \
	} \
	\
	static inline bool prefix##_remove(T##Set* set, T* item) { \
		int i = prefix##_index(set, item); \
		if (i < 0) return false; \
		set->count -= 1; \
		set->items[i] = set->items[set->count]; \
		return true; \
	}\
	\
	static inline int prefix##_avail(T##Set* set) { \
		return capacity - set->count; \
	} \
	\
	static inline bool prefix##_eq(T* p, T* q)

// TODO define a capped linked list

/*
 * # Allocator backed data structures
 *
 * Like ring buffers, vectors, queues, stacks
 * backed by various allocation strategies and
 * locations like arenas, static, dynamic, heap,
 * stack, mmap, bump
 *
 * ## Fundamental building blocks
 *
 * ### getting backing memory from the OS
 *
 * - mmap
 * - static (BSS, data segment)
 * - heap (malloc)
 * - disk backed?!?
 *
 * ### Allocation/free strategies
 *
 * - Linear memory blocks (bump)
 * - Buddy allocation
 * - Alignment
 * - Pools (homogenous type)
 * - Bitmaps
 * - Free/allocated lists
 * - Circular indexing
 * - arenas (resettable/freeable aggregates)
 *
 * ### Data structures on top
 *
 * - ring buffer
 * - stack
 * - queue
 * - vector
 * - list
 * - string
 * - map
 * - graph
 * - trees, tries
 */

#ifdef BUNUEL_STRIP_PREFIX

#define LEN(array) BUNUEL_LEN(array)

#endif //BUNUEL_STRIP_PREFIX

#endif // BUNUELIB_H
