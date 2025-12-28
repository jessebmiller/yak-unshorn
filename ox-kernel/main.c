#include <SDL3/SDL.h>
#include <stdbool.h>

#include "main.h"

// TODO make header files for module and event
#include "module.c"
#include "event.h"

int main() {

	Module modules = load_modules();
	if (modules.stop == NULL) {
		return -1;
	}

	bool ok = SDL_Init(SDL_INIT_VIDEO);
	if (!ok) {
		SDL_Log("Failed to init video");
		SDL_Quit();
		unload_module(modules);
		return -1;
	}
	SDL_Log("Video initialized");

	SDL_Window* window;
	window = SDL_CreateWindow("Yak Unshorn", 800, 600,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Log("NULL Window");
		SDL_Quit();
		unload_module(modules);
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	Ox ox = { 0 };
	while(!ox.should_exit) {
		ox_Event event;
		if (!ox_WaitEvent(&event)) {
			SDL_Log("WaitEvent error%s\n", SDL_GetError());
		}
		if (!handle_event(&ox, event)) {
			SDL_Log("Handle Event Failure");
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return unload_module(modules);
}
