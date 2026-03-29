#pragma once

#include <stddef.h>

#include "pico/stdlib.h"

#define MATRIX_ROW_COUNT 4u
#define MATRIX_COLUMN_COUNT 4u

#define MATRIX_SCAN_INTERVAL_MS 1u
#define MATRIX_DEBOUNCE_MS 8u
#define STATUS_LED_PULSE_MS 80u
#define STATUS_LED_PIN PICO_DEFAULT_LED_PIN

extern const uint MATRIX_ROW_PINS[MATRIX_ROW_COUNT];
extern const uint MATRIX_COLUMN_PINS[MATRIX_COLUMN_COUNT];
