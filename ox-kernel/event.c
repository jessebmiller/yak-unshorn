#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL3/SDL.h>

#include "event.h"
#include "event_types.h"

// ox_time_ns returns the number of nanoseconds since the unix epoch
uint64_t oxi_time_ns() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

// oxi_make_event_system returns a pointer to a newly initialized event system
ox_EventSystem* oxi_make_event_system() {
	ox_EventSystem* event_system = calloc(1, sizeof(ox_EventSystem));
	return event_system;
}

// oxi_destroy_event_system frees all allocations for the event system
bool oxi_destroy_event_system(ox_EventSystem* event_system) {
	free(event_system);
}

void ox_init_event(ox_Event* event, ox_EventType type) {
	event->type = type;
	event->common.timestamp = oxi_time_ns();
}

// NOTE operation to sync the h file from the c too
// NOTE operation to identify where h and c files are out of sync
// NOTE operation to make and manage a tracability matrix
// NOTE operation to jump from error message to code
// NOTE operation to pull notes into a central document

// ox_from_sdl_event transform SDL_Event to an ox_Event
bool oxi_from_sdl_event(SDL_Event sdl_event, ox_Event* event) {
	switch(sdl_event.type) {
		case SDL_EVENT_WINDOW_EXPOSED:
			ox_init_event(event, OX_EVENT_WINDOW_EXPOSED);
			break;

		case SDL_EVENT_KEY_UP:
		case SDL_EVENT_KEY_DOWN:
			ox_EventType type = sdl_event.type == SDL_EVENT_KEY_UP
				                            ? OX_EVENT_KEY_UP
							    : OX_EVENT_KEY_DOWN;
			ox_init_event(event, type);
			event->key_press.scancode = sdl_event.key.scancode;
			event->key_press.key = sdl_event.key.key;
			event->key_press.mod = sdl_event.key.mod;
			event->key_press.repeat = sdl_event.key.repeat;
			break;
		
		case SDL_EVENT_QUIT:
			ox_init_event(event, OX_EVENT_QUIT);
			break;
		
		default:
			return false;
	}
	return true;
}

// ox_from_sdl_event translates an ox_Event to an SDL_Event
bool oxi_to_sdl_event(ox_Event* ox_event, SDL_Event* sdl_event) {
	if (ox_event == NULL) {
		fprintf(stderr, "INFO: Trying to transform NULL ox_Event*");
		return NULL;
	}
	switch(ox_event->type) {
		case OX_EVENT_QUIT:
			sdl_event->type = SDL_EVENT_QUIT;
			sdl_event->quit.timestamp = ox_event->quit.timestamp;
			return true;
		default:
			fprintf(stderr, "WARN: Unknown ox_Event type %d\n", ox_event->type);
			return false;
	}
}

// TODO:2 consider passing ox_event by value
// ox_publish_event queues an event to be delivered to subscribers. Always destroys the ox_event
bool oxi_publish_event(ox_EventSystem* event_system, ox_Event* ox_event) {
	SDL_Event sdl_event;
	SDL_zero(sdl_event);
	bool could_translate = oxi_to_sdl_event(ox_event, &sdl_event);
	if (!could_translate) {
		printf("ERROR: Could not translate ox_Event.type(%d) to SDL_Event\n",
		       ox_event->type);
		return false;
	}
	return SDL_PushEvent(&sdl_event);
}

// ox_dispatch_next waits until the next event is published then dispatches it to subscribers
bool oxi_dispatch_next(ox_EventSystem* event_system) {
	ox_Event event;
	bool got_event = false;
	while (!got_event) {
		// Try translating SDL events until we find one we can translate
		SDL_Event sdl_event;
		if (!SDL_WaitEvent(&sdl_event)) {
			printf("ERROR: oxi_dispatch_next failed. %s\n", SDL_GetError());
			return false;
		}

		got_event = oxi_from_sdl_event(sdl_event, &event);
	}

	ox_Subscription* sub = event_system->topics[event.type];
	while(sub != NULL) {
		if (sub->callback == NULL) {
			// TODO register this error somewhere
			printf("WARN: Found ox_Subscription with NULL callback. Skipping\n");
			sub = sub->next;
			break;
		}
		sub->callback(event, sub->user_data);
		sub = sub->next;
	}

	return true;
}


// ox_subscribe_events registers a callback for events of the given type
int oxi_subscribe_events(
		ox_EventSystem* event_system,
		ox_EventType type,
		void (*callback)(ox_Event event, void* user_data),
		void* user_data
) {
	int id = rand();
	if (ox_sub_avail(&event_system->subscription_set) >= 0) {
		ox_Subscription sub = {
			.id = id,
			.callback = callback,
			.user_data = user_data,
			.next = event_system->topics[type],
		};
		ox_Subscription* subbed = ox_sub_add(&event_system->subscription_set, sub);
		event_system->topics[type] = subbed;
	} else {
		printf("ERROR: Max subscriptions");
		return 0;
	}
	return id;
}

static ox_Subscription* pop_by_id(ox_Subscription* sub_list, size_t id) {
	ox_Subscription* sub = sub_list;
	ox_Subscription* prev = NULL;
	while (sub != NULL) {
		if (sub->id == id) {
			prev->next = NULL;
			if (sub->next != NULL) {
				prev->next = sub->next->next;
			}
			return sub;
		}
		prev = sub;
		sub = sub->next;
	}
	return NULL;
}

// ox_unsubscribe unsubscribes a subscription by id
bool oxi_unsubscribe(ox_EventSystem* event_system, size_t id) {
	// scan subscriptions in topics for id
	ox_Subscription* sub;
	for (int i = 0; i < TOPICS_COUNT; i++) {
		sub = pop_by_id(event_system->topics[i], id);
		if (sub != NULL) break;
	}
	if (sub == NULL) return 0;
	return ox_sub_remove(&event_system->subscription_set, sub);
}
