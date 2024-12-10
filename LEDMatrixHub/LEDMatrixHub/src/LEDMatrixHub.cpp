#include "LEDMatrixHub.hpp"

LEDMatrixHub::LEDMatrixHub(QWidget* parent)
	: QMainWindow(parent)
{
	((InteractiveDisplayWindow*) m_view)->setBackground("resources/background.png");
	resize(QSize(1024, 768));

	setDockNestingEnabled(true);

	addDockWidget(Qt::BottomDockWidgetArea, m_sceneConfig->getDock());
	addDockWidget(Qt::BottomDockWidgetArea, m_output->getDock());
	addDockWidget(Qt::RightDockWidgetArea, m_ledColorConfiguration->getDock());
	addDockWidget(Qt::RightDockWidgetArea, m_animationPlayer->getDock());
	addDockWidget(Qt::RightDockWidgetArea, m_clientConnectWindow->getDock());

	setCentralWidget(m_view);
	configureMenuBar();
	setupConnections();
	
	m_ledModel->matrixDimensionsChanged(
		m_sceneConfig->DEFAULT_WIDTH, 
		m_sceneConfig->DEFAULT_HEIGHT
	);
	m_view->viewReset();
	Animations::GetInstance()->setupConnections(m_sceneConfig);
}

void LEDMatrixHub::configureMenuBar(void) {
	setMenuBar(new QMenuBar());
	
	QMenu* windowsMenu = menuBar()->addMenu("Windows");
	configureWindowsMenu(windowsMenu);
}

void LEDMatrixHub::configureWindowsMenu(QMenu* menu) {
	configureWindowsMenuOption(menu, m_output);
	configureWindowsMenuOption(menu, m_sceneConfig);
	configureWindowsMenuOption(menu, m_ledColorConfiguration);
	configureWindowsMenuOption(menu, m_animationPlayer);
	configureWindowsMenuOption(menu, m_clientConnectWindow);
}

void LEDMatrixHub::configureWindowsMenuOption(QMenu* menu, AbstractDockWindow* dockWindow) {
	QAction* outputWindow = new QAction(dockWindow->m_windowTitle);
	outputWindow->setCheckable(true);

	connect(dockWindow->getDock(), &QDockWidget::visibilityChanged, this, [=](bool visible) {
		outputWindow->setChecked(visible);
	});
	connect(outputWindow, &QAction::triggered, this, [=](bool checked ) {
		dockWindow->getDock()->setVisible(checked);
	});

	menu->addAction(outputWindow);
}

void LEDMatrixHub::setupConnections(void) {
	connect(
		m_sceneConfig, &SceneProperties::matrixDimensionsChanged, 
		m_ledModel, &LEDMatrixModel::matrixDimensionsChanged
	);

	connect(
		m_clientConnectWindow, &ClientConnectWindow::connectionStateChanged, this, [=](CONNECTION_STATE state) {
			switch (state) {
				case DISCONNECTED:
					m_sceneConfig->setEnableControls(true);
					break;

				case CONNECTED:
					m_sceneConfig->setEnableControls(false);
					break;
			}
		}
	);

	connect(
		m_clientConnectWindow, &ClientConnectWindow::updateWithClient, 
		m_sceneConfig, &SceneProperties::updateWithClient
	);

	connect(
		this, &LEDMatrixHub::sendColorUpdate, 
		m_clientConnectWindow, &ClientConnectWindow::sendColorUpdate
	);

	connect(
		m_animationPlayer, &AnimationPlayer::AnimationChanged, 
		m_clientConnectWindow, &ClientConnectWindow::sendSetAnimation
	);
	
	connect(m_animationPlayer, &AnimationPlayer::redrawMatrix, this, &LEDMatrixHub::updateLEDColors);
	connect(m_ledModel, &LEDMatrixModel::redrawMatrix, this, &LEDMatrixHub::redrawMatrix);
	connect(m_sceneConfig, &SceneProperties::ledShapeChanged, this, &LEDMatrixHub::redrawMatrix);
	connect(m_sceneConfig, &SceneProperties::ledDistanceChanged, this, &LEDMatrixHub::redrawMatrix);
	connect(m_sceneConfig, &SceneProperties::showLedIndexes, this, [=](bool visible) {
		for (auto ledLabel : m_LEDMatrixIndicies) ledLabel->setVisible(visible);
	});
	
	connect(m_ledColorConfiguration, &LEDColorConfiguration::colorChanged, this, &LEDMatrixHub::updateLEDColor);
	connect(
		m_animationPlayer, &AnimationPlayer::AnimationStateChanged, 
		m_sceneConfig, &SceneProperties::AnimationStateChanged
	);

	connect(m_animationPlayer, &AnimationPlayer::AnimationStateChanged, this, [=](AnimationState state) {
		switch (state) {
			case PAUSED:
				m_clientConnectWindow->sendAnimationStateChange(PAUSE);
				break;

			case PLAYING:
				m_clientConnectWindow->sendAnimationStateChange(PLAY);
				break;
		}
	});
}

