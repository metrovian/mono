#include "abc/common.h"

#include <ctype.h>

extern void parser_read_body(abc_ctx_t *ctx) {
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
			abc_event_t event = {0};
			event.duration_us = (ctx->meta.duration_us * num) / den;
			event.note = semitone[note - 'A'] + 12 * (octave + 5);
			buffer_push(ctx, &event);
		}
	}

	return;
}