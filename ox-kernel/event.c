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

// TODO change other comments to oxi prefix
// oxi_make_event_system returns a pointer to a newly initialized event system
ox_EventSystem* oxi_make_event_system() {
	ox_EventSystem* event_system = calloc(1, sizeof(ox_EventSystem));
	return event_system;
}

// oxi_destroy_event_system frees all allocations for the event system
bool oxi_destroy_event_system(ox_EventSystem* event_system) {
	free(event_system);
}

// ox_make_zero_event initializes an event with the given type and a timestamp
ox_Event* oxi_make_event(ox_EventSystem* event_system, ox_EventType type) {
	ox_EventArena* event_arena = &event_system->event_arena;
	ox_Event* event = NULL;
	// First look for a processed event we can reuse
	// offsets point at the highest unused slot
	// these slots represent avaiable memory
	if (event_arena->tombstone_offset > 0) {
		event_arena->tombstone_offset -= 1;
		event = &event_arena->events[event_arena->tombstone_offset];
	}

	// Otherwise grab the next available from the buffer
	// EVENT_MAX is the size of the array, so EVENT_MAX-1 is the last available
	// and again the offset points at the highest unused slot
	// these slots represent unavailable memory
	if (event_arena->offset < EVENT_MAX) {
		event = &event_arena->events[event_arena->offset];
		event_arena->offset += 1;
	}

	if (event == NULL) {
		printf("ERROR: Max events"); 
		return NULL;
	}

	event->type = type;
	event->common.timestamp = oxi_time_ns();
	return event;
}

// NOTE operation to sync the h file from the c too
// NOTE operation to identify where h and c files are out of sync
// NOTE operation to make and manage a tracability matrix
// NOTE operation to jump from error message to code
// NOTE operation to pull notes into a central document

// oxi_destroy_event frees the memory for an ox_Event*
bool oxi_destroy_event(ox_EventSystem* event_system, ox_Event* event) {
	ox_EventArena* event_arena = &event_system->event_arena;
	// check for existing tombstones
	for(int i = 0; i < EVENT_MAX; i++) {
		if (event_arena->tombstones[i] == event) {
			printf("WARN: Event(%p) already destroyed", event);
			return 0;
		}
	}
	
	if (event_arena->tombstone_offset >= EVENT_MAX) {
		printf("ERROR: Event tombstone max");
		return 0;
	}

	event_arena->tombstones[event_arena->tombstone_offset] = event;
	
	return 1;
}

// oxi_from_sdl_key translates an SDL_Keycode to an ox_Key
ox_Key oxi_from_sdl_key(SDL_Keycode sdl_key) {
	switch(sdl_key) {
		case SDLK_ESCAPE: return OX_KEY_ESCAPE;
		default:          return OX_KEY_NULL;
	}
}

// ox_from_sdl_event transform SDL_Event to an ox_Event
ox_Event* oxi_from_sdl_event(ox_EventSystem* event_system, SDL_Event* sdl_event) {
	if (sdl_event == NULL) {
		printf("INFO: Trying to transform NULL SDL_Event*\n");
		return NULL;
	}
	switch(sdl_event->type) {
		case SDL_EVENT_QUIT:
			return oxi_make_event(event_system, OX_EVENT_QUIT);
		case SDL_EVENT_KEY_DOWN:
			ox_Key key = oxi_from_sdl_key(sdl_event->key.key);
			if (!key) {
				printf("WARN: Unknown SDL_Keycode(%d)\n", sdl_event->key);
				return false;
			}
			ox_Event* event = oxi_make_event(event_system, OX_EVENT_KEY_DOWN);
			event->key_press.key = key;
			return event;
		default:
			fprintf(stderr, "WARN: Unknown SDL_Event type %d\n", sdl_event->type);
			return false;
	}
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

// ox_publish_event queues an event to be delivered to subscribers. Always destroys the ox_event
bool oxi_publish_event(ox_EventSystem* event_system, ox_Event* ox_event) {
	SDL_Event sdl_event;
	SDL_zero(sdl_event);
	bool could_translate = oxi_to_sdl_event(ox_event, &sdl_event);
	if (!oxi_destroy_event(event_system, ox_event)) {
		printf("WARN: Failed to destroy event. Likely memory leak");
	}
	if (!could_translate) {
		printf("ERROR: Could not translate ox_Event.type(%d) to SDL_Event\n",
		       ox_event->type);
		return false;
	}
	return SDL_PushEvent(&sdl_event);
}

// ox_dispatch_next waits until the next event is published then dispatches it to subscribers
bool oxi_dispatch_next(ox_EventSystem* event_system) {
	ox_Event* event = NULL;
	while (event == NULL) {
		// Try translating SDL events until we find one we can translate
		SDL_Event sdl_event;
		if (!SDL_WaitEvent(&sdl_event)) {
			printf("ERROR: oxi_dispatch_next failed. %s\n", SDL_GetError());
			return false;
		}

		event = oxi_from_sdl_event(event_system, &sdl_event);
	}

	ox_Subscription* sub = event_system->topics[event->type];
	while(sub != NULL) {
		if (sub->callback == NULL) {
			// TODO register this error somewhere
			printf("WARN: Found ox_Subscription with NULL callback. Skipping");
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
		void (*callback)(const ox_Event* event, const void* user_data),
		void* user_data
) {
	ox_Subscription* sub;
	ox_SubArena* sub_arena = &event_system->sub_arena;

	// if there is an tombstone sub available use it
	if (sub_arena->tombstone_offset >= 1) {
		sub_arena->tombstone_offset -= 1;
		sub = sub_arena->tombstones[sub_arena->tombstone_offset];
	} else if (sub_arena->offset < SUB_MAX) {
		// use the next unused subscription
		sub = &sub_arena->subs[sub_arena->offset];
		sub_arena->offset += 1;
	} else {
		printf("ERROR: Max subscriptions");
		return 0;
	}

	uint32_t id = rand();
	
	sub->callback = callback;
	sub->user_data = user_data;
	sub->id = id;
	sub->next =  event_system->topics[type];
	
	event_system->topics[type] = sub;

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
int oxi_unsubscribe(ox_EventSystem* event_system, size_t id) {
	ox_SubArena* sub_arena = &event_system->sub_arena;

	// scan subscriptions in topics for id
	ox_Subscription* sub;
	// TODO:1 build todo parser that respects that syntax
	// TODO:4 consider. is this too clever?
	int i = 0;
	while((sub = pop_by_id(event_system->topics[i], id)) == NULL) {
		if ((i += 1) >= TOPICS_COUNT) break;
	}
	if (sub == NULL) return 0;

	// free the sub
	// Check if it's already in the tombstone array
	for (int i = 0; i < sub_arena->tombstone_offset; i++) {
		if (sub_arena->tombstones[i]->id == id) {
			printf("WARN: Subscription(%d) already tombstone but was found in a topic", id);
			return 0;
		}
	}

	// abort if there isn't room in the tombstone array
	if (sub_arena->tombstone_offset == SUB_MAX) {
		printf("ERROR: Unsub max");
		return 0;
	}

	// add it to tombstones
	sub_arena->tombstones[sub_arena->tombstone_offset] = sub;
	sub_arena->tombstone_offset += 1;
	return 1;
}
