#include <stdio.h>
#include <stdint.h>

extern int mono_create(const char *path, uint8_t *data, uint32_t size) {
	FILE *fp = fopen(path, "wb");
	if (fp == NULL) {
		return -1;
	} else if (fwrite("mono", 1, 4, fp) != 4) {
		fclose(fp);
		return -2;
	} else if (fwrite(&size, sizeof(uint32_t), 1, fp) != 1) {
		fclose(fp);
		return -3;
	} else if (fwrite(data, 1, size, fp) != size) {
		fclose(fp);
		return -4;
	}

	fclose(fp);
	return 0;
}