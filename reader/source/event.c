#include "mono_export.h"
#include "common.h"

#include <string.h>

typedef struct {
	uint32_t duration_ms;
	uint8_t note;
} mono_event_t;

extern int mono_verify(uint8_t *data, uint32_t size) {
	if (data == NULL ||
	    size < 8) {
		return -1;
	} else if (memcmp(data, "mono", 4) != 0) {
		return -2;
	}

	return 0;
}

extern int mono_play(uint8_t *data) {
	if (data == NULL) {
		return -1;
	}

	uint32_t len = 0;
	data += 4;
	memcpy(&len, data, sizeof(len));
	len /= sizeof(mono_event_t);
	if (len == 0) {
		return -2;
	}

	data += 4;
	for (uint32_t i = 0; i < len; ++i) {
		mono_event_t event = {0};
		memcpy(&event, data, sizeof(mono_event_t));
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