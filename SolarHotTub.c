// SolarHotTub.c
// author: Lorenz Pullwitt
// copyright 2022
// license: Public Domain
// version: 22

#include <stdio.h>
#include <stdlib.h>
#ifdef NO_PICO_DEBUG
// gcc -Wall -g -O0 -D NO_PICO_DEBUG -fsanitize=address SolarHotTub.c -o solarhottub
#include <stdint.h>
#include <stdbool.h>

typedef unsigned int uint;
enum gpio_function { GPIO_FUNC_XIP = 0, GPIO_FUNC_SPI = 1, GPIO_FUNC_UART = 2, GPIO_FUNC_I2C = 3, GPIO_FUNC_PWM = 4, GPIO_FUNC_SIO = 5, GPIO_FUNC_PIO0 = 6, GPIO_FUNC_PIO1 = 7, GPIO_FUNC_GPCK = 8, GPIO_FUNC_USB = 9, GPIO_FUNC_NULL = 0x1f };
static void gpio_put (uint gpio, bool value)
{
}
static void gpio_init(uint gpio)
{
}
static void gpio_set_dir(uint gpio, bool out)
{
}
static void sleep_ms(uint32_t ms)
{
}
static void adc_init()
{
}
static void adc_set_temp_sensor_enabled(bool enable)
{
}
static void adc_select_input(uint input)
{
}
static uint16_t adc_read()
{
    return 4095;
}
static enum gpio_function gpio_get_function(uint gpio)
{
    return GPIO_FUNC_PWM;
}
static void gpio_set_function(uint gpio, enum gpio_function fn)
{
}
static uint pwm_gpio_to_slice_num(uint gpio)
{
    return 0;
}
static void pwm_set_enabled(uint slice_num, bool enabled)
{
}
static void pwm_set_wrap(uint slice_num, uint16_t wrap)
{
}
static void pwm_set_chan_level(uint slice_num, uint chan, uint16_t level)
{
}
static uint pwm_gpio_to_channel(uint gpio)
{
    return 0;
}
static void pwm_set_gpio_level(uint gpio, uint16_t level)
{
}
static bool gpio_is_pulled_down(uint gpio)
{
    return false;
}
static uint16_t pwm_get_counter(uint slice_num)
{
    return 0;
}
static void adc_gpio_init(uint gpio)
{
}
#define PICO_DEFAULT_LED_PIN 25
#define GPIO_OUT 1

#else

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning requires a board with a regular LED
#endif

#endif

const uint BUZZER_SIO = 17; // 0
const uint LED_PWM = 0; // 17

