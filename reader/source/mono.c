#include "mono_export.h"

#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
	uint16_t duration_ms;
	uint8_t note;
} mono_event_t;
#pragma pack(pop)

typedef struct {
	void (*play_note)(uint16_t, uint8_t);
	void (*play_rest)(uint16_t);
} mono_port_t;
static mono_port_t mono_port = {0};

extern int mono_link(
    void (*play_note)(uint16_t, uint8_t),
    void (*play_rest)(uint16_t)) {
	if (play_note == NULL) {
		return -1;
	} else if (play_rest == NULL) {
		return -2;
	}

	mono_port.play_note = play_note;
	mono_port.play_rest = play_rest;
	return 0;
}

extern int mono_verify(const uint8_t *data, uint32_t size) {
	if (data == NULL ||
	    size < 8) {
		return -1;
	} else if (memcmp_P(data, "mono", 4) != 0) {
		return -2;
	}

	return 0;
}

extern int mono_play(const uint8_t *data) {
	if (data == NULL) {
		return -1;
	} else if (mono_port.play_note == NULL) {
		return -2;
	} else if (mono_port.play_rest == NULL) {
		return -3;
	}

	uint32_t len = 0;
	data += 4;
	memcpy_P(&len, data, sizeof(len));
	len /= sizeof(mono_event_t);
	if (len == 0) {
		return -4;
	}

	data += sizeof(len);
	for (uint32_t i = 0; i < len; ++i) {
		mono_event_t event = {0};
		memcpy_P(&event, data, sizeof(mono_event_t));
		if (event.note == 0) {
			mono_port.play_rest(event.duration_ms);
		} else {
			mono_port.play_note(
			    event.duration_ms,
			    event.note);
		}

		data += sizeof(mono_event_t);
	}

	return 0;
}