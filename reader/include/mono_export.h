#pragma once
#include <stdint.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#define memcpy_P memcpy
#define memcmp_P memcmp
#endif

extern int mono_link(
    void (*play_note)(uint16_t duration_ms, uint8_t note),
    void (*play_rest)(uint16_t duration_ms));

extern int mono_verify(const uint8_t *data, uint32_t size);
extern int mono_play(const uint8_t *data);