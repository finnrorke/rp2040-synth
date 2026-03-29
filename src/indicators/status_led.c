#include "indicators/status_led.h"

#include <stdbool.h>

#include "config/board_config.h"
#include "pico/stdlib.h"

static bool led_active;
static absolute_time_t led_active_until;

void status_led_init(void) {
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
    gpio_put(STATUS_LED_PIN, 0);

    led_active = false;
    led_active_until = get_absolute_time();
}

void status_led_pulse(void) {
    led_active = true;
    led_active_until = make_timeout_time_ms(STATUS_LED_PULSE_MS);
    gpio_put(STATUS_LED_PIN, 1);
}

void status_led_process(void) {
    if(!led_active) {
        return;
    }

    if(absolute_time_diff_us(get_absolute_time(), led_active_until) > 0) {
        return;
    }

    gpio_put(STATUS_LED_PIN, 0);
    led_active = false;
}
