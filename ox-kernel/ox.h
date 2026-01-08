#ifndef OX_H
#define OX_H

#include "event_types.h"
#include "display.h"
#include <stdbool.h>

typedef struct {
	ox_EventSystem* event_system;
	ox_Display* display_system;
	bool should_exit;
} Ox;

typedef struct {
	bool (*oxi_publish_event)(ox_EventSystem* es, ox_Event* event);
	int (*oxi_subscribe_events)(
			ox_EventSystem* es,
			ox_EventType type, 
			void(*callback)(const ox_Event* event, const void* user_data),
			void* user_data);
	ox_Event* (*oxi_make_event)(ox_EventSystem* es, ox_EventType type);
} ox_Api;

static Ox* _ox = NULL;
static const ox_Api* _api = NULL;

static inline bool ox_publish_event(ox_Event* event) {
	return _api->oxi_publish_event(_ox->event_system, event);
}

static inline bool ox_subscribe_events(
		ox_EventType type,
		void(*callback)(const ox_Event* event, const void* user_data),
		void* user_data) {
	return _api->oxi_subscribe_events(_ox->event_system, type, callback, user_data);
}

static inline ox_Event* ox_make_event(ox_EventType type) {
	return _api->oxi_make_event(_ox->event_system, type);
}

int ox_init(Ox* ox, const ox_Api* api);

#define OX_INIT(name)\
    int name##_ox_init(void);\
    int ox_init(Ox* ox, const ox_Api* api) {\
        _ox = ox;\
        _api = api;\
        return name##_ox_init();\
    }\
    int name##_ox_init(void) 			       

#endif
