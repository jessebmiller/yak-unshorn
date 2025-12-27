#include <SDL3/SDL.h>
#include <stdbool.h>

#include "module.c"

typedef struct {
	bool should_exit;
} Ox;

bool handle_event(Ox* ox, SDL_Event event) {
	if (event.type == SDL_EVENT_QUIT) {
		ox->should_exit = 1;
	} else if (event.type == SDL_EVENT_KEY_DOWN) {
		ox->should_exit = 1;
	}
	return true;
}

int main() {

	int (*stop_modules)() = load_modules();

	bool ok = SDL_Init(SDL_INIT_VIDEO);
	if (!ok) {
		SDL_Log("Failed to init video");
		SDL_Quit();
		stop_modules();
		return -1;
	}
	SDL_Log("Video initialized");

	SDL_Window* window;
	window = SDL_CreateWindow("Yak Unshorn", 800, 600,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Log("NULL Window");
		SDL_Quit();
		stop_modules();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	Ox ox = { 0 };
	while(!ox.should_exit) {
		SDL_Event event;
		if (!SDL_WaitEvent(&event)) {
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
	return stop_modules();
}
