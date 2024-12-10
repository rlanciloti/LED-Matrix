#pragma once
#include <tuple>

#include "common.hpp"
#include "Constants.hpp"
#include "AnimationPlayer.hpp"
#include "ClientConnectWindow.hpp"
#include "AbstractDockWindow.hpp"
#include "OutputWindow.hpp"
#include "ScrollEventFilter.hpp"

class SceneProperties : public AbstractDockWindow {
	Q_OBJECT

public:
	static SceneProperties* getInstance() { return getInstance(DEFAULT_DOCK_AREAS); }
	static SceneProperties* getInstance(QFlags<Qt::DockWidgetArea> allowedArea) {
		static SceneProperties* m_instance = new SceneProperties(allowedArea);
		return m_instance;
	}

	const static QStringList LED_LAYOUTS;

	const static QStringList LED_SHAPES;

	static const LED_LAYOUT_DIRECTION DEFAULT_LED_LAYOUT_DIRECTION;
	static const LED_SHAPE DEFAULT_SHAPE;
	static const int DEFAULT_WIDTH;
	static const int DEFAULT_HEIGHT;
	static const int DEFAULT_LED_DISTANCE;

	LED_SHAPE getShape() { return (LED_SHAPE) ledShape->currentIndex(); }
	int getLedDistance() { return ledDistance->value(); }

	int getLedSize() { return 20; }
	bool getShowLedIndexesState(void) { return showLedIds->isChecked(); }

public slots:
	void AnimationStateChanged(AnimationState state);
	
	void setEnableControls(bool enabled) { setFieldEnabledState(enabled); }

	void updateWithClient(int height, int width);

signals:
	void matrixDimensionsChanged(int width, int height);
	void ledDirectionChanged(LED_LAYOUT_DIRECTION direction);
	void ledShapeChanged(LED_SHAPE shape);
	void ledDistanceChanged(int distance);
	void showLedIndexes(bool toggled);

private:
	void setupSizeFrame(void);
	void setupLedLayoutFrame(void);

	void makeConnections(void);
	void setFieldEnabledState(bool enabled);

	SceneProperties(QFlags<Qt::DockWidgetArea> allowedArea);

	static const QFlags<Qt::DockWidgetArea> DEFAULT_DOCK_AREAS;
	
	QObject* scrollEventFilter = new ScrollEventFilter();

	QVBoxLayout* scrollLayout = new QVBoxLayout();

	QWidget* container = new QWidget();

	QCheckBox* enableConfiguration = new QCheckBox();
	QSpinBox* heightEntry = new QSpinBox();
	QSpinBox* widthEntry = new QSpinBox();

	QCheckBox* showLedIds = new QCheckBox();
	QComboBox* layoutDirection = new QComboBox();
	QComboBox* ledShape = new QComboBox();
	QSpinBox* ledDistance = new QSpinBox();

	OutputWindow* m_console = OutputWindow::getInstance();

	AnimationPlayer* m_player = AnimationPlayer::getInstance();
	ClientConnectWindow* m_client = ClientConnectWindow::getInstance();
};
