#ifndef OX_KERNEL_DISPLAY_H
#define OX_KERNEL_DISPLAY_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define MAX_RENDER_CBS 32

typedef struct ox_RenderCB {
	bool (*f)(SDL_Renderer* renderer, void* user_data);
	void* user_data;
	struct ox_RenderCB* next;
	struct ox_RenderCB* prev;
} ox_RenderCB;

typedef struct {
	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	ox_RenderCB render_cb_store[MAX_RENDER_CBS];
	ox_RenderCB* first_render_cb;
} ox_Display;

// ox_make_display_system returns display system of the given type
ox_Display* oxi_make_display();

// ox_destroy_display_system frees a display system
void oxi_destroy_display(ox_Display* display);

// ox_register_render_cb registers a render cb with the display
ox_RenderCB* ox_register_render_cb(
		ox_Display* display,
		bool (*render_cb)(SDL_Renderer* renderer, void* user_data),
		void* user_data
		);

// ox_deregister_render_cb deregisters a cb from the display if present
void ox_deregister_render_cb(ox_Display* display, ox_RenderCB* cb);

// oxi_render_and_present calls all registered renderers and presents the result
bool oxi_render_and_present(ox_Display* display);

#endif
