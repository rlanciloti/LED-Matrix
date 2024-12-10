#pragma once

#include "common.hpp"
#include "AbstractDockWindow.hpp"
#include "OutputWindow.hpp"
#include "Constants.hpp"

extern "C" {
#include "message_headers.h"
}

enum CONNECTION_STATE {
	DISCONNECTED = 0,
	CONNECTED = 1,
};

class ClientConnectWindow : public AbstractDockWindow {
	Q_OBJECT

public:
	static ClientConnectWindow* getInstance() { return getInstance(DEFAULT_DOCK_AREAS); }
	static ClientConnectWindow* getInstance(QFlags<Qt::DockWidgetArea> allowedArea) { 
		static ClientConnectWindow* m_instance = new ClientConnectWindow(allowedArea);
		return m_instance; 
	}

	bool connectedState() { return m_connectionState; }

public slots:
	void sendColorUpdate(ColorRGB* leds, int ledCount);
	void sendSetAnimation(char* animationName);
	void sendAnimationStateChange(ANIMATION_CTRL_T state);

signals:
	void connectionStateChanged(CONNECTION_STATE state);
	void updateWithClient(int height, int width);

private slots:
	void handleIncomingData(void);

private:
	ClientConnectWindow(QFlags<Qt::DockWidgetArea> allowedArea);

	void setupConnections(void);
	void connectToClient(void);
	void disconnectFromClient(void);

	void sendConnectRequest(void);

	QGridLayout* m_gridLayout = new QGridLayout();
	QWidget* m_container = new QWidget();

	QLineEdit* m_IPAddress = new QLineEdit();
	QSpinBox* m_port = new QSpinBox();
	QPushButton* m_connectBtn = new QPushButton();

	CONNECTION_STATE m_connectionState = DISCONNECTED;
	QTcpSocket* m_clientSocket = new QTcpSocket();

	OutputWindow* m_console = OutputWindow::getInstance();

	static const QFlags<Qt::DockWidgetArea> DEFAULT_DOCK_AREAS;
	static const QRegularExpression IP_ADDRESS_REGEX;
	static const QRegularExpressionValidator* IP_ADDRESS_VALIDATOR;
};
