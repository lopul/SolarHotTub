// SolarHotTub.c
// author: Lorenz Pullwitt
// copyright 2022
// license: GPL 2
// version: 8

#include <stdio.h>
#ifdef NO_PICO_DEBUG
// gcc -Wall -g -O0 -D NO_PICO_DEBUG -fsanitize=address SolarHotTub.c -o solarhottub
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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

struct Morse {
    int dit;
    int symb_space;
    int letter_space;
    int word_space;
};

static int play_symbol(uint8_t ch, struct Morse *m)
{
    int e;
    uint16_t morse;
    uint16_t sig;
    bool symb_active;
    morse = ch2morse(ch);
    e = morse == 0;
    if (e == 0) {
        symb_active = false;
        do {
            if (symb_active) {
                sleep_ms(m->symb_space);
            }
            sig = morse & 0x3;
            switch (sig) {
            case 1:
                gpio_put(PICO_DEFAULT_LED_PIN, 1);
                sleep_ms(m->dit);
                gpio_put(PICO_DEFAULT_LED_PIN, 0);
                break;
            case 2:
                gpio_put(PICO_DEFAULT_LED_PIN, 1);
                sleep_ms(m->dit * 3);
                gpio_put(PICO_DEFAULT_LED_PIN, 0);
                break;
            default: e = 1;
            }
            if (e == 0) {
                symb_active = true;
            }
            morse >>= 2;
        } while (morse > 0 && e == 0);
    }
    return e;
}

static int play_morse(char *str, struct Morse *m)
{
    int e;
    int i;
    uint8_t ch;
    bool word_active;
    e = str == NULL;
    if (e == 0) {
        i = 0;
        do {
            ch = str[i++];
            if (ch != '\0') {
                if (ch != ' ') {
                    if (word_active)
                        sleep_ms(m->letter_space);
                    e = play_symbol(ch, m);
                    if (e == 0)
                        word_active = true;
                } else {
                    sleep_ms(m->word_space);
                    word_active = false;
                }
            }
        } while (ch != '\0' && e == 0);
    }
    return e;
}

int main()
{
    int e;
    uint16_t morse;
    uint8_t ch;
    uint16_t sig;
    char symb;
    struct Morse *m;
    size_t size;
    size = sizeof(struct Morse);
    m = malloc(size);
    e = m == NULL;
    if (e == 0) {
        m->dit = 120;
        m->symb_space = 120;
        m->letter_space = 120 * 3;
        m->word_space = 120 * 7 * 2;
        for (ch = 0; ch <= 90 && e == 0; ch++) {
            morse = ch2morse(ch);
            if (morse > 0) {
                printf("%c = ", ch);
                do {
                    sig = morse & 0x3;
                    switch (sig) {
                    case 1:
                        symb = '.'; break;
                    case 2:
                        symb = '-'; break;
                    default:
                        e = 1;
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
        while (e == 0) {
            e = play_morse("MORSE CODE ", m);
        }
    }
    return e;
}



