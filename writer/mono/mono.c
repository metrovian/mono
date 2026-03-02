#include "mono/export.h"

#include <stdio.h>

typedef struct {
	FILE *fp;
	uint8_t *data;
	uint32_t size;
	uint32_t count;
} mono_ctx_t;

static void write_magic(mono_ctx_t *ctx);
static void write_size(mono_ctx_t *ctx);
static void write_payload(mono_ctx_t *ctx);

extern int mono_create(const char *path, uint8_t *data, uint32_t size) {
	mono_ctx_t ctx = {0};
	ctx.fp = fopen(path, "w");
	if (ctx.fp == NULL) {
		return -1;
	}

	fprintf(ctx.fp, "#pragma once\n");
	fprintf(ctx.fp, "#include <stdint.h>\n\n");
	fprintf(ctx.fp, "static const uint8_t mono_data[] = {\n");
	ctx.data = data;
	ctx.size = size;
	write_magic(&ctx);
	write_size(&ctx);
	write_payload(&ctx);
	fprintf(ctx.fp, "\n};\n\n");
	fclose(ctx.fp);
	return 0;
}

static void write_magic(mono_ctx_t *ctx) {
	const char magic[4] = "mono";
	for (uint8_t i = 0; i < 4; ++i) {
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "  ");
		}

		fprintf(ctx->fp, "0x%02X,", (uint8_t)magic[i]);
		++ctx->count;
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "\n");
		} else {
			fprintf(ctx->fp, " ");
		}
	}

	return;
}

static void write_size(mono_ctx_t *ctx) {
	for (uint8_t i = 0; i < 4; ++i) {
		uint8_t bit = (ctx->size >> (8 * i)) & 0xFF;
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "  ");
		}

		fprintf(ctx->fp, "0x%02X,", bit);
		++ctx->count;
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "\n");
		} else {
			fprintf(ctx->fp, " ");
		}
	}

	return;
}

static void write_payload(mono_ctx_t *ctx) {
	for (uint32_t i = 0; i < ctx->size; i++) {
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "  ");
		}

		fprintf(ctx->fp, "0x%02X,", ctx->data[i]);
		++ctx->count;
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "\n");
		} else {
			fprintf(ctx->fp, " ");
		}
	}

	return;
}