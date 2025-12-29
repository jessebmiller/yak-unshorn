#ifndef OX_KERNEL_EVENT_H
#define OX_KERNEL_EVENT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <SDL3/SDL.h>

#include "event_types.h"

// oxi_time_ns returns the number of nanoseconds since the unix epoch
uint64_t oxi_time_ns();

// oxi_make_event_system returns a pointer to a newly initialized event system
ox_EventSystem* oxi_make_event_system();

// oxi_destroy_event_system frees all allocations fo the event system
bool oxi_destroy_event_system(ox_EventSystem* es);

// oxi_make_event returns a zero initialized event of the given type
ox_Event* oxi_make_event(ox_EventSystem* event_system, ox_EventType type);

// oxi_destroy_event frees the memory for an ox_Event*
bool oxi_destroy_event(ox_EventSystem* event_system, ox_Event* event);

// oxi_from_sdl_event translates an SDL_Event to an ox_Event
ox_Event* oxi_from_sdl_event(ox_EventSystem* event_system, SDL_Event* sdl_event);

// oxi_from_sdl_key translates an SDL_Key to an ox_Key
ox_Key oxi_from_sdl_key(SDL_Keycode sdl_key);

// ox_from_sdl_event translates an ox_Event to an SDL_Event
bool oxi_to_sdl_event(ox_Event* ox_event, SDL_Event* sdl_event);

// ox_publish_event queues an event to be delivered to subscribers
bool oxi_publish_event(ox_EventSystem* event_system, ox_Event* event);

// ox_dispatch_next blocks until the next event is published and dispatches it to subscribers
bool oxi_dispatch_next(ox_EventSystem* event_system);

// ox_subscribe_events registers a callback for events of the given type
int oxi_subscribe_events(
	ox_EventSystem* event_system,
	ox_EventType type,
	void (*callback)(const ox_Event* event, const void* user_data),
	void* user_data
);

// ox_unsubscribe unsubscribes a subscription by id returns number of subscriptions unsubbed
int oxi_unsubscribe(ox_EventSystem* event_system, size_t id);

#endif
