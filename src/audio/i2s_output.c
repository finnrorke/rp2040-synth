#include "audio/i2s_output.h"

#include "audio/tone_generator.h"
#include "config/board_config.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "i2s_tx.pio.h"

#define AUDIO_I2S_PIO pio0
#define AUDIO_I2S_SM 0u
#define AUDIO_TX_FRAME_WORDS 2u

static inline void i2s_output_enqueue_frame(void) {
    const uint32_t sample = (uint32_t)tone_generator_next_sample();

    pio_sm_put(AUDIO_I2S_PIO, AUDIO_I2S_SM, sample);
    pio_sm_put(AUDIO_I2S_PIO, AUDIO_I2S_SM, sample);
}

void i2s_output_init(void) {
    const PIO pio = AUDIO_I2S_PIO;
    const uint sm = AUDIO_I2S_SM;
    const uint offset = pio_add_program(pio, &i2s_tx_program);
    pio_sm_config config = i2s_tx_program_get_default_config(offset);
    const float bit_clock_hz =
        (float)AUDIO_SAMPLE_RATE_HZ * (float)AUDIO_BITS_PER_SAMPLE * (float)AUDIO_CHANNEL_COUNT;
    const float instruction_rate_hz = bit_clock_hz * 2.0f;
    const float clock_divider = (float)clock_get_hz(clk_sys) / instruction_rate_hz;

    pio_gpio_init(pio, AUDIO_I2S_BCK_PIN);
    pio_gpio_init(pio, AUDIO_I2S_LRCK_PIN);
    pio_gpio_init(pio, AUDIO_I2S_DATA_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, AUDIO_I2S_BCK_PIN, 3u, true);

    sm_config_set_out_pins(&config, AUDIO_I2S_DATA_PIN, 1u);
    sm_config_set_sideset_pins(&config, AUDIO_I2S_BCK_PIN);
    sm_config_set_out_shift(&config, false, true, 32u);
    sm_config_set_clkdiv(&config, clock_divider);

    pio_sm_init(pio, sm, offset, &config);
    pio_sm_clear_fifos(pio, sm);

    for(uint32_t frame = 0; frame < (4u / AUDIO_TX_FRAME_WORDS); ++frame) {
        i2s_output_enqueue_frame();
    }

    pio_sm_set_enabled(pio, sm, true);
}

void i2s_output_process(void) {
    while(pio_sm_get_tx_fifo_level(AUDIO_I2S_PIO, AUDIO_I2S_SM) <= 2u) {
        i2s_output_enqueue_frame();
    }
}
