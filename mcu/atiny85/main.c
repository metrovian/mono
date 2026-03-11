#include "mono/mono_export.h"
#include "example.h"

#include <assert.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define GPIO_PIN PB1
#define GPIO_RELEASE_TIME_MS 10

static const uint16_t period_us[] PROGMEM = {
    61156, 57724, 54484, 51426, 48540, 45815, 43243, 40816,
    38526, 36364, 34323, 32396, 30578, 28862, 27242, 25713,
    24270, 22908, 21622, 20408, 19263, 18182, 17161, 16198,
    15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204,
    9631, 9091, 8581, 8099, 7645, 7215, 6811, 6428,
    6067, 5727, 5405, 5102, 4815, 4545, 4290, 4050,
    3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551,
    2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,
    1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,
    956, 902, 851, 804, 758, 716, 676, 638,
    602, 568, 536, 506, 478, 451, 426, 402,
    379, 358, 338, 319, 301, 284, 268, 253,
    239, 225, 213, 201, 190, 179, 169, 159,
    150, 142, 134, 127, 119, 113, 106, 100, 95, 89, 84, 80};

static void gpio_play_note(uint16_t duration_ms, uint8_t note);
static void gpio_play_rest(uint16_t duration_ms);
static void delay_us(uint16_t duration_us);

int main(void) {
	DDRB |= (1 << GPIO_PIN);
	mono_link(gpio_play_note, gpio_play_rest);
	assert(mono_verify(mono_data, sizeof(mono_data)) == 0);
	while (1) {
		mono_play(mono_data);
		gpio_play_rest(10000);
	}

	return 0;
}

static void gpio_play_note(uint16_t duration_ms, uint8_t note) {
	if (duration_ms > GPIO_RELEASE_TIME_MS) {
		uint16_t us = pgm_read_word(&period_us[note]);
		uint32_t cycles = (duration_ms - GPIO_RELEASE_TIME_MS) * 1000UL;
		cycles /= us * 2UL;
		for (uint32_t i = 0; i < cycles; i++) {
			PORTB |= (1 << GPIO_PIN);
			delay_us(us);
			PORTB &= ~(1 << GPIO_PIN);
			delay_us(us);
		}
	}

	gpio_play_rest(GPIO_RELEASE_TIME_MS);
	return;
}

static void gpio_play_rest(uint16_t duration_ms) {
	PORTB &= ~(1 << GPIO_PIN);
	while (duration_ms--) {
		_delay_ms(1);
	}

	return;
}

static void delay_us(uint16_t duration_us) {
	while (duration_us--) {
		_delay_us(1);
	}

	return;
}
