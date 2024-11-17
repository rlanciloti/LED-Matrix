#include <stdio.h>
#include <inttypes.h>
#include "esp_flash.h"
#include "esp_chip_info.h"
#include "commands.h"

#ifndef DISABLE_VERSION
static int get_version(int argc, char** argv);
static void register_version(void);
#endif  // DISABLE_VERSION

#ifndef DISABLE_ECHO
static int echo(int argc, char** argv);
static void register_echo(void);
#endif  // DISABLE_ECHO

void register_commands(void) {
#ifndef DISABLE_VERSION
    register_version();
#endif  // DISABLE_VERSION
#ifndef DISABLE_ECHO
    register_echo();
#endif  // DISABLE_ECHO
}

#ifndef DISABLE_VERSION
static int get_version(int argc, char** argv) {
    const char* model;
    esp_chip_info_t info;
    uint32_t flash_size;
    esp_chip_info(&info);

    switch(info.model) {
        case CHIP_ESP32:
            model = "ESP32";
            break;
        case CHIP_ESP32S2:
            model = "ESP32-S2";
            break;
        case CHIP_ESP32S3:
            model = "ESP32-S3";
            break;
        case CHIP_ESP32C3:
            model = "ESP32-C3";
            break;
        case CHIP_ESP32H2:
            model = "ESP32-H2";
            break;
        case CHIP_ESP32C2:
            model = "ESP32-C2";
            break;
        case CHIP_ESP32P4:
            model = "ESP32-P4";
            break;
        default:
            model = "Unknown";
            break;
    }

    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed.");
        return 1;
    }

    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", model);
    printf("\tcores:%d\r\n", info.cores);
    printf("\tfeature:%s%s%s%s%"PRIu32"%s\r\n",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
           flash_size / (1024 * 1024), " MB");
    printf("\trevision number:%d\r\n", info.revision);
    return 0;
}

static void register_version(void) {
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif  // DISABLE_VERSION

#ifndef DISABLE_ECHO
static int echo(int argc, char** argv) {
    for(int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

static void register_echo(void) {
    const esp_console_cmd_t cmd = {
        .command = "echo",
        .help = "Echo the provided arguments to the console",
        .hint = NULL,
        .func = &echo
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif  // DISABLE_ECHO
