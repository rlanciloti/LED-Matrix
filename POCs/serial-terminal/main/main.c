#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_console.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "commands.h"

#define CONFIG_ESP_CONSOLE_UART_DEFAULT 1

static void initialize_nvs(void);
static void initialize_console(void);

static void initialize_nvs(void) {
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

static void initialize_console(void) {
    esp_console_repl_t* repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = "> "; 
    repl_config.max_cmdline_length = 512;

    initialize_nvs();

    esp_console_register_help_command();
    register_commands();

    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    #endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void app_main(void) {
    initialize_console();
}