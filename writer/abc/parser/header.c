#include "abc/common.h"

#include <string.h>

extern void parser_read_header(abc_ctx_t *ctx) {
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

	ctx->meta.duration_us = (240000000UL * base_num) / base_den / base_bpm;
	return;
}