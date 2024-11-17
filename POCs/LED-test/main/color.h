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

#define HUE_SAFE(x)         ( x <= HUE_MAX          ? x : fmod(x, HUE_MAX       ) )
#define SATURATION_SAFE(x)  ( x <= SATURATION_MAX   ? x : fmod(x, SATURATION_MAX) )
#define VALUE_SAFE(x)       ( x <= VALUE_MAX        ? x : fmod(x, VALUE_MAX     ) )

#define RED_SAFE(x)     ( x <= RED_MAX      ? x : fmod(x, RED_MAX   ) )
#define GREEN_SAFE(x)   ( x <= GREEN_MAX    ? x : fmod(x, GREEN_MAX ) )
#define BLUE_SAFE(x)    ( x <= BLUE_MAX     ? x : fmod(x, BLUE_MAX  ) )

#define MAX(x, y) ((x > y) ? x : y)
#define MIN(x, y) ((x < y) ? x : y)
#define CLAMP(lower_bound, x, upper_bound) (MIN(MAX(x, lower_bound), upper_bound))

#endif //_COLOR_H_
