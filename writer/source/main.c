#include <stdio.h>
#include <stdint.h>

extern int mono_parse(const char *path);
extern int mono_write(const char *path, uint8_t *data, uint32_t size);

int main(int, char **) {
	return 0;
}