#ifndef _LED_HELPER_
#define _LED_HELPER_
#include <stdint.h>
#include "color.h"
#include "esp.h"
#include "gpio_helper.h"
#include "builtin_animations.h"

#define HZ_TO_KHZ(freq) (freq * 1000)
#define HZ_TO_MHZ(freq) (HZ_TO_KHZ(freq) * 1000)
#define HZ_TO_GHZ(freq) (HZ_TO_MHZ(freq) * 1000)

#define LED_GPIO_PIN 33
#define LED_TIMER_RESOLUTION HZ_TO_MHZ(40)

#define LED_WIDTH 21
#define LED_HEIGHT 7

#define LED_COUNT (LED_WIDTH * LED_HEIGHT)

typedef struct Color {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} Color;

typedef struct {
    const char* animationName;
    AnimationFunc func;
} AnimationTuple;

extern ColorRGB DATA_ARR[LED_COUNT];
extern SemaphoreHandle_t DATA_ARR_MUTEX;
extern int animationEnabled;

void init_led_driver();
void display_led_frame(void* pvParameters);
void playAnimation(void* pvParameters);

void setAnimation(char* animationName);

#endif //_LED_HELPER_
