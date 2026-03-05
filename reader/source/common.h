#pragma once
#include <stdint.h>

typedef struct {
	void (*play_note)(uint32_t, uint8_t);
	void (*play_rest)(uint32_t);
} mono_port_t;
extern mono_port_t mono_port;