#ifndef WIN32
#include "esp_log.h"
#include "led_helper.h"
#endif  // WIN32

#include <stdarg.h>
#include "color.h"
#include "math_funcs.h"
#include "builtin_animations.h"

void rainbow(ColorRGB* data_arr, ...) {
#ifndef WIN32
    static ColorHSV leds[LED_COUNT] = { 0 };
	static int value_counter = 0;
	static int value_counter_max = 75;
#else
	va_list args;
	va_start(args, data_arr);
	ColorHSV* leds = va_arg(args, ColorHSV*);
	if (leds == NULL) {
		return;
	}

	int LED_COUNT = va_arg(args, int);
	int value_counter = va_arg(args, int);
	int value_counter_max = va_arg(args, int);
	va_end(args);
#endif  // WIN32

	double value_calculated = VALUE_SAFE(scale_func_bound(value_counter, value_counter_max, 0.05, 1.0));

    for(int i = LED_COUNT-1; i > 0; i--) {
        leds[i].hue = leds[i-1].hue;
        leds[i].saturation = leds[i-1].saturation;
        leds[i].value = value_calculated;
        data_arr[i] = HSVtoRGB(leds[i]);
    }

    leds[0].hue = HUE_SAFE(leds[0].hue + 2.0);
    leds[0].saturation = 1.0;
    leds[0].value = value_calculated;
    data_arr[0] = HSVtoRGB(leds[0]);

    value_counter = (value_counter + 1) % (value_counter_max * 2);
}

void wave(ColorRGB* data_arr, ...) {
#ifndef WIN32
	static ColorHSV leds[LED_COUNT] = { 0 };
	static double value = 0.5;
#else
	va_list args;
	va_start(args, data_arr);
	ColorHSV* leds = va_arg(args, ColorHSV*);
	if (leds == NULL) {
		return;
	}

	int LED_WIDTH = va_arg(args, int);
	int LED_HEIGHT = va_arg(args, int);
	double value = va_arg(args, double);

	va_end(args);
#endif  // WIN32

	ColorHSV* led;
	for (int x = LED_WIDTH - 1; x > 0; x--) {
		for (int y = 0; y < LED_HEIGHT; y++) {
			if (y % 2 == 0) {
				led = &(leds[(y * LED_WIDTH) + x]);
			
				*led = leds[(y * LED_WIDTH) + (x - 1)];
			
				led->saturation = 1.0;
				led->value = value;
			} else {
				led = &(leds[y * LED_WIDTH + LED_WIDTH - x - 1]);

				*led = leds[y * LED_WIDTH + LED_WIDTH - x];

				led->saturation = 1.0;
				led->value = value;
			}
		}
	}

	for (int y = 0; y < LED_HEIGHT; y++) {
		if (y % 2 == 0) {
			leds[y * LED_WIDTH].hue = HUE_SAFE(leds[(y * LED_WIDTH) + 1].hue + 5.0);
			leds[y * LED_WIDTH].saturation = 1.0;
			leds[y * LED_WIDTH].value = value;
		} else {
			leds[y * LED_WIDTH + LED_WIDTH - 1].hue = HUE_SAFE(leds[y * LED_WIDTH + LED_WIDTH - 2].hue + 5.0);
			leds[y * LED_WIDTH + LED_WIDTH - 1].saturation = 1.0;
			leds[y * LED_WIDTH + LED_WIDTH - 1].value = value;
		}

	}

	for (int i = 0; i < LED_HEIGHT * LED_WIDTH; i++) data_arr[i] = HSVtoRGB(leds[i]);
}