void LEDMatrixHub::redrawMatrix(void) {
	m_view->clearItems();
	m_LEDMatrixEntities.clear();

	LED_SHAPE shape = m_sceneConfig->getShape();
	int ledDistance = m_sceneConfig->getLedDistance();
	int ledSize = m_sceneConfig->getLedSize();
	int counter = 0;

	for (int y = 0; y < m_ledModel->getHeight(); y++) {
		for (int x = 0; x < m_ledModel->getWidth(); x++) {
			QGraphicsItem* item;
			switch (shape) {
				default:
				case LED_SHAPE::SQUARE:
					item = new QGraphicsRectItem (
						x * (ledSize + ledDistance),
						y * (ledSize + ledDistance),
						ledSize, ledSize
					);
					break;

				case LED_SHAPE::CIRCLE:
					item = new QGraphicsEllipseItem(
						x * (ledSize + ledDistance),
						y * (ledSize + ledDistance),
						ledSize, ledSize
					);
					break;
			}

			m_LEDMatrixEntities.push_back(item);
			m_view->addItem(item, RGB_TO_QCOLOR(m_ledModel->getColorAtIndex(counter++)));
		}
	}

	redrawIndexes();
}

void LEDMatrixHub::redrawIndexes(void) {
	m_LEDMatrixIndicies.clear();

	int ledDistance = m_sceneConfig->getLedDistance();
	int ledSize = m_sceneConfig->getLedSize();

	int height = m_ledModel->getHeight();
	int width = m_ledModel->getWidth();
	int counter = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			auto item = new QGraphicsSimpleTextItem(STRING("{}", m_ledModel->getLedOrder(counter++)));
			item->setPos(x * (ledSize + ledDistance), y * (ledSize + ledDistance));
			item->setVisible(m_sceneConfig->getShowLedIndexesState());
			m_LEDMatrixIndicies.push_back(item);
			m_view->addItem(item, Qt::white);
		}
	}
}

void LEDMatrixHub::updateLEDColor(QColor color) {
	int index;
	QGraphicsItem* item;
	for (int i = 0; i < m_ledModel->getHeight() * m_ledModel->getWidth(); i++) {
		index = m_ledModel->getLedOrder(i);
		item = m_LEDMatrixEntities[index];
		if (m_view->isSelected(item)) {
			m_ledModel->setColorAtIndex(index, color);
			m_view->setItemColor(item, color);
		}
	}

	emit sendColorUpdate(m_ledModel->GetMatrixColors(), m_ledModel->getLEDCount());
}

void LEDMatrixHub::updateLEDColors(void) {
	int index;
	QGraphicsItem* item;
	for (int i = 0; i < m_ledModel->getHeight() * m_ledModel->getWidth(); i++) {
		index = m_ledModel->getLedOrder(i);
		item = m_LEDMatrixEntities[index];
		ColorRGB color = m_ledModel->getColorAtIndex(index);
		m_view->setItemColor(item, QColor::fromRgb(color.red, color.green, color.blue));
	}
}

LEDMatrixHub::~LEDMatrixHub()
{}
