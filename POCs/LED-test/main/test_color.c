#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "esp_log.h"
#include "color.h"

void test_color();

typedef struct {
    ColorHSV hsv;
    ColorRGB rgb;
} ColorPairs;

ColorPairs pairs[] = {
    { 
        { .hue = 0, .saturation = 0, .value = 0},
        { .red = 0, .green = 0, .blue = 0} 
    },
    { 
        { .hue = 0, .saturation = 0, .value = 1.0},
        { .red = 255, .green = 255, .blue = 255} 
    },
    { 
        { .hue = 0, .saturation = 1.0, .value = 1.0},
        { .red = 255, .green = 0, .blue = 0} 
    },
    { 
        { .hue = 120, .saturation = 1.0, .value = 1.0},
        { .red = 0, .green = 255, .blue = 0} 
    },
    { 
        { .hue = 240, .saturation = 1.0, .value = 1.0},
        { .red = 0, .green = 0, .blue = 255} 
    },
    { 
        { .hue = 60, .saturation = 1.0, .value = 1.0},
        { .red = 255, .green = 255, .blue = 0} 
    },
    { 
        { .hue = 180, .saturation = 1.0, .value = 1.0},
        { .red = 0, .green = 255, .blue = 255} 
    },
    { 
        { .hue = 300, .saturation = 1.0, .value = 1.0},
        { .red = 255, .green = 0, .blue = 255} 
    },
    { 
        { .hue = 0, .saturation = 0, .value = 0.75},
        { .red = 191, .green = 191, .blue = 191} 
    },
    { 
        { .hue = 0, .saturation = 0, .value = 0.5},
        { .red = 128, .green = 128, .blue = 128} 
    },
    { 
        { .hue = 0, .saturation = 1.0, .value = 0.5},
        { .red = 128, .green = 0, .blue = 0} 
    },
    { 
        { .hue = 60, .saturation = 1.0, .value = 0.5},
        { .red = 128, .green = 128, .blue = 0} 
    },
    { 
        { .hue = 120, .saturation = 1.0, .value = 0.5},
        { .red = 0, .green = 128, .blue = 0} 
    },
    { 
        { .hue = 300, .saturation = 1.0, .value = 0.5},
        { .red = 128, .green = 0, .blue = 128} 
    },
    { 
        { .hue = 180, .saturation = 1.0, .value = 0.5},
        { .red = 0, .green = 128, .blue = 128} 
    },
    { 
        { .hue = 240, .saturation = 1.0, .value = 0.5},
        { .red = 0, .green = 0, .blue = 128} 
    }
};

uint8_t rgb_equal(ColorRGB lhs, ColorRGB rhs) {
    return (
        lhs.red == rhs.red &&
        lhs.green == rhs.green &&
        lhs.blue == rhs.blue
    );
}

uint8_t hsv_equal(ColorHSV lhs, ColorHSV rhs) {
    return (
        (fabs(lhs.hue - rhs.hue) < ROUNDING_ERROR) &&
        (fabs(lhs.saturation - rhs.saturation) < ROUNDING_ERROR) &&
        (fabs(lhs.value - rhs.value) < ROUNDING_ERROR)
    );
}

void test_color() {
    static int pair_count = sizeof(pairs)/sizeof(ColorPairs);
    
    ColorHSV hsv;
    ColorRGB rgb;
    for(int i = 0; i < pair_count; i++) {
        ESP_LOGI("ColorTest", "[%d] Original - H: %lf | S: %lf | V: %lf", 
            i + 1,
            pairs[i].hsv.hue, 
            pairs[i].hsv.saturation, 
            pairs[i].hsv.value 
        );
        
        ESP_LOGI("ColorTest", "[%d] Original - R: %d | G: %d | B: %d", 
            i + 1,
            pairs[i].rgb.red, 
            pairs[i].rgb.green, 
            pairs[i].rgb.blue
        );

        hsv = RGBtoHSV(pairs[i].rgb);
        rgb = HSVtoRGB(pairs[i].hsv);

        ESP_LOGI("ColorTest", "[%d] Converted - H: %lf | S: %lf | V: %lf", 
            i + 1,
            hsv.hue, 
            hsv.saturation, 
            hsv.value 
        );
        
        ESP_LOGI("ColorTest", "[%d] Converted - R: %d | G: %d | B: %d", 
            i + 1,
            rgb.red, 
            rgb.green, 
            rgb.blue
        );

        ESP_LOGI("ColorTest", "HSVs Equal: %s", hsv_equal(pairs[i].hsv, hsv) ? "True" : "False");
        ESP_LOGI("ColorTest", "RGBs Equal: %s", rgb_equal(pairs[i].rgb, rgb) ? "True" : "False");
        printf("\n");
    }
}
