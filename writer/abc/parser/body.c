#include "abc/common.h"

#include <ctype.h>

typedef struct {
	int accidental;
	int name;
	int octave;
	int duration_num;
	int duration_den;
} abc_note_t;

static int process_note(abc_ctx_t *ctx);
static int process_accidental(abc_ctx_t *ctx, abc_note_t *note);
static int process_name(abc_ctx_t *ctx, abc_note_t *note);
static void process_octave(abc_ctx_t *ctx, abc_note_t *note);
static void process_duration(abc_ctx_t *ctx, abc_note_t *note);
static void process_push(abc_ctx_t *ctx, abc_note_t *note);

extern void parser_read_body(abc_ctx_t *ctx) {
	while (process_note(ctx) != EOF) {
		// continue;
	}

	return;
}

static int process_note(abc_ctx_t *ctx) {
	abc_note_t note = {0};
	if (process_accidental(ctx, &note) == EOF) {
		return EOF;
	}

	return process_name(ctx, &note);
}

static int process_accidental(abc_ctx_t *ctx, abc_note_t *note) {
	int now = fgetc(ctx->fp);
	if (now == EOF) {
		return EOF;
	}

	switch (now) {
	case '^':
		now = fgetc(ctx->fp);
		if (now == '^') {
			note->accidental += 2;
		} else {
			note->accidental += 1;
			ungetc(now, ctx->fp);
		}
		break;
	case '_':
		now = fgetc(ctx->fp);
		if (now == '_') {
			note->accidental -= 2;
		} else {
			note->accidental -= 1;
			ungetc(now, ctx->fp);
		}
		break;
	case '=':
		note->accidental = now;
		break;
	default:
		ungetc(now, ctx->fp);
		break;
	}

	return 0;
}

static int process_name(abc_ctx_t *ctx, abc_note_t *note) {
	int now = fgetc(ctx->fp);
	if (now == EOF) {
		return EOF;
	}

	if (now >= 'a' &&
	    now <= 'g') {
		now -= 'g' - 'G';
		++note->octave;
	}

	if (now >= 'A' &&
	    now <= 'G') {
		note->name = now;
		if (note->accidental == '=') {
			note->accidental = 0;
		} else {
			note->accidental += ctx->meta.accidental[now - 'A'];
		}

		process_octave(ctx, note);
		process_duration(ctx, note);
		process_push(ctx, note);
	} else if (now == 'z') {
		note->name = 'C';
		note->accidental = 0;
		note->octave = -5;
		process_duration(ctx, note);
		process_push(ctx, note);
	}

	return 0;
}

static void process_octave(abc_ctx_t *ctx, abc_note_t *note) {
	int now = 0;
	while ((now = fgetc(ctx->fp)) != EOF) {
		if (now == '\'') {
			++note->octave;
		} else if (now == ',') {
			--note->octave;
		} else {
			ungetc(now, ctx->fp);
			break;
		}
	}

	return;
}

static void process_duration(abc_ctx_t *ctx, abc_note_t *note) {
	int now = fgetc(ctx->fp);
	if (isdigit(now) != 0) {
		note->duration_num = now - '0';
		now = fgetc(ctx->fp);
	} else {
		note->duration_num = 1;
	}

	if (now == '/') {
		now = fgetc(ctx->fp);
		if (isdigit(now) != 0) {
			note->duration_den = now - '0';
		} else {
			note->duration_den = 2;
			ungetc(now, ctx->fp);
		}
	} else {
		note->duration_den = 1;
		ungetc(now, ctx->fp);
	}

	return;
}

static void process_push(abc_ctx_t *ctx, abc_note_t *note) {
	static const uint8_t semitone[7] = {9, 11, 0, 2, 4, 5, 7};
	abc_event_t event = {0};
	event.duration_us = (ctx->meta.duration_us * note->duration_num) / note->duration_den;
	event.note = note->accidental + semitone[note->name - 'A'] + (note->octave + 5) * 12;
	buffer_push(ctx, &event);
	return;
}