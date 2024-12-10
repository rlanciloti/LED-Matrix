#pragma once

#include "common.hpp"
#include "InteractiveDisplayWindow.hpp"
#include "AbstractDockWindow.hpp"
#include "LEDMatrixModel.hpp"
#include "AnimationPlayer.hpp"

class LEDColorConfiguration : public AbstractDockWindow {
	Q_OBJECT

public:
	static LEDColorConfiguration* getInstance() { return getInstance(DEFAULT_DOCK_AREAS); }
	static LEDColorConfiguration* getInstance(QFlags<Qt::DockWidgetArea> allowedArea) {
		static LEDColorConfiguration* m_instance = new LEDColorConfiguration(allowedArea);
		return m_instance;
	}

public slots:
	void AnimationStateChanged(AnimationState state);

signals:
	void colorChanged(QColor color);

private:
	void setupConnections(void);

	void updateButtonColor(const QColor& color) {
		m_buttonPalette.setColor(QPalette::Button, color);
		m_showColorDialog->setPalette(m_buttonPalette);
	}

	void updateHSV(const QColor& color) {
		m_hue->setValue(color.hue());
		m_saturation->setValue(color.saturation());
		m_value->setValue(color.value());
	}

	void updateRGB(const QColor& color) {
		m_red->setValue(color.red());
		m_green->setValue(color.green());
		m_blue->setValue(color.blue());
	}

	LEDColorConfiguration(QFlags<Qt::DockWidgetArea> allowedArea);

	QGridLayout* m_gridLayout = new QGridLayout();
	QWidget* m_container = new QWidget();

	InteractiveDisplayWindow* m_displayWindow = InteractiveDisplayWindow::getInstance();
	LEDMatrixModel* m_matrixModel = LEDMatrixModel::getInstance();

	QColorDialog* m_colorDialog;
	QPushButton* m_showColorDialog;

	QSpinBox* m_red = new QSpinBox();
	QSpinBox* m_green = new QSpinBox();
	QSpinBox* m_blue = new QSpinBox();

	QSpinBox* m_hue = new QSpinBox();
	QSpinBox* m_saturation = new QSpinBox();
	QSpinBox* m_value = new QSpinBox();

	QPalette m_buttonPalette;

	bool updatingHSV = false;
	bool updatingRGB = false;

	static const QFlags<Qt::DockWidgetArea> DEFAULT_DOCK_AREAS;
};