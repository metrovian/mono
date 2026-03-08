#pragma once
#include <stdint.h>

extern void tim_play_note(uint16_t duration_ms, uint8_t note);
extern void tim_play_rest(uint16_t duration_ms);