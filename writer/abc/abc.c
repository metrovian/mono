#include "abc/export.h"
#include "abc/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int abc_parse(const char *path, uint8_t *data) {
	abc_ctx_t ctx = {0};
	ctx.fp = fopen(path, "r");
	if (ctx.fp == NULL) {
		return -1;
	} else if (buffer_create(&ctx) != 0) {
		return -2;
	}

	parser_read_header(&ctx);
	parser_read_body(&ctx);
	if (data == NULL) {
		return sizeof(abc_event_t) * ctx.buffer.size;
	}

	memcpy(data, ctx.buffer.data, sizeof(abc_event_t) * ctx.buffer.size);
	buffer_destroy(&ctx);
	return sizeof(abc_event_t) * ctx.buffer.size;
}
