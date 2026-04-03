#define BUNUELIB_STRIP_PREFIX

#include "../../bunuelib.h"
#include "../../ox-kernel/ox.h"

#define POLYPHONY 10
#define MAX_PARSERS 5

// mutable borrow of state
typedef ox_Command (*parse)(cmd_Chord chord, void* mut_state);

BUNUEL_FIXED_SET(key_set, SDL_Keycode, POLYPHONY) {
	return *p == *q;
}

typedef struct {
	SDL_KeycodeSet held_keys;
	ox_KeyPress key_press;
} cmd_Chord;

bool chord_eq(cmd_Chord a, cmd_Chord b) {
	// TODO
}

typedef struct {
	ox_Command (*parse)(cmd_Chord, void* state);
	void* state;
} cmd_Parser

typedef struct {
	SDL_KeycodeSet keys_down;
	cmd_Parser* command_parsers[MAX_PARSERS];
	int parser_count;
} cmd_State;

static cmd_State cmd_state = {0};

