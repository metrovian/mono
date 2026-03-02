#include "mono/export.h"

#include <stdio.h>

typedef struct {
	FILE *fp;
	uint32_t count;
} mono_ctx_t;

static void write_magic(mono_ctx_t *ctx);
static void write_size(mono_ctx_t *ctx, uint32_t size);
static void write_payload(mono_ctx_t *ctx, uint8_t *data, uint32_t size);

extern int mono_create(const char *path, uint8_t *data, uint32_t size) {
	mono_ctx_t ctx = {0};
	ctx.fp = fopen(path, "w");
	if (ctx.fp == NULL) {
		return -1;
	}

	fprintf(ctx.fp, "#pragma once\n");
	fprintf(ctx.fp, "#include <stdint.h>\n\n");
	fprintf(ctx.fp, "static const uint8_t mono_data[] = {\n");
	write_magic(&ctx);
	write_size(&ctx, size);
	write_payload(&ctx, data, size);
	fprintf(ctx.fp, "\n};\n\n");
	fclose(ctx.fp);
	return 0;
}

static void write_magic(mono_ctx_t *ctx) {
	uint8_t magic[4] = "mono";
	write_payload(ctx, magic, sizeof(magic));
	return;
}

static void write_size(mono_ctx_t *ctx, uint32_t size) {
	uint8_t bits[4] = {0};
	for (uint8_t i = 0; i < 4; ++i) {
		bits[i] = (size >> (8 * i)) & 0xFF;
	}

	write_payload(ctx, bits, sizeof(bits));
	return;
}

static void write_payload(mono_ctx_t *ctx, uint8_t *data, uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "  ");
		}

		fprintf(ctx->fp, "0x%02X,", data[i]);
		++ctx->count;
		if (ctx->count % 12 == 0) {
			fprintf(ctx->fp, "\n");
		} else {
			fprintf(ctx->fp, " ");
		}
	}

	return;
}