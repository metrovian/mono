#include "test/alsa/export.h"

#include <unistd.h>
#include <alloca.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define ALSA_BUFFER_SIZE 64
#define ALSA_RELEASE_TIME_MS 10
#define ALSA_MICRO 1000000
#define ALSA_A4 440
#define ALSA_VOLUME 30000

typedef struct {
	int16_t note[ALSA_BUFFER_SIZE];
	int16_t rest[ALSA_BUFFER_SIZE];
} alsa_buffer_t;

typedef struct {
	snd_pcm_t *handle;
	uint32_t channel;
	uint32_t sample_rate;
	alsa_buffer_t buffer;
} alsa_ctx_t;
static alsa_ctx_t alsa_ctx = {
    .channel = 1,
    .sample_rate = 44100,
};

static uint64_t loop_count(uint32_t duration_us);
static double step_phase(uint8_t note);

extern int alsa_open() {
	if (snd_pcm_open(
		&alsa_ctx.handle,
		"default",
		SND_PCM_STREAM_PLAYBACK,
		0) != 0) {
		return -1;
	} else if (
	    snd_pcm_set_params(
		alsa_ctx.handle,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		alsa_ctx.channel,
		alsa_ctx.sample_rate,
		1,
		500000) != 0) {
		return -2;
	}

	return 0;
}

extern void alsa_close() {
	if (alsa_ctx.handle != NULL) {
		snd_pcm_drain(alsa_ctx.handle);
		snd_pcm_close(alsa_ctx.handle);
		alsa_ctx.handle = NULL;
	}

	return;
}

extern void alsa_play_note(uint32_t duration_us, uint8_t note) {
	uint64_t loop = loop_count(duration_us - ALSA_RELEASE_TIME_MS * 1000);
	double step = step_phase(note);
	double phase = 0.0;
	for (uint64_t i = 0; i < loop; ++i) {
		for (uint64_t j = 0; j < ALSA_BUFFER_SIZE; ++j) {
			alsa_ctx.buffer.note[j] = (phase < 0.5) ? ALSA_VOLUME : -ALSA_VOLUME;
			phase += step;
			if (phase >= 1.0) {
				phase -= 1.0;
			}
		}

		snd_pcm_writei(
		    alsa_ctx.handle,
		    alsa_ctx.buffer.note,
		    ALSA_BUFFER_SIZE);
	}

	alsa_play_rest(ALSA_RELEASE_TIME_MS * 1000);
	return;
}

extern void alsa_play_rest(uint32_t duration_us) {
	uint64_t loop = loop_count(duration_us);
	for (uint64_t i = 0; i < loop; ++i) {
		snd_pcm_writei(
		    alsa_ctx.handle,
		    alsa_ctx.buffer.rest,
		    ALSA_BUFFER_SIZE);
	}

	return;
}

static uint64_t loop_count(uint32_t duration_us) {
	uint64_t loop = duration_us;
	loop *= alsa_ctx.sample_rate;
	loop /= ALSA_MICRO;
	loop /= ALSA_BUFFER_SIZE;
	return loop;
}

static double step_phase(uint8_t note) {
	double frequency = ALSA_A4 * pow(2.0, (note - 69) / 12.0);
	return frequency / (double)alsa_ctx.sample_rate;
}
