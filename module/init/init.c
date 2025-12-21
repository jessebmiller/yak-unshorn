// standard init loads all configured modules

#include <stdio.h>

int init() {
	printf("running init.\n");
	return 0;
}

int start() {
	printf("running start\n");
	return 0;
}

int stop() {
	printf("running stop\n");
	return 0;
}
