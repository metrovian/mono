#include "abc/common.h"

#include <string.h>

static void process_accidental(const char *ptr, abc_ctx_t *ctx);
static int major_fifths(char root, char accidental);

extern void parser_read_header(abc_ctx_t *ctx) {
	char line[256] = {0};
	uint32_t base_bpm = 120;
	uint32_t base_num = 1;
	uint32_t base_den = 4;
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
			sscanf(line + 2, "%u/%u", &base_num, &base_den);
			break;
		case 'K':
			process_accidental(line + 2, ctx);
			break;
		default:
			break;
		}
	}

	ctx->meta.duration_us = (240000000UL * base_num) / base_den / base_bpm;
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

	int fifths = major_fifths(root, accidental);
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