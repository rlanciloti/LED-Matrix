#include "LEDMatrixModel.hpp"

void LEDMatrixModel::matrixDimensionsChanged(int width, int height) {
	if (m_LEDMatrixColors != nullptr) free(m_LEDMatrixColors);
	
	m_LEDMatrixColors = (ColorRGB*) calloc(width * height, sizeof(ColorRGB));

	m_width = width;
	m_height = height;

	for (int i = 0; i < width * height; i++) m_LEDMatrixColors[i] = DEFAULT_COLOR;
	emit redrawMatrix();
}

void LEDMatrixModel::updateMatrixColors(ColorRGB* colorMatrix) {
	for (int i = 0; i < m_width * m_height; i++) setColorAtIndex(i, QColor::fromRgb(
		colorMatrix[i].red,
		colorMatrix[i].green,
		colorMatrix[i].blue
	));

	emit redrawMatrix();
}

int LEDMatrixModel::getLedOrder(int index) {
	int row = 0;
	int column = 0;
	switch (m_direction) {
		default:
		case SNAKE_NW_RIGHT:
			row = index / m_width;
			column = (row % 2 == 0) ?
				index % m_width :
				m_width - 1 - (index % m_width);
			break;

		case SNAKE_NW_DOWN:
			break;

		case SNAKE_SW_RIGHT:
			break;

		case SNAKE_SW_UP:
			break;
	}
	
	return row * m_width + column;
}

ColorRGB LEDMatrixModel::getColorAtIndex(int index) {
	if (index < 0 || index >= m_width * m_height) return DEFAULT_COLOR;
	return m_LEDMatrixColors[getLedOrder(index)];
}

void LEDMatrixModel::setColorAtIndex(int index, QColor color) {
	if (index < 0 || index >= m_width * m_height) return;
	m_LEDMatrixColors[getLedOrder(index)] = {
		.blue = (uint8_t) color.blue(),
		.red = (uint8_t) color.red(),
		.green = (uint8_t) color.green()
	};
}
