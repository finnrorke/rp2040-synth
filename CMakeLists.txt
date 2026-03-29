cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rp2040_synth C CXX ASM)

set(CMAKE_C_STANDARD 23)

pico_sdk_init()

add_executable(rp2040_synth
    src/audio/i2s_output.c
    src/audio/tone_generator.c
    src/config/board_config.c
    src/indicators/status_led.c
    src/main.c
    src/matrix/matrix.c
)

pico_generate_pio_header(rp2040_synth
    ${CMAKE_CURRENT_LIST_DIR}/src/audio/i2s_tx.pio
)

target_include_directories(rp2040_synth PRIVATE src)

target_link_libraries(rp2040_synth
    hardware_clocks
    hardware_pio
    m
    pico_stdlib
)

pico_enable_stdio_usb(rp2040_synth 1)
pico_enable_stdio_uart(rp2040_synth 0)
pico_add_extra_outputs(rp2040_synth)
