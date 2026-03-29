#include "matrix/matrix.h"

#include <stddef.h>

#include "config/board_config.h"
#include "pico/stdlib.h"

#define MATRIX_KEY_COUNT (MATRIX_ROW_COUNT * MATRIX_COLUMN_COUNT)

static uint16_t raw_state;
static uint16_t stable_state;
static uint16_t pending_presses;
static absolute_time_t last_change_at[MATRIX_KEY_COUNT];

static size_t matrix_index(size_t row, size_t column) {
    return (row * MATRIX_COLUMN_COUNT) + column;
}

static uint16_t matrix_scan_raw_state(void) {
    uint16_t state = 0u;

    for(size_t row = 0; row < MATRIX_ROW_COUNT; ++row) {
        // Rows idle high; a pressed switch pulls the active row's column low.
        gpio_put(MATRIX_ROW_PINS[row], 0);
        sleep_us(3);

        for(size_t column = 0; column < MATRIX_COLUMN_COUNT; ++column) {
            if(gpio_get(MATRIX_COLUMN_PINS[column]) == 0) {
                state |= (uint16_t)(1u << matrix_index(row, column));
            }
        }

        gpio_put(MATRIX_ROW_PINS[row], 1);
    }

    return state;
}

void matrix_init(void) {
    const absolute_time_t now = get_absolute_time();

    raw_state = 0u;
    stable_state = 0u;
    pending_presses = 0u;

    for(size_t row = 0; row < MATRIX_ROW_COUNT; ++row) {
        gpio_init(MATRIX_ROW_PINS[row]);
        gpio_set_dir(MATRIX_ROW_PINS[row], GPIO_OUT);
        gpio_put(MATRIX_ROW_PINS[row], 1);
    }

    for(size_t column = 0; column < MATRIX_COLUMN_COUNT; ++column) {
        gpio_init(MATRIX_COLUMN_PINS[column]);
        gpio_set_dir(MATRIX_COLUMN_PINS[column], GPIO_IN);
        gpio_pull_up(MATRIX_COLUMN_PINS[column]);
    }

    for(size_t key = 0; key < MATRIX_KEY_COUNT; ++key) {
        last_change_at[key] = now;
    }
}

void matrix_process(void) {
    const uint16_t scanned_state = matrix_scan_raw_state();
    const absolute_time_t now = get_absolute_time();

    for(size_t row = 0; row < MATRIX_ROW_COUNT; ++row) {
        for(size_t column = 0; column < MATRIX_COLUMN_COUNT; ++column) {
            const size_t index = matrix_index(row, column);
            const uint16_t mask = (uint16_t)(1u << index);
            const bool scanned_pressed = (scanned_state & mask) != 0u;
            const bool raw_pressed = (raw_state & mask) != 0u;
            const bool stable_pressed = (stable_state & mask) != 0u;

            if(scanned_pressed != raw_pressed) {
                last_change_at[index] = now;
                raw_state ^= mask;
                continue;
            }

            if((scanned_pressed != stable_pressed) &&
               (absolute_time_diff_us(last_change_at[index], now) >=
                (int64_t)MATRIX_DEBOUNCE_MS * 1000)) {
                stable_state ^= mask;

                if(scanned_pressed) {
                    pending_presses |= mask;
                }
            }
        }
    }
}

bool matrix_take_pressed_event(matrix_key_t *key) {
    for(size_t row = 0; row < MATRIX_ROW_COUNT; ++row) {
        for(size_t column = 0; column < MATRIX_COLUMN_COUNT; ++column) {
            const uint16_t mask = (uint16_t)(1u << matrix_index(row, column));

            if((pending_presses & mask) == 0u) {
                continue;
            }

            pending_presses &= (uint16_t)~mask;

            if(key != NULL) {
                key->row = (uint8_t)row;
                key->column = (uint8_t)column;
            }

            return true;
        }
    }

    return false;
}

bool matrix_key_is_pressed(uint8_t row, uint8_t column) {
    if((row >= MATRIX_ROW_COUNT) || (column >= MATRIX_COLUMN_COUNT)) {
        return false;
    }

    return (stable_state & (uint16_t)(1u << matrix_index(row, column))) != 0u;
}
