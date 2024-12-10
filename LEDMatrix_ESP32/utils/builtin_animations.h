#ifndef BUILTIN_ANIMATIONS_H
#define BUILTIN_ANIMATIONS_H

#include "color.h"

typedef void (*AnimationFunc)(ColorRGB* data_arr, ...);

void rainbow(ColorRGB* data_arr, ...);

void wave(ColorRGB* data_arr, ...);

#endif  // BUILTIN_ANIMATIONS_H
