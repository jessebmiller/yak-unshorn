#define BUNUELIB_STRIP_PREFIX

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "bunuelib.h"

static LogLevel get_log_level() {
	char* level = getenv("BUNUEL_LOG_LEVEL");
	if (!level) return LOG_LEVEL_UNKNOWN;
	if (strcmp(level, "FATAL") == 0) {
		return LOG_LEVEL_FATAL;
	} else if (strcmp(level, "ERROR") == 0) {
		return LOG_LEVEL_ERROR;
	} else if (strcmp(level, "WARNING") == 0) {
		return LOG_LEVEL_WARNING;
	} else if (strcmp(level, "INFO") == 0) {
		return LOG_LEVEL_INFO;
	} else if (strcmp(level, "DEBUG") == 0) {
		return LOG_LEVEL_DEBUG;
	}
	return LOG_LEVEL_UNKNOWN;
}

static int print_standard_log_prefix(LogLevel log_level) {
	return printf("%s %s %s ", __DATE__, __TIME__, log_level_name(log_level));
}

static int print_file_line_log_prefix() {
	return printf("%s:%d ", __FILE__, __LINE__);
}

static int print_function_log_prefix() {
	return printf("%s ", __FUNCTION__);
}

static int unknown_prefix_already_warned = false;

static int print_log_prefix(LogLevel log_level) {
	char* prefix_type = getenv("BUNUEL_LOG_PREFIX_TYPE");
	int printed = 0;
	if (prefix_type == NULL) {
		return printed;
	} else if (strcmp(prefix_type, "standard") == 0) {
		printed += print_standard_log_prefix(log_level);
		return printed;
	} else if (strcmp(prefix_type, "function") == 0) {
		printed += print_standard_log_prefix(log_level);
		printed += print_function_log_prefix();
		return printed;
	} else if (strcmp(prefix_type, "line") == 0) {
		printed += print_standard_log_prefix(log_level);
		printed += print_file_line_log_prefix();
		return printed;
	} else if (strcmp(prefix_type, "full") == 0) {
		printed += print_standard_log_prefix(log_level);
		printed += print_file_line_log_prefix();
		printed += print_function_log_prefix();
		return printed;
	} else if (!unknown_prefix_already_warned) {
		printed += printf("unknown env BUNUEL_LOG_PREFIX_TYPE %s, "
		       "expected one of 'standard', "
		       "'function', 'line', 'full'\n", prefix_type);
		unknown_prefix_already_warned = true;
		return printed;
	}
	return printed;
}

static int vlog_line(
	bunuel_LogLevel log_level,
	const char *restrict format, 
	va_list args
) {
	int printed = 0;
	if (log_level > get_log_level()) return printed;
	printed += print_log_prefix(log_level);
	printed += vprintf(format, args);
	putchar('\n');
	printed += 1;
	return printed;
}

char* log_level_name(LogLevel log_level) {
	switch(log_level) {	
	case(LOG_LEVEL_FATAL):   return "FATAL";
	case(LOG_LEVEL_ERROR):   return "ERROR";
	case(LOG_LEVEL_WARNING): return "WARNING";
	case(LOG_LEVEL_INFO):    return "INFO";
	case(LOG_LEVEL_DEBUG):   return "DEBUG";
	case(LOG_LEVEL_UNKNOWN): return "LOG_LEVEL_UNKNOWN";
	default: assert(false && "UNREACHABLE");
	}
}

#define DEF_LOGGER(name, LOG_LEVEL) \
int name(const char *restrict format, ...) {\
	va_list args;\
	va_start(args, format);\
	int printed = vlog_line(LOG_LEVEL, format, args);\
	va_end(args);\
	return printed;\
}

DEF_LOGGER(fatal, LOG_LEVEL_FATAL)
DEF_LOGGER(error, LOG_LEVEL_ERROR)
DEF_LOGGER(warning, LOG_LEVEL_WARNING)
DEF_LOGGER(info, LOG_LEVEL_INFO)
DEF_LOGGER(debug, LOG_LEVEL_DEBUG)

Arena* bunuel_make_arena(size_t capacity, size_t elem_size) {
	int size = capacity * elem_size;
	size += sizeof(Arena);
	Arena* arena = malloc(size);
	if (!arena) return NULL;
	
	arena->elem_size = elem_size;
	arena->length = 0;
	arena->capacity = capacity;
	
	return arena;
}

void bunuel_drop_arena(Arena* arena) {
	free(arena);
}

void bunuel_wipe_arena(Arena* arena) {
	arena->length = 0;
}

void* bunuel_bump_arena(Arena* arena) {
	assert(arena->length <= arena->capacity);
	if (arena->length >= arena->capacity) return NULL;

	char* block = arena->data + (arena->length * arena->elem_size); 
	arena->length += 1;
	return (void*)block;
}

Pool* bunuel_make_pool(size_t capacity, size_t elem_size) {
	assert(elem_size >= sizeof(void*));
	if (elem_size < sizeof(void*)) return NULL;

	Pool* pool = malloc(sizeof(Pool));
	if (!pool) return NULL;

	pool->free_list = NULL;
	pool->arena = make_arena(capacity, elem_size);

	return pool;
}

void bunuel_drop_pool(bunuel_Pool* pool) {
	assert(pool != NULL);
	if(!pool) return;
	drop_arena(pool->arena);
	free(pool);
}

void bunuel_wipe_pool(bunuel_Pool* pool) {
	pool->free_list = NULL;
	wipe_arena(pool->arena);
}

void* bunuel_take_pool(bunuel_Pool* pool) {
	// bump arena if nothing in free list
	if (!pool->free_list) return bump_arena(pool->arena);
	
	// otherwise use the free list
	void* elem = pool->free_list;
	pool->free_list = *(void**)pool->free_list;
	return elem;
}

void bunuel_give_pool(bunuel_Pool* pool, void* elem) {
	*(void**)elem = pool->free_list;
	pool->free_list = elem;
}
