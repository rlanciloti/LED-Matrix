#pragma once

#include "Animations.hpp"
#include "SceneConfiguration.hpp"

extern "C" {
	#include "builtin_animations.h"
}

class WaveAnimation : public AbstractAnimation {
	Q_OBJECT
public:
	void Reset() override {}
	void Step(ColorRGB* data_arr) override {
		m_func(data_arr, leds, m_led_width, m_led_height, m_value);
	}

	WaveAnimation() : AbstractAnimation(wave) {
		leds = (ColorHSV*) calloc(m_led_width * m_led_height, sizeof(ColorHSV));
	}

	~WaveAnimation() {
		free(leds);
	}

public slots:
	void matrixDimensionsChanged(int width, int height) override {
		free(leds);
		m_led_width = width;
		m_led_height = height;
		leds = (ColorHSV*) calloc(width * height, sizeof(ColorHSV));
	}

private:
	ColorHSV* leds;

	int m_led_width = 0;
	int m_led_height = 0;
	double m_value = 0.5;
};
