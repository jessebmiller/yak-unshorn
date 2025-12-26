#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>
#include <raylib.h>

#define MAX_LINE 256
#define MAX_VALUE 64

void* start_native_module(char* lib_path) {
	void* handle;
	int (*init)();
	int (*start)();
	int (*stop)();
	char* error;

	handle = dlopen(lib_path, RTLD_NOW);
	if (!handle) {
		fprintf(stderr,
			"Error loading mod %s: %s\n",
			lib_path,
			dlerror()
		       );
		return NULL;
	}

	dlerror();

	init = dlsym(handle, "init");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol init %s\n", error);
		dlclose(handle);
		return NULL;
	}

	start = dlsym(handle, "start");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol start %s\n", error);
		dlclose(handle);
		return NULL;
	}

	stop = dlsym(handle, "stop");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol stop %s\n", error);
		dlclose(handle);
		return NULL;
	}

	int code = init();
	if (code != 0) {
		fprintf(stderr, "Init failure: %d\n", code);
		return NULL;
	}

	code = start();
	if (code != 0) {
		fprintf(stderr, "Start failure: %d\n", code);
		return NULL;
	}

	fprintf(stderr, "Started native module %s\n", lib_path);

	return stop;
}

void* start_module(const char* mod_path) {
	char* modfile = "oxmod";
	int path_len = strlen(modfile) + strlen(mod_path) + 2;
	char modfile_path[path_len];
	sprintf(modfile_path, "%s/%s", mod_path, modfile);

	FILE* file = fopen(modfile_path, "r");
	if (!file) {
		perror("Failed to open modfile");
		return NULL;
	}

	char line[MAX_LINE];
	char* runtime = NULL;
	char* lib = NULL;
	while(fgets(line, sizeof(line), file)) {
		fprintf(stderr, "CHECKING LINE: %s", line);
		if (line[0] == '#' || line[0] == '\n') {
			continue;
		}

		char value[MAX_VALUE];
		if (sscanf(line, "runtime %s", value) == 1) {
			runtime = strdup(value);
		}

		if (sscanf(line, "lib %s", value) == 1) {
			lib = strdup(value);
		}

		if (runtime && lib) {
			break;
		}
	}

	fclose(file);

	if(!runtime || !lib) {
		fprintf(stderr, "Missing required config values\n");
		free(runtime);
		free(lib);
		return NULL;
	}
	
	if (strcmp(runtime, "native") == 0) {
		free(runtime);
		void (*stop)() = start_native_module("./build/module/init/libinit.so");
		free(lib);
		return stop;
	}

	fprintf(stderr, "Unknown runtime %s", runtime);

	free(runtime);
	free(lib);
	return NULL;
}

int main() {

	// read the config to find the init module
	const char* OXINIT = getenv("YAK_UNSHORN_OXINIT");
	if (OXINIT == NULL) {
		OXINIT = "./module/init";
	}

	// Run the init module in its runtime
	int (*stop)();
	stop = start_module(OXINIT);
	if (stop == NULL) {
		fprintf(stderr, "Module failed to load. Exiting\n");
		return -1;
	}

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "Yak Unshorn");

	char text[16];
	while (!WindowShouldClose())
	{
		int chr = GetCharPressed();
		while(chr != 0) {
			sprintf(text, "-%d-", chr);
			fprintf(stderr, "%d\n", chr);
			chr = GetCharPressed();
		}
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(text, 190, 200, 20, DARKGRAY);
		EndDrawing();
	}

	CloseWindow();

	return stop();
}
