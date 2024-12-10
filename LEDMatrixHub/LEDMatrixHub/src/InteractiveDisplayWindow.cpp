#include "InteractiveDisplayWindow.hpp"

InteractiveDisplayWindow::InteractiveDisplayWindow(
	QWidget* parent, 
	std::string backgroundPath
) : QGraphicsView(parent) {
	setScene(new QGraphicsScene());

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setResizeAnchor(QGraphicsView::NoAnchor);

	setBackground(backgroundPath);
	setSceneRect(BOUNDING_BOX);
}

void InteractiveDisplayWindow::setBackground(std::string backgroundPath) {
	if (scene() == NULL || backgroundPath.empty()) return;

	QPixmap backgroundImage(backgroundPath.c_str());
	QBrush backgroundBrush(backgroundImage);

	scene()->setBackgroundBrush(backgroundBrush);
}

void InteractiveDisplayWindow::zoom(float scale) {
	if (scene() == NULL) return;

	QPointF center = mapToScene(viewport()->rect().center());
	
	setTransform(transform().scale(scale, scale));
	centerOn(center);
}

void InteractiveDisplayWindow::viewReset(void) {
	if (scene() == NULL) return;
	resetTransform();

	int item_count = scene()->items().count();
	double x = 0; 
	double y = 0;
	
	for (auto item : scene()->items()) {
		x += item->boundingRect().x();
		y += item->boundingRect().y();
	}

	centerOn(x / item_count, y / item_count);
}

void InteractiveDisplayWindow::moveSceneKeyboard(MOVE_DIRECTION direction) {
	if (scene() == NULL) return;

	switch (direction) {
		case UP:
			translate(0, 10);
			break;

		case DOWN:
			translate(0, -10);
			break;

		case LEFT:
			translate(10, 0);
			break;

		case RIGHT:
			translate(-10, 0);
			break;
	}
}

void InteractiveDisplayWindow::mousePressEvent(QMouseEvent* event) {
	if (scene() == NULL || event == NULL) return;

	if (event->button() == Qt::RightButton) {
		setCursor(Qt::ClosedHandCursor);
		m_originX = event->position().x();
		m_originY = event->position().y();
	}

	if (event->buttons() & Qt::LeftButton) {
		QGraphicsItem* item = itemAt(event->pos());

		if (item) {
			if (!m_multiSelect) clearSelectedItems();
			addSelectedItem(item);
		} else {
			m_dragSelect = true;
			m_dragSelectStart = mousePosition(event);
			scene()->addItem(m_dragSelectRect);
		}
	}
}

void InteractiveDisplayWindow::mouseMoveEvent(QMouseEvent* event) {
	if (scene() == NULL || event == NULL) return;
	
	if (event->buttons() & Qt::RightButton) {
		QPointF oldp = mapToScene(m_originX, m_originY);
		QPointF newP = mousePosition(event);
		QPointF translation = newP - oldp;

		translate(translation.x(), translation.y());
		
		m_originX = event->position().x();
		m_originY = event->position().y();
	}

	if (event->buttons() & Qt::LeftButton) {
		getMutex().lock();
		if (m_dragSelect) {
			m_dragSelectRect->setRect(
				QRect_Fixed(m_dragSelectStart, mousePosition(event))
			);

			for (auto item : scene()->items()) {
				if (item == m_dragSelectRect) continue;

				if (m_dragSelectRect->collidesWithItem(item)) {
					addSelectedItem(item);
				} else {
					removeSelectedItem(item);
				}
			}

			if (!m_dragSelectRect->isVisible()) m_dragSelectRect->setVisible(true);
		}
		getMutex().unlock();
	}
}

void InteractiveDisplayWindow::mouseReleaseEvent(QMouseEvent* event) {
	if (scene() == NULL || event == NULL) return;
	if (event->button() == Qt::RightButton) setCursor(Qt::ArrowCursor);
	if (event->button() == Qt::LeftButton) {
		getMutex().lock();
		if (m_dragSelectRect->isVisible()) {
			m_dragSelectRect->setVisible(false);
		}
		m_dragSelect = false;
		scene()->removeItem(m_dragSelectRect);
		getMutex().unlock();
	}
}

void InteractiveDisplayWindow::wheelEvent(QWheelEvent* event) {
	if (scene() == NULL || event == NULL) return;
	event->angleDelta().y() > 0 ? 
		zoom(SCROLLWHEEL_ZOOM_IN_SCALE) : 
		zoom(SCROLLWHEEL_ZOOM_OUT_SCALE);
}

