#include <stdbool.h>

#include "main.h"

// TODO handle_event should call all registered 
// TODO callbacks with the event

// TODO modules need to be able to register
// TODO event callbacks

bool handle_event(Ox* ox, SDL_Event event) {
	if (event.type == SDL_EVENT_QUIT) {
		ox->should_exit = 1;
	} else if (event.type == SDL_EVENT_KEY_DOWN) {
		ox->should_exit = 1;
	}
	return true;
}

