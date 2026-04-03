#include "janet.h"

#include "../../ox-kernel/ox.h"

JanetTable* env;

static Janet cfun_ox_log(int32_t argc, Janet* argv) {
	janet_fixarity(argc, 1);
	const char* message = janet_getcstring(argv, 0);
	ox_log((char*)message);
	return janet_wrap_nil();
}

JanetReg ox_cfuns[] = {
	{"log", cfun_ox_log, "(ox/log `message`)"},
	{NULL, NULL, NULL},
};

OX_INIT(janet) {
	printf("Initializing Janet Runtime\n");

	janet_init();
	env = janet_core_env(NULL);

	janet_cfuns_prefix(env, "ox", ox_cfuns);

	janet_dostring(env, "(ox/log `Janet Runtime Initialized`)", "main", NULL);
	return 0;
}

void start(char* module_name, char* src) {
	// TODO: set module name to janet/<module_name>
	// TODO: run src in env
	janet_dostring(env, "(ox/log `Starting Janet Runtime`)", "main", NULL);
}

void stop() {
	janet_deinit();
}
