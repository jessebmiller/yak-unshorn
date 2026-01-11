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

#define POLYPHONY 10

// TODO:2 move to bunuelib and document usage
#define BUNUEL_FIXED_SET(prefix, T, capacity) \
	typedef struct { T items[capacity]; int count; } T##Set; \
	\
	static inline int prefix##_find(T##Set* set, T item) { \
		int found_at = -1; \
		for (int i = 0; i < set->count; i++) { \
			if (set->items[i] == item) { \
				found_at = i; \
				break; \
			} \
		} \
		return found_at; \
	} \
	\
	static inline bool prefix##_add(T##Set* set, T item) { \
		if (prefix##_find(set, item) >= 0) return true; \
		if (set->count > capacity) return false; \
		set->items[set->count++] = item; \
		return true; \
	} \
	\
	static inline bool prefix##_remove(T##Set* set, T item) { \
		int found_at = prefix##_find(set, item); \
		if (found_at < 0) return false; \
		set->count -= 1; \
		set->items[found_at] = set->items[set->count]; \
		return true; \
	}\
	\
	static inline int prefix##_avail(T##Set* set) { \
		return capacity - set->count; \
	}

BUNUEL_FIXED_SET(key_set, ox_Key, POLYPHONY)

typedef struct {
	ox_KeySet keys_down;
} cmd_State;

static cmd_State cmd_state = {0};

static bool set_key_down(ox_KeySet* keys_down, ox_Key key) {
	return key_set_add(keys_down, key);
}

static void handle_key_down(const ox_Event* event, const void* user_data) {
	assert(event->type == OX_EVENT_KEY_DOWN);
	cmd_State* cmd_state = (cmd_State*)user_data;

	if (event->key_press.key == OX_KEY_ESCAPE) {
		ox_Event* quit = ox_make_event(OX_EVENT_QUIT);
		ox_publish_event(quit);
		return;
	}

	set_key_down(&cmd_state->keys_down, event->key_press.key);
}

static void handle_key_up(const ox_Event* event, const void* user_data) {
	assert(event->type == OX_EVENT_KEY_UP);
	cmd_State* cmd_state = (cmd_State*)user_data;
	key_set_remove(&cmd_state->keys_down, event->key_press.key);
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

