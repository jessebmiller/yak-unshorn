#ifndef OX_EVENT_TYPES_H
#define OX_EVENT_TYPES_H

#include <stdint.h>

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

typedef struct ox_Subscription {
	void (*callback)(const ox_Event* event, const void* user_data);
	void* user_data;
	uint32_t id;
	struct ox_Subscription* next;
} ox_Subscription;

#define SUB_MAX 1024
typedef struct {
	ox_Subscription subs[SUB_MAX];
	size_t offset;
	
	ox_Subscription* tombstones[SUB_MAX];
	size_t tombstone_offset;
} ox_SubArena;

#define EVENT_MAX 16
typedef struct {
	ox_Event events[EVENT_MAX];
	size_t offset;
	
	ox_Event* tombstones[EVENT_MAX];
	size_t tombstone_offset;
} ox_EventArena;

#define TOPICS_COUNT OX_EVENT_LAST
typedef struct {
	ox_Subscription* topics[TOPICS_COUNT];
	ox_SubArena sub_arena;
	ox_EventArena event_arena;
} ox_EventSystem;

#endif

