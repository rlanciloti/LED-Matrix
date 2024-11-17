#ifndef _ESP_
#define _ESP_

#include <freertos/FreeRTOS.h>

#define PRINT(...) printf(__VA_ARGS__); fflush(stdout);
#define DBG_PRINT(...) if(DEBUG) { printf(__VA_ARGS__); fflush(stdout); }

#define MIN(x,y) ((x < y) ? x : y)
#define SECONDS_TO_MS(x) (int)(x * 1000)

inline void sleep_ms(int ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }
inline void sleep_s(int s) { vTaskDelay(SECONDS_TO_MS(s) / portTICK_PERIOD_MS); }

int strlen_unicode(const char* str);
void memcpy_safe(char* dest, char* src, int max_size);

#endif  // _ESP_