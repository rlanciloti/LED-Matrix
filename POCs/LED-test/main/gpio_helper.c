#include "gpio_helper.h"

void set_pin_mode(uint8_t pin, gpio_mode_t mode) {
    gpio_config_t conf = {
        .pin_bit_mask = PIN_INDEX(pin),
        .mode = mode,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&conf));
}
