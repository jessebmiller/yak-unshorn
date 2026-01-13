#ifndef OX_EVENT_TYPES_H
#define OX_EVENT_TYPES_H

#include <stdint.h>
#include "../bunuelib.h"

typedef enum {
	OX_EVENT_ZERO = 0,

	OX_EVENT_WINDOW_EXPOSED,

	OX_EVENT_KEY_DOWN,
	OX_EVENT_KEY_UP,

	OX_EVENT_TEXT_INPUT,

	OX_EVENT_QUIT,
	
	OX_EVENT_LAST
} ox_EventType;

typedef struct {
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;
} ox_EventCommon;

typedef struct {
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;
} ox_WindowExposed;

// TODO change quit to quit cmd
typedef struct {
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;		
} ox_Quit;

typedef enum {
	OX_KEY_NULL,
	OX_KEY_ESCAPE,
	OX_KEY_A,
	OX_KEY_B,
	OX_KEY_Q,
} ox_Key;

typedef struct {
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;
	ox_Key key;
} ox_KeyPress;

typedef union {
	uint32_t type;			// ox_EventType
	ox_EventCommon common;
	ox_KeyPress key_press;		// OX_KEY_DOWN, OX_KEY_UP
	ox_Quit quit;			// OX_EVENT_QUIT
} ox_Event;

// TODO:8 events is overflowing because we don't remove them...
#define EVENT_MAX 16
typedef struct {
	ox_Event events[EVENT_MAX];
	size_t offset;
	
	ox_Event* tombstones[EVENT_MAX];
	size_t tombstone_offset;
} ox_EventArena;

typedef struct ox_Subscription {
	int id;
	void (*callback)(ox_Event* event, void* user_data);
	void* user_data;
	struct ox_Subscription* next;
} ox_Subscription;

#define SUB_MAX 1024
BUNUEL_FIXED_SET(ox_sub, ox_Subscription, SUB_MAX) {
	return p->id == q->id;
}

#define TOPICS_COUNT OX_EVENT_LAST
typedef struct {
	ox_Subscription* topics[TOPICS_COUNT];
	ox_SubscriptionSet subscription_set;
	ox_EventArena event_arena;
} ox_EventSystem;

#endif

