#include "audio/tone_generator.h"

#include <math.h>
#include <stdint.h>

#define TONE_LUT_SIZE 256u
#define TONE_LUT_INDEX_SHIFT 24u
#define TONE_AMPLITUDE INT32_C(0x28000000)
#define TAU_F 6.28318530717958647692f

static int32_t sine_lut[TONE_LUT_SIZE];
static uint32_t phase_accumulator;
static uint32_t phase_step;

void tone_generator_init(float frequency_hz, uint32_t sample_rate_hz) {
    for(uint32_t index = 0; index < TONE_LUT_SIZE; ++index) {
        const float phase = ((float)index * TAU_F) / (float)TONE_LUT_SIZE;
        sine_lut[index] = (int32_t)(sinf(phase) * (float)TONE_AMPLITUDE);
    }

    phase_accumulator = 0u;
    phase_step = (uint32_t)(((double)frequency_hz * 4294967296.0) / (double)sample_rate_hz);
}

int32_t tone_generator_next_sample(void) {
    const int32_t sample = sine_lut[phase_accumulator >> TONE_LUT_INDEX_SHIFT];
    phase_accumulator += phase_step;
    return sample;
}
