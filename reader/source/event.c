#include "mono_export.h"
#include "common.h"

#include <string.h>

#pragma pack(push, 1)
typedef struct {
	uint32_t duration_us;
	uint8_t note;
} mono_event_t;
#pragma pack(pop)

extern int mono_verify(const uint8_t *data, uint32_t size) {
	if (data == NULL ||
	    size < 8) {
		return -1;
	} else if (memcmp(data, "mono", 4) != 0) {
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
	memcpy(&len, data, sizeof(len));
	len /= sizeof(mono_event_t);
	if (len == 0) {
		return -4;
	}

	data += sizeof(len);
	for (uint32_t i = 0; i < len; ++i) {
		mono_event_t event = {0};
		memcpy(&event, data, sizeof(mono_event_t));
		if (event.note == 0) {
			mono_port.play_rest(event.duration_us);
		} else {
			mono_port.play_note(
			    event.duration_us,
			    event.note);
		}

		data += sizeof(mono_event_t);
	}

	return 0;
}