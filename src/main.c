#include "config/board_config.h"
#include "indicators/status_led.h"
#include "matrix/matrix.h"
#include "pico/stdlib.h"

int main(void) {
    stdio_init_all();
    matrix_init();
    status_led_init();

    while(true) {
        matrix_process();

        while(matrix_take_pressed_event(NULL)) {
            status_led_pulse();
        }

        status_led_process();
        sleep_ms(MATRIX_SCAN_INTERVAL_MS);
    }
}
