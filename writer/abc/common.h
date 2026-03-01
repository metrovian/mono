#pragma once
#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
	uint32_t duration_us;
	uint8_t note;
} abc_event_t;
#pragma pack(pop)

typedef struct {
	uint32_t capacity;
	uint32_t size;
	abc_event_t *data;
} abc_buffer_t;

typedef struct {
	uint32_t duration_us;
	int accidental[7];
} abc_meta_t;

typedef struct {
	FILE *fp;
	abc_buffer_t buffer;
	abc_meta_t meta;
} abc_ctx_t;

extern int buffer_create(abc_ctx_t *ctx);
extern void buffer_destroy(abc_ctx_t *ctx);
extern void buffer_push(abc_ctx_t *ctx, abc_event_t *event);

extern void parser_read_header(abc_ctx_t *ctx);
extern void parser_read_body(abc_ctx_t *ctx);