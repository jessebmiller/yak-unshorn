#ifndef OX_KERNEL_EVENT_H
#define OX_KERNEL_EVENT_H

#include <stdbool.h>
#include "arena.h"

typedef enum {
	OX_EVENT_QUIT;
	OX_EVENT_LAST;
} ox_EventType

typedef struct{
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;		
} ox_Quit

typedef struct {
	ox_EventType type;
	uint32_t reserved;
	uint64_t timestamp;
} ox_OpenWindow

typedef union {
	uint32_t type;			// ox_EventType
	ox_OpenWindow open_window;	// OX_EVENT_OPEN_WINDOW
	ox_Quit quit;			// OX_EVENT_QUIT
} ox_Event

typedef struct {
	void (*callback)(const ox_Event* event, const void* user_data);
	void* user_data;
	uint32_t id;
	ox_Subscription* next;
} ox_Subscription

#define SUB_MAX = 1024
#define TOPICS_COUNT = OX_LAST_EVENT

typedef struct {
	ox_Subscription sub_store[sizeof(ox_Subscription) * SUB_MAX];
	size_t sub_store_offset;
	ox_Subscription* unsubbed[sizeof(ox_Subscription*) * SUB_MAX];
	size_t unsubbed_offset;
	ox_Subscription* topics[TOPICS_COUNT];
} ox_EventSystem

// ox_publish_event queues an event to be delivered to subscribers
bool ox_publish_event(ox_EventSystem event_system, ox_Event* event);

// ox_dispatch_next blocks until the next event is published and dispatches it to subscribers
bool ox_dispatch_next(ox_EventSystem* event_system);

// ox_subscribe_events registers a callback for events of the given type
uint32_t ox_subscribe_events(
	ox_EventType type,
	void (*callback)(const ox_Event* event, const void* user_data),
	void* user_data
);

// ox_unsubscribe unsubscribes a subscription by id
void ox_unsubscribe(ox_EventSystem* event_system, size_t id);

#endif
