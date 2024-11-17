#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_console.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "wifi_helper.h"

#include "console.h"
#include "commands.h"

static void register_commands();

static void register_commands() {
    register_scan_networks_cmd();
    register_join_network_cmd();
    register_disconnect_from_network_cmd();
    register_display_network_info_cmd();
}

void initialize_console(void) {
    esp_console_repl_t* repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = "> "; 
    repl_config.max_cmdline_length = 512;

    esp_console_register_help_command();
    register_commands();

    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    #endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
