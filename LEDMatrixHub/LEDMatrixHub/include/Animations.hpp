#pragma once

#include <map>
#include <list>
#include <string>
#include "common.hpp"


extern "C" {
	#include "builtin_animations.h"
}

class SceneProperties;

class AbstractAnimation : public QObject {
	Q_OBJECT
public:
	virtual void Reset() = 0;
	virtual void Step(ColorRGB*) = 0;

public slots:
	virtual void matrixDimensionsChanged(int width, int height) = 0;

protected:
	AbstractAnimation(AnimationFunc func) {
		m_func = func;
	}

	AnimationFunc m_func = nullptr;
};

class Animations : QObject {
	Q_OBJECT
public:
	static Animations* GetInstance() {
		static Animations* m_instance = new Animations();
		return m_instance;
	}

	static std::map<std::string, AbstractAnimation*> GetAnimations() {
		return m_animations;
	}

	static void AddAnimation(std::string id, AbstractAnimation* animation) {
		m_animations.insert({ id, animation });
	}

	static AbstractAnimation* GetAnimation(std::string id) {
		return m_animations[id];
	}
	
	static void setupConnections(SceneProperties* properties);

private:
	Animations();

	static std::map<std::string, AbstractAnimation*> m_animations;
};