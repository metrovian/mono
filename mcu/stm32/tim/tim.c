#include "tim.h"
#include "stm32f1xx_hal.h"

#include <math.h>

#define TIM_HANDLE htim1
#define TIM_CHANNEL TIM_CHANNEL_1
#define TIM_RELEASE_TIME_MS 10
#define TIM_UNIT_TIME 1000000
#define TIM_A4 440

extern TIM_HandleTypeDef TIM_HANDLE;

extern void tim_play_note(uint16_t duration_ms, uint8_t note) {
	float frequency = TIM_A4 * powf(2.0f, ((int)note - 69) / 12.0f);
	if (duration_ms > TIM_RELEASE_TIME_MS) {
		uint32_t period = TIM_UNIT_TIME / frequency;
		__HAL_TIM_SET_AUTORELOAD(&TIM_HANDLE, period - 1);
		__HAL_TIM_SET_COUNTER(&TIM_HANDLE, 0);
		__HAL_TIM_SET_COMPARE(&TIM_HANDLE, TIM_CHANNEL, period / 2);
		HAL_Delay(duration_ms - TIM_RELEASE_TIME_MS);
	}

	tim_play_rest(TIM_RELEASE_TIME_MS);
	return;
}

extern void tim_play_rest(uint16_t duration_ms) {
	__HAL_TIM_SET_COMPARE(&TIM_HANDLE, TIM_CHANNEL, 0);
	HAL_Delay(duration_ms);
	return;
}