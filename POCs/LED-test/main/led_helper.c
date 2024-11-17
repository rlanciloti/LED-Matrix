#include "esp_timer.h"
#include "esp_log.h"
#include "string.h"
#include "driver/rmt_tx.h"
#include "led_helper.h"
#include "color.h"

#include "test_color.c"
#include "math_funcs.h"

#define LED_COUNT 147

volatile uint8_t led_state = GPIO_LOW;

uint64_t static trans_failure = 0;
uint64_t static trans_success = 0;

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

static ColorHSV data_arr[LED_COUNT] = {0};

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
    if(symbols_free < 25) {
        trans_failure++;
        return 0;
    }

    trans_success++;

    static int led_index = 0; 
    int written = 0;

    ColorRGB rgb = HSVtoRGB(data_arr[led_index]);
    uint32_t color = *(uint32_t*)&rgb;

    for(int offset = 0; offset < 24; offset++,written++) {
        symbols[offset] = (color >> (23 - offset) & 0x01) ? 
            WS2812B_HIGH : 
            WS2812B_LOW;
    }

    led_index = (led_index + 1) % LED_COUNT;
    
    if(led_index == 0) {
        symbols[24] = WS2812B_RST;
        *done = 1;
        written++;
    }

    return written;
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

void init_led_driver(void* pvParameters) {
    set_pin_mode(LED_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO_PIN, GPIO_LOW);
    configure_transmit_channel();

    int value_counter = 0;
    int value_counter_max = 75;
    double value_calculated = 0.0;

    for(int i = 0; i < LED_COUNT; i++) {
        data_arr[i] = (ColorHSV) {
            .hue = HUE_SAFE(i*2), 
            .saturation = 1.0,
            .value = 0.0
        };
    }
    
    int64_t start_time; 
    while(1) {
        start_time = esp_timer_get_time();
        ESP_ERROR_CHECK(rmt_transmit(tx_channel, encoder, (void*) 1, 1, &tx_no_loop));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_channel, portMAX_DELAY));

        value_calculated = VALUE_SAFE(scale_func_bound(value_counter, value_counter_max, 0.05, 1.0));

        for(int i = LED_COUNT-1; i > 0; i--) {
            data_arr[i].hue = data_arr[i-1].hue;
            data_arr[i].value = value_calculated;
        }

        data_arr[0].hue = HUE_SAFE(data_arr[0].hue + 2.0);
        data_arr[0].value = value_calculated;

        value_counter = (value_counter + 1) % (value_counter_max * 2);
   
        vTaskDelay(pdMS_TO_TICKS(50));
        // ESP_LOGI("LED_HELPER", "Loop time (milliseconds): %lf | Success: %lld | Failure: %lld | Symbol availability: %lf", 
        //     (esp_timer_get_time() - start_time)/1000.0, 
        //     trans_success,
        //     trans_failure,
        //     ((double)trans_success/(trans_failure + trans_success))
        // );
    } 
}
