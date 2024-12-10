#include <math.h>

#include "math_funcs.h"
#include "color.h"

#ifndef WIN32
#include "esp_log.h"
#endif  // WIN32

ColorHSV RGBtoHSV(ColorRGB c) {
    double hue = 0;
    double saturation = 0;
    double value = 0;

    double r = (c.red   % (RED_MAX   + 1)) / 255.0;
    double g = (c.green % (GREEN_MAX + 1)) / 255.0;
    double b = (c.blue  % (BLUE_MAX  + 1)) / 255.0;

    double cmax = value = MAX(MAX(r, g), b);
    double cmin = MIN(MIN(r, g), b);
    double delta = cmax - cmin;

    if(fabs(delta) < ROUNDING_ERROR)    hue = 0;
	else {
		if(fabs(cmax - r) < ROUNDING_ERROR) hue = HUE_SAFE(60 * (fmod((g - b)/delta, 6)) + HUE_MAX);
		if(fabs(cmax - g) < ROUNDING_ERROR) hue = HUE_SAFE(60 * ((b - r)/delta + 2) + HUE_MAX);
		if(fabs(cmax - b) < ROUNDING_ERROR) hue = HUE_SAFE(60 * ((r - g)/delta + 4) + HUE_MAX);
	}

    saturation = fabs(cmax - 0) < ROUNDING_ERROR ? 0 : delta/cmax;

    return (ColorHSV) { 
        .hue        = HUE_SAFE(hue),
        .saturation = SATURATION_SAFE(saturation), 
        .value      = VALUE_SAFE(value)
    };
}

ColorRGB HSVtoRGB(ColorHSV c) {
    double hue          = CLAMP(0.0, c.hue, HUE_MAX);
    double saturation   = CLAMP(0.0, c.saturation, SATURATION_MAX);
    double value        = CLAMP(0.0, c.value, VALUE_MAX);

    double chroma = value * saturation;
    double x = chroma * (1 - fabs(fmod(hue/60.0, 2) - 1));
    double m = value - chroma;

    double r_prime = 0.0;
    double g_prime = 0.0;
    double b_prime = 0.0;

    switch (((int) hue % 360) / 60) {
        case 0: r_prime = chroma,   g_prime = x,        b_prime = 0;        break;
        case 1: r_prime = x,        g_prime = chroma,   b_prime = 0;        break;
        case 2: r_prime = 0,        g_prime = chroma,   b_prime = x;        break;
        case 3: r_prime = 0,        g_prime = x,        b_prime = chroma;   break;
        case 4: r_prime = x,        g_prime = 0,        b_prime = chroma;   break;
        case 5: r_prime = chroma,   g_prime = 0,        b_prime = x;        break;
    }

    return (ColorRGB) { 
        .red    = (uint8_t) RED_SAFE(round((r_prime + m) * 255)), 
        .green  = (uint8_t) GREEN_SAFE(round((g_prime + m) * 255)),
        .blue   = (uint8_t) BLUE_SAFE(round((b_prime + m) * 255))
    };
}
