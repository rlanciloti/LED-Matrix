#include "ClientConnectWindow.hpp"

const QFlags<Qt::DockWidgetArea> ClientConnectWindow::DEFAULT_DOCK_AREAS =
	Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea |
	Qt::BottomDockWidgetArea;

const QRegularExpression ClientConnectWindow::IP_ADDRESS_REGEX = QRegularExpression(
	"^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
	"(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
);

const QRegularExpressionValidator* ClientConnectWindow::IP_ADDRESS_VALIDATOR = 
	new QRegularExpressionValidator(IP_ADDRESS_REGEX);

ClientConnectWindow::ClientConnectWindow(QFlags<Qt::DockWidgetArea> allowedArea) 
	: AbstractDockWindow("Client Connect", allowedArea, new QScrollArea()) {
	QDockWidget* dock = getDock();

	m_mainWidget->setParent(dock);

	m_IPAddress->setValidator(IP_ADDRESS_VALIDATOR);

	m_port->setMinimum(0x0001);
	m_port->setMaximum(0xFFFF);
	m_port->setValue(9999);

	m_connectBtn->setText("connect");

	m_gridLayout->addWidget(new QLabel("IP Address:"), 0, 0);
	m_gridLayout->addWidget(m_IPAddress, 0, 1);
	m_gridLayout->addWidget(new QLabel("Port:"), 1, 0);
	m_gridLayout->addWidget(m_port, 1, 1);
	m_gridLayout->addWidget(m_connectBtn, 2, 0, 1, 2);

	m_container->setLayout(m_gridLayout);

	((QScrollArea*) m_mainWidget)->setWidget(m_container);
	
	setupConnections();
}

void ClientConnectWindow::setupConnections(void) {
	connect(this, &ClientConnectWindow::connectionStateChanged, this, [=] (CONNECTION_STATE state) {
		switch (state) {
			case DISCONNECTED:
				m_connectBtn->setText("connect");
				break;
			case CONNECTED:
				m_connectBtn->setText("disconnect");
				sendConnectRequest();
				break;
		}
	});

	connect(m_connectBtn, &QPushButton::pressed, this, [=] {
		switch (m_connectionState) {
			case DISCONNECTED:
				connectToClient();
				break;

			case CONNECTED:
				disconnectFromClient();
				break;
		}
	});

	connect(m_clientSocket, &QTcpSocket::connected, this, [=]() {
		m_connectionState = CONNECTED;

		m_console->Log(STRING("Connected to {}:{}", 
			m_clientSocket->peerAddress().toString().toStdString(),
			m_clientSocket->peerPort()
		));

		emit connectionStateChanged(m_connectionState);
	});

	connect(m_clientSocket, &QTcpSocket::disconnected, this, [=]() {
		m_connectionState = DISCONNECTED;

		m_console->Log("Disconnected from client.");
		emit connectionStateChanged(m_connectionState);
	});

	connect(m_clientSocket, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
		m_console->Log(STRING("Network Error Occured: {}", (uint32_t) error), ERROR);
	});

	connect(m_clientSocket, &QIODevice::readyRead, this, &ClientConnectWindow::handleIncomingData);
}

void ClientConnectWindow::connectToClient() {
	if (m_IPAddress->text().isEmpty()) return;
	
	QString addr = m_IPAddress->text();
	QHostAddress ipAddress(addr);
	int port = m_port->value();

	if (m_clientSocket->state() == QAbstractSocket::UnconnectedState) {
		m_clientSocket->connectToHost(ipAddress, port);
	}
}

void ClientConnectWindow::disconnectFromClient(void) {
	if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
		m_clientSocket->disconnectFromHost();
	}
}

void ClientConnectWindow::sendConnectRequest(void) {
	uint8_t buffer[32] = { 0 };
	if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
		uint32_t size = pack_client_connect_request_handshake_frame(buffer, 32);
		m_clientSocket->write((char*) buffer, size);
	}
}

void ClientConnectWindow::sendColorUpdate(ColorRGB* leds, int ledCount) {
	uint8_t buffer[4096] = { 0 };
	if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
		uint32_t size = pack_cmd_set_frame(buffer, 4096, leds, ledCount);
		m_clientSocket->write((char*) buffer, size);
	}
}

void ClientConnectWindow::sendSetAnimation(char* animationName) {
	uint8_t buffer[4096] = { 0 };
	if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
		uint32_t size = pack_set_animation_frame(
			buffer, 4096, animationName, strlen(animationName)
		);

		m_clientSocket->write((char*) buffer, size);
	}
}

void ClientConnectWindow::sendAnimationStateChange(ANIMATION_CTRL_T state) {
	uint8_t buffer[32] = { 0 };
	if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
		uint32_t size = pack_animation_ctrl_frame(buffer, 32, state);

		m_clientSocket->write((char*) buffer, size);
	}
}

void ClientConnectWindow::handleIncomingData(void) {
	QByteArray data = m_clientSocket->read(8196);
	MessageHeader_T msgHeader;

	if (data.length() < sizeof(MessageHeader_T)) return;

	memcpy(&msgHeader, data, sizeof(MessageHeader_T));

	QString sb = "";
	
	for (uint8_t byte : data) {
		sb.append((byte < 16 ? "0" : "") + QString::number(byte, 16) + " ");
	}

	m_console->Log(STRING("Data Recieved: {}", sb.toStdString()));

	switch (msgHeader.message_type) {
		case CMD_LED:
			m_console->Log("Message Recieved: CMD_LED");
			break;

		case ANIMATION_CTRL:
			m_console->Log("Message Recieved: ANIMATION_CTRL");
			break;

		case CONNECT_REQUEST:
			m_console->Log("Message Recieved: CONNECT_REQUEST");
			break;

		case CONNECT_REPLY:
			m_console->Log("Message Recieved: CONNECT_REPLY");
			
			uint32_t height;
			uint32_t width;
			uint32_t clientIdLength = msgHeader.remaining_message_length - sizeof(uint32_t) * 2;
			char* clientId = (char*) calloc(clientIdLength, sizeof(char));

			unpack_client_connect_response_handshake_frame(
				(uint8_t*) data.constData(), 8196, &height, &width, clientId, clientIdLength
			);


			m_console->Log(STRING("Device ID: {}", clientId));
			m_console->Log(STRING("Dimensions: {}x{}", width, height));

			emit updateWithClient(height, width);

			free(clientId);
			break;
	}
}
