#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_VALUE 64

typedef int (*ModuleInit)(void);
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

int unload_native_module(void* handle) {
	return dlclose(handle);
}

Module start_native_module(char* lib_path) 
{
	void* handle;
	ModuleInit init;
	ModuleStart start;
	ModuleStop stop;
	char* error;

	handle = dlopen(lib_path, RTLD_NOW);
	if (!handle) {
		fprintf(stderr,
			"Error loading mod %s: %s\n",
			lib_path,
			dlerror()
		       );
		return (Module){ 0 };
	}

	dlerror();

	init = dlsym(handle, "init");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "Error loading symbol init %s\n", error);
		dlclose(handle);
		return (Module){0};
	}

	start = dlsym(handle, "start");
	error = dlerror();
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

	int code = init();
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

	FILE* file = fopen(modfile_path, "r");
	if (!file) {
		perror("Failed to open modfile");
		return (Module){0};
	}

	char line[MAX_LINE];
	char* runtime = NULL;
	char* lib = NULL;
	while(fgets(line, sizeof(line), file)) {
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
		return (Module){0};
	}
	
	if (strcmp(runtime, "native") == 0) {
		free(runtime);
		Module module = start_native_module("./build/module/init/libinit.so");
		free(lib);
		return module;
	}

	fprintf(stderr, "Unknown runtime %s\n", runtime);

	free(runtime);
	free(lib);
	return (Module){0};
}

// TODO load all modules, not just ./module/init
Module load_modules() {
	// read the config to find the init module
	const char* OXINIT = getenv("YAK_UNSHORN_OXINIT");
	if (OXINIT == NULL) {
		OXINIT = "./module/init";
	}

	return start_module(OXINIT);
}
