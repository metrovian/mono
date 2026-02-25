#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#pragma pack(push, 1)
typedef struct {
	uint32_t duration_us;
	uint8_t note;
} mono_event_t;
#pragma pack(pop)

typedef struct {
	FILE *fp;
	mono_event_t *events;
	uint32_t capacity;
	uint32_t size;
	uint32_t duration_us;
	uint8_t key;
} mono_ctx_t;

static int create(mono_ctx_t *ctx) {
	ctx->capacity = 256;
	ctx->size = 0;
	ctx->events = malloc(sizeof(mono_event_t) * ctx->capacity);
	return ctx->events == NULL;
}

static void delete(mono_ctx_t *ctx) {
	if (ctx->events != NULL) {
		free(ctx->events);
		ctx->events = NULL;
	}

	return;
}

static void put(mono_ctx_t *ctx, mono_event_t *event) {
	ctx->events[ctx->size].duration_us = event->duration_us;
	ctx->events[ctx->size].note = event->note;
	++ctx->size;
	if (ctx->capacity < ctx->size) {
		ctx->capacity *= 2;
		ctx->events = realloc(ctx->events, sizeof(mono_event_t) * ctx->capacity);
	}

	return;
}

static void read_header(mono_ctx_t *ctx) {
	char line[256] = {0};
	uint32_t base_bpm = 120;
	uint32_t base_num = 1;
	uint32_t base_den = 4;
	while (fgets(line, sizeof(line), ctx->fp) != NULL) {
		if (strlen(line) < 2) {
			break;
		} else if (line[1] != ':') {
			break;
		}

		switch (line[0]) {
		case 'L':
			sscanf(line + 2, "%u/%u", &base_num, &base_den);
			break;
		default:
			break;
		}
	}

	ctx->duration_us = (240000000UL * base_num) / base_den / base_bpm;
	return;
}

static void read_score(mono_ctx_t *ctx) {
	int note = 0;
	while ((note = fgetc(ctx->fp)) != EOF) {
		if (note == ' ' ||
		    note == '|' ||
		    note == '\n') {
			continue;
		}

		int octave = 0;
		if (note >= 'a' &&
		    note <= 'g') {
			note -= 'g' - 'G';
			++octave;
		}

		if (note >= 'A' &&
		    note <= 'G') {
			int next = fgetc(ctx->fp);
			if (octave == 1) {
				if (next == '\'') {
					next = fgetc(ctx->fp);
					if (next == '\'') {
						++octave;
					} else {
						ungetc(next, ctx->fp);
					}

					++octave;
				}
			} else {
				if (next == ',') {
					next = fgetc(ctx->fp);
					if (next == ',') {
						--octave;
					} else {
						ungetc(next, ctx->fp);
					}

					--octave;
				}
			}

			int num = 1;
			if (isdigit(next) != 0) {
				num = next - '0';
				next = fgetc(ctx->fp);
			}

			int den = 1;
			if (next == '/') {
				next = fgetc(ctx->fp);
				if (isdigit(next) != 0) {
					den = next - '0';
				} else {
					den = 2;
					ungetc(next, ctx->fp);
				}
			} else {
				ungetc(next, ctx->fp);
			}

			static const uint8_t semitone[7] = {9, 11, 0, 2, 4, 5, 7};
			mono_event_t event = {0};
			event.duration_us = (ctx->duration_us * num) / den;
			event.note = semitone[note - 'A'] + 12 * (octave + 5);
			put(ctx, &event);
		}
	}

	return;
}

extern int mono_parse(const char *path, uint8_t *data) {
	mono_ctx_t ctx = {0};
	ctx.fp = fopen(path, "r");
	if (ctx.fp == NULL) {
		return -1;
	} else if (create(&ctx) != 0) {
		return -2;
	}

	read_header(&ctx);
	read_score(&ctx);
	if (data == NULL) {
		return sizeof(mono_event_t) * ctx.size;
	}

	memcpy(data, ctx.events, sizeof(mono_event_t) * ctx.size);
	delete(&ctx);
	return sizeof(mono_event_t) * ctx.size;
}