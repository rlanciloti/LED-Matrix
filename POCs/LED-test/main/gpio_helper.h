#ifndef _GPIO_HELPER_
#define _GPIO_HELPER_
#include "esp.h"
#include "driver/gpio.h"

#define GPIO_LOW    0
#define GPIO_HIGH   1

#define PIN_INDEX(x) (1ULL << x)

void set_pin_mode(uint8_t pin, gpio_mode_t mode);

#endif //_GPIO_HELPER_
