#ifndef BUNUELIB_H
#define BUNUELIB_H

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define BUNUEL_LEN(array) (sizeof(array)/sizeof((array)[0]))

typedef enum {
	BUNUEL_LOG_LEVEL_FATAL,
	BUNUEL_LOG_LEVEL_ERROR,
	BUNUEL_LOG_LEVEL_WARNING,
	BUNUEL_LOG_LEVEL_INFO,
	BUNUEL_LOG_LEVEL_DEBUG,
	BUNUEL_LOG_LEVEL_UNKNOWN,
} bunuel_LogLevel;

char* bunuel_log_level_name(bunuel_LogLevel log_level);

int bunuel_fatal(const char *restrict format, ...);
int bunuel_error(const char *restrict format, ...);
int bunuel_warning(const char *restrict format, ...);
int bunuel_info(const char *restrict format, ...);
int bunuel_debug(const char *restrict format, ...);

int logln(const char *restrict format, ...);

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

typedef struct {
	size_t elem_size;
	size_t length;
	size_t capacity;
	char data[];
} bunuel_Arena;

bunuel_Arena* bunuel_make_arena(size_t capacity, size_t elem_size);
void bunuel_drop_arena(bunuel_Arena* arena);
void bunuel_wipe_arena(bunuel_Arena* arena);
void* bunuel_bump_arena(bunuel_Arena* arena);

typedef struct {
	void* free_list;
	bunuel_Arena* arena;
} bunuel_Pool;

bunuel_Pool* bunuel_make_pool(size_t capacity, size_t elem_size);
void bunuel_drop_pool(bunuel_Pool* pool);
void bunuel_wipe_pool(bunuel_Pool* pool);
void* bunuel_take_pool(bunuel_Pool* pool);
void bunuel_give_pool(bunuel_Pool* pool, void* elem);

/*
 * # Allocator backed collections
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
 * - Linear memory arenas (bump)
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

#ifdef BUNUELIB_STRIP_PREFIX

#define LEN(array) BUNUEL_LEN(array)

#define Arena bunuel_Arena
#define make_arena bunuel_make_arena
#define drop_arena bunuel_drop_arena
#define wipe_arena bunuel_wipe_arena
#define bump_arena bunuel_bump_arena

#define Pool bunuel_Pool
#define make_pool bunuel_make_pool
#define drop_pool bunuel_drop_pool
#define wipe_pool bunuel_wipe_pool
#define take_pool bunuel_take_pool
#define give_pool bunuel_give_pool

#define LogLevel bunuel_LogLevel
#define log_level_name bunuel_log_level_name
#define LOG_LEVEL_FATAL   BUNUEL_LOG_LEVEL_FATAL
#define LOG_LEVEL_ERROR   BUNUEL_LOG_LEVEL_ERROR
#define LOG_LEVEL_WARNING BUNUEL_LOG_LEVEL_WARNING
#define LOG_LEVEL_INFO    BUNUEL_LOG_LEVEL_INFO
#define LOG_LEVEL_DEBUG   BUNUEL_LOG_LEVEL_DEBUG
#define LOG_LEVEL_UNKNOWN BUNUEL_LOG_LEVEL_UNKNOWN
#define fatal bunuel_fatal
#define error bunuel_error
#define warning bunuel_warning
#define info bunuel_info
#define debug bunuel_debug

#endif //BUNUEL_STRIP_PREFIX

#ifdef NDEBUG // turn of dbg in production builds
  #define dbg(...) ((void)0)
#else
  #define dbg(...) bunuel_debug(__VA_ARGS__)
#endif // NDEBUG

#endif // BUNUELIB_H
