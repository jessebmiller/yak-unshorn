#include <SDL3/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "ox.h"

// TODO make module.h don't import modlue.c
#include "module.c"
#include "event.h"

static void quit_handler(const ox_Event* event, const void* user_data) {
	if (event->type != OX_EVENT_QUIT) return;
	Ox* ox = (Ox*)user_data;
	ox->should_exit = true;
}

static bool setup(Ox* ox) {
	printf("SDL Version: %ds\n", SDL_GetRevision());
	
	bool ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if (!ok) {
		SDL_Log("SDL failed to init");
		ox->should_exit = true;
		SDL_Quit(); // TODO setup teardown registration on Ox and move this there
		return false;
	}
	SDL_Log("SDL initialized");

	return true;
}

int main() {
	srand(time(NULL));

	Ox ox = { oxi_make_event_system(), false };
	const ox_Api api = {
		oxi_publish_event,
		oxi_subscribe_events,
		oxi_make_event,
	};
	
	setup();
	SDL_SetHint(SDL_HINT_EVENT_LOGGING, "1");

	// TODO:4 move this to ox.modules.init()
	Module modules = load_modules(&ox, &api);
	if (modules.stop == NULL) {
		return -1;
	}

	// TODO:9 move this to ox.display.init()
	SDL_Window* window;
	window = SDL_CreateWindow("Yak Unshorn", 800, 600,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Log("NULL Window");
		SDL_Quit();
		unload_module(modules);
		return -1;
	}

	oxi_subscribe_events(ox.event_system, OX_EVENT_QUIT, &quit_handler, &ox);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x55, 0x33, 0xFF);
	SDL_RenderPresent(renderer);
	while(!ox.should_exit) {
		oxi_dispatch_next(ox.event_system);
		
		// TODO wrap SDL_RenderPresent in oxi_render
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	// TODO:1 make cleanup registry and call them all here
	// nice
	oxi_destroy_event_system(ox.event_system);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return unload_module(modules);
}
