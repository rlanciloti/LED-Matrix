#include "Animations.hpp"
#include "RainbowAnimation.hpp"
#include "WaveAnimation.hpp"

std::map<std::string, AbstractAnimation*> Animations::m_animations = {};

Animations::Animations() {
	AddAnimation("rainbow", new RainbowAnimation());
	AddAnimation("wave", new WaveAnimation());
}

void Animations::setupConnections(SceneProperties* properties) {
	for (auto animation : m_animations) {
		connect(
			properties, &SceneProperties::matrixDimensionsChanged,
			animation.second, &AbstractAnimation::matrixDimensionsChanged
		);
	}
}
