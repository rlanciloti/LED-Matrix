#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "sysconfig.h"
#include "esp.h"
#include "wifi_helper.h"
#include "console.h"

extern int main();

static void initialize_nvs(void) {
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void memcpy_safe(char* dest, char* src, int max_size) {
    memcpy(dest, src, MIN(max_size, strlen(src)));
}

int strlen_unicode(const char* str) {
    int count = 0;
    
    for(int i = 0; i < strlen(str); i++)
    {
        if(!(str[i] & 0x80)) count++;
        else if((str[i] & 0xC0) == 0x80) continue;
        else if((str[i] & 0xE0) == 0xC0) count++;
        else if((str[i] & 0xF0) == 0xE0) count++;
        else if((str[i] & 0xF8) == 0xF0) count++;
    }

    return count;
}

void app_main(void){
    initialize_nvs();
    wifi_sta_init();
    initialize_console();
    main();
}
