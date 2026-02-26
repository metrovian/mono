#include "abc/common.h"

#include <stdlib.h>

extern int buffer_create(abc_ctx_t *ctx) {
	abc_buffer_t *buffer = &ctx->buffer;
	buffer->capacity = 256;
	buffer->size = 0;
	buffer->data = malloc(sizeof(uint8_t) * buffer->capacity);
	return buffer->data == NULL;
}

extern void buffer_destroy(abc_ctx_t *ctx) {
	abc_event_t *data = ctx->buffer.data;
	if (data != NULL) {
		free(data);
		ctx->buffer.data = NULL;
	}

	return;
}

extern void buffer_push(abc_ctx_t *ctx, abc_event_t *event) {
	abc_buffer_t *buffer = &ctx->buffer;
	buffer->data[buffer->size].duration_us = event->duration_us;
	buffer->data[buffer->size].note = event->note;
	++buffer->size;
	if (buffer->capacity < buffer->size) {
		buffer->capacity *= 2;
		buffer->data = realloc(buffer->data, sizeof(abc_event_t) * buffer->capacity);
	}

	return;
}