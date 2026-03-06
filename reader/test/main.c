#include "mono_export.h"
#include "test/alsa/export.h"
#include "test/example.h"

#include <assert.h>

int main(int, char **) {
	assert(alsa_open() == 0);
	assert(mono_link(alsa_play_note, alsa_play_rest) == 0);
	assert(mono_verify(mono_data, sizeof(mono_data)) == 0);
	assert(mono_play(mono_data) == 0);
	alsa_close();
	return 0;
}