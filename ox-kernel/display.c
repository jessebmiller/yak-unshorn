#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL.h>

#include "display.h"

ox_Display* oxi_make_display() {
	// TODO move window config to environment
	SDL_Window* window = SDL_CreateWindow("Yak Unshorn", 800, 600,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Log("NULL Window, quitting SDL");
		SDL_Quit();
		return NULL;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL) {
		SDL_Log("NULL Renderer, quitting SDL");
		SDL_Quit();
		return NULL;
	}
	
	ox_Display* display  = calloc(1, sizeof(ox_Display));
	display->sdl_window   = window;
	display->sdl_renderer = renderer;
	return display;
}

void oxi_destroy_display(ox_Display* display) {
	SDL_DestroyRenderer(display->sdl_renderer);
	SDL_DestroyWindow(display->sdl_window);
	free(display);
}

static ox_RenderCB* avail_render_cb(ox_Display* display) {
	for (int i = 0; i < MAX_RENDER_CBS; i++) {
		ox_RenderCB* cb = &display->render_cb_store[i];
		if(cb->f == NULL) {
			return cb;
		}
	}
	return NULL;
}

ox_RenderCB* ox_register_render_cb(
	ox_Display* display,
	bool (*render_cb)(SDL_Renderer* renderer, void* user_data),
	void* user_data
) {
	if (render_cb == NULL) return NULL;

	ox_RenderCB* cb = avail_render_cb(display);
	if (cb == NULL) {
		printf("ERROR: Display render callback store exausted");
		return NULL;
	}
	cb->f = render_cb;
	cb->user_data = user_data;
	if (display->first_render_cb != NULL) {
		display->first_render_cb->prev = cb;
	}
	display->first_render_cb = cb;
}

void ox_deregister_render_cb(ox_Display* display, ox_RenderCB* cb) {
	cb->f = NULL;
	if (cb->next != NULL) {
		cb->next->prev = cb->prev;
	}
	if (cb->prev != NULL) {
		cb->prev->next = cb->next;
	}
	if (cb == display->first_render_cb) {
		display->first_render_cb = cb->next;
	}
}

bool oxi_render_and_present(ox_Display* display) {
	bool did_error = false;
	SDL_SetRenderDrawColor(display->sdl_renderer, 0xFF, 0x55, 0x33, 0xFF);
	SDL_RenderClear(display->sdl_renderer);

	ox_RenderCB* cb = display->first_render_cb;
	while(cb != NULL) {
		if(!cb->f(display->sdl_renderer, cb->user_data)) {
			printf("ERROR: RenderCB Failed");
			did_error = true;
		}
		cb = cb->next;
	}

	SDL_RenderPresent(display->sdl_renderer);
	return did_error;
}
