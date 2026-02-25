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
	int size = mono_parse(input, NULL);
	if (size < 0) {
		fprintf(stderr, "failed to parse abc format\n");
		return -2;
	}

	uint8_t *data = malloc(size);
	if (data == NULL) {
		fprintf(stderr, "failed to allocate memory\n");
		return -3;
	} else if (mono_parse(input, data) != size) {
		fprintf(stderr, "failed to parse abc format\n");
		return -4;
	} else if (mono_create(output, data, size) != 0) {
		fprintf(stderr, "failed to create mono format\n");
		return -5;
	}

	fprintf(stderr, "created %s\n", output);
	free(data);
	data = NULL;
	return 0;
}