static uint16_t morse_chart[91] = {
    0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 0, 0, 0, 0, 0, 0, 0, // 8
    0, 0, 0, 0, 0, 0, 0, 0, // 16
    0, 0, 0, 0, 0, 0, 0, 0, // 24
    0, 0, 0, 0, 0, 0, 0, 0, // 32
    0, 0, 0, 0, 0, 0, 0x999, 0, // 40 .
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
    int dit; // di-dah
    int symb_space;
    int letter_space; // inter-character
    int word_space; // inter-word
    uint slice_num;
    uint channel;
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
                pwm_set_gpio_level(LED_PWM, 32767);
//                pwm_set_enabled(m->slice_num, true);
//                pwm_set_chan_level(m->slice_num, m->channel, 32767); // 17857
                gpio_put(BUZZER_SIO, 1);
                sleep_ms(m->dit);
                pwm_set_gpio_level(LED_PWM, 0); // 65535
//                pwm_set_chan_level(m->slice_num, m->channel, 0); // 65535
//                pwm_set_enabled(m->slice_num, false);
                gpio_put(BUZZER_SIO, 0);
                break;
            case 2:
                pwm_set_gpio_level(LED_PWM, 32767);
//                pwm_set_enabled(m->slice_num, true);
//                pwm_set_chan_level(m->slice_num, m->channel, 32767); // 17857
                gpio_put(BUZZER_SIO, 1);
                sleep_ms(m->dit * 3);
                pwm_set_gpio_level(LED_PWM, 0); // 65535
//                pwm_set_chan_level(m->slice_num, m->channel, 0); // 65535
//                pwm_set_enabled(m->slice_num, false);
                gpio_put(BUZZER_SIO, 0);
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
    uint16_t adc_res;
    uint32_t mvolt;
    uint32_t digit1;
    uint32_t digit2;
    uint32_t digit3;
    uint32_t digit4;
    char morse_str[32];
    char symb;
    struct Morse *m;
    size_t size;
    enum gpio_function gpio_fn;
    char *str_fn;
    uint16_t count;
    bool is_pulled_down;
    bool do_waiting;
    bool give_report;
    int32_t report_cnt;
    uint32_t vs; // cV * s (centi volt seconds)
    size = sizeof(struct Morse);
    m = malloc(size);
    e = m == NULL;
    if (e == 0) {
        m->dit = 240;
        m->symb_space = 240;
        m->letter_space = 240 * 3 * 4;
        m->word_space = 240 * 7 * 4;
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
        gpio_init(BUZZER_SIO);
        gpio_set_dir(BUZZER_SIO, GPIO_OUT);
        gpio_set_function(LED_PWM, GPIO_FUNC_PWM);
        gpio_fn = gpio_get_function(LED_PWM);
        m->slice_num = pwm_gpio_to_slice_num(LED_PWM);
        pwm_set_enabled(m->slice_num, true);
        pwm_set_wrap(m->slice_num, 65535); // 35714 3500Hz ?
        m->channel = pwm_gpio_to_channel(LED_PWM);
        pwm_set_chan_level(m->slice_num, m->channel, 0);
        is_pulled_down = gpio_is_pulled_down(LED_PWM);
        adc_init();
        adc_gpio_init(28);
//        adc_set_temp_sensor_enabled(true);
        adc_select_input(2);
        report_cnt = 0; // -1
        vs = 0;
        while (e == 0) {
            adc_res = adc_read();
            mvolt = adc_res * 3540 / 4095;
            give_report = false;
            if (report_cnt <= 0) {
                give_report = true;
            }
            if (mvolt > 2700 || mvolt < 1335) {
                do_waiting = false;
                give_report = true;
            } else {
                do_waiting = true;
            }
            if (give_report) {
                digit1 = mvolt / 1000;
                digit2 = mvolt / 100 % 10;
                digit3 = mvolt / 10 % 10;
                digit4 = mvolt / 1 % 10;
                snprintf(morse_str, 31, "%c%c%c%c ", '0' + digit1, '0' + digit2, '0' + digit3, '0' + digit4);
                e = play_morse(morse_str, m); // "MORSE CODE "
                report_cnt = 300000;
            }
            if (e == 0) {
                switch (gpio_fn) {
                case GPIO_FUNC_PWM:
                    str_fn = "PWM";
                    break;
                case GPIO_FUNC_SIO:
                    str_fn = "SIO";
                    break;
                case GPIO_FUNC_PIO0:
                    str_fn = "PIO0";
                    break;
                case GPIO_FUNC_PIO1:
                    str_fn = "PIO1";
                    break;
                default:
                    str_fn = "UNK";
                    break;
                }
                snprintf(morse_str, 31, "%s ", str_fn);
//                e = play_morse(morse_str, m);
            }
            if (e == 0) {
                snprintf(morse_str, 31, "%d ", m->slice_num);
//                e = play_morse(morse_str, m);
            }
            if (e == 0) {
                count = pwm_get_counter(m->slice_num);
                snprintf(morse_str, 31, "%d ", count);
//                e = play_morse(morse_str, m);
            }
            if (e == 0) {
                snprintf(morse_str, 31, "%d ", m->channel);
//                e = play_morse(morse_str, m);
            }
            if (e == 0) {
                snprintf(morse_str, 31, "%d ", is_pulled_down);
//                e = play_morse(morse_str, m);
            }
            if (e == 0 && do_waiting) {
                sleep_ms(5000); // 1000
                report_cnt -= 5000;
            }
        }
    }
    return e;
}
