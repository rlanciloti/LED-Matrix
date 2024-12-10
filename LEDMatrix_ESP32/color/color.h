#ifndef _COLOR_H_
#define _COLOR_H_
#include <stdint.h>

typedef struct {
    uint8_t blue  : 8;
    uint8_t red   : 8;
    uint8_t green : 8;
} ColorRGB;

typedef struct ColorHSV_S {
    double hue;
    double saturation;
    double value;
} ColorHSV;

ColorHSV RGBtoHSV(ColorRGB);
ColorRGB HSVtoRGB(ColorHSV); 

#define ROUNDING_ERROR 0.01

#define HUE_MAX        360.0
#define SATURATION_MAX 1.0
#define VALUE_MAX      1.0

#define RED_MAX   255
#define GREEN_MAX 255
#define BLUE_MAX  255

#define SET_SAFE(MAX, x) ( x <= MAX ? x : fmod(x, MAX) )

#define HUE_SAFE(x)         SET_SAFE(HUE_MAX, x)
#define SATURATION_SAFE(x)  SET_SAFE(SATURATION_MAX, x)
#define VALUE_SAFE(x)       SET_SAFE(VALUE_MAX, x)

#define RED_SAFE(x)     SET_SAFE(RED_MAX, x)
#define GREEN_SAFE(x)   SET_SAFE(GREEN_MAX, x)
#define BLUE_SAFE(x)    SET_SAFE(BLUE_MAX, x)

#define MAX(x, y) ((x > y) ? x : y)
#define MIN(x, y) ((x < y) ? x : y)
#define CLAMP(lower_bound, x, upper_bound) (MIN(MAX(x, lower_bound), upper_bound))

#endif //_COLOR_H_
