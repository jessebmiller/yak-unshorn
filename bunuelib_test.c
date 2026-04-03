#define BUNUELIB_STRIP_PREFIX
#include "bunuelib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// test arenas and pools
#define CAPACITY 3
#define STRING "Hello"

typedef struct {
	int x;
	char bytes[sizeof(STRING)];
} Block;

void log_block(Block* block) {
	info("Block{x: %d, bytes: %s}", block->x, block->bytes);
}

int main() {
	char* val = STRING;
	dbg("dbg test (%s)", val);

	Arena* arena = make_arena(CAPACITY, sizeof(Block));

	Block* block_a = bump_arena(arena);
	block_a->x = 1;
	strcpy(block_a->bytes, val);

	Block* block_b = bump_arena(arena);
	block_b->x = 2;
	strcpy(block_b->bytes, val);

	Block* block_c = bump_arena(arena);
	block_c->x = 3;
	strcpy(block_c->bytes, val);

	Block* block_null = bump_arena(arena);
	assert(block_null == NULL);

	wipe_arena(arena);
	Block* block_d = bump_arena(arena);
	block_d->x = 4;

	info("Arena test %d, %d, %d, %d, %s",
	    block_a->x, block_b->x, block_c->x, block_d->x, block_a->bytes);

	Pool* block_pool = make_pool(CAPACITY, sizeof(Block));
	Block* block_e = take_pool(block_pool);
	strcpy(block_e->bytes, "hihih");
	block_e->x = 100;
	log_block(block_e);

	Block* block_f = take_pool(block_pool);
	strcpy(block_f->bytes, "NIGHT");
	block_f->x = 101;
	log_block(block_f);

	Block* block_g = take_pool(block_pool);
	strcpy(block_g->bytes, "mooon");
	block_g->x = 102;
	log_block(block_g);

	assert(take_pool(block_pool) == NULL);

	give_pool(block_pool, block_f);

	Block* block_h = take_pool(block_pool);
	block_h->x = 200;
	strcpy(block_h->bytes, "REUSE");
	log_block(block_h);
	log_block(block_f);

	assert(take_pool(block_pool) == NULL);

	give_pool(block_pool, block_e);
	give_pool(block_pool, block_f);
	
}


