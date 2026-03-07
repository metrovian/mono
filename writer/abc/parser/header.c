#include "abc/common.h"

#include <string.h>

typedef struct {
	uint32_t unit_num;
	uint32_t unit_den;
	uint32_t tempo_num;
	uint32_t tempo_den;
	uint32_t bpm;
} abc_duration_t;

static void process_unit(const char *ptr, abc_duration_t *duration);
static void process_tempo(const char *ptr, abc_duration_t *duration);
static void process_accidental(const char *ptr, abc_ctx_t *ctx);
static int minor_fifths(char root, char accidental);
static int major_fifths(char root, char accidental);

extern void parser_read_header(abc_ctx_t *ctx) {
	abc_duration_t duration = {
	    .unit_num = 1,
	    .unit_den = 4,
	    .tempo_num = 1,
	    .tempo_den = 4,
	    .bpm = 120,
	};

	char line[256] = {0};
	while (1) {
		long pos = ftell(ctx->fp);
		if (fgets(line, sizeof(line), ctx->fp) == NULL ||
		    strlen(line) < 2) {
			break;
		} else if (line[1] != ':') {
			fseek(ctx->fp, pos, SEEK_SET);
			break;
		}

		switch (line[0]) {
		case 'L':
			process_unit(line + 2, &duration);
			break;
		case 'Q':
			process_tempo(line + 2, &duration);
			break;
		case 'K':
			process_accidental(line + 2, ctx);
			break;
		default:
			break;
		}
	}

	ctx->meta.duration_ms = 60000UL;
	ctx->meta.duration_ms *= duration.unit_num * duration.tempo_den;
	ctx->meta.duration_ms /= duration.unit_den * duration.tempo_num * duration.bpm;
	return;
}

static void process_unit(const char *ptr, abc_duration_t *duration) {
	if (sscanf(
		ptr,
		"%u/%u",
		&duration->unit_num,
		&duration->unit_den) != 2) {
		duration->unit_num = 1;
		duration->unit_den = 4;
	}

	return;
}

static void process_tempo(const char *ptr, abc_duration_t *duration) {
	if (sscanf(
		ptr,
		"%u/%u=%u",
		&duration->tempo_num,
		&duration->tempo_den,
		&duration->bpm) != 3) {
		duration->tempo_num = 1;
		duration->tempo_den = 4;
		sscanf(ptr, "%u", &duration->bpm);
	}

	return;
}

static void process_accidental(const char *ptr, abc_ctx_t *ctx) {
	static const uint8_t sharp_order[7] = {5, 2, 6, 3, 0, 4, 1};
	static const uint8_t flat_order[7] = {1, 4, 0, 3, 6, 2, 5};
	for (uint8_t i = 0; i < 7; ++i) {
		ctx->meta.accidental[i] = 0;
	}

	char root = 0;
	char accidental = 0;
	if (strncmp(ptr, "none", 4) == 0) {
		return;
	}

	root = *ptr++;
	if (root >= 'a' &&
	    root <= 'g') {
		root -= 'g' - 'G';
	}

	if (*ptr == '#') {
		accidental = 1;
		++ptr;
	} else if (*ptr == 'b') {
		accidental = -1;
		++ptr;
	}

	int fifths = 0;
	if (*ptr == 'm') {
		fifths = minor_fifths(root, accidental);
	} else {
		fifths = major_fifths(root, accidental);
	}

	if (fifths > 0) {
		for (uint8_t i = 0; i < fifths; ++i) {
			ctx->meta.accidental[sharp_order[i]] = 1;
		}
	} else if (fifths < 0) {
		for (uint8_t i = 0; i < -fifths; ++i) {
			ctx->meta.accidental[flat_order[i]] = -1;
		}
	}

	return;
}

static int minor_fifths(char root, char accidental) {
	if (root < 'A' ||
	    root > 'G') {
		return 0;
	}

	return major_fifths(root, accidental + 3);
}

static int major_fifths(char root, char accidental) {
	static const int semitone[7] = {9, 11, 0, 2, 4, 5, 7};
	if (root < 'A' ||
	    root > 'G') {
		return 0;
	}

	int base = (semitone[root - 'A'] + accidental + 12) % 12;
	int fifths = (base * 7) % 12;
	if (fifths > 6) {
		fifths -= 12;
	}

	return fifths;
}