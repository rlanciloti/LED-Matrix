#pragma once

#include <functional>
#include "common.hpp"

#include "InteractiveDisplayWindow.hpp"
#include "OutputWindow.hpp"
#include "SceneConfiguration.hpp"
#include "LEDColorConfiguration.hpp"
#include "LEDMatrixModel.hpp"
#include "AnimationPlayer.hpp"
#include "ClientConnectWindow.hpp"
#include "RainbowAnimation.hpp"

class LEDMatrixHub : public QMainWindow {
    Q_OBJECT

public:
    LEDMatrixHub(QWidget *parent = nullptr);
    ~LEDMatrixHub();

signals:
	void sendColorUpdate(ColorRGB* leds, int ledCount);

private:
	void configureMenuBar(void);
	void configureWindowsMenu(QMenu* menu);
	void configureWindowsMenuOption(QMenu* menu, AbstractDockWindow* dockWindow);

	void setupConnections(void);

	InteractiveDisplayWindow* m_view = InteractiveDisplayWindow::getInstance();

	OutputWindow* m_output = OutputWindow::getInstance();
	SceneProperties* m_sceneConfig = SceneProperties::getInstance();
	LEDColorConfiguration* m_ledColorConfiguration = LEDColorConfiguration::getInstance();
	LEDMatrixModel* m_ledModel = LEDMatrixModel::getInstance();
	AnimationPlayer* m_animationPlayer = AnimationPlayer::getInstance();
	ClientConnectWindow* m_clientConnectWindow = ClientConnectWindow::getInstance();

	std::vector<QGraphicsItem*> m_LEDMatrixEntities;
	std::vector<QGraphicsSimpleTextItem*> m_LEDMatrixIndicies;

private slots:
	void redrawMatrix(void);
	void redrawIndexes(void);
	void updateLEDColor(QColor color);
	void updateLEDColors(void);
};