void InteractiveDisplayWindow::keyPressEvent(QKeyEvent* event) {
	if (scene() == NULL || event == NULL) return;
	switch (event->key()) {
		case Qt::Key_R:
			viewReset();
			break;

		case Qt::Key_Up:
		case Qt::Key_W:
			moveSceneKeyboard(UP);
			break;

		case Qt::Key_Down:
		case Qt::Key_S:
			moveSceneKeyboard(DOWN);
			break;

		case Qt::Key_Left:
		case Qt::Key_A:
			moveSceneKeyboard(LEFT);
			break;

		case Qt::Key_Right:
		case Qt::Key_D:
			moveSceneKeyboard(RIGHT);
			break;

		case Qt::Key_Z:
			zoom(BUTTON_ZOOM_IN_SCALE);
			break;

		case Qt::Key_X:
			zoom(BUTTON_ZOOM_OUT_SCALE);
			break;

		case Qt::Key_Control:
			m_multiSelect = true;
			break;
		
		case Qt::Key_Escape:
			clearSelectedItems();
			break;
	}
}

void InteractiveDisplayWindow::keyReleaseEvent(QKeyEvent* event) {
	if (scene() == NULL || event == NULL) return;
	switch (event->key()) {
		case Qt::Key_Control:
			m_multiSelect = false;
			break;
	}
}

void InteractiveDisplayWindow::addSelectedItem(QGraphicsItem* item) {
	if (scene() == NULL || item == NULL) return;

	if (contains(m_selectedItems, item)) {
		if (m_multiSelect) {
			deselectItem(item);
			m_selectedItems.remove(item);
		} else {
			return;
		}
	} else {
		selectItem(item);
		m_selectedItems.push_back(item);
	}
}

void InteractiveDisplayWindow::removeSelectedItem(QGraphicsItem* item) {
	if (scene() == NULL || item == NULL) return;

	if (contains(m_selectedItems, item)) {
		deselectItem(item);
		m_selectedItems.remove(item);
	}
}

void InteractiveDisplayWindow::clearSelectedItems(void) {
	if (scene() == NULL) return;
	for (auto item : m_selectedItems) deselectItem(item);

	m_selectedItems.clear();
}

void InteractiveDisplayWindow::addItem(QGraphicsItem* item, const QColor bg_color) {
	if (scene() == NULL) return;
	if (auto rect = dynamic_cast<QGraphicsRectItem*>(item)) {
		rect->setBrush(QBrush(bg_color));
		rect->setPen(QPen(DEFAULT_DESELECT_COLOR));
	} else if (auto circle = dynamic_cast<QGraphicsEllipseItem*>(item)) {
		circle->setBrush(QBrush(bg_color));
		circle->setPen(QPen(DEFAULT_DESELECT_COLOR));
	} else if (auto text = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
		text->setBrush(QBrush(bg_color));
		text->setPen(QPen(bg_color));
	}

	scene()->addItem(item);
}

void InteractiveDisplayWindow::removeItem(QGraphicsItem* item) {
	if (scene()->items().contains(item)) scene()->removeItem(item);
	if (contains(m_selectedItems, item)) m_selectedItems.remove(item);
}

void InteractiveDisplayWindow::clearItems(void) {
	if (scene() == NULL) return;
	scene()->clear();
	m_selectedItems.clear();
}

inline void InteractiveDisplayWindow::selectItem(QGraphicsItem* item) {
	if (scene() == NULL || item == NULL) return;
	if (auto rect = dynamic_cast<QGraphicsRectItem*>(item)) {
		rect->setPen(QPen(DEFAULT_SELECT_COLOR));
	} else if (auto circle = dynamic_cast<QGraphicsEllipseItem*>(item)) {
		circle->setPen(QPen(DEFAULT_SELECT_COLOR));
	}
}

inline void InteractiveDisplayWindow::deselectItem(QGraphicsItem* item) {
	if (scene() == NULL || item == NULL) return;
	if (auto rect = dynamic_cast<QGraphicsRectItem*>(item)) {
		rect->setPen(QPen(DEFAULT_DESELECT_COLOR));
	} else if (auto circle = dynamic_cast<QGraphicsEllipseItem*>(item)) {
		circle->setPen(QPen(DEFAULT_DESELECT_COLOR));
	}
}

void InteractiveDisplayWindow::setItemColor(QGraphicsItem* item, const QColor& color) {
	if (auto rect = dynamic_cast<QGraphicsRectItem*>(item)) {
		rect->setBrush(QBrush(color));
	} else if (auto circle = dynamic_cast<QGraphicsEllipseItem*>(item)) {
		circle->setBrush(QBrush(color));
	} else if (auto text = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
		text->setBrush(QBrush(color));
	}
}

inline QPointF InteractiveDisplayWindow::mousePosition(QMouseEvent* event) {
	return mousePosition(event->position());
}
	
inline QPointF InteractiveDisplayWindow::mousePosition(QPointF point) {
	return mapToScene(point.x(), point.y());
}
