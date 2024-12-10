#include "AnimationPlayer.hpp"
#include "RainbowAnimation.hpp"

const QFlags<Qt::DockWidgetArea> AnimationPlayer::DEFAULT_DOCK_AREAS =
	Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea |
	Qt::BottomDockWidgetArea;

const QIcon::ThemeIcon AnimationPlayer::PAUSE_ICON = QIcon::ThemeIcon::MediaPlaybackPause;
const QIcon::ThemeIcon AnimationPlayer::PLAY_ICON = QIcon::ThemeIcon::MediaPlaybackStart;

AnimationPlayer::AnimationPlayer(QFlags<Qt::DockWidgetArea> allowedArea) 
	: AbstractDockWindow("Animation Selector", allowedArea, new QScrollArea()) {
	QDockWidget* dock = getDock();

	m_mainWidget->setParent(dock);
	m_container->setLayout(m_gridLayout);

	for (auto& kvp : Animations::GetAnimations()) {
		m_animationSelector->addItem(QString::fromStdString(kvp.first));
	}

	m_activeAnimation = m_animations->GetAnimation(
		m_animationSelector->currentText().toStdString()
	);

	m_pausePlayButton->setIcon(QIcon::fromTheme(PLAY_ICON));
	m_animationDelay->setMinimum(1);
	m_animationDelay->setMaximum(10000);
	m_animationDelay->setValue(100);

	m_gridLayout->addWidget(m_animationSelector, 0, 0, 1, 3);
	m_gridLayout->addWidget(m_pausePlayButton, 1, 0);
	m_gridLayout->setColumnMinimumWidth(0, 25);
	m_gridLayout->setRowMinimumHeight(2, 10);
	m_gridLayout->addWidget(new QLabel("Frame delay (ms):"), 3, 0, 1, 2);
	m_gridLayout->addWidget(m_animationDelay, 3, 2);

	((QScrollArea*) m_mainWidget)->setWidget(m_container);
	setupConnections();

	m_workerThread = std::thread(&AnimationPlayer::workerFunc, this);
}

void AnimationPlayer::setupConnections() {
	connect(m_pausePlayButton, &QPushButton::pressed, this, [=]() {
		switch (m_state) {
			case PAUSED:
				emit AnimationStateChanged(PLAYING);
				break;
			case PLAYING:
				emit AnimationStateChanged(PAUSED);
				break;
		}
	});

	connect(this, &AnimationPlayer::AnimationStateChanged, this, [=](AnimationState state) {
		m_state = state;
		switch (state) {
			case PAUSED:
				m_pausePlayButton->setIcon(QIcon::fromTheme(PLAY_ICON));
				m_animationSelector->setDisabled(false);
				break;
			case PLAYING:
				m_pausePlayButton->setIcon(QIcon::fromTheme(PAUSE_ICON));
				m_animationSelector->setDisabled(true);
				break;
		}
	});

	connect(m_animationSelector, &QComboBox::currentTextChanged, this, [=](const QString& text) {
		m_activeAnimation = m_animations->GetAnimation(text.toStdString());
		emit AnimationChanged((char*) text.toStdString().c_str());
	});
}

void AnimationPlayer::workerFunc() {
	while (1) {
		switch (m_state) {
			case PAUSED:
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;

			case PLAYING:
				m_displayWindow->getMutex().lock();
				m_activeAnimation->Step(m_matrixModel->GetMatrixColors());
				m_displayWindow->getMutex().unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(m_animationDelay->value()));
				emit redrawMatrix();
				continue;
		}
	}
}
