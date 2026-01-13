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
// - vim motions (updown left right delete replace etc.)
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

#include <assert.h>
#include <stdio.h>

#include "../../ox-kernel/ox.h"
#include "../../bunuelib.h"

#define POLYPHONY 10

BUNUEL_FIXED_SET(key_set, ox_Key, POLYPHONY) {
	return *p == *q;
}

typedef struct {
	ox_KeySet keys_down;
} cmd_State;

static cmd_State cmd_state = {0};

static void debug_log_cmd_state(cmd_State state) {
	printf("cmd_State(keys_down: ");
	for (int i = 0; i < state.keys_down.count; i++) {
		printf("%d", state.keys_down.items[i]);
	}
	printf(")\n");
}

static void handle_key_down(ox_Event* event, void* user_data) {
	assert(event->type == OX_EVENT_KEY_DOWN);
	cmd_State* cmd_state = (cmd_State*)user_data;

	if (event->key_press.key == OX_KEY_ESCAPE) {
		ox_Event* quit = ox_make_event(OX_EVENT_QUIT);
		ox_publish_event(quit);
		return;
	}

	key_set_add(&cmd_state->keys_down, event->key_press.key);
	debug_log_cmd_state(*cmd_state);
}

static void handle_key_up(ox_Event* event, void* user_data) {
	assert(event->type == OX_EVENT_KEY_UP);
	cmd_State* cmd_state = (cmd_State*)user_data;
	key_set_remove(&cmd_state->keys_down, &event->key_press.key);
	debug_log_cmd_state(*cmd_state);
}

OX_INIT(command) {
	printf("Initializing command module\n");
	ox_subscribe_events(OX_EVENT_KEY_DOWN, handle_key_down, &cmd_state);
	ox_subscribe_events(OX_EVENT_KEY_UP, handle_key_up, &cmd_state);
	return 0;
}

// TODO:1 should we to prefixspace init, start, and stop in modules?
int start() {
	printf("Starting command module\n");
	return 0;
}

int stop() {
	printf("Stopping command module\n");
	return 0;
}

