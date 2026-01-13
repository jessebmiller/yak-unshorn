#ifndef BUNUELIB_H
#define BUNUELIB_H

// TODO:2 document usage and consider compare function
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
		set->items[set->count++] = item; \
		return &set->items[set->count]; \
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

// TODO define a linked list

#endif // BUNUELIB_H
