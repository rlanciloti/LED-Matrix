#include <gtest/gtest.h>
#include <math.h>

extern "C" {
#include "color.h"
}

typedef struct {
    ColorHSV hsv;
    ColorRGB rgb;
} ColorPair;

ColorPair pairs[] = { 
	{ { .hue =   0, .saturation = 0.0, .value = 0.00 }, { .blue =   0, .red =   0, .green =   0} },
    { { .hue =   0, .saturation = 0.0, .value = 1.00 }, { .blue = 255, .red = 255, .green = 255} },
    { { .hue = 360, .saturation = 1.0, .value = 1.00 }, { .blue =   0, .red = 255, .green =   0} },
    { { .hue = 120, .saturation = 1.0, .value = 1.00 }, { .blue =   0, .red =   0, .green = 255} },
    { { .hue = 240, .saturation = 1.0, .value = 1.00 }, { .blue = 255, .red =   0, .green =   0} },
    { { .hue =  60, .saturation = 1.0, .value = 1.00 }, { .blue =   0, .red = 255, .green = 255} },
    { { .hue = 180, .saturation = 1.0, .value = 1.00 }, { .blue = 255, .red =   0, .green = 255} },
    { { .hue = 300, .saturation = 1.0, .value = 1.00 }, { .blue = 255, .red = 255, .green =   0} },
    { { .hue =   0, .saturation = 0.0, .value = 0.75 }, { .blue = 191, .red = 191, .green = 191} },
    { { .hue =   0, .saturation = 0.0, .value = 0.50 }, { .blue = 128, .red = 128, .green = 128} },
    { { .hue = 360, .saturation = 1.0, .value = 0.50 }, { .blue =   0, .red = 128, .green =   0} },
    { { .hue =  60, .saturation = 1.0, .value = 0.50 }, { .blue =   0, .red = 128, .green = 128} },
    { { .hue = 120, .saturation = 1.0, .value = 0.50 }, { .blue =   0, .red =   0, .green = 128} },
    { { .hue = 300, .saturation = 1.0, .value = 0.50 }, { .blue = 128, .red = 128, .green =   0} },
    { { .hue = 180, .saturation = 1.0, .value = 0.50 }, { .blue = 128, .red =   0, .green = 128} },
    { { .hue = 240, .saturation = 1.0, .value = 0.50 }, { .blue = 128, .red =   0, .green =   0} }
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

static int pair_count = sizeof(pairs) / sizeof(ColorPair);

class ColorTest : public ::testing::TestWithParam<ColorPair> {};

TEST_P(ColorTest, RGBtoHSV){
	ColorPair pair = GetParam();
   
	EXPECT_TRUE(hsv_equal(pair.hsv, RGBtoHSV(pair.rgb)));
}

TEST_P(ColorTest, HSVtoRGB){
	ColorPair pair = GetParam();
   
	EXPECT_TRUE(rgb_equal(pair.rgb, HSVtoRGB(pair.hsv)));
}

INSTANTIATE_TEST_SUITE_P(ColorConversionTests, ColorTest, testing::ValuesIn(pairs));
