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
// TODO:
// - key chords (all keys pressed on a key down)
// - Runs: sequence of chords
// - intrepreter that reads the sequence and publishes commands when parsed

// Chords: (experimental)
// given some sequence of key presses down and up, a chord is the change
// over the consistent keys pressed.
//
// If a user presses down A then S then D then lifts up S then D then A
// it looks like this. Chords are intrepreted on the way up and the way
// down.
//
// chord: +D/AS ------> [D] 
// chord: +S/A ----> [S][S][D] <------- chord: -S/AD
// chord: +A/_ -> [A][A][A][A][A] <---- chord: -D/A
//             [_][_][_][_][_][_][_] <- chord: -A/_

// The chord sequence
// a sequence of chords parsed by the command parser:
// - <+D/_, -D/_, +D/_>
// - <+X/C, -X/C, +S/C>

// Commands: (specific intrepretable sequences)
// built with parser combinators. registerable from any module and runtime

// Modes:
// state machine that swaps command parsers in response to mode switch
// commands. A set of priority commands always parse first no matter
// what mode we're in, so we don't get stuck in a buggy mode, and so 
// priority commands like "quit" or "normal mode" always work.
// common commands like "undo", "save", "fuzzy finder" etc. can be built
// in parser combinators and reused.

#define BUNUEL_STRIP_PREFIX

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../ox-kernel/ox.h"
#include "../../bunuelib.h"
#include "parsers.h"

#define POLYPHONY 10

BUNUEL_FIXED_SET(key_set, SDL_Keycode, POLYPHONY) {
	return *p == *q;
}

typedef struct {
	SDL_KeycodeSet held_keys;
	ox_KeyPress key_press;
} cmd_Chord;

typedef struct {
	bool (*parse)(cmd_Chord, ox_Command* command, void* state);
	void* state;
} cmd_Parser


typedef bool (*cmd_Parser)(cmd_Chord chord, ox_Command* command);

#define CHORD_RING_SIZE 3
int shibboleth = rand();
typedef struct {
	cmd_Chord chords[CHORD_RING_SIZE];
	int next;
	int shibboleth;
} cmd_PriorityParseState;

bool priority_parse(cmd_Chord chord, ox_Command* command, void* state) {
	if(state->shibboleth != shibboleth) {
		*state = {0};
		state->shibboleth = shibboleth;
	}

	state->chords[state->next] = chord;
	state->next = (state->next + 1) % CHORD_RING_SIZE;

	cmd_Chord run[3];

}

#define MAX_PARSERS 5

typedef struct {
	SDL_KeycodeSet keys_down;
	cmd_Parser* command_parsers[MAX_PARSERS];
	int parser_count;
} cmd_State;

static cmd_State cmd_state = {0};

static void debug_log_cmd_state(cmd_State* state) {
	printf("cmd_State.keys_down(");
	for (int i = 0; i < state->keys_down.count; i++) {
		printf("[%d]", state->keys_down.items[i]);
	}
	printf(")\n");
}

static ox_Command parse_command(cmd_Parser* parsers, cmd_Chord chord) {
	if (parsers == NULL) {
		printf("ERROR: Cannot parse command, NULL parsers\n");
		exit(-1);
	}
	ox_Command command = {0};
	for (int i = 0; i < LEN(parsers); i++) {
		if(parsers[i](chord, &command)) break;	
	}
	return command;
}



static void handle_key(ox_Event event, void* user_data) {
	assert(event.type == OX_EVENT_KEY_DOWN
	    || event.type == OX_EVENT_KEY_UP);
	cmd_State* cmd_state = (cmd_State*)user_data;

	// TODO: move this so configurable key sequence to quit
	if (event.key_press.key == SDLK_ESCAPE) {
		ox_Event quit;
		ox_init_event(&quit, OX_EVENT_QUIT);
		ox_publish_event(&quit);
		return;
	}

	cmd_Chord chord = {cmd_state->keys_down, event.key_press};
	ox_Command command = parse_command(*cmd_state->command_parsers, chord);
	ox_Event cmd_event;
	ox_init_event(&cmd_event, OX_EVENT_COMMAND);
	cmd_event.cmd = command;
	ox_publish_event(&cmd_event);

	event.type == OX_EVENT_KEY_UP
		? (void*)key_set_remove(&cmd_state->keys_down, &event.key_press.key)
		: (void*)key_set_add(&cmd_state->keys_down, event.key_press.key);
}

OX_INIT(command) {
	printf("Initializing command module\n");
	ox_subscribe_events(OX_EVENT_KEY_DOWN, handle_key, &cmd_state);
	ox_subscribe_events(OX_EVENT_KEY_UP, handle_key, &cmd_state);
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

