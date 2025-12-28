#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "event.h"
#include "arena.h"

// ox_publish_event queues an event to be delivered to subscribers
bool ox_publish_event(oxEventSystem event_system, ox_Event* event) {
	SDL_Event sdl_event;
	SDL_Zero(sdl_event);
	switch(event.type) {
		case OX_EVENT_QUIT:
			sdl_event.type = SDL_QUIT_EVENT;
			sdl_event.timestamp = SDL_GetTicksNS();
			break;
		case default:
			// TODO handle unknown event type error/warning
			// TODO handle module/user event types
			return false;
	}
			
	return SDL_PushEvent(sdl_event);
}

// ox_dispatch_next blocks until the next event is published and dispatches it to subscribers
bool ox_dispatch_next(ox_EventSystem event_system) {
	SDL_Event event;
	if (!SDL_WaitEvent(&event)) {
		// TODO expose SDL_GetError() maybe with error events?!?
		return false
	}
	
	ox_Subscription* sub = event_system.subs[event.type];
	while(sub != NULL) {
		if (sub.callback == NULL) {
			// TODO register this error somewhere
			// Skip this one
			sub = sub.next;
			break;
		}
		sub.callback(event, sub.user_data);
		sub = sub.next;
	}

	return true
}


// ox_subscribe_events registers a callback for events of the given type
uint32_t ox_subscribe_events(
		ox_EventStytem* es,
		ox_EventType type,
		void (*callback)(const ox_Event* event, const void* user_data),
		void* user_data
) {
	ox_Subscription* sub;

	// if there is an unsubbed sub available use it
	if (es->unsubbed_offset) {
		sub = es->unsubbed[es->unsubbed_offset];
		es->unsubbed_offset -= 1;
	} else if (es->sub_store_offset < SUB_MAX) {
		// use the next unused subscription in the sub_store
		sub = &es->sub_store[es->sub_store_offset];
		es->sub_store_offset += 1;
	} else {
		fprintf(stderr, "Error: Max subscriptions");
		return 0;
	}

	uint32_t id = rand();
	
	sub->callback = callback;
	sub->user_data = user_data;
	sub->id = id;
	sub->next =  event_system->subs[type];
	
	event_system->subs[type] = sub;

	return id;
}

ox_Subscription* find_by_id(ox_Subscription* sub_list, size_t id) {
	ox_Subscription* sub = sub_list;
	while (sub != NULL) {
		if (sub->id == id) return sub;
		sub = sub->next;
	}
	return NULL
}

// ox_unsubscribe unsubscribes a subscription by id
int ox_unsubscribe(ox_EventSystem* es, size_t id) {
	// scan subscriptions in topics for id
	ox_Subscription* found
	for (int i = 0; i < TOPICS_COUNT; i++) {
		found = find_by_id(es->topics[i]);
		if (found != NULL) break;
	}

	if (found == NULL) return 0;

	// TODO figure out how to get the previous one too
	// so we can move the found one to the unsubbed array
	// and patch up the linked list
}
