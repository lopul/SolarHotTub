// SolarHotTub.c
// author: Lorenz Pullwitt
// copyright 2022
// license: GPL 2
// version: 4

#ifdef NO_PICO_DEBUG
// gcc -Wall -g -O0 -D NO_PICO_DEBUG -fsanitize=address SolarHotTub.c -o solarhottub
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
typedef unsigned int uint;
static void gpio_put (uint gpio, bool value)
{
}
static void gpio_init (uint gpio)
{
}
static void gpio_set_dir (uint gpio, bool out)
{
}
static void sleep_ms (uint32_t ms)
{
}
#define PICO_DEFAULT_LED_PIN 25
#define GPIO_OUT 1

#else

#include "pico/stdlib.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#endif

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
    int e;
    uint16_t morse;
    uint8_t ch;
    uint16_t sig;
    char symb;
    e = 0;
    for (ch = 0; ch <= 90 && e == 0; ch++) {
        morse = ch2morse(ch);
        if (morse > 0) {
            printf("%c = ", ch);
            do {
                sig = morse & 0x3;
                switch (sig) {
                    case 1: symb = '.'; break;
                    case 2: symb = '-'; break;
                    default: e = 1;
                }
                if (e == 0)
                    printf("%c", symb);
                morse >>= 2;
            } while (morse > 0 && e == 0);
            printf("\n");
        }
    }
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(250);
    }
    return e;
}



