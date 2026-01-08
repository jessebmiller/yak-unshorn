#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "ox.h"

// TODO make module.h don't import modlue.c
#include "module.c"
#include "event.h"
#include "display.h"

static void quit_handler(const ox_Event* event, const void* user_data) {
	if (event->type != OX_EVENT_QUIT) return;
	Ox* ox = (Ox*)user_data;
	ox->should_exit = true;
}

static bool render_callback(SDL_Renderer* renderer, void* user_data) {
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x55, 0x33, 0xFF);
	SDL_FRect rect = { 5, 5, 50, 50 };
	SDL_RenderFillRect(renderer, rect);
}

int main() {
	srand(time(NULL));

	Ox ox = { 
		oxi_make_event_system(),
		oxi_make_display(),
		false,
	};
	const ox_Api api = {:
		oxi_publish_event,
		oxi_subscribe_events,
		oxi_make_event,
	};

	Module modules = load_modules(&ox, &api);
	if (modules.stop == NULL) {
		return -1;
	}

	printf("SDL Version: %s\n", SDL_GetRevision());
	bool ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_SetHint(SDL_HINT_EVENT_LOGGING, "1");

	if (!ok) {
		SDL_Log("Failed to init SDL");
		SDL_Quit();
		unload_module(modules);
		return -1;
	}
	SDL_Log("SDL initialized");

	// TODO:1 should this be ox_ not oxi_
	oxi_subscribe_events(
			ox.event_system,
			OX_EVENT_QUIT,
			&quit_handler,
			&ox);

	ox_register_render_cb(&ox.display, render_callback, NULL);

	oxi_render_and_present(ox.display);
	while(!ox.should_exit) {
		oxi_dispatch_next(ox.event_system);
		oxi_render_and_present(ox.display);
	}

	oxi_destroy_event_system(ox.event_system);
	oxi_destroy_display(ox.display)
	SDL_Quit();
	return unload_module(modules);
}
