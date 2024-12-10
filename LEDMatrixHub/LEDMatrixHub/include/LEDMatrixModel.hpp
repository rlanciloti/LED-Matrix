#pragma once

#include <vector>
#include "common.hpp"
#include "Constants.hpp"
#include "OutputWindow.hpp"

extern "C" {
#include "color.h"
#include "math_funcs.h"
}

#define HSV_TO_QCOLOR(color) QColor().fromHsv(color.hue, color.saturation, color.value)
#define RGB_TO_QCOLOR(color) QColor().fromRgb(color.red, color.green, color.blue)

class SceneProperties;

class LEDMatrixModel : public QObject{
	Q_OBJECT

public:
	static LEDMatrixModel* getInstance() {
		static LEDMatrixModel* m_instance = new LEDMatrixModel();

		return m_instance;
	}

	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	int getLEDCount() { return m_width * m_height; }

	int getLedOrder(int index);

	ColorRGB getColorAtIndex(int index);
	void setColorAtIndex(int index, QColor color);

	ColorRGB* GetMatrixColors() { return m_LEDMatrixColors; }
	
	const ColorRGB DEFAULT_COLOR = { 0, 0, 0 };

public slots:
	void matrixDimensionsChanged(int width, int height);
	void updateMatrixColors(ColorRGB* colorMatrix);

signals:
	void redrawMatrix(void);

private:
	LEDMatrixModel() {}

	LED_LAYOUT_DIRECTION m_direction = LED_LAYOUT_DIRECTION::SNAKE_NW_RIGHT;
	ColorRGB* m_LEDMatrixColors = nullptr;
	
	OutputWindow* m_console = OutputWindow::getInstance();

	int m_width = 1;
	int m_height = 1;
};
