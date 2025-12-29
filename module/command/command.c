// translates events into commands
// like 'x pressed' -> 'Insert x into active buffer'
//
// Commands are contextual...
// therefore this module is going to need to know a lot about the rest of the system
//

// commands we might want to implement
// exit!
// new buffer
// split frame
// new window?
// buffer editing
// - vim motions (up down left right delete replace etc.)
// - multiple cursors
// - run buffer
// - run line
// - find/replace
// - open file
// - save
// filesystem nav
// - ls
// - cd
// - touch
// - mkdir
// modal commands
// - normal mode
// - insert mode
// - git mode
// - future mode submodules...
//
//
// Let's start with just exit to get the communication paths figured out
//

#include <stdio.h>

#include "../../ox-kernel/ox.h"

static void handle_keyboard_event(const ox_Event* event, const void* user_data) {
	if (event->type != OX_EVENT_KEY_DOWN) return;

	if (event->key_press.key == OX_KEY_ESCAPE) {
		ox_Event* quit = ox_make_event(OX_EVENT_QUIT);
		ox_publish_event(quit);
	}
}

OX_INIT(command) {
	printf("Initializing command module\n");
	ox_subscribe_events(OX_EVENT_KEY_DOWN, handle_keyboard_event, NULL);
	return 0;
}

int start() {
	printf("Starting command module\n");
	return 0;
}

int stop() {
	printf("Stopping command module\n");
	return 0;
}

