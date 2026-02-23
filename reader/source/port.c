#include "mono_export.h"
#include "common.h"

extern int mono_link(
    void (*play_note)(uint32_t, uint8_t),
    void (*play_rest)(uint32_t)) {
	if (play_note == NULL) {
		return -1;
	} else if (play_rest == NULL) {
		return -2;
	}

	mono_port.play_note = play_note;
	mono_port.play_rest = play_rest;
	return 0;
}