#pragma once
#include "common.hpp"

class ScrollEventFilter : public QObject {
	Q_OBJECT

public:
	inline bool eventFilter(QObject* obj, QEvent* event) {
		return event->type() == QEvent::Wheel;
	}
};