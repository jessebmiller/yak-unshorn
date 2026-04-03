#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include "ox.h"

#define MAX_MODS 4
#define MAX_LINE 1024
#define MAX_VALUE 64

// TODO move types to module.h
typedef int (*ModuleInit)(Ox* ox, const ox_Api* api);
typedef int (*ModuleStart)(void);
typedef int (*ModuleStop)(void);
typedef int (*UnloadFunc)(void*);

typedef struct {
	char* path;
	ModuleStop stop;
	void* ctx;
	UnloadFunc unload; // self.unload(self.ctx)
} Module;


int unload_module(Module module) {
	int stop_result = module.stop();
	if (stop_result != 0) {
		fprintf(stderr,
			"Error stopping module %s\n",
			module.path
		       );
	}
	int unload_result = module.unload(module.ctx);
	if (unload_result != 0) {
		fprintf(stderr,
			"Error unloading module %s\n",
			module.path
		       );
	}
	if (stop_result !=0 || unload_result != 0) {
		return 1;
	}

	return 0;
}

int unload_modules(Module* modules, int module_count) {
	int return_code = 0;
	for (int i = 0; i < module_count; i++) {
		if(unload_module(modules[i]) != 0) {
			return_code = 1;
		}
	}
	return return_code;
}

int unload_native_module(void* handle) {
	return dlclose(handle);
}

Module start_native_module(char* lib_path, int dlopen_flags) 
{
	void* handle;
	ModuleInit ox_init;
	ModuleStart start;
	ModuleStop stop;
	char* error;

	handle = dlopen(lib_path, dlopen_flags);
	if (!handle) {
		fprintf(stderr,
			"Error loading mod %s: %s\n",
			lib_path,
			dlerror()
		       );
		return (Module){ 0 };
	}

	dlerror();

	ox_init = dlsym(handle, "ox_init");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol init %s\n", error);
		dlclose(handle);
		return (Module){0};
	}

	start = dlsym(handle, "start");
	error = dlerror();
	// TODO make start optional
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol start %s\n", error);
		dlclose(handle);
		return (Module){0};
	}

	stop = dlsym(handle, "stop");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol stop %s\n", error);
		dlclose(handle);
		return (Module){0};
	}

	int code = ox_init(_ox, _api);
	if (code != 0) {
		fprintf(stderr, "Init failure: %d\n", code);
		dlclose(handle);
		return (Module){0};
	}

	code = start();
	if (code != 0) {
		fprintf(stderr, "Start failure: %d\n", code);
		dlclose(handle);
		return (Module){0};
	}

	fprintf(stderr, "Started native module %s\n", lib_path);

	Module module = {
		.path = lib_path,
		.stop = stop,
		.ctx = handle,
		.unload = unload_native_module
	};
	return module;
}

Module start_module(const char* mod_path) 
{
	char* modfile = "oxmod";
	int path_len = strlen(modfile) + strlen(mod_path) + 2;
	char modfile_path[path_len];
	snprintf(modfile_path, path_len, "%s/%s", mod_path, modfile);

	printf("Opening oxmod at %s\n", modfile_path);
	FILE* file = fopen(modfile_path, "r");
	if (!file) {
		perror("Failed to open modfile");
		return (Module){0};
	}

	char line[MAX_LINE];
	char* runtime = NULL;
	char* lib = NULL;
	while(fgets(line, sizeof(line), file)) {
		if (line[0] == '#' || line[0] == '\n') continue;

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


	if(!runtime || !lib) {
		fprintf(stderr, "Missing required config values\n");
		free(runtime);
		free(lib);
		return (Module){0};
	}
	
	if (strcmp(runtime, "native") == 0) {
		free(runtime);
		int dlopen_flags = RTLD_NOW;
		while(fgets(line, sizeof(line), file)) {
			if (line[0] == '#' || line[0] == '\n') continue;
			char value[MAX_VALUE];
			errno = 0;
			if (sscanf(line, "dlopen_flag %s", value) == 1) {
				if (strcmp(value, "RTLD_GLOBAL") == 0) {
					dlopen_flags = dlopen_flags | RTLD_GLOBAL;
				}
			}
		}
		printf("%d %d %d %d\n", dlopen_flags, RTLD_NOW, RTLD_GLOBAL, RTLD_NOW | RTLD_GLOBAL);
		Module module = start_native_module(lib, dlopen_flags);
		free(lib);
		return module;
	}

	fprintf(stderr, "Unknown runtime %s\n", runtime);

	fclose(file);
	free(runtime);
	free(lib);
	return (Module){0};
}

// TODO prefix all module functions with ox_
Module* load_modules(Ox* ox, const ox_Api* api, int* mods_count) {
	_ox = ox;
	_api = api;
	// read the config to find the init module
	const char* load_list = getenv("YAK_UNSHORN_OXMOD_LOAD_LIST");
	if (load_list == NULL) {
		// TODO:1 make this relative to $HOME
		load_list = "/home/jesse/.config/yak_unshorn/oxmod_load_list";
	}

	FILE* file = fopen(load_list, "r");
	if (!file) {
		perror("Could not open YAK_UNSHORN_LOAD_LIST");
		return NULL;
	}

	Module* mods = calloc(MAX_MODS, sizeof(Module));
	char line[MAX_LINE];
	for(int i = 0; fgets(line, sizeof(line), file) != NULL && i < MAX_MODS; i++) {
		if (line[0] == '#' || line[0] == '\n') continue;
		line[strcspn(line, "\n")] = '\0';
		printf("Loading module at %s\n", line);
		mods[i] = start_module(line);
		*mods_count = i + 1;
	}

	return mods;
}
