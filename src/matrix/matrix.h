#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t row;
    uint8_t column;
} matrix_key_t;

void matrix_init(void);
void matrix_process(void);
bool matrix_take_pressed_event(matrix_key_t *key);
bool matrix_key_is_pressed(uint8_t row, uint8_t column);
