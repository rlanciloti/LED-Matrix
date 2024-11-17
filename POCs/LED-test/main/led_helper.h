#ifndef _LED_HELPER_
#define _LED_HELPER_
#include <stdint.h>
#include "esp.h"
#include "gpio_helper.h"

#define HZ_TO_KHZ(freq) (freq * 1000)
#define HZ_TO_MHZ(freq) (HZ_TO_KHZ(freq) * 1000)
#define HZ_TO_GHZ(freq) (HZ_TO_MHZ(freq) * 1000)

#define LED_GPIO_PIN 33
#define LED_TIMER_RESOLUTION HZ_TO_MHZ(40)

typedef struct Color {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} Color;

void init_led_driver(void* pvParameters);

#endif //_LED_HELPER_
