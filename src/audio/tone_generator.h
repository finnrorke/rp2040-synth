#pragma once

#include <stdint.h>

void tone_generator_init(float frequency_hz, uint32_t sample_rate_hz);
int32_t tone_generator_next_sample(void);
