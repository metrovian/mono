#pragma once
#include <stdlib.h>
#include <stdint.h>

extern int mono_link(
    void (*play_note)(uint32_t duration_ms, uint8_t note),
    void (*play_rest)(uint32_t duration_ms));

extern int mono_verify(uint8_t *data, uint32_t size);
extern int mono_play(uint8_t *data);