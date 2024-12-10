#include "esp_timer.h"
#include "esp_log.h"
#include "string.h"
#include "driver/rmt_tx.h"
#include "led_helper.h"
#include "color.h"

#include "math_funcs.h"

volatile uint8_t led_state = GPIO_LOW;

const rmt_symbol_word_t WS2812B_HIGH = {
    .duration0 = 16,
    .level0 = 1,
    .duration1 = 9,
    .level1 = 0
}; 

const rmt_symbol_word_t WS2812B_LOW = {
    .duration0 = 9,
    .level0 = 1,
    .duration1 = 16,
    .level1 = 0
}; 

const rmt_symbol_word_t WS2812B_RST = {
    .duration0 = 1,
    .level0 = 1,
    .duration1 = 1000,
    .level1 = 0,
}; 

const rmt_transmit_config_t tx_no_loop = {
    .loop_count = 0
};

const AnimationTuple animations[] = {
    { .animationName = "rainbow", .func = rainbow },
    { .animationName = "wave", .func = wave },
};

static uint8_t values[256] = {0};

static AnimationFunc activeAnimation = rainbow;
static int animationDelay = 100;

static ColorRGB BUFFER1[LED_COUNT] = {0};
static ColorRGB BUFFER2[LED_COUNT] = {0};
ColorRGB DATA_ARR[LED_COUNT] = {0};

SemaphoreHandle_t DATA_ARR_MUTEX;
int animationEnabled = 1;

static ColorRGB* FRONT_BUFFER = BUFFER1;
static ColorRGB* BACK_BUFFER = BUFFER2;

rmt_channel_handle_t tx_channel = NULL;
rmt_encoder_handle_t encoder = NULL;

static size_t encoder_callback(
    const void* data, 
    size_t data_size,
    size_t symbols_written,
    size_t symbols_free,
    rmt_symbol_word_t* symbols, 
    bool* done, 
    void* args
) {
    if(symbols_free < 25) return 0;

    static int led_index = 0; 
    int offset = 0;

    uint32_t color = *(uint32_t*)&FRONT_BUFFER[led_index];

    for(offset = 0; offset < 24; offset++) {
        symbols[offset] = (color >> (23 - offset) & 0x01) ? 
            WS2812B_HIGH : 
            WS2812B_LOW;
    }

    led_index = (led_index + 1) % LED_COUNT;

    if(led_index == 0) {
        symbols[offset++] = WS2812B_RST;
        *done = 1;
    }

    values[led_index] = 1;

    return offset;
}

static void configure_transmit_channel() {
    rmt_tx_channel_config_t rmt_conf = {
        .gpio_num = LED_GPIO_PIN,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .mem_block_symbols = 512,
        .resolution_hz = HZ_TO_MHZ(20),
        .trans_queue_depth = 4
    };

    const rmt_simple_encoder_config_t encoder_conf = {
        .callback = encoder_callback
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&rmt_conf, &tx_channel));
    ESP_ERROR_CHECK(rmt_new_simple_encoder(&encoder_conf, &encoder));

    ESP_ERROR_CHECK(rmt_enable(tx_channel));
}

void init_led_driver() {
    DATA_ARR_MUTEX = xSemaphoreCreateMutex();
    set_pin_mode(LED_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO_PIN, GPIO_LOW);
    configure_transmit_channel();
}

void playAnimation(void* pvParameters) {
    while(1) {
        ESP_ERROR_CHECK(rmt_transmit(tx_channel, encoder, (void*) 1, 1, &tx_no_loop));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_channel, portMAX_DELAY));

        if(animationEnabled && activeAnimation != NULL) {
            if(xSemaphoreTake(DATA_ARR_MUTEX, portMAX_DELAY) == pdTRUE) {
                activeAnimation(DATA_ARR);
                xSemaphoreGive(DATA_ARR_MUTEX);
                vTaskDelay(pdMS_TO_TICKS(animationDelay));
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        } 
    } 
}

void display_led_frame(void* pvParameters) {
    while(1) {
        if(xSemaphoreTake(DATA_ARR_MUTEX, portMAX_DELAY) == pdTRUE) {
            memcpy(BACK_BUFFER, DATA_ARR, sizeof(DATA_ARR));
            if(xSemaphoreGive(DATA_ARR_MUTEX) == pdFALSE) {
                ESP_LOGE("LED_HELPER", "Could not release mutex");
            }

            FRONT_BUFFER = FRONT_BUFFER == BUFFER1 ? BUFFER2 : BUFFER1;
            BACK_BUFFER = BACK_BUFFER == BUFFER1 ? BUFFER2 : BUFFER1;
        }

        vTaskDelay(pdMS_TO_TICKS(SECONDS_TO_MS(1) / 80));
    }
}

void setAnimation(char* animationName) {
    for(int i = 0; i < sizeof(animations)/sizeof(AnimationTuple); i++) {
        if(!strcmp(animationName, animations[i].animationName)) {
            ESP_LOGI("LED_HELPER", "Setting animation function to %s", animationName);
            activeAnimation = animations[i].func;
            return;
        }
    }
}
