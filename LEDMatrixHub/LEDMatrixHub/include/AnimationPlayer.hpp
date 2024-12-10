#pragma once

#include <thread>
#include <chrono>

#include "common.hpp"
#include "Animations.hpp"
#include "LEDMatrixModel.hpp"
#include "AbstractDockWindow.hpp"
#include "InteractiveDisplayWindow.hpp"

enum AnimationState {
	PAUSED = 0,
	PLAYING = 1
};

class AnimationPlayer : public AbstractDockWindow {
	Q_OBJECT

public:
	static AnimationPlayer* getInstance() { return getInstance(DEFAULT_DOCK_AREAS); }
	static AnimationPlayer* getInstance(QFlags<Qt::DockWidgetArea> allowedArea) { 
		static AnimationPlayer* m_instance = new AnimationPlayer(allowedArea);
		return m_instance;
	}

	AnimationState GetAnimationState() { return m_state; }

signals:
	void AnimationStateChanged(AnimationState state);
	void AnimationChanged(char* animationId);
	void redrawMatrix();

private:
	static const QFlags<Qt::DockWidgetArea> DEFAULT_DOCK_AREAS;
	static const QIcon::ThemeIcon PAUSE_ICON;
	static const QIcon::ThemeIcon PLAY_ICON;

	AnimationPlayer(QFlags<Qt::DockWidgetArea> allowedArea);
	void setupConnections();

	void workerFunc();

	std::thread m_workerThread;

	QGridLayout* m_gridLayout = new QGridLayout();
	QWidget* m_container = new QWidget();

	AnimationState m_state = AnimationState::PAUSED;

	QComboBox* m_animationSelector = new QComboBox();
	QPushButton* m_pausePlayButton = new QPushButton();
	Animations* m_animations = Animations::GetInstance();
	LEDMatrixModel* m_matrixModel = LEDMatrixModel::getInstance();
	
	QSpinBox* m_animationDelay = new QSpinBox();

	AbstractAnimation* m_activeAnimation;
	InteractiveDisplayWindow* m_displayWindow = InteractiveDisplayWindow::getInstance();
};
