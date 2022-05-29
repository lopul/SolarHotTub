// SolarHotTub.c
// author: Lorenz Pullwitt
// copyright 2022
// license: GPL 2
// version: 0

#include "pico/stdlib.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#endif

static uint16_t morse_chart[91] = {
    0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 0, 0, 0, 0, 0, 0, 0, // 8
    0, 0, 0, 0, 0, 0, 0, 0, // 16
    0, 0, 0, 0, 0, 0, 0, 0, // 24
    0, 0, 0, 0, 0, 0, 0, 0, // 32
    0, 0, 0, 0, 0, 0, 0, 0, // 40
    0x2AA, 0x2A9, 0x2A5, 0x295, 0x255, 0x155, 0x156, 0x15A, // 48 (0 - 7)
    0x16A, 0x1AA, 0, 0, 0, 0, 0, 0, // 56 (8 - 9)
    0, 0x9, 0x56, 0x66, 0x16, 0x1, 0x65, 0x1A, // 64 (A - G)
    0x55, 0x5, 0xA9, 0x26, 0x59, 0xA, 0x6, 0x2A, // 72 (H - O)
    0x69, 0x9A, 0x19, 0x15, 0x2, 0x25, 0x95, 0x29, // 80 (P - W)
    0x96, 0xA6, 0x5A // 88 (X - Z)
};

static uint16_t ch2morse(uint8_t ch)
{
    uint16_t morse;
    morse = 0;
    if (ch <= 90)
        morse = morse_chart[ch];
    return morse;
}

int main()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
}



