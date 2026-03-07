#pragma once
#include <stdint.h>

extern int alsa_open();
extern void alsa_close();
extern void alsa_play_note(uint16_t duration_ms, uint8_t note);
extern void alsa_play_rest(uint16_t duration_ms);