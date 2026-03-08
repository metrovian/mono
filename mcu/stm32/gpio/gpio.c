#include "gpio.h"
#include "stm32f1xx_hal.h"

#include <math.h>

#define GPIO_PORT GPIOA
#define GPIO_PIN GPIO_PIN_8
#define GPIO_RELEASE_TIME_MS 10
#define GPIO_UNIT_TIME 1000000
#define GPIO_A4 440

static void delay_us(uint32_t duration_us);

extern void gpio_play_note(uint16_t duration_ms, uint8_t note) {
	float frequency = GPIO_A4 * powf(2.0f, ((int)note - 69) / 12.0f);
	if (duration_ms > GPIO_RELEASE_TIME_MS) {
		uint32_t period_us = GPIO_UNIT_TIME / (frequency * 2);
		uint32_t duration_us = (duration_ms - GPIO_RELEASE_TIME_MS) * 1000;
		uint32_t cycles = (duration_us * frequency) / (float)GPIO_UNIT_TIME;
		for (uint32_t i = 0; i < cycles; ++i) {
			HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN, GPIO_PIN_SET);
			delay_us(period_us);
			HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN, GPIO_PIN_RESET);
			delay_us(period_us);
		}
	}

	gpio_play_rest(GPIO_RELEASE_TIME_MS);
	return;
}

extern void gpio_play_rest(uint16_t duration_ms) {
	HAL_Delay(duration_ms);
	return;
}

static void delay_us(uint32_t duration_us) {
	uint32_t start = SysTick->VAL;
	uint32_t load = SysTick->LOAD;
	uint32_t ticks = duration_us * (SystemCoreClock / 1000000);
	while (ticks > 0) {
		uint32_t now = SysTick->VAL;
		uint32_t elapsed = 0;
		if (start >= now) {
			elapsed = start - now;
		} else {
			elapsed = start + (load - now);
		}

		if (elapsed >= ticks) {
			break;
		}

		ticks -= elapsed;
		start = now;
	}

	return;
}