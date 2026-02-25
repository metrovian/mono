#include "abc/common.h"

#include <stdlib.h>

extern int buffer_create(abc_ctx_t *ctx) {
	ctx->capacity = 256;
	ctx->size = 0;
	ctx->buffer = malloc(sizeof(abc_ctx_t) * ctx->capacity);
	return ctx->buffer == NULL;
}

extern void buffer_destroy(abc_ctx_t *ctx) {
	if (ctx->buffer != NULL) {
		free(ctx->buffer);
		ctx->buffer = NULL;
	}

	return;
}

extern void buffer_push(abc_ctx_t *ctx, abc_event_t *event) {
	ctx->buffer[ctx->size].duration_us = event->duration_us;
	ctx->buffer[ctx->size].note = event->note;
	++ctx->size;
	if (ctx->capacity < ctx->size) {
		ctx->capacity *= 2;
		ctx->buffer = realloc(ctx->buffer, sizeof(abc_event_t) * ctx->capacity);
	}

	return;
}