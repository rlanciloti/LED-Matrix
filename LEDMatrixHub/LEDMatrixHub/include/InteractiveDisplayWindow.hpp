#pragma once

#include <mutex>
#include <functional>
#include "common.hpp"
#include "OutputWindow.hpp"

typedef enum { UP, DOWN, LEFT, RIGHT } MOVE_DIRECTION;

template<typename T>
inline bool contains(std::list<T> l, T element) {
	return std::find(l.begin(), l.end(), element) != l.end();
}

template<typename T>
inline bool contains(std::vector<T> l, T element) {
	return std::find(l.begin(), l.end(), element) != l.end();
}

inline QRectF QRect_Fixed(QPointF p1, QPointF p2) {
	return QRectF(
		QPointF(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y())),
		QPointF(std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()))
	);
}

inline QRectF Circle_Rect(QPointF center, float radius) {
	return QRectF(
		QPoint(center.x() - radius, center.y() - radius),
		QPoint(center.x() + radius, center.y() + radius)
	);
}

class InteractiveDisplayWindow : public QGraphicsView {

public:
	static InteractiveDisplayWindow* getInstance(
		QWidget* parent = NULL,
		std::string backgroundPath = ""
	) {
		static InteractiveDisplayWindow* m_instance = new InteractiveDisplayWindow (
			parent, 
			backgroundPath
		);

		return m_instance;
	}

	const double SCROLLWHEEL_ZOOM_IN_SCALE = 1.1;
	const double SCROLLWHEEL_ZOOM_OUT_SCALE = 1 / SCROLLWHEEL_ZOOM_IN_SCALE;

	const double BUTTON_ZOOM_IN_SCALE = 1.25;
	const double BUTTON_ZOOM_OUT_SCALE = 1 / BUTTON_ZOOM_IN_SCALE;

	const QRectF BOUNDING_BOX = QRectF(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);

	const QColor DEFAULT_SELECT_COLOR = Qt::red;
	const QColor DEFAULT_DESELECT_COLOR = Qt::black;

	void viewReset(void);

	void addItem(QGraphicsItem* item, const QColor bg_color);
	void removeItem(QGraphicsItem* item);
	void clearItems();

	void setItemColor(QGraphicsItem* item, const QColor& color);

	void setBackground(std::string backgroundPath);

	std::mutex& getMutex() { return m_sceneMutex; }

	inline bool isSelected(QGraphicsItem* item) {
		return contains(m_selectedItems, item);
	}

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

	void zoom(float scale);

	void moveSceneKeyboard(MOVE_DIRECTION direction);

	void addSelectedItem(QGraphicsItem* item);
	void removeSelectedItem(QGraphicsItem* item);
	void clearSelectedItems(void);

	void selectItem(QGraphicsItem* item);
	void deselectItem(QGraphicsItem* item);

	QPointF mousePosition(QMouseEvent* event);
	QPointF mousePosition(QPointF point);
	
private:
	explicit InteractiveDisplayWindow(
		QWidget* parent = NULL,
		std::string backgroundPath = ""
	);

	int m_originX;
	int m_originY;

	bool m_multiSelect = false;
	bool m_dragSelect = false;

	QPointF m_dragSelectStart = QPointF();
	QGraphicsRectItem* m_dragSelectRect = new QGraphicsRectItem();

	std::list<QGraphicsItem*> m_selectedItems;

	OutputWindow* m_console = OutputWindow::getInstance();
	std::mutex m_sceneMutex = std::mutex();
};
