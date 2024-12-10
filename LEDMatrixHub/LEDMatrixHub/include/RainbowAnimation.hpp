#pragma once

#include "Animations.hpp"
#include "SceneConfiguration.hpp"

extern "C" {
	#include "builtin_animations.h"
}

class RainbowAnimation : public AbstractAnimation {
	Q_OBJECT
public:
	void Reset() override {}
	void Step(ColorRGB* data_arr) override {
		m_func(data_arr, leds, led_count, m_value_counter, m_value_counter_max);
		m_value_counter = (m_value_counter + 1) % (m_value_counter_max * 2);
	}

	RainbowAnimation() : AbstractAnimation(rainbow) {
		leds = (ColorHSV*) calloc(led_count, sizeof(ColorHSV));
	}

	~RainbowAnimation() {
		free(leds);
	}

public slots:
	void matrixDimensionsChanged(int width, int height) override {
		free(leds);
		led_count = width * height;
		leds = (ColorHSV*) calloc(led_count, sizeof(ColorHSV));
	}

private:
	ColorHSV* leds;
	int led_count = 1;

	int m_value_counter = 0;
	int m_value_counter_max = 75;
};
