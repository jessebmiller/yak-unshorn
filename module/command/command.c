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

#define BUNUELIB_STRIP_PREFIX

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../ox-kernel/ox.h"
#include "../../bunuelib.h"
#include "parsers.h"
#include "command.h"

#define MAX_CHORDS 3

static cmd_Chord latest_chords[MAX_CHORDS] = {0};

static void shift_chords(cmd_Chord chord) {
	size_t len = MAX_CHORDS;
	
	// shift the array down 1 (dropping the 0th element)
	memmove(&latest_chords[0], &latest_chords[1],
		(MAX_CHORDS - 1) * sizeof(cmd_Chord));
	
	// set the last chord
	latest_chords[MAX_CHORDS - 1] = chord;
}

static ox_Command parse_quit() {
	// quit is [+q/esc, +q/esc] (ignoring key ups)
	if (latest_chords[0] == q_esc && latest_chords[1] == q_esc) {
		
	}
}

static ox_Command parse_priority_commands(cmd_Chord chord) {
	assert(chord.key_press.type == OX_EVENT_KEY_DOWN
	    || chord.key_press.type == OX_EVENT_KEY_UP);
	if (chord.key_press.type != OX_EVENT_KEY_DOWN
	 && chord.key_press.type != OX_EVENT_KEY_UP) {
		return ox_Command {0};
	}

	shift_chords(chord);

	ox_Command cmd = parse_quit();
	if (cmd.type != OX_COMMAND_NONE) return cmd;

	return parse_default_mode();
}

static ox_Command parse_command(cmd_Parser* parsers, cmd_Chord chord) {
	if (parsers == NULL) {
		printf("ERROR: Cannot parse command, NULL parsers\n");
		exit(-1);
	}
	ox_Command command;
	for (int i = 0; i < LEN(parsers); i++) {
		command = parsers[i]->parse(chord, parsers[i]->state);
		if (command.type != OX_COMMAND_NONE) break;
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
	if(command.type != OX_CMD_NONE) {
		ox_Event cmd_event;
		ox_init_event(&cmd_event, OX_EVENT_COMMAND);
		cmd_event.cmd = command;
		ox_publish_event(&cmd_event);
	}

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

// TODO:1 should we to prefix/namespace init, start, and stop in modules?
int start() {
	printf("Starting command module\n");
	return 0;
}

int stop() {
	printf("Stopping command module\n");
	return 0;
}

