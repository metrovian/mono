#include "abc/export.h"
#include "mono/export.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "%s <input.abc> <output.mono>\n", argv[0]);
		return -1;
	}

	const char *input = argv[1];
	const char *output = argv[2];
	uint8_t *data = NULL;
	int size = abc_parse(input, &data);
	if (size < 0) {
		fprintf(stderr, "failed to parse abc format\n");
		return -2;
	} else if (mono_create(output, data, size) != 0) {
		fprintf(stderr, "failed to create mono format\n");
		return -3;
	}

	free(data);
	data = NULL;
	fprintf(stderr, "created %s\n", output);
	return 0;
}