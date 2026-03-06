#pragma once
#include <stdint.h>

extern int alsa_open();
extern void alsa_close();
extern void alsa_play_note(uint32_t duration_us, uint8_t note);
extern void alsa_play_rest(uint32_t duration_us);