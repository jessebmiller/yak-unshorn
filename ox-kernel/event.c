#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>

#include "event.h"

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
		ox_EventStytem event_system,
		ox_EventType type,
		void (*callback)(const ox_Event* event, const void* user_data),
		void* user_data
) {
	
}
