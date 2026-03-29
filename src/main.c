#include "audio/i2s_output.h"
#include "audio/tone_generator.h"
#include "config/board_config.h"
#include "indicators/status_led.h"
#include "matrix/matrix.h"
#include "pico/stdlib.h"

int main(void) {
    absolute_time_t next_matrix_scan_at;

    stdio_init_all();
    matrix_init();
    status_led_init();
    tone_generator_init(AUDIO_TEST_TONE_HZ, AUDIO_SAMPLE_RATE_HZ);
    i2s_output_init();
    next_matrix_scan_at = make_timeout_time_ms(MATRIX_SCAN_INTERVAL_MS);

    while(true) {
        i2s_output_process();

        if(absolute_time_diff_us(get_absolute_time(), next_matrix_scan_at) <= 0) {
            matrix_process();

            while(matrix_take_pressed_event(NULL)) {
                status_led_pulse();
            }

            next_matrix_scan_at = delayed_by_ms(next_matrix_scan_at, MATRIX_SCAN_INTERVAL_MS);

            if(absolute_time_diff_us(next_matrix_scan_at, get_absolute_time()) < 0) {
                next_matrix_scan_at = make_timeout_time_ms(MATRIX_SCAN_INTERVAL_MS);
            }
        }

        status_led_process();
        tight_loop_contents();
    }
}